#include <thandor/ui/controls/text.h>

/* Implementation ownership: ui/controls/text. */

/* Address: 0x004B0200.
   Ownership: ui/controls/text.
   Purpose: Decrements the tooltip countdown while the target remains eligible. On expiry, resolves and prepares
   the target text; otherwise refreshes hover tracking from the last pointer position.
   Local calls: UiTooltip_PrepareTargetText, UiTooltip_UpdateHoverTarget.
*/
void UiTooltip_TickCountdown(void)

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
void UiNumericTextEditControl_HandleKeyboardAndCommitCf
               (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
               UiNumericTextControl *control)

{
  int *piVar1;
  ushort uVar2;
  UiNumericTextEditStateFlags UVar3;
  UiTextCodeUnitIndex UVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  word *pwVar8;
  word *pwVar9;
  
  if ((((control->editStateFlags & UI_NUMERIC_TEXT_READ_ONLY) != 0) ||
      (((control->base).nodeFlags & UI_NODE_SUPPRESSED) != 0)) || ((keyboardStateMask & 0x30) != 0))
  goto UiNumericTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
  if ((keyCode & 0xffff0000) == 0) {
    if (keyCode == 0x2d) {
      UVar3 = control->editStateFlags & UI_NUMERIC_TEXT_SIGNED_VALUE;
joined_r0x004b61ef:
      if (UVar3 == 0) {
UiNumericTextEdit_DelegateRejectedOrUnhandledKeyboardEvent:
        UiNode_DefaultKeyboardEventMoveFocusNextCf(keyboardStateMask,keyCode,&control->base);
        return;
      }
    }
    else {
      if (keyCode < 0x30) goto UiNumericTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
      if (0x39 < keyCode) {
        if ((keyCode < 0x41) || ((0x46 < keyCode && ((keyCode < 0x61 || (0x66 < keyCode))))))
        goto UiNumericTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
        UVar3 = control->editStateFlags & UI_NUMERIC_TEXT_HEXADECIMAL_FORMAT;
        goto joined_r0x004b61ef;
      }
    }
    uVar7 = control->cursorIndex;
    if ((uVar7 != control->selectionStart) || (uVar7 != control->selectionEnd)) {
      UVar4 = control->selectionEnd;
      iVar5 = UVar4 - control->selectionStart;
      pwVar8 = control->textBuffer + UVar4;
      pwVar9 = control->textBuffer + control->selectionStart;
      for (iVar6 = 0x10 - UVar4; iVar6 != 0; iVar6 = iVar6 + -1) {
        *pwVar9 = *pwVar8;
        pwVar8 = pwVar8 + 1;
        pwVar9 = pwVar9 + 1;
      }
      for (; iVar5 != 0; iVar5 = iVar5 + -1) {
        *pwVar9 = 0;
        pwVar9 = pwVar9 + 1;
      }
      uVar7 = control->selectionStart;
      control->cursorIndex = uVar7;
      control->selectionEnd = uVar7;
    }
    if (uVar7 < 0xe) {
      control->cursorIndex = control->cursorIndex + 1;
      control->selectionStart = control->selectionStart + 1;
      control->selectionEnd = control->selectionEnd + 1;
      if ((control->editStateFlags & UI_NUMERIC_TEXT_OVERWRITE_MODE) == 0) {
        do {
          LOCK();
          uVar2 = control->textBuffer[uVar7];
          control->textBuffer[uVar7] = (ushort)keyCode;
          keyCode = (UiKeyboardEventCode)uVar2;
          UNLOCK();
          uVar7 = uVar7 + 1;
        } while (uVar7 < 0xe);
      }
      else {
        control->textBuffer[uVar7] = (word)keyCode;
      }
    }
  }
  else if ((keyboardStateMask & 0xc) == 0) {
    if ((keyboardStateMask & 3) == 0) {
      if (keyCode == 0x10003) {
        UVar4 = control->cursorIndex;
        if ((UVar4 != control->selectionStart) || (UVar4 != control->selectionEnd))
        goto UiNumericTextEdit_DeleteSelectedRange;
        if (control->cursorIndex == 0) goto UiNumericTextEdit_ParseCommitInvalidateAndReturn;
        pwVar8 = control->textBuffer + UVar4;
        pwVar9 = control->textBuffer + (UVar4 - 1);
        for (iVar5 = 0x10 - UVar4; iVar5 != 0; iVar5 = iVar5 + -1) {
          *pwVar9 = *pwVar8;
          pwVar8 = pwVar8 + 1;
          pwVar9 = pwVar9 + 1;
        }
        control->cursorIndex = control->cursorIndex - 1;
      }
      else {
        if (keyCode == 0x10006) {
          UVar4 = control->cursorIndex;
          if ((UVar4 == control->selectionStart) && (UVar4 == control->selectionEnd)) {
            if (control->textBuffer[UVar4] == 0)
            goto UiNumericTextEdit_ParseCommitInvalidateAndReturn;
            pwVar8 = control->textBuffer + UVar4 + 1;
            pwVar9 = control->textBuffer + UVar4;
            for (iVar5 = 0xf - UVar4; iVar5 != 0; iVar5 = iVar5 + -1) {
              *pwVar9 = *pwVar8;
              pwVar8 = pwVar8 + 1;
              pwVar9 = pwVar9 + 1;
            }
            goto UiNumericTextEdit_RecomputeLayoutAfterEdit;
          }
UiNumericTextEdit_DeleteSelectedRange:
          UVar4 = control->selectionEnd;
          iVar5 = UVar4 - control->selectionStart;
          pwVar8 = control->textBuffer + UVar4;
          pwVar9 = control->textBuffer + control->selectionStart;
          for (iVar6 = 0x10 - UVar4; iVar6 != 0; iVar6 = iVar6 + -1) {
            *pwVar9 = *pwVar8;
            pwVar8 = pwVar8 + 1;
            pwVar9 = pwVar9 + 1;
          }
          for (; iVar5 != 0; iVar5 = iVar5 + -1) {
            *pwVar9 = 0;
            pwVar9 = pwVar9 + 1;
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
                return;
              }
            }
            else if ((keyCode & 0x30000) == 0x30000) {
              return;
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
        UVar4 = control->cursorIndex;
        if (UVar4 == 0) goto UiNumericTextEdit_ParseCommitInvalidateAndReturn;
        control->cursorIndex = 0;
        if (UVar4 == control->selectionStart) {
          control->selectionStart = 0;
        }
        else {
          control->selectionEnd = 0;
        }
        goto UiNumericTextEdit_NormalizeSelectionOrder;
      }
      if (keyCode == 0x10018) goto UiNumericTextEdit_ExtendSelectionToEnd;
      if (keyCode == 0x10014) {
        UVar4 = control->cursorIndex;
        if (UVar4 == 0) goto UiNumericTextEdit_ParseCommitInvalidateAndReturn;
        control->cursorIndex = control->cursorIndex - 1;
        if (UVar4 == control->selectionStart) {
          control->selectionStart = control->selectionStart - 1;
        }
        else {
          control->selectionEnd = control->selectionEnd - 1;
        }
      }
      else {
        if (keyCode != 0x10016) goto UiNumericTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
        UVar4 = control->cursorIndex;
        if (control->textBuffer[UVar4] == 0) goto UiNumericTextEdit_ParseCommitInvalidateAndReturn;
        control->cursorIndex = control->cursorIndex + 1;
        if (UVar4 == control->selectionEnd) {
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
    UVar4 = control->cursorIndex;
    if (control->textBuffer[UVar4] == 0) goto UiNumericTextEdit_ParseCommitInvalidateAndReturn;
    control->cursorIndex = 0;
    iVar5 = 0x60;
    if (UVar4 == control->selectionEnd) {
      iVar5 = 100;
    }
    do {
      piVar1 = (int *)((int)control->textBuffer + iVar5 + -0x6c);
      *piVar1 = *piVar1 + 1;
      control->cursorIndex = control->cursorIndex + 1;
      iVar6 = UVar4 + 1;
      UVar4 = UVar4 + 1;
    } while (control->textBuffer[iVar6] != 0);
UiNumericTextEdit_NormalizeSelectionOrder:
    if (control->selectionEnd < control->selectionStart) {
      LOCK();
      UVar4 = control->selectionEnd;
      control->selectionEnd = control->selectionStart;
      UNLOCK();
      control->selectionStart = UVar4;
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
  return;
}

/* Address: 0x004B68C0.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B6800[12]@004B6800.
   Local calls: UiTextEditControl_RecomputeLayoutAndClampScroll, UiPathTextControl_UpdateDos83Validity.
   Cross-module calls: UiNode_DefaultKeyboardEventMoveFocusNextCf [ui/controls/input], UiActionQueue_Enqueue
   [ui/core/runtime], UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiPathTextEditControl_HandleKeyboardAndValidateCf
               (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
               UiPathTextEditControl *control)

{
  int *piVar1;
  ushort uVar2;
  UiTextCodeUnitIndex UVar3;
  UiTextCodeUnitIndex UVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  word *pwVar8;
  word *pwVar9;
  
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
            UiNode_DefaultKeyboardEventMoveFocusNextCf(keyboardStateMask,keyCode,&control->base);
            return;
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
    uVar7 = control->cursorIndex;
    if ((uVar7 != control->selectionStart) || (uVar7 != control->selectionEnd)) {
      UVar4 = control->selectionEnd;
      iVar5 = UVar4 - control->selectionStart;
      pwVar8 = control->pathBuffer + UVar4;
      pwVar9 = control->pathBuffer + control->selectionStart;
      for (iVar6 = 0x100 - UVar4; iVar6 != 0; iVar6 = iVar6 + -1) {
        *pwVar9 = *pwVar8;
        pwVar8 = pwVar8 + 1;
        pwVar9 = pwVar9 + 1;
      }
      for (; iVar5 != 0; iVar5 = iVar5 + -1) {
        *pwVar9 = 0;
        pwVar9 = pwVar9 + 1;
      }
      uVar7 = control->selectionStart;
      control->cursorIndex = uVar7;
      control->selectionEnd = uVar7;
    }
    if (uVar7 < 0xfe) {
      control->cursorIndex = control->cursorIndex + 1;
      control->selectionStart = control->selectionStart + 1;
      control->selectionEnd = control->selectionEnd + 1;
      if ((control->editStateFlags & UI_TEXT_EDIT_OVERWRITE_MODE) == 0) {
        do {
          LOCK();
          uVar2 = control->pathBuffer[uVar7];
          control->pathBuffer[uVar7] = (ushort)keyCode;
          keyCode = (UiKeyboardEventCode)uVar2;
          UNLOCK();
          uVar7 = uVar7 + 1;
        } while (uVar7 < 0xfe);
      }
      else {
        control->pathBuffer[uVar7] = (word)keyCode;
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
          UVar4 = control->cursorIndex;
          if (UVar4 == 0) goto UiPathTextEdit_UpdateValidityNotifyInvalidateAndReturn;
          control->cursorIndex = 0;
          if (UVar4 == control->selectionStart) {
            control->selectionStart = 0;
          }
          else {
            control->selectionEnd = 0;
          }
        }
        else {
          if (keyCode != 0x10018) {
            if (keyCode == 0x10014) {
              UVar4 = control->cursorIndex;
              if (UVar4 == 0) goto UiPathTextEdit_UpdateValidityNotifyInvalidateAndReturn;
              control->cursorIndex = control->cursorIndex - 1;
              if (UVar4 == control->selectionStart) {
                control->selectionStart = control->selectionStart - 1;
              }
              else {
                control->selectionEnd = control->selectionEnd - 1;
              }
            }
            else {
              if (keyCode != 0x10016) goto UiPathTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
              UVar4 = control->cursorIndex;
              if (control->pathBuffer[UVar4] == 0)
              goto UiPathTextEdit_UpdateValidityNotifyInvalidateAndReturn;
              control->cursorIndex = control->cursorIndex + 1;
              if (UVar4 == control->selectionEnd) {
                control->selectionEnd = control->selectionEnd + 1;
              }
              else {
                control->selectionStart = control->selectionStart + 1;
              }
            }
            goto UiPathTextEdit_RecomputeLayoutAfterEdit;
          }
          UVar4 = control->cursorIndex;
          if (control->pathBuffer[UVar4] == 0)
          goto UiPathTextEdit_UpdateValidityNotifyInvalidateAndReturn;
          control->cursorIndex = 0;
          iVar5 = 0x60;
          if (UVar4 == control->selectionEnd) {
            iVar5 = 100;
          }
          do {
            piVar1 = (int *)((int)control->pathBuffer + iVar5 + -0x6c);
            *piVar1 = *piVar1 + 1;
            control->cursorIndex = control->cursorIndex + 1;
            iVar6 = UVar4 + 1;
            UVar4 = UVar4 + 1;
          } while (control->pathBuffer[iVar6] != 0);
        }
        goto UiPathTextEdit_NormalizeSelectionOrder;
      }
      if (keyCode == 0x10003) {
        UVar4 = control->cursorIndex;
        if ((UVar4 != control->selectionStart) || (UVar4 != control->selectionEnd))
        goto UiPathTextEdit_DeleteSelectedRange;
        if (control->cursorIndex == 0) goto UiPathTextEdit_UpdateValidityNotifyInvalidateAndReturn;
        pwVar8 = control->pathBuffer + UVar4;
        pwVar9 = control->pathBuffer + (UVar4 - 1);
        for (iVar5 = 0x100 - UVar4; iVar5 != 0; iVar5 = iVar5 + -1) {
          *pwVar9 = *pwVar8;
          pwVar8 = pwVar8 + 1;
          pwVar9 = pwVar9 + 1;
        }
        control->cursorIndex = control->cursorIndex - 1;
      }
      else {
        if (keyCode == 0x10006) {
          UVar4 = control->cursorIndex;
          if ((UVar4 == control->selectionStart) && (UVar4 == control->selectionEnd)) {
            if (control->pathBuffer[UVar4] == 0)
            goto UiPathTextEdit_UpdateValidityNotifyInvalidateAndReturn;
            pwVar8 = control->pathBuffer + UVar4 + 1;
            pwVar9 = control->pathBuffer + UVar4;
            for (iVar5 = 0xff - UVar4; iVar5 != 0; iVar5 = iVar5 + -1) {
              *pwVar9 = *pwVar8;
              pwVar8 = pwVar8 + 1;
              pwVar9 = pwVar9 + 1;
            }
            goto UiPathTextEdit_RecomputeLayoutAfterEdit;
          }
UiPathTextEdit_DeleteSelectedRange:
          UVar4 = control->selectionEnd;
          iVar5 = UVar4 - control->selectionStart;
          pwVar8 = control->pathBuffer + UVar4;
          pwVar9 = control->pathBuffer + control->selectionStart;
          for (iVar6 = 0x100 - UVar4; iVar6 != 0; iVar6 = iVar6 + -1) {
            *pwVar9 = *pwVar8;
            pwVar8 = pwVar8 + 1;
            pwVar9 = pwVar9 + 1;
          }
          for (; iVar5 != 0; iVar5 = iVar5 + -1) {
            *pwVar9 = 0;
            pwVar9 = pwVar9 + 1;
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
                return;
              }
            }
            else if ((keyCode & 0x30000) == 0x30000) {
              return;
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
        UVar4 = control->cursorIndex;
        if (UVar4 != 0) {
          UVar3 = 0;
          if (UVar4 != 1) {
            for (iVar5 = UVar4 - 2;
                ((UVar3 = 0, iVar5 != 0 && (UVar3 = iVar5 + 1, control->pathBuffer[iVar5] != 0x5c))
                && (control->pathBuffer[iVar5] != 0x2e)); iVar5 = iVar5 + -1) {
            }
          }
          control->cursorIndex = UVar3;
          control->selectionStart = UVar3;
          control->selectionEnd = UVar3;
        }
      }
      else {
        if (keyCode != 0x10016) goto UiPathTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
        UVar4 = control->cursorIndex;
        do {
          UVar3 = UVar4;
          if ((control->pathBuffer[UVar4] == 0) ||
             (UVar3 = UVar4 + 1, control->pathBuffer[UVar4] == 0x5c)) break;
          pwVar8 = control->pathBuffer + UVar4;
          UVar4 = UVar3;
        } while (*pwVar8 != 0x2e);
        control->cursorIndex = UVar3;
        control->selectionStart = UVar3;
        control->selectionEnd = UVar3;
      }
    }
    else {
      if (keyCode == 0x10014) {
        UVar4 = control->cursorIndex;
        iVar5 = 0x60;
        if (UVar4 == 0) goto UiPathTextEdit_RecomputeLayoutAfterEdit;
        if (UVar4 != control->selectionStart) {
          iVar5 = 100;
        }
        UVar3 = 0;
        if (UVar4 != 1) {
          for (iVar6 = UVar4 - 2;
              ((UVar3 = 0, iVar6 != 0 && (UVar3 = iVar6 + 1, control->pathBuffer[iVar6] != 0x5c)) &&
              (control->pathBuffer[iVar6] != 0x2e)); iVar6 = iVar6 + -1) {
          }
        }
        control->cursorIndex = UVar3;
        *(UiTextCodeUnitIndex *)((int)control->pathBuffer + iVar5 + -0x6c) = UVar3;
      }
      else {
        if (keyCode != 0x10016) goto UiPathTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
        UVar4 = control->cursorIndex;
        iVar5 = 0x60;
        if (UVar4 != control->selectionStart) {
          iVar5 = 100;
        }
        do {
          UVar3 = UVar4;
          if ((control->pathBuffer[UVar4] == 0) ||
             (UVar3 = UVar4 + 1, control->pathBuffer[UVar4] == 0x5c)) break;
          pwVar8 = control->pathBuffer + UVar4;
          UVar4 = UVar3;
        } while (*pwVar8 != 0x2e);
        control->cursorIndex = UVar3;
        *(UiTextCodeUnitIndex *)((int)control->pathBuffer + iVar5 + -0x6c) = UVar3;
      }
UiPathTextEdit_NormalizeSelectionOrder:
      if (control->selectionEnd < control->selectionStart) {
        LOCK();
        UVar4 = control->selectionEnd;
        control->selectionEnd = control->selectionStart;
        UNLOCK();
        control->selectionStart = UVar4;
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
  return;
}

/* Address: 0x004B7110.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7050[12]@004B7050.
   Local calls: UiTextEditControl_RecomputeLayoutAndClampScroll, UiTextControl_UpdateNonEmptyValidity.
   Cross-module calls: UiNode_DefaultKeyboardEventMoveFocusNextCf [ui/controls/input], UiActionQueue_Enqueue
   [ui/core/runtime], UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiRequiredTextEditControl_HandleKeyboardAndValidateCf
               (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
               UiRequiredTextEditControl *control)

{
  int *piVar1;
  ushort uVar2;
  UiTextCodeUnitIndex UVar3;
  UiTextCodeUnitIndex UVar4;
  UiTextCodeUnitIndex UVar5;
  UiTextCodeUnitCount UVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  word *pwVar11;
  word *pwVar12;
  
  uVar10 = control->cursorIndex;
  if (((control->editStateFlags & UI_REQUIRED_TEXT_READ_ONLY) != 0) ||
     (((control->base).nodeFlags & UI_NODE_SUPPRESSED) != 0)) {
UiRequiredTextEdit_DelegateRejectedOrUnhandledKeyboardEvent:
    UiNode_DefaultKeyboardEventMoveFocusNextCf(keyboardStateMask,keyCode,&control->base);
    return;
  }
  if ((((keyCode == 0x40) ||
       ((((keyCode == 0x7c || (keyCode == 0x7e)) || (keyCode == 0xb2)) ||
        ((keyCode == 0xb3 || (keyCode == 0x7b)))))) || (keyCode == 0x5b)) ||
     (((keyCode == 0x5d || (keyCode == 0x7d)) ||
      ((keyCode == 0x5c || ((keyCode == 0xb5 || (keyCode == 0x80)))))))) {
UiRequiredTextEdit_InsertCharacterOrReplaceSelection:
    uVar9 = control->bufferCapacityCodeUnits - 1;
    if ((uVar10 != control->selectionStart) || (uVar10 != control->selectionEnd)) {
      UVar3 = control->selectionEnd;
      iVar7 = UVar3 - control->selectionStart;
      pwVar11 = control->textPrefix6C + UVar3;
      pwVar12 = control->textPrefix6C + control->selectionStart;
      for (iVar8 = control->bufferCapacityCodeUnits - UVar3; iVar8 != 0; iVar8 = iVar8 + -1) {
        *pwVar12 = *pwVar11;
        pwVar11 = pwVar11 + 1;
        pwVar12 = pwVar12 + 1;
      }
      for (; iVar7 != 0; iVar7 = iVar7 + -1) {
        *pwVar12 = 0;
        pwVar12 = pwVar12 + 1;
      }
      uVar10 = control->selectionStart;
      control->cursorIndex = uVar10;
      control->selectionEnd = uVar10;
    }
    if (uVar10 < uVar9) {
      control->cursorIndex = control->cursorIndex + 1;
      control->selectionStart = control->selectionStart + 1;
      control->selectionEnd = control->selectionEnd + 1;
      if ((control->editStateFlags & UI_REQUIRED_TEXT_OVERWRITE_MODE) == 0) {
        do {
          LOCK();
          uVar2 = control->textPrefix6C[uVar10];
          control->textPrefix6C[uVar10] = (ushort)keyCode;
          keyCode = (UiKeyboardEventCode)uVar2;
          UNLOCK();
          uVar10 = uVar10 + 1;
        } while (uVar10 < uVar9);
      }
      else {
        control->textPrefix6C[uVar10] = (word)keyCode;
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
          UVar3 = control->cursorIndex;
          if (UVar3 == 0) goto UiRequiredTextEdit_UpdateValidityNotifyInvalidateAndReturn;
          control->cursorIndex = 0;
          if (UVar3 == control->selectionStart) {
            control->selectionStart = 0;
          }
          else {
            control->selectionEnd = 0;
          }
        }
        else {
          if (keyCode != 0x10018) {
            if (keyCode == 0x10014) {
              UVar3 = control->cursorIndex;
              if (UVar3 == 0) goto UiRequiredTextEdit_UpdateValidityNotifyInvalidateAndReturn;
              control->cursorIndex = control->cursorIndex - 1;
              if (UVar3 == control->selectionStart) {
                control->selectionStart = control->selectionStart - 1;
              }
              else {
                control->selectionEnd = control->selectionEnd - 1;
              }
            }
            else {
              if (keyCode != 0x10016)
              goto UiRequiredTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
              UVar3 = control->cursorIndex;
              if (control->textPrefix6C[UVar3] == 0)
              goto UiRequiredTextEdit_UpdateValidityNotifyInvalidateAndReturn;
              control->cursorIndex = control->cursorIndex + 1;
              if (UVar3 == control->selectionEnd) {
                control->selectionEnd = control->selectionEnd + 1;
              }
              else {
                control->selectionStart = control->selectionStart + 1;
              }
            }
            goto UiRequiredTextEdit_RecomputeLayoutAfterEdit;
          }
          UVar3 = control->cursorIndex;
          if (control->textPrefix6C[UVar3] == 0)
          goto UiRequiredTextEdit_UpdateValidityNotifyInvalidateAndReturn;
          control->cursorIndex = 0;
          iVar7 = 0x60;
          if (UVar3 == control->selectionEnd) {
            iVar7 = 100;
          }
          do {
            piVar1 = (int *)((int)control->textPrefix6C + iVar7 + -0x6c);
            *piVar1 = *piVar1 + 1;
            control->cursorIndex = control->cursorIndex + 1;
            iVar8 = UVar3 + 1;
            UVar3 = UVar3 + 1;
          } while (control->textPrefix6C[iVar8] != 0);
        }
        goto UiRequiredTextEdit_NormalizeSelectionOrder;
      }
      if (keyCode == 0x10003) {
        UVar3 = control->cursorIndex;
        if ((UVar3 != control->selectionStart) || (UVar3 != control->selectionEnd))
        goto UiRequiredTextEdit_DeleteSelectedRange;
        if (control->cursorIndex == 0)
        goto UiRequiredTextEdit_UpdateValidityNotifyInvalidateAndReturn;
        pwVar11 = control->textPrefix6C + UVar3;
        pwVar12 = control->textPrefix6C + (UVar3 - 1);
        for (iVar7 = control->bufferCapacityCodeUnits - UVar3; iVar7 != 0; iVar7 = iVar7 + -1) {
          *pwVar12 = *pwVar11;
          pwVar11 = pwVar11 + 1;
          pwVar12 = pwVar12 + 1;
        }
        control->cursorIndex = control->cursorIndex - 1;
      }
      else {
        if (keyCode == 0x10006) {
          UVar3 = control->cursorIndex;
          if ((UVar3 == control->selectionStart) && (UVar3 == control->selectionEnd)) {
            if (control->textPrefix6C[UVar3] == 0)
            goto UiRequiredTextEdit_UpdateValidityNotifyInvalidateAndReturn;
            iVar7 = control->bufferCapacityCodeUnits - UVar3;
            pwVar11 = control->textPrefix6C + UVar3 + 1;
            pwVar12 = control->textPrefix6C + UVar3;
            while (iVar7 = iVar7 + -1, iVar7 != 0) {
              *pwVar12 = *pwVar11;
              pwVar11 = pwVar11 + 1;
              pwVar12 = pwVar12 + 1;
            }
            goto UiRequiredTextEdit_RecomputeLayoutAfterEdit;
          }
UiRequiredTextEdit_DeleteSelectedRange:
          UVar3 = control->selectionEnd;
          iVar7 = UVar3 - control->selectionStart;
          pwVar11 = control->textPrefix6C + UVar3;
          pwVar12 = control->textPrefix6C + control->selectionStart;
          for (iVar8 = control->bufferCapacityCodeUnits - UVar3; iVar8 != 0; iVar8 = iVar8 + -1) {
            *pwVar12 = *pwVar11;
            pwVar11 = pwVar11 + 1;
            pwVar12 = pwVar12 + 1;
          }
          for (; iVar7 != 0; iVar7 = iVar7 + -1) {
            *pwVar12 = 0;
            pwVar12 = pwVar12 + 1;
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
                return;
              }
            }
            else if (keyCode == 0x10000) {
              if ((control->editStateFlags & UI_REQUIRED_TEXT_ESCAPE_CLEARS_AND_QUEUES_ACTION) != 0)
              {
                pwVar11 = control->textPrefix6C;
                for (UVar6 = control->bufferCapacityCodeUnits; UVar6 != 0; UVar6 = UVar6 - 1) {
                  *pwVar11 = 0;
                  pwVar11 = pwVar11 + 1;
                }
                control->cursorIndex = 0;
                control->selectionStart = 0;
                control->selectionEnd = 0;
                UiActionQueue_Enqueue(control->actionId,control);
                if (((control->editStateFlags & UI_REQUIRED_TEXT_PLAY_INTERACTION_SOUND) != 0) &&
                   (control->activationSound != (DirectSoundVoiceSet *)0x0)) {
                  (*g_SoundPlayOneShot)(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
                }
                return;
              }
            }
            else if ((keyCode & 0x30000) == 0x30000) {
              return;
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
        UVar3 = control->cursorIndex;
        if (UVar3 != 0) {
          if (control->textPrefix6C[UVar3 - 1] == 0x20) {
            do {
              UVar4 = UVar3 - 1;
              if (UVar4 == 0) break;
              iVar7 = UVar3 - 2;
              UVar3 = UVar4;
            } while (control->textPrefix6C[iVar7] == 0x20);
          }
          else {
            do {
              UVar5 = UVar3;
              UVar4 = UVar5 - 1;
              if (UVar4 == 0) goto UiRequiredTextEdit_MoveCursorToPreviousWordBoundary;
              UVar3 = UVar4;
            } while (control->textPrefix6C[UVar5 - 2] != 0x20);
            if ((1 < (int)UVar4) && (control->textPrefix6C[UVar5 - 3] != 0x20)) {
              UVar4 = UVar5 - 2;
            }
          }
UiRequiredTextEdit_MoveCursorToPreviousWordBoundary:
          control->cursorIndex = UVar4;
          control->selectionStart = UVar4;
          control->selectionEnd = UVar4;
        }
      }
      else {
        if (keyCode != 0x10016) goto UiRequiredTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
        UVar3 = control->cursorIndex;
        if (control->textPrefix6C[UVar3] == 0x20) {
          for (; (control->textPrefix6C[UVar3] != 0 && (control->textPrefix6C[UVar3] == 0x20));
              UVar3 = UVar3 + 1) {
          }
        }
        else {
          do {
            UVar4 = UVar3;
            UVar3 = UVar4;
            if (control->textPrefix6C[UVar4] == 0)
            goto UiRequiredTextEdit_MoveCursorToNextWordBoundary;
            UVar3 = UVar4 + 1;
          } while (control->textPrefix6C[UVar4] != 0x20);
          if (control->textPrefix6C[UVar4 + 1] == 0x20) {
            UVar3 = UVar4;
          }
        }
UiRequiredTextEdit_MoveCursorToNextWordBoundary:
        control->cursorIndex = UVar3;
        control->selectionStart = UVar3;
        control->selectionEnd = UVar3;
      }
    }
    else {
      if (keyCode == 0x10014) {
        UVar3 = control->cursorIndex;
        iVar7 = 0x60;
        if (UVar3 == 0) goto UiRequiredTextEdit_RecomputeLayoutAfterEdit;
        if (UVar3 != control->selectionStart) {
          iVar7 = 100;
        }
        if (control->textPrefix6C[UVar3 - 1] == 0x20) {
          do {
            UVar4 = UVar3 - 1;
            if (UVar4 == 0) break;
            iVar8 = UVar3 - 2;
            UVar3 = UVar4;
          } while (control->textPrefix6C[iVar8] == 0x20);
        }
        else {
          do {
            UVar5 = UVar3;
            UVar4 = UVar5 - 1;
            if (UVar4 == 0) goto UiRequiredTextEdit_ExtendSelectionToPreviousWordBoundary;
            UVar3 = UVar4;
          } while (control->textPrefix6C[UVar5 - 2] != 0x20);
          if ((1 < (int)UVar4) && (control->textPrefix6C[UVar5 - 3] != 0x20)) {
            UVar4 = UVar5 - 2;
          }
        }
UiRequiredTextEdit_ExtendSelectionToPreviousWordBoundary:
        control->cursorIndex = UVar4;
        *(UiTextCodeUnitIndex *)((int)control->textPrefix6C + iVar7 + -0x6c) = UVar4;
      }
      else {
        if (keyCode != 0x10016) goto UiRequiredTextEdit_DelegateRejectedOrUnhandledKeyboardEvent;
        UVar3 = control->cursorIndex;
        iVar7 = 0x60;
        if ((UVar3 == control->selectionStart) ||
           (iVar7 = 100, control->textPrefix6C[UVar3] != 0x20)) {
          do {
            UVar4 = UVar3;
            UVar3 = UVar4;
            if (control->textPrefix6C[UVar4] == 0)
            goto UiRequiredTextEdit_ExtendSelectionToNextWordBoundary;
            UVar3 = UVar4 + 1;
          } while (control->textPrefix6C[UVar4] != 0x20);
          if (control->textPrefix6C[UVar4 + 1] == 0x20) {
            UVar3 = UVar4;
          }
        }
        else {
          for (; (control->textPrefix6C[UVar3] != 0 && (control->textPrefix6C[UVar3] == 0x20));
              UVar3 = UVar3 + 1) {
          }
        }
UiRequiredTextEdit_ExtendSelectionToNextWordBoundary:
        control->cursorIndex = UVar3;
        *(UiTextCodeUnitIndex *)((int)control->textPrefix6C + iVar7 + -0x6c) = UVar3;
      }
UiRequiredTextEdit_NormalizeSelectionOrder:
      if (control->selectionEnd < control->selectionStart) {
        LOCK();
        UVar3 = control->selectionEnd;
        control->selectionEnd = control->selectionStart;
        UNLOCK();
        control->selectionStart = UVar3;
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
  return;
}

/* Address: 0x004227B0.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00422720[2]@00422720.
   Local calls: UiTextButtonControl_DrawClipped.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector
   [assets/text/richtext].
*/
void UiGraphicsAdapterTextButton_DrawFormattedAdapterText
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiTextButtonControl *control)

{
  UiPackedTextStyle UVar1;
  GraphicsAdapterRecord *pGVar2;
  word *pwVar3;
  word *replacementPayload;
  word *stream;
  word *stream_00;
  word *extraout_EDX;
  undefined8 uVar4;
  
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    pwVar3 = TextResource_Resolve(control->textResourceId);
    pGVar2 = g_GraphicsAdapters;
    if (((control->selectable).stateFlags & 0x80) != 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control[-1].packedTextStyle,
                 (word *)&g_GraphicsAdapterFormatScratch0Utf16);
      RichTextCommandStream_PatchPayloadBySelector
                (0,&g_GraphicsAdapterFormatScratch0Utf16,stream_00);
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
      uVar4 = RichTextCommandStream_PatchPayloadBySelector
                        (0,&g_GraphicsAdapterFormatScratch0Utf16,stream);
      RichTextCommandStream_PatchPayloadBySelector
                (1,&g_GraphicsAdapterFormatScratch1Utf16,(word *)((ulonglong)uVar4 >> 0x20));
      UiTextButtonControl_DrawClipped(clipTop,clipLeft,clipBottom,clipRight,control);
      return;
    }
    UVar1 = control[-1].packedTextStyle;
    uVar4 = RichTextCommandStream_PatchPayloadBySelector
                      (0,g_GraphicsAdapters[UVar1].driverDescriptionUtf16,pwVar3);
    pwVar3 = (word *)((ulonglong)uVar4 >> 0x20);
    if (pGVar2[UVar1].deviceGuid.Data1 == 0) {
      replacementPayload = TextResource_Resolve(0x111);
      pwVar3 = extraout_EDX;
    }
    else {
      replacementPayload = pGVar2[UVar1].deviceNameUtf16;
    }
    RichTextCommandStream_PatchPayloadBySelector(1,replacementPayload,pwVar3);
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
void UiNumericTextEditControl_RelocateAndRebuildText
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
void UiTextEditControl_DrawTextSelectionAndCaret
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiTextEditControl *control)

{
  UiTextCodeUnitCount prefixLength;
  UiPixelCoordinate tileEnd;
  UiPixelCoordinate tileEnd_00;
  int extraout_ECX;
  int iVar1;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  UiPixelCoordinate tileStart;
  UiPixelCoordinate tileStart_00;
  int extraout_EDX;
  int iVar2;
  int extraout_EDX_00;
  int iVar3;
  UiPackedTextStyle packedStyle;
  int iVar4;
  undefined1 in_CF;
  qword qVar5;
  undefined8 uVar6;
  dword arg6;
  GraphicsTextureSourceAsset *arg7;
  SoftwareFramebufferAccess *arg8;
  
  (*g_GraphicsFramebufferBeginAccess)();
  if (!(bool)in_CF) {
    if ((control->editStateFlags & UI_TEXT_EDIT_DRAW_FRAMED_CHROME) == 0) {
      iVar1 = (control->base).left;
      iVar2 = (control->base).top;
      iVar4 = (control->base).right;
      iVar3 = (control->base).bottom;
    }
    else {
      iVar4 = (control->base).layoutWidth;
      iVar3 = (control->base).layoutHeight;
      qVar5 = (*g_GraphicsTextureSourceGetLogicalSize)(0x6d,g_UiWindowTextureSource);
      iVar1 = (int)(qVar5 >> 0x20);
      iVar4 = iVar4 - (int)qVar5;
      iVar3 = iVar3 - iVar1;
      if ((control->editStateFlags & UI_TEXT_EDIT_DRAW_TILED_INTERIOR) != 0) {
        UiWindow_BlitTiledInterior
                  (clipTop,clipLeft,clipBottom,clipRight,0x7a,iVar3,iVar4,iVar1,(int)qVar5,control);
      }
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,(control->base).top,(control->base).left,0x6a
                 ,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,(control->base).top,
                 iVar4 + (control->base).left,0x6b,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,iVar3 + (control->base).top,
                 (control->base).left,0x6c,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,iVar3 + (control->base).top,
                 iVar4 + (control->base).left,0x6d,g_UiWindowTextureSource,g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x6e,iVar4,0,tileEnd,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x6f,iVar3,tileStart,0,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x70,iVar3,tileStart_00,iVar4,control);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x71,iVar4,iVar3,tileEnd_00,control);
      iVar2 = extraout_EDX + (control->base).top;
      iVar4 = iVar4 + (control->base).left;
      iVar1 = extraout_ECX + (control->base).left;
      iVar3 = iVar3 + (control->base).top;
    }
    if (clipRight < iVar1) {
      clipRight = iVar1;
    }
    if (clipBottom < iVar2) {
      clipBottom = iVar2;
    }
    FontGlyph_GetLogicalSizeActiveRegs(0);
    if (iVar4 < clipLeft) {
      clipLeft = iVar4;
    }
    iVar1 = (extraout_ECX_00 - control->horizontalScrollPixels) + 2;
    if (iVar3 < clipTop) {
      clipTop = iVar3;
    }
    iVar2 = (control->base).layoutHeight - extraout_EDX_00 >> 1;
    prefixLength = control->selectionEnd;
    if (((control->editStateFlags & UI_TEXT_EDIT_READ_ONLY) == 0) &&
       (control->selectionStart != prefixLength)) {
      uVar6 = UiTextEditControl_MeasurePrefixWidth(control->selectionStart,control);
      iVar1 = (int)uVar6;
      uVar6 = UiTextEditControl_MeasurePrefixWidth(prefixLength,control);
      iVar2 = (int)((ulonglong)uVar6 >> 0x20);
      if (iVar1 == 0) {
        iVar1 = -2;
      }
      iVar4 = (int)uVar6 + extraout_ECX_01;
      if (clipLeft < iVar4) {
        iVar4 = clipLeft;
      }
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x8b,iVar4,iVar2 + -1,iVar1 + extraout_ECX_01
                 ,control);
      iVar1 = extraout_ECX_02;
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
                 iVar2 + (control->base).top,iVar1 + (control->base).left);
    }
    else {
      uVar6 = UiTextEditControl_MeasurePrefixWidth(control->cursorIndex,control);
      iVar1 = (int)((ulonglong)uVar6 >> 0x20);
      iVar2 = (int)uVar6 + -2 + extraout_ECX_03 + (control->base).left;
      arg6 = 0x89;
      if ((control->editStateFlags & UI_TEXT_EDIT_OVERWRITE_MODE) != 0) {
        arg6 = 0x8a;
      }
      iVar3 = (int)(packedStyle << 0x10) >> 0x18;
      iVar4 = iVar1 + -1 + (control->base).top + iVar3;
      if ((control->editStateFlags & UI_TEXT_EDIT_OVERWRITE_MODE) != 0) {
        iVar4 = iVar4 + 1;
      }
      arg7 = g_UiWindowTextureSource;
      arg8 = g_FramebufferAccess;
      (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,iVar4,iVar2 + iVar3,0x7f000000,0x89,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      uVar6 = RichTextCommandStream_DrawSingleLine
                        (clipTop,clipLeft,clipBottom,clipRight,packedStyle,control->textPrefix6C,
                         iVar1 + (control->base).top,extraout_ECX_04 + (control->base).left);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,
                 (int)((ulonglong)uVar6 >> 0x20) + (control->base).top + -1,iVar2,arg6,arg7,arg8);
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
void UiTextEditControl_BeginSelectionAtPointer
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiTextEditControl *control)

{
  UiTextCodeUnitIndex cursorIndexAtPointer;
  undefined8 uVar1;
  
  if ((control->editStateFlags & UI_TEXT_EDIT_READ_ONLY) == 0) {
    control->editStateFlags = control->editStateFlags | UI_TEXT_EDIT_POINTER_SELECTION_ACTIVE;
    uVar1 = UiTextEditControl_FindCursorIndexAtX(pointerX,control);
    cursorIndexAtPointer = (UiTextCodeUnitIndex)uVar1;
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
void UiTextEditControl_UpdateSelectionFromPointer
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiTextEditControl *control)

{
  UiTextCodeUnitIndex UVar1;
  UiNodeVtable *pUVar2;
  uint uVar3;
  undefined8 uVar4;
  
  if ((control->editStateFlags & UI_TEXT_EDIT_POINTER_SELECTION_ACTIVE) != 0) {
    uVar4 = UiTextEditControl_FindCursorIndexAtX(pointerX,control);
    uVar3 = (uint)uVar4;
    control->cursorIndex = uVar3;
    if ((UiTextCodeUnitIndex)((ulonglong)uVar4 >> 0x20) == control->selectionStart) {
      control->selectionStart = uVar3;
      uVar3 = control->selectionEnd;
    }
    else {
      control->selectionEnd = uVar3;
    }
    pUVar2 = (control->base).vtable;
    if (uVar3 < control->selectionStart) {
      LOCK();
      UVar1 = control->selectionStart;
      control->selectionStart = uVar3;
      UNLOCK();
      control->selectionEnd = UVar1;
    }
    (*pUVar2->layout)(&control->base);
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
void UiPathTextEditControl_RelocateAndValidateDos83
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
void UiRequiredTextEditControl_RelocateAndValidateNonEmpty
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
void UiPointerList_SortByExpandedTextFieldAscending
               (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control)

{
  void *pvVar1;
  int extraout_ECX;
  UiListRowCount UVar2;
  int clipBottom;
  void **ppvVar3;
  void **ppvVar4;
  bool bVar5;
  undefined8 uVar6;
  void *pvVar7;
  
  ppvVar4 = control->rowSlots;
  if (((ppvVar4 != (void **)0x0) && (control->rowCount != 1)) && (-1 < (int)(control->rowCount - 1))
     ) {
    pvVar7 = *control->selectedRowSlot;
    ppvVar3 = ppvVar4;
    do {
      ppvVar4 = ppvVar4 + 1;
      bVar5 = CARRY4((uint)*ppvVar4,fieldOffset);
      uVar6 = UiPointerList_CompareExpandedTextFlags
                        ((word *)((int)*ppvVar4 + fieldOffset),(word *)((int)*ppvVar3 + fieldOffset)
                        );
      if (!bVar5) {
        LOCK();
        pvVar1 = *ppvVar4;
        *ppvVar4 = *ppvVar3;
        UNLOCK();
        *ppvVar3 = pvVar1;
      }
    } while (((int)((ulonglong)uVar6 >> 0x20) != 1) ||
            (ppvVar4 = ppvVar3 + 1, ppvVar3 = ppvVar4, extraout_ECX != 1));
    ppvVar4 = control->rowSlots;
    UVar2 = control->rowCount;
    clipBottom = 0;
    do {
      if (pvVar7 == *ppvVar4)
      goto 
      UiPointerList_SortByExpandedTextFieldAscending_CommitResolvedSelectedRowSlotAndClampViewport;
      clipBottom = clipBottom + control->rowHeight;
      ppvVar4 = ppvVar4 + 1;
      UVar2 = UVar2 - 1;
    } while (UVar2 != 0);
    ppvVar4 = control->rowSlots;
UiPointerList_SortByExpandedTextFieldAscending_CommitResolvedSelectedRowSlotAndClampViewport:
    control->selectedRowSlot = ppvVar4;
    UiScrollableControl_ClampOffsetsToViewport
              (clipBottom + 1 + control->rowHeight,(control->base).rightOffset,clipBottom,0,
               (UiScrollableControl *)(control->base).parent);
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
undefined8
UiPointerList_SortByExpandedTextFieldDescending
          (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control)

{
  void *pvVar1;
  undefined4 in_EAX;
  int extraout_ECX;
  UiListRowCount UVar2;
  undefined4 in_EDX;
  int clipBottom;
  word *rightText;
  void **ppvVar3;
  void **ppvVar4;
  bool bVar5;
  bool bVar6;
  undefined8 uVar7;
  void *pvVar8;
  
  ppvVar4 = control->rowSlots;
  if (((ppvVar4 != (void **)0x0) && (control->rowCount != 1)) && (-1 < (int)(control->rowCount - 1))
     ) {
    pvVar8 = *control->selectedRowSlot;
    ppvVar3 = ppvVar4;
    do {
      ppvVar4 = ppvVar4 + 1;
      bVar5 = CARRY4((uint)*ppvVar4,fieldOffset);
      rightText = (word *)((int)*ppvVar4 + fieldOffset);
      bVar6 = rightText == (word *)0x0;
      uVar7 = UiPointerList_CompareExpandedTextFlags
                        (rightText,(word *)((int)*ppvVar3 + fieldOffset));
      if (bVar5 || bVar6) {
        LOCK();
        pvVar1 = *ppvVar4;
        *ppvVar4 = *ppvVar3;
        UNLOCK();
        *ppvVar3 = pvVar1;
      }
    } while (((int)((ulonglong)uVar7 >> 0x20) != 1) ||
            (ppvVar4 = ppvVar3 + 1, ppvVar3 = ppvVar4, extraout_ECX != 1));
    ppvVar4 = control->rowSlots;
    UVar2 = control->rowCount;
    clipBottom = 0;
    do {
      if (pvVar8 == *ppvVar4)
      goto 
      UiPointerList_SortByExpandedTextFieldDescending_CommitResolvedSelectedRowSlotAndClampViewport;
      clipBottom = clipBottom + control->rowHeight;
      ppvVar4 = ppvVar4 + 1;
      UVar2 = UVar2 - 1;
    } while (UVar2 != 0);
    ppvVar4 = control->rowSlots;
UiPointerList_SortByExpandedTextFieldDescending_CommitResolvedSelectedRowSlotAndClampViewport:
    control->selectedRowSlot = ppvVar4;
    UiScrollableControl_ClampOffsetsToViewport
              (clipBottom + 1 + control->rowHeight,(control->base).rightOffset,clipBottom,0,
               (UiScrollableControl *)(control->base).parent);
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x005156A0.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00515610[2]@00515610.
   Local calls: UiTextButtonControl_DrawClipped.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector
   [assets/text/richtext].
*/
void UiNumericPairTextButton_DrawFormattedValues
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiTextButtonControl *control)

{
  word *stream;
  undefined8 patchedStreamCursorPair;
  
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    TextResource_Resolve(control->textResourceId);
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)control[1].selectable.base.nextSibling,
               (word *)&g_UiNumericPairFirstValueScratchUtf16);
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)control[1].selectable.base.firstChild,
               (word *)&g_UiNumericPairSecondValueScratchUtf16);
    patchedStreamCursorPair =
         RichTextCommandStream_PatchPayloadBySelector
                   (0,&g_UiNumericPairFirstValueScratchUtf16,stream);
    RichTextCommandStream_PatchPayloadBySelector
              (1,&g_UiNumericPairSecondValueScratchUtf16,
               (word *)((ulonglong)patchedStreamCursorPair >> 0x20));
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
void UiPayloadPairTextButton_DrawFormattedPayloads
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiTextButtonControl *control)

{
  word *stream;
  undefined8 patchedStreamCursorPair;
  
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    stream = TextResource_Resolve(control->textResourceId);
    patchedStreamCursorPair =
         RichTextCommandStream_PatchPayloadBySelector
                   (0,control[1].selectable.base.nextSibling,stream);
    RichTextCommandStream_PatchPayloadBySelector
              (1,control[1].selectable.base.firstChild,
               (word *)((ulonglong)patchedStreamCursorPair >> 0x20));
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
void UiTooltip_Draw(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,
                   UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft)

{
  UiNodeBase *pUVar1;
  UiNodeBase *pUVar2;
  word *commandStream;
  sdword sVar3;
  int extraout_ECX;
  int iVar4;
  int extraout_ECX_00;
  sdword extraout_ECX_01;
  int extraout_ECX_02;
  uint uVar5;
  int arg5;
  uint arg4;
  bool bVar6;
  qword qVar7;
  undefined8 uVar8;
  qword qVar9;
  
  pUVar1 = g_UiTooltipState.targetNode;
  if ((g_UiTooltipState.targetNode != (UiNodeBase *)0x0) && (g_UiTooltipState.countdownFrames == 0))
  {
    pUVar2 = UiNode_GetRoot(g_UiTooltipState.targetNode);
    commandStream = (word *)pUVar1[-1].nodeFlags;
    if ((pUVar1->nodeFlags & UI_NODE_TOOLTIP_REFERENCE_DIRECT_UTF16) == 0) {
      commandStream = TextResource_Resolve((TextResourceId)commandStream);
    }
    RichTextCommandStream_MeasureRegs(g_UiTooltipTextStyle,commandStream);
    qVar7 = (*g_GraphicsTextureSourceGetLogicalSize)(0xbc,g_UiWindowTextureSource);
    uVar5 = (uint)(qVar7 >> 0x20);
    arg4 = pUVar1->top - uVar5;
    bVar6 = CARRY4(uVar5,arg4);
    uVar8 = (*g_GraphicsFramebufferBeginAccess)();
    if (!bVar6) {
      sVar3 = (int)uVar8 - extraout_ECX >> 1;
      if (pUVar2 == (UiNodeBase *)0xffffffff) {
        pUVar2 = pUVar1;
      }
      iVar4 = extraout_ECX + sVar3;
      if (sVar3 < pUVar2->left) {
        iVar4 = iVar4 - (sVar3 - pUVar2->left);
        sVar3 = pUVar2->left;
      }
      if (pUVar2->right < iVar4) {
        sVar3 = sVar3 - (iVar4 - pUVar2->right);
      }
      if ((int)arg4 < pUVar2->top) {
        arg4 = pUVar1->layoutHeight + (int)((ulonglong)uVar8 >> 0x20);
      }
      qVar9 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                        (clipBottom,clipRight,clipTop,clipLeft,arg4,sVar3,0xbc,
                         g_UiWindowTextureSource,g_FramebufferAccess);
      iVar4 = (int)qVar7 + (int)qVar9;
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipBottom,clipRight,clipTop,clipLeft,arg4,extraout_ECX_00 - (int)qVar7,0xbe,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceGetLogicalSize)(0xbd,g_UiWindowTextureSource);
      sVar3 = extraout_ECX_01;
      arg5 = iVar4;
      do {
        qVar7 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                          (clipBottom,sVar3,clipTop,clipLeft,arg4,arg5,0xbd,g_UiWindowTextureSource,
                           g_FramebufferAccess);
        arg5 = (int)(qVar7 >> 0x20) + (int)qVar7;
        sVar3 = extraout_ECX_02;
      } while (arg5 < extraout_ECX_02);
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,g_UiTooltipTextStyle,commandStream,arg4 + 3,
                 iVar4);
      (*g_GraphicsFramebufferEndAccess)();
    }
  }
  bVar6 = g_UiRootNode != (UiRootNode *)0xffffffff;
  if ((g_UiRootNode == (UiRootNode *)0xffffffff) && ((*g_GraphicsFramebufferBeginAccess)(), !bVar6))
  {
    (*g_GraphicsFramebufferFillRectArgb)
              (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0,0
               ,0x80000000,g_FramebufferAccess);
    (*g_GraphicsFramebufferEndAccess)();
  }
  return;
}

/* Address: 0x004B0F90.
   Ownership: ui/controls/text.
   Purpose: Pops UI roots until the current root equals the protected global boundary pointer at 004B0E34 or the
   stack sentinel. CF set from a pop stops the loop; EAX is preserved.
   Cross-module calls: UiRootStack_PopCf [ui/controls/layout].
*/
undefined4 UiRootStack_PopUntilWindowTextureBoundaryCf(void)

{
  undefined4 in_EAX;
  bool rootWasValid;
  
  while( true ) {
    if ((GraphicsTextureSourceAsset *)g_UiRootNode == g_UiWindowTextureSource) {
      return in_EAX;
    }
    rootWasValid = g_UiRootNode != (UiRootNode *)0xffffffff;
    if (g_UiRootNode == (UiRootNode *)0xffffffff) break;
    UiRootStack_PopCf(g_UiRootNode);
    if (rootWasValid) {
      return in_EAX;
    }
  }
  return in_EAX;
}

/* Address: 0x004B1DD0.
   Ownership: ui/controls/text.
   Purpose: Expands the serialized layout rectangle by the configured frame inset when state flag 0x04 is set, then
   relocates child pointers.
   Cross-module calls: UiContainer_RelocateChildren [ui/controls/layout].
*/
void UiFramedTextButtonControl_Relocate
               (UiSerializedRelocationDelta relocationDelta,UiFramedTextButtonControl *control)

{
  sdword *psVar1;
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
    psVar1 = &(control->selectable).base.bottomOffset;
    *psVar1 = *psVar1 + frameInset;
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
void UiFramedTextButtonControl_DrawClipped
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiFramedTextButtonControl *control)

{
  UiPixelCoordinate tileEnd;
  word *commandStream;
  dword dVar1;
  dword arg6;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  int iVar2;
  int extraout_ECX_06;
  sdword extraout_ECX_07;
  int extraout_ECX_08;
  sdword sVar3;
  sdword arg5;
  int extraout_ECX_09;
  sdword extraout_ECX_10;
  int extraout_ECX_11;
  UiPixelCoordinate tileStart;
  UiPixelCoordinate tileStart_00;
  sdword sVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  bool bVar8;
  qword qVar9;
  RichTextExtentRegs RVar10;
  undefined8 uVar11;
  
  bVar8 = &stack0xffffffe4 < (undefined1 *)0x4;
  (*g_GraphicsFramebufferBeginAccess)();
  if (bVar8) goto UiFramedTextButtonControl_DrawClipped_DrawChildrenIfEnabledAndReturn;
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & 4) == 0) {
      dVar1 = 0x4a;
    }
    else {
      dVar1 = 0x94;
    }
    if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
      dVar1 = dVar1 + 8;
    }
UiFramedTextButtonControl_DrawClipped_RenderSelectedFrameTextAndFocusChrome:
    qVar9 = (*g_GraphicsTextureSourceGetLogicalSize)(dVar1,g_UiWindowTextureSource);
    iVar2 = (int)qVar9;
    iVar7 = (control->selectable).base.layoutWidth - iVar2;
    iVar5 = (control->selectable).base.layoutHeight - (int)(qVar9 >> 0x20);
    if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) ||
       (((control->selectable).stateFlags & 0x800) == 0)) {
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,(control->selectable).base.top,
                 (control->selectable).base.left,arg6,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,(control->selectable).base.top,
                 iVar7 + (control->selectable).base.left,extraout_ECX + 1,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,iVar5 + (control->selectable).base.top,
                 (control->selectable).base.left,extraout_ECX_00 + 1,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,iVar5 + (control->selectable).base.top,
                 iVar7 + (control->selectable).base.left,extraout_ECX_01 + 1,g_UiWindowTextureSource
                 ,g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,extraout_ECX_02 + 1,iVar7,0,iVar2,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,extraout_ECX_03 + 1,iVar5,tileStart,0,control
                );
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,extraout_ECX_04 + 1,iVar5,tileStart_00,iVar7,
                 control);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,extraout_ECX_05 + 1,iVar7,iVar5,tileEnd,
                 control);
    }
    commandStream = TextResource_Resolve(control->textResourceId);
    RVar10 = RichTextCommandStream_MeasureRegs(control->packedTextStyle,commandStream);
    dVar1 = RVar10.widthPixels;
    iVar2 = (int)((control->selectable).base.layoutWidth - dVar1) >> 1;
    iVar5 = (int)((control->selectable).base.layoutHeight - RVar10.heightPixels) >> 1;
    uVar6 = g_UiTextStyleDisabled;
    if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
       (uVar6 = g_UiTextStyleNormal,
       ((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0)) {
      uVar6 = g_UiTextStyleSelected;
    }
    if (((control->selectable).stateFlags & 0x100) == 0) {
      control->packedTextStyle = control->packedTextStyle & 0xff0000;
    }
    else {
      uVar6 = uVar6 & 0xffffff;
    }
    if (((control->selectable).stateFlags & 0x200) == 0) {
      control->packedTextStyle = control->packedTextStyle & 0xff000000;
    }
    else {
      uVar6 = uVar6 & 0xff00ffff;
    }
    uVar6 = uVar6 | control->packedTextStyle;
    if ((((control->selectable).base.nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) == 0) ||
       (((control->selectable).stateFlags & 0x1000) != 0)) {
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,uVar6,commandStream,
                 iVar5 + (control->selectable).base.top,iVar2 + (control->selectable).base.left);
    }
    else {
      iVar2 = iVar2 + (control->selectable).base.left;
      iVar5 = iVar5 + (control->selectable).base.top;
      qVar9 = (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                        (clipTop,clipLeft,clipBottom,clipRight,iVar5,iVar2 + -2,0x7f000000,0x86,
                         g_UiWindowTextureSource,g_FramebufferAccess);
      sVar3 = (sdword)(qVar9 >> 0x20);
      qVar9 = (*g_GraphicsTextureSourceGetLogicalSize)(0x88,g_UiWindowTextureSource);
      (*g_GraphicsTextureSourceGetLogicalSize)(0x86,g_UiWindowTextureSource);
      (*g_GraphicsTextureSourceGetLogicalSize)(0x87,g_UiWindowTextureSource);
      iVar7 = ((extraout_ECX_06 + 6) - (int)qVar9) + dVar1;
      qVar9 = (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                        (clipTop,clipLeft,clipBottom,clipRight,sVar3,iVar7,0x7f000000,0x88,
                         g_UiWindowTextureSource,g_FramebufferAccess);
      sVar4 = (sdword)(qVar9 >> 0x20);
      sVar3 = extraout_ECX_07;
      if (clipLeft < iVar7) {
        iVar7 = clipLeft;
      }
      do {
        qVar9 = (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                          (clipTop,iVar7,clipBottom,clipRight,sVar4,sVar3,0x7f000000,0x87,
                           g_UiWindowTextureSource,g_FramebufferAccess);
        sVar4 = (sdword)(qVar9 >> 0x20);
        sVar3 = extraout_ECX_08 + (int)qVar9;
      } while (sVar3 < iVar7);
      uVar11 = RichTextCommandStream_DrawSingleLine
                         (clipTop,clipLeft,clipBottom,clipRight,uVar6,commandStream,iVar5,iVar2);
      qVar9 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                        (clipTop,clipLeft,clipBottom,clipRight,(sdword)((ulonglong)uVar11 >> 0x20),
                         arg5,0x86,g_UiWindowTextureSource,g_FramebufferAccess);
      sVar4 = (sdword)(qVar9 >> 0x20);
      qVar9 = (*g_GraphicsTextureSourceGetLogicalSize)(0x88,g_UiWindowTextureSource);
      (*g_GraphicsTextureSourceGetLogicalSize)(0x86,g_UiWindowTextureSource);
      (*g_GraphicsTextureSourceGetLogicalSize)(0x87,g_UiWindowTextureSource);
      iVar2 = ((extraout_ECX_09 + 6) - (int)qVar9) + dVar1;
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,sVar4,iVar2,0x88,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      sVar3 = extraout_ECX_10;
      if (clipLeft < iVar2) {
        iVar2 = clipLeft;
      }
      do {
        qVar9 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                          (clipTop,iVar2,clipBottom,clipRight,sVar4,sVar3,0x87,
                           g_UiWindowTextureSource,g_FramebufferAccess);
        sVar3 = extraout_ECX_11 + (int)qVar9;
      } while (sVar3 < iVar2);
    }
  }
  else if (((control->selectable).stateFlags & 0x400) == 0) {
    if (((control->selectable).stateFlags & 4) == 0) {
      dVar1 = 0x8c;
    }
    else {
      dVar1 = 0xa4;
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
void UiFramedTextButtonControl_NonRightPress
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiFramedTextButtonControl *control)

{
  UiSelectableStateFlags *pUVar1;
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
      pUVar1 = &(control->selectable).stateFlags;
      *pUVar1 = *pUVar1 | UI_SELECTABLE_SELECTED_OR_CHECKED;
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
void UiFramedTextButtonControl_NonRightRelease
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiFramedTextButtonControl *control)

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
void UiFramedTextButtonControl_NonRightDrag
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiFramedTextButtonControl *control)

{
  UiSelectableStateFlags *pUVar1;
  sdword *psVar2;
  int iVar3;
  int iVar4;
  UiSelectableStateFlags *stateFlagsField;
  
  if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) == 0)) {
    psVar2 = &(control->selectable).base.left;
    iVar3 = pointerX - *psVar2;
    if (((pointerX < *psVar2) ||
        (((psVar2 = &(control->selectable).base.top, iVar4 = pointerY - *psVar2, pointerY < *psVar2
          || ((control->selectable).base.layoutWidth <= iVar3)) ||
         ((control->selectable).base.layoutHeight <= iVar4)))) ||
       ((((control->selectable).stateFlags & 4) != 0 &&
        (((iVar3 < g_UiWindowFrameInset || (iVar4 < g_UiWindowFrameInset)) ||
         (((control->selectable).base.layoutWidth <= iVar3 + g_UiWindowFrameInset ||
          ((control->selectable).base.layoutHeight <= iVar4 + g_UiWindowFrameInset)))))))) {
      if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
        pUVar1 = &(control->selectable).stateFlags;
        *pUVar1 = *pUVar1 & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
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
UiNodeBase *
UiFramedTextButtonControl_HitTestRect
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiFramedTextButtonControl *control)

{
  sdword *psVar1;
  UiFramedTextButtonControl *pUVar2;
  int iVar3;
  int iVar4;
  
  pUVar2 = (UiFramedTextButtonControl *)0xffffffff;
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    psVar1 = &(control->selectable).base.left;
    iVar3 = pointerX - *psVar1;
    if (((((*psVar1 <= pointerX) &&
          (psVar1 = &(control->selectable).base.top, iVar4 = pointerY - *psVar1, *psVar1 <= pointerY
          )) && (iVar3 < (control->selectable).base.layoutWidth)) &&
        (iVar4 < (control->selectable).base.layoutHeight)) &&
       ((((control->selectable).stateFlags & 4) == 0 ||
        (((g_UiWindowFrameInset <= iVar3 && (g_UiWindowFrameInset <= iVar4)) &&
         ((iVar3 + g_UiWindowFrameInset < (control->selectable).base.layoutWidth &&
          (iVar4 + g_UiWindowFrameInset < (control->selectable).base.layoutHeight)))))))) {
      pUVar2 = control;
    }
  }
  return (UiNodeBase *)pUVar2;
}

/* Address: 0x004B27D0.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B2740[2]@004B2740.
   Cross-module calls: UiWindow_BlitTiledHorizontalEdge [ui/controls/layout], UiWindow_BlitTiledVerticalEdge
   [ui/controls/layout], TextResource_Resolve [assets/text/resources], RichTextCommandStream_MeasureRegs
   [assets/text/richtext], RichTextCommandStream_DrawSingleLine [assets/text/richtext].
*/
void UiWindowControl_DrawFramedTextAndChrome
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiNodeBase *control)

{
  char cVar1;
  char cVar2;
  UiPixelCoordinate tileEnd;
  word *commandStream;
  dword dVar3;
  dword arg6;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  int extraout_ECX_06;
  int iVar4;
  int extraout_ECX_07;
  sdword extraout_ECX_08;
  int extraout_ECX_09;
  sdword sVar5;
  sdword arg5;
  int extraout_ECX_10;
  sdword extraout_ECX_11;
  int extraout_ECX_12;
  UiPixelCoordinate tileStart;
  UiPixelCoordinate tileStart_00;
  sdword sVar6;
  int arg4;
  int iVar7;
  uint uVar8;
  int iVar9;
  bool bVar10;
  qword qVar11;
  RichTextExtentRegs RVar12;
  undefined8 uVar13;
  
  bVar10 = &stack0xffffffe4 < (undefined1 *)0x4;
  (*g_GraphicsFramebufferBeginAccess)();
  if (bVar10) {
    return;
  }
  if ((control->nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((uint)control[1].nextSibling & 4) == 0) {
      dVar3 = 0x4a;
    }
    else {
      dVar3 = 0x94;
    }
    if (((uint)control[1].nextSibling & 2) != 0) {
      dVar3 = dVar3 + 8;
    }
  }
  else {
    if (((uint)control[1].nextSibling & 0x400) != 0)
    goto UiWindowControl_DrawFramedTextAndChrome_EndFramebufferAccess;
    if (((uint)control[1].nextSibling & 4) == 0) {
      dVar3 = 0x8c;
    }
    else {
      dVar3 = 0xa4;
    }
  }
  qVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(dVar3,g_UiWindowTextureSource);
  iVar4 = (int)qVar11;
  iVar9 = control->layoutWidth - iVar4;
  iVar7 = control->layoutHeight - (int)(qVar11 >> 0x20);
  (*g_GraphicsTextureSourceBlitSourceAlpha)
            (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,arg6,
             g_UiWindowTextureSource,g_FramebufferAccess);
  (*g_GraphicsTextureSourceBlitSourceAlpha)
            (clipTop,clipLeft,clipBottom,clipRight,control->top,iVar9 + control->left,
             extraout_ECX + 1,g_UiWindowTextureSource,g_FramebufferAccess);
  (*g_GraphicsTextureSourceBlitSourceAlpha)
            (clipTop,clipLeft,clipBottom,clipRight,iVar7 + control->top,control->left,
             extraout_ECX_00 + 1,g_UiWindowTextureSource,g_FramebufferAccess);
  (*g_GraphicsTextureSourceBlitSourceAlpha)
            (clipTop,clipLeft,clipBottom,clipRight,iVar7 + control->top,iVar9 + control->left,
             extraout_ECX_01 + 1,g_UiWindowTextureSource,g_FramebufferAccess);
  UiWindow_BlitTiledHorizontalEdge
            (clipTop,clipLeft,clipBottom,clipRight,extraout_ECX_02 + 1,iVar9,0,iVar4,control);
  UiWindow_BlitTiledVerticalEdge
            (clipTop,clipLeft,clipBottom,clipRight,extraout_ECX_03 + 1,iVar7,tileStart,0,control);
  UiWindow_BlitTiledVerticalEdge
            (clipTop,clipLeft,clipBottom,clipRight,extraout_ECX_04 + 1,iVar7,tileStart_00,iVar9,
             control);
  UiWindow_BlitTiledHorizontalEdge
            (clipTop,clipLeft,clipBottom,clipRight,extraout_ECX_05 + 1,iVar9,iVar7,tileEnd,control);
  commandStream = TextResource_Resolve(control[1].top);
  RVar12 = RichTextCommandStream_MeasureRegs(control[1].right,commandStream);
  dVar3 = RVar12.widthPixels;
  iVar7 = (int)(control->layoutHeight - RVar12.heightPixels) >> 1;
  iVar4 = ((int)(((uint)(extraout_ECX_06 * 3) >> 2) - dVar3) >> 1) +
          ((uint)control->layoutWidth >> 2);
  uVar8 = g_UiTextStyleDisabled;
  if (((control->nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (uVar8 = g_UiTextStyleNormal, ((uint)control[1].nextSibling & 2) != 0)) {
    uVar8 = g_UiTextStyleSelected;
  }
  if (((uint)control[1].nextSibling & 0x100) == 0) {
    control[1].right = control[1].right & 0xff0000;
  }
  else {
    uVar8 = uVar8 & 0xffffff;
  }
  if (((uint)control[1].nextSibling & 0x200) == 0) {
    control[1].right = control[1].right & 0xff000000;
  }
  else {
    uVar8 = uVar8 & 0xff00ffff;
  }
  uVar8 = uVar8 | control[1].right;
  if ((control->nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) == 0) {
    RichTextCommandStream_DrawSingleLine
              (clipTop,clipLeft,clipBottom,clipRight,uVar8,commandStream,iVar7 + control->top,
               iVar4 + control->left);
  }
  else {
    iVar4 = iVar4 + control->left;
    iVar7 = iVar7 + control->top;
    iVar9 = (int)(uVar8 << 0x10) >> 0x18;
    qVar11 = (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                       (clipTop,clipLeft,clipBottom,clipRight,iVar9 + -1 + iVar7,iVar9 + -3 + iVar4,
                        0x7f000000,0x86,g_UiWindowTextureSource,g_FramebufferAccess);
    sVar5 = (sdword)(qVar11 >> 0x20);
    qVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(0x88,g_UiWindowTextureSource);
    (*g_GraphicsTextureSourceGetLogicalSize)(0x86,g_UiWindowTextureSource);
    (*g_GraphicsTextureSourceGetLogicalSize)(0x87,g_UiWindowTextureSource);
    iVar9 = ((extraout_ECX_07 + 6) - (int)qVar11) + dVar3;
    qVar11 = (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                       (clipTop,clipLeft,clipBottom,clipRight,sVar5,iVar9,0x7f000000,0x88,
                        g_UiWindowTextureSource,g_FramebufferAccess);
    sVar6 = (sdword)(qVar11 >> 0x20);
    sVar5 = extraout_ECX_08;
    if (clipLeft < iVar9) {
      iVar9 = clipLeft;
    }
    do {
      qVar11 = (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                         (clipTop,iVar9,clipBottom,clipRight,sVar6,sVar5,0x7f000000,0x87,
                          g_UiWindowTextureSource,g_FramebufferAccess);
      sVar6 = (sdword)(qVar11 >> 0x20);
      sVar5 = extraout_ECX_09 + (int)qVar11;
    } while (sVar5 < iVar9);
    uVar13 = RichTextCommandStream_DrawSingleLine
                       (clipTop,clipLeft,clipBottom,clipRight,uVar8,commandStream,iVar7,iVar4);
    qVar11 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                       (clipTop,clipLeft,clipBottom,clipRight,(sdword)((ulonglong)uVar13 >> 0x20),
                        arg5,0x86,g_UiWindowTextureSource,g_FramebufferAccess);
    sVar6 = (sdword)(qVar11 >> 0x20);
    qVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(0x88,g_UiWindowTextureSource);
    (*g_GraphicsTextureSourceGetLogicalSize)(0x86,g_UiWindowTextureSource);
    (*g_GraphicsTextureSourceGetLogicalSize)(0x87,g_UiWindowTextureSource);
    iVar4 = ((extraout_ECX_10 + 6) - (int)qVar11) + dVar3;
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,sVar6,iVar4,0x88,g_UiWindowTextureSource,
               g_FramebufferAccess);
    sVar5 = extraout_ECX_11;
    if (clipLeft < iVar4) {
      iVar4 = clipLeft;
    }
    do {
      qVar11 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                         (clipTop,iVar4,clipBottom,clipRight,sVar6,sVar5,0x87,
                          g_UiWindowTextureSource,g_FramebufferAccess);
      sVar5 = extraout_ECX_12 + (int)qVar11;
    } while (sVar5 < iVar4);
  }
  qVar11 = (*g_GraphicsTextureSourceGetLogicalSize)
                     ((dword)control[1].vtable,(GraphicsTextureSourceAsset *)control[1].parent);
  iVar9 = (((uint)control->layoutWidth >> 2) - (int)qVar11) + control->left;
  arg4 = (control->layoutHeight - (int)(qVar11 >> 0x20) >> 1) + control->top;
  iVar4 = iVar9;
  iVar7 = arg4;
  if ((control->nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((uint)control[1].nextSibling & 2) == 0) {
      cVar1 = (char)control[1].left;
      cVar2 = *(char *)((int)&control[1].left + 1);
    }
    else {
      cVar1 = *(char *)((int)&control[1].left + 2);
      cVar2 = *(char *)((int)&control[1].left + 3);
    }
    iVar4 = iVar9 + cVar1;
    iVar7 = arg4 + cVar2;
  }
  (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
            (clipTop,clipLeft,clipBottom,clipRight,iVar7,iVar4,0x7f000000,(dword)control[1].vtable,
             (GraphicsTextureSourceAsset *)control[1].parent,g_FramebufferAccess);
  (*g_GraphicsTextureSourceBlitSourceAlpha)
            (clipTop,clipLeft,clipBottom,clipRight,arg4,iVar9,(dword)control[1].vtable,
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
void UiTextButtonControl_Relocate
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
void UiTextButtonControl_NonRightPress
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiTextButtonControl *control)

{
  UiSelectableStateFlags *pUVar1;
  bool opaquePixelHit;
  UiSelectableStateFlags *toggleStateFlagsField;
  UiSelectableStateFlags *stateFlagsField;
  
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    opaquePixelHit = false;
    if (((control->selectable).stateFlags & UI_SELECTABLE_TOGGLE_ON_ACTIVATION) == 0) {
      (*g_GraphicsTextureSourceTestOpaquePixel)
                (pointerY,pointerX,(control->selectable).base.top,(control->selectable).base.left,
                 0x46,g_UiWindowTextureSource);
      if (opaquePixelHit) {
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
      (*g_GraphicsTextureSourceTestOpaquePixel)
                (pointerY,pointerX,(control->selectable).base.top,(control->selectable).base.left,
                 0x40,g_UiWindowTextureSource);
      if (opaquePixelHit) {
        if ((((control->selectable).stateFlags & 0x80) != 0) && (control->activationSoundId != 0)) {
          (*g_SoundPlayOneShot)
                    (g_UiSoundGainQ15,g_UiSoundGainQ15,
                     (DirectSoundVoiceSet *)control->activationSoundId);
        }
        toggleStateFlagsField = &(control->selectable).stateFlags;
        *toggleStateFlagsField = *toggleStateFlagsField ^ UI_SELECTABLE_SELECTED_OR_CHECKED;
        pUVar1 = &(control->selectable).stateFlags;
        *pUVar1 = *pUVar1 & 0xffffffbf;
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
void UiTextButtonControl_KeyboardEventCf
               (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
               UiTextButtonControl *control)

{
  UiSelectableStateFlags *pUVar1;
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
      return;
    }
    if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
      if ((((control->selectable).stateFlags & 0x80) != 0) && (control->activationSoundId != 0)) {
        (*g_SoundPlayOneShot)
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   (DirectSoundVoiceSet *)control->activationSoundId);
      }
      pUVar1 = &(control->selectable).stateFlags;
      *pUVar1 = *pUVar1 | UI_SELECTABLE_SELECTED_OR_CHECKED;
      UiActionQueue_Enqueue((control->selectable).actionId,control);
      UiNode_InvalidateRoot((UiNodeBase *)control);
      return;
    }
  }
  UiNode_DefaultKeyboardEventMoveFocusNextCf(keyboardStateMask,keyCode,(UiNodeBase *)control);
  return;
}

/* Address: 0x004B37C0.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B3770[2]@004B3770.
   Cross-module calls: UiContainer_DrawIntersectingChildren [ui/controls/layout].
*/
void UiImagePanelControl_DrawAlignedTextureAndChildren
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiNodeBase *control)

{
  int arg1;
  int arg3;
  int extraout_ECX;
  sdword extraout_ECX_00;
  int arg0;
  int iVar1;
  uint uVar2;
  int arg2;
  uint uVar3;
  uint arg4;
  bool bVar4;
  qword qVar5;
  
  if ((control->nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    arg3 = control->left;
    if (control->left < clipRight) {
      arg3 = clipRight;
    }
    arg2 = control->top;
    if (control->top < clipBottom) {
      arg2 = clipBottom;
    }
    arg1 = control->right;
    if (clipLeft < control->right) {
      arg1 = clipLeft;
    }
    arg0 = control->bottom;
    if (clipTop < control->bottom) {
      arg0 = clipTop;
    }
    uVar3 = control->top;
    if ((GraphicsTextureSourceAsset *)control[1].parent != (GraphicsTextureSourceAsset *)0x0) {
      qVar5 = (*g_GraphicsTextureSourceGetLogicalSize)
                        ((dword)control[1].vtable,(GraphicsTextureSourceAsset *)control[1].parent);
      iVar1 = control->layoutHeight - (int)(qVar5 >> 0x20);
      if (((uint)control[1].nextSibling & 8) != 0) {
        uVar3 = uVar3 + iVar1;
      }
      uVar2 = iVar1 >> 1;
      bVar4 = ((uint)control[1].nextSibling & 4) != 0;
      arg4 = uVar3;
      if (bVar4) {
        arg4 = uVar3 + uVar2;
      }
      bVar4 = bVar4 && CARRY4(uVar3,uVar2);
      (*g_GraphicsFramebufferBeginAccess)();
      if (!bVar4) {
        iVar1 = extraout_ECX;
        if (((uint)control[1].nextSibling & 0x10) != 0) {
          (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                    (arg0,arg1,arg2,arg3,(int)*(char *)((int)&control[1].firstChild + 1) + arg4,
                     *(char *)&control[1].firstChild + extraout_ECX,0x7f000000,
                     (dword)control[1].vtable,(GraphicsTextureSourceAsset *)control[1].parent,
                     g_FramebufferAccess);
          iVar1 = extraout_ECX_00;
        }
        if (((uint)control[1].nextSibling & 0x80) == 0) {
          (*g_GraphicsTextureSourceBlitSourceAlpha)
                    (arg0,arg1,arg2,arg3,arg4,iVar1,(dword)control[1].vtable,
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
UiNodeBase *
UiImagePanelControl_HitTestAlignedTextureAndChildren(int pointerY,int pointerX,UiNodeBase *control)

{
  bool bVar1;
  UiNodeBase *pUVar2;
  int iVar3;
  int extraout_ECX;
  int arg3;
  int iVar4;
  int arg2;
  bool bVar5;
  qword qVar6;
  
  pUVar2 = (UiNodeBase *)0xffffffff;
  bVar1 = false;
  if (((uint)control[1].nextSibling & 0x20) != 0) {
    return (UiNodeBase *)0xffffffff;
  }
  if ((control->nodeFlags & UI_NODE_ALLOW_CHILD_HIT_TEST_OUTSIDE_BOUNDS) != 0)
  goto UiImagePanelHitTest_CheckChildren;
  do {
    if (((uint)control[1].nextSibling & 0x40) == 0) {
      arg2 = control->top;
      if ((GraphicsTextureSourceAsset *)control[1].parent == (GraphicsTextureSourceAsset *)0x0) {
        return pUVar2;
      }
      qVar6 = (*g_GraphicsTextureSourceGetLogicalSize)
                        ((dword)control[1].vtable,(GraphicsTextureSourceAsset *)control[1].parent);
      iVar3 = control->layoutWidth - (int)qVar6;
      iVar4 = control->layoutHeight - (int)(qVar6 >> 0x20);
      arg3 = extraout_ECX;
      if (((uint)control[1].nextSibling & 2) != 0) {
        arg3 = extraout_ECX + iVar3;
      }
      if (((uint)control[1].nextSibling & 8) != 0) {
        arg2 = arg2 + iVar4;
      }
      if (((uint)control[1].nextSibling & 1) != 0) {
        arg3 = arg3 + (iVar3 >> 1);
      }
      if (((uint)control[1].nextSibling & 4) != 0) {
        arg2 = arg2 + (iVar4 >> 1);
      }
      bVar5 = false;
      pUVar2 = (UiNodeBase *)
               (*g_GraphicsTextureSourceTestOpaquePixel)
                         (pointerY,pointerX,arg2,arg3,(dword)control[1].vtable,
                          (GraphicsTextureSourceAsset *)control[1].parent);
      if (!bVar5) {
        return pUVar2;
      }
    }
UiImagePanelHitTest_CheckChildren:
    pUVar2 = UiContainer_HitTestChildren(pointerY,pointerX,control);
    if (bVar1) {
      return pUVar2;
    }
    bVar1 = true;
  } while (pUVar2 == control);
  return pUVar2;
}

/* Address: 0x004B3AA0.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B3A50[2]@004B3A50.
   Cross-module calls: UiContainer_DrawIntersectingChildren [ui/controls/layout].
*/
void UiFillPanelControl_DrawColorOrTiledTextureAndChildren
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiNodeBase *control)

{
  uint uVar1;
  int extraout_ECX;
  sdword extraout_ECX_00;
  int extraout_ECX_01;
  int arg5;
  sdword arg7;
  sdword arg6;
  bool bVar2;
  undefined1 uVar3;
  undefined8 uVar4;
  
  uVar1 = control->bottom;
  arg6 = control->top;
  bVar2 = false;
  if (control[1].parent == (UiNodeBase *)0x0) {
    uVar4 = (*g_GraphicsFramebufferBeginAccess)();
    if (!bVar2) {
      (*g_GraphicsFramebufferFillRectArgb)
                (clipTop,clipLeft,clipBottom,clipRight,(sdword)((ulonglong)uVar4 >> 0x20),
                 (sdword)uVar4,arg6,arg7,(dword)control[1].vtable,g_FramebufferAccess);
      (*g_GraphicsFramebufferEndAccess)();
    }
  }
  else {
    if (control->right < clipLeft) {
      clipLeft = control->right;
    }
    uVar3 = uVar1 < (uint)clipTop;
    if ((int)uVar1 < clipTop) {
      clipTop = uVar1;
    }
    (*g_GraphicsTextureSourceGetLogicalSize)
              ((dword)control[1].vtable,(GraphicsTextureSourceAsset *)control[1].parent);
    uVar4 = (*g_GraphicsFramebufferBeginAccess)();
    arg5 = extraout_ECX;
    if (!(bool)uVar3) {
UiFillPanelControl_DrawColorOrTiledTextureAndChildren_BlitNextTextureTile:
      do {
        if (((uint)control[1].nextSibling & 0x10) != 0) {
          (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                    (clipTop,clipLeft,clipBottom,clipRight,
                     *(char *)((int)&control[1].firstChild + 1) + arg6,
                     *(char *)&control[1].firstChild + arg5,0x7f000000,(dword)control[1].vtable,
                     (GraphicsTextureSourceAsset *)control[1].parent,g_FramebufferAccess);
          arg5 = extraout_ECX_00;
        }
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,arg6,arg5,(dword)control[1].vtable,
                   (GraphicsTextureSourceAsset *)control[1].parent,g_FramebufferAccess);
        arg5 = extraout_ECX_01;
        if (((uint)control[1].nextSibling & 1) != 0) {
          arg5 = extraout_ECX_01 + (int)uVar4;
          if (arg5 < clipLeft)
          goto UiFillPanelControl_DrawColorOrTiledTextureAndChildren_BlitNextTextureTile;
          arg5 = control->left;
        }
        if ((((uint)control[1].nextSibling & 2) == 0) ||
           (arg6 = arg6 + (int)((ulonglong)uVar4 >> 0x20), clipTop <= arg6))
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
void UiTextEditControl_EndSelection(void)

{
  int in_stack_00000010;
  
  *(uint *)(in_stack_00000010 + 0x4c) = *(uint *)(in_stack_00000010 + 0x4c) & 0xffffffbf;
  if (((*(uint *)(in_stack_00000010 + 0x4c) & 0x400) != 0) &&
     (*(int *)(in_stack_00000010 + 0x68) != 0)) {
    (*g_SoundPlayOneShot)
              (g_UiSoundGainQ15,g_UiSoundGainQ15,*(DirectSoundVoiceSet **)(in_stack_00000010 + 0x68)
              );
  }
  return;
}

/* Address: 0x004B6480.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B58A0[14]@004B58A0; g_UiNodeVtable_004B6800[14]@004B6800;
   g_UiNodeVtable_004B7050[14]@004B7050.
   Cross-module calls: UiKeyboardFocus_ReleaseNode [ui/controls/input], UiNode_InvalidateRoot [ui/core/runtime].
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
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B58A0[15]@004B58A0; g_UiNodeVtable_004B6800[15]@004B6800;
   g_UiNodeVtable_004B7050[15]@004B7050.
   Cross-module calls: UiKeyboardFocus_AcquireIfNone [ui/controls/input], UiNode_InvalidateRoot [ui/core/runtime].
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
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B58A0[16]@004B58A0; g_UiNodeVtable_004B6800[16]@004B6800;
   g_UiNodeVtable_004B7050[16]@004B7050.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiTextEditControl_TickCaretBlink(UiTextEditControl *control)

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
void UiSingleLineTextControl_DrawClipped
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiNodeBase *control)

{
  UiNodeBase *pUVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  uint uVar2;
  UiPackedTextStyle packedStyle;
  UiPackedTextStyle extraout_EDX;
  UiNodeBase *extraout_EDX_00;
  int extraout_EDX_01;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  bool bVar7;
  qword qVar8;
  undefined8 uVar9;
  UiPixelCoordinate arg1;
  UiPixelCoordinate UVar10;
  
  arg1 = clipLeft;
  uVar2 = g_UiTextStyleNormal;
  iVar5 = 0;
  iVar3 = 0;
  if (((control->nodeFlags & UI_NODE_SUPPRESSED) == 0) ||
     (((uint)control[1].nextSibling & 0x40) == 0)) {
    if (((uint)control[1].nextSibling & 0x100) == 0) {
      control[1].vtable = (UiNodeVtable *)((uint)control[1].vtable & 0xffffff);
    }
    else {
      uVar2 = g_UiTextStyleNormal & 0xffffff;
    }
    if (((uint)control[1].nextSibling & 0x200) == 0) {
      control[1].vtable = (UiNodeVtable *)((uint)control[1].vtable & 0xff00ffff);
    }
    else {
      uVar2 = uVar2 & 0xff00ffff;
    }
    packedStyle = (uVar2 | (uint)control[1].vtable) & 0xffff0000;
    pUVar1 = control[1].parent;
    if (((uint)control[1].nextSibling & 0x10) == 0) {
      pUVar1 = (UiNodeBase *)TextResource_Resolve((TextResourceId)pUVar1);
      packedStyle = extraout_EDX;
    }
    RichTextCommandStream_MeasureRegs(packedStyle,(word *)pUVar1);
    qVar8 = (*g_GraphicsTextureSourceGetLogicalSize)(0x86,g_UiWindowTextureSource);
    iVar6 = (int)(qVar8 >> 0x20);
    iVar4 = extraout_ECX;
    if (control[1].firstChild != (UiNodeBase *)0x0) {
      iVar4 = extraout_ECX + (int)qVar8 * 2;
    }
    if (((uint)control[1].nextSibling & 2) != 0) {
      iVar5 = control->layoutWidth - iVar4;
    }
    if (((uint)control[1].nextSibling & 8) != 0) {
      iVar3 = control->layoutHeight - iVar6;
    }
    if (((uint)control[1].nextSibling & 4) != 0) {
      iVar3 = control->layoutHeight - iVar6 >> 1;
    }
    if (((uint)control[1].nextSibling & 1) != 0) {
      iVar5 = control->layoutWidth - iVar4 >> 1;
    }
    bVar7 = iVar4 == 0;
    uVar9 = (*g_GraphicsFramebufferBeginAccess)();
    pUVar1 = (UiNodeBase *)((ulonglong)uVar9 >> 0x20);
    if (!bVar7) {
      UVar10 = clipLeft;
      if ((control->nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) != 0) {
        iVar6 = iVar5 + (int)uVar9 + control->left;
        iVar4 = iVar3 + (int)uVar9 + control->top;
        qVar8 = (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                          (clipTop,clipLeft,clipBottom,clipRight,iVar4,iVar6,0x7f000000,0x86,
                           g_UiWindowTextureSource,g_FramebufferAccess);
        qVar8 = (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                          (clipTop,clipLeft,clipBottom,clipRight,iVar4,
                           extraout_ECX_00 - (int)(qVar8 >> 0x20),0x7f000000,0x88,
                           g_UiWindowTextureSource,g_FramebufferAccess);
        pUVar1 = (UiNodeBase *)(qVar8 >> 0x20);
        iVar6 = (int)&pUVar1->nextSibling + iVar6;
        if (extraout_ECX_01 <= clipLeft) {
          clipLeft = extraout_ECX_01;
        }
        (*g_GraphicsTextureSourceGetLogicalSize)(0x87,g_UiWindowTextureSource);
        do {
          qVar8 = (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                            (clipTop,clipLeft,clipBottom,clipRight,iVar4,iVar6,0x7f000000,0x87,
                             g_UiWindowTextureSource,g_FramebufferAccess);
          iVar6 = iVar6 + (int)qVar8;
        } while (iVar6 < clipLeft);
        iVar6 = iVar5 + control->left;
        iVar4 = iVar3 + control->top;
        UVar10 = arg1;
        qVar8 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                          (clipTop,arg1,clipBottom,clipRight,iVar4,iVar6,0x86,
                           g_UiWindowTextureSource,g_FramebufferAccess);
        qVar8 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                          (clipTop,arg1,clipBottom,clipRight,iVar4,
                           extraout_ECX_02 - (int)(qVar8 >> 0x20),0x88,g_UiWindowTextureSource,
                           g_FramebufferAccess);
        iVar6 = iVar6 + (int)(qVar8 >> 0x20);
        clipLeft = arg1;
        if (extraout_ECX_03 <= arg1) {
          clipLeft = extraout_ECX_03;
        }
        (*g_GraphicsTextureSourceGetLogicalSize)(0x87,g_UiWindowTextureSource);
        do {
          qVar8 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                            (clipTop,clipLeft,clipBottom,clipRight,iVar4,iVar6,0x87,
                             g_UiWindowTextureSource,g_FramebufferAccess);
          iVar6 = iVar6 + (int)qVar8;
        } while (iVar6 < clipLeft);
      }
      clipLeft = UVar10;
      if (control[1].firstChild != (UiNodeBase *)0x0) {
        iVar5 = (int)&pUVar1->nextSibling + iVar5;
        iVar3 = iVar3 + 1;
      }
      pUVar1 = control[1].parent;
      if (((uint)control[1].nextSibling & 0x10) == 0) {
        pUVar1 = (UiNodeBase *)TextResource_Resolve((TextResourceId)pUVar1);
      }
      uVar2 = g_UiTextStyleNormal;
      if ((control[1].firstChild != (UiNodeBase *)0x0) &&
         (((control[1].firstChild)->nodeFlags & UI_NODE_SUPPRESSED) != 0)) {
        uVar2 = g_UiTextStyleDisabled;
      }
      if (((uint)control[1].nextSibling & 0x100) == 0) {
        control[1].vtable = (UiNodeVtable *)((uint)control[1].vtable & 0xffffff);
      }
      else {
        uVar2 = uVar2 & 0xffffff;
      }
      if (((uint)control[1].nextSibling & 0x200) == 0) {
        control[1].vtable = (UiNodeVtable *)((uint)control[1].vtable & 0xff00ffff);
      }
      else {
        uVar2 = uVar2 & 0xff00ffff;
      }
      control[1].vtable = (UiNodeVtable *)((uint)control[1].vtable & 0xffff0000);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,uVar2 | (uint)control[1].vtable,
                 (word *)pUVar1,iVar3 + control->top,iVar5 + control->left);
      (*g_GraphicsFramebufferEndAccess)();
      pUVar1 = extraout_EDX_00;
    }
    if (control == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = pUVar1;
      pUVar1->nodeFlags = pUVar1->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
      UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,control);
      *(uint *)(extraout_EDX_01 + 0x48) = *(uint *)(extraout_EDX_01 + 0x48) & 0xfffffffb;
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
void UiTextListControl_DrawRowsAndSelection
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiTextListControl *control)

{
  int iVar1;
  uint uVar2;
  int extraout_ECX;
  UiPixelCoordinate edgeY;
  UiPixelCoordinate edgeY_00;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  word **ppwVar3;
  word **ppwVar4;
  bool bVar5;
  RichTextExtentRegs RVar6;
  qword qVar7;
  
  if (control->rowCount != 0) {
    iVar1 = (clipBottom - (control->base).top) / (int)control->rowHeight;
    if (iVar1 < 0) {
      iVar1 = 0;
    }
    ppwVar4 = control->rowTextSlots + iVar1;
    uVar2 = (int)((clipTop - (control->base).top) + control->rowHeight) / (int)control->rowHeight;
    if (control->rowCount <= uVar2) {
      uVar2 = control->rowCount - 1;
    }
    ppwVar3 = control->rowTextSlots + uVar2;
    bVar5 = ppwVar4 < ppwVar3;
    if (ppwVar4 <= ppwVar3) {
      (*g_GraphicsFramebufferBeginAccess)();
      iVar1 = extraout_ECX;
      if (!bVar5) {
        do {
          if (ppwVar4 == control->selectedRowSlot) {
            RVar6 = RichTextCommandStream_MeasureRegs(g_UiListTextStyle,*ppwVar4);
            iVar1 = RVar6.widthPixels + 6;
            if (((control->base).nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) == 0) {
              UiWindow_BlitTiledHorizontalEdge
                        (clipTop,clipLeft,clipBottom,clipRight,0x82,iVar1,edgeY,0,control);
              iVar1 = extraout_ECX_03;
            }
            else {
              qVar7 = (*g_GraphicsTextureSourceGetLogicalSize)(0x83,g_UiWindowTextureSource);
              iVar1 = iVar1 - (int)qVar7;
              UiWindow_BlitTiledHorizontalEdge
                        (clipTop,clipLeft,clipBottom,clipRight,0x84,iVar1,edgeY_00,(int)qVar7,
                         control);
              (*g_GraphicsTextureSourceBlitSourceAlpha)
                        (clipTop,clipLeft,clipBottom,clipRight,extraout_ECX_00 + (control->base).top
                         ,(control->base).left,0x83,g_UiWindowTextureSource,g_FramebufferAccess);
              (*g_GraphicsTextureSourceBlitSourceAlpha)
                        (clipTop,clipLeft,clipBottom,clipRight,extraout_ECX_01 + (control->base).top
                         ,iVar1 + (control->base).left,0x85,g_UiWindowTextureSource,
                         g_FramebufferAccess);
              iVar1 = extraout_ECX_02;
            }
          }
          RichTextCommandStream_DrawSingleLine
                    (clipTop,clipLeft,clipBottom,clipRight,g_UiListTextStyle,*ppwVar4,
                     iVar1 + 1 + (control->base).top,(control->base).left + 3);
          ppwVar4 = ppwVar4 + 1;
          iVar1 = extraout_ECX_04 + -1 + control->rowHeight;
        } while (ppwVar4 <= ppwVar3);
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
void UiTextListControl_SelectRowFromPointer
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiTextListControl *control)

{
  sdword *psVar1;
  uint uVar2;
  int clipBottom;
  int extraout_ECX;
  RichTextExtentRegs RVar3;
  word **ppwVar4;
  
  psVar1 = &(control->base).top;
  if (((*psVar1 <= pointerY) && ((control->base).left <= pointerX)) &&
     (uVar2 = (uint)(pointerY - *psVar1) / control->rowHeight, uVar2 < control->rowCount)) {
    ppwVar4 = control->rowTextSlots + uVar2;
    RVar3 = RichTextCommandStream_MeasureRegs(g_UiListTextStyle,*ppwVar4);
    if (extraout_ECX < (int)(RVar3.widthPixels + 6)) {
      control->listStateFlags = control->listStateFlags | UI_TEXT_LIST_SELECTION_CONFIRMED;
      if ((((control->base).nodeFlags & UI_NODE_REPEAT_OR_DOUBLE_CLICK) != 0) ||
         (control->listStateFlags = control->listStateFlags & ~UI_TEXT_LIST_SELECTION_CONFIRMED,
         ppwVar4 != control->selectedRowSlot)) {
        control->selectedRowSlot = ppwVar4;
        clipBottom = uVar2 * control->rowHeight;
        UiScrollableControl_ClampOffsetsToViewport
                  (clipBottom + 1 + control->rowHeight,(control->base).rightOffset,clipBottom,0,
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
void UiTextListControl_HandleKeyboardNavigationAndSearchCf
               (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
               UiTextListControl *control)

{
  word **ppwVar1;
  word **ppwVar2;
  int iVar3;
  uint uVar4;
  dword extraout_ECX;
  int extraout_EDX;
  word **ppwVar5;
  bool bVar6;
  UiScrollableContentDimensionsEdxEax8 UVar7;
  
  ppwVar1 = control->selectedRowSlot;
  if ((keyCode & 0xffff0000) == 0) {
    if (((keyboardStateMask & 0x3c) != 0) ||
       ((control->listStateFlags & UI_TEXT_LIST_TYPE_SEARCH_ENABLED) == 0)) {
UiTextListControl_DelegateUnhandledKeyboardEvent:
      UiNode_DefaultKeyboardEventMoveFocusNextCf(keyboardStateMask,keyCode,&control->base);
      return;
    }
    bVar6 = false;
    ppwVar2 = control->rowTextSlots;
    if (control->rowCount != 0) {
      do {
        ppwVar5 = ppwVar2;
        (*g_KeyboardAsciiCaseTransformCallbacks3.compareCaseInsensitiveFlags)
                  (keyCode,*(dword *)*ppwVar5);
        if (!bVar6) break;
        bVar6 = (word **)0xfffffffb < ppwVar5;
        keyCode = extraout_ECX;
        ppwVar2 = ppwVar5 + 1;
      } while (extraout_EDX != 1);
      control->selectedRowSlot = ppwVar5;
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
    UVar7 = UiScrollableControl_QueryContentSizeRegs((UiScrollableControl *)(control->base).parent);
    iVar3 = ((uint)((int)control->selectedRowSlot - (int)control->rowTextSlots) >> 2) -
            ((int)((UVar7 >> 0x20) / (ulonglong)control->rowHeight) + -1);
    if (iVar3 < 0) {
      iVar3 = 0;
    }
    control->selectedRowSlot = control->rowTextSlots + iVar3;
  }
  else if (keyCode == 0x1001a) {
    UVar7 = UiScrollableControl_QueryContentSizeRegs((UiScrollableControl *)(control->base).parent);
    uVar4 = ((uint)((int)control->selectedRowSlot - (int)control->rowTextSlots) >> 2) +
            (int)((UVar7 >> 0x20) / (ulonglong)control->rowHeight) + -1;
    if (control->rowCount <= uVar4) {
      uVar4 = control->rowCount - 1;
    }
    control->selectedRowSlot = control->rowTextSlots + uVar4;
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
  ppwVar2 = control->selectedRowSlot;
  if (ppwVar2 != ppwVar1) {
    if (((control->listStateFlags & UI_TEXT_LIST_PLAY_SELECTION_SOUND) != 0) &&
       (control->activationSound != (DirectSoundVoiceSet *)0x0)) {
      (*g_SoundPlayOneShot)(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
    }
    iVar3 = ((uint)((int)ppwVar2 - (int)control->rowTextSlots) >> 2) * control->rowHeight;
    UiScrollableControl_ClampOffsetsToViewport
              (iVar3 + control->rowHeight + 1,(control->base).rightOffset,iVar3,0,
               (UiScrollableControl *)(control->base).parent);
    iVar3 = g_UiListActivationPulseFrames;
    control->listStateFlags = control->listStateFlags | UI_TEXT_LIST_DEFERRED_ACTION_PENDING;
    control->listStateFlags = control->listStateFlags & 0xffffff;
    control->listStateFlags = control->listStateFlags | iVar3 << 0x18;
  }
  return;
}

/* Address: 0x004BA390.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9E40[16]@004B9E40.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime].
*/
void UiTextListControl_TickActivationPulse(UiTextListControl *control)

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
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9E40[14]@004B9E40.
   Cross-module calls: UiContainer_SuppressActionId [ui/controls/layout].
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
   Ownership: ui/controls/text.
   Purpose: Initializes a pointer-list control from rich-text row pointers, computes line height and maximum
   measured row width, clears offsets, and requests parent layout.
   Cross-module calls: FontGlyph_GetLogicalSizeForStyleRegs [assets/text/resources],
   RichTextCommandStream_MeasureRegs [assets/text/richtext].
*/
void UiPointerList_InitializeMeasuredTextRows
               (UiListRowCount rowCount,void **rowPointers,UiPointerListControl *control)

{
  UiNodeBase *control_00;
  int extraout_ECX;
  int extraout_EDX;
  uint maximumTextWidthPixels;
  RichTextExtentRegs measuredTextExtent;
  UiNodeVtable *parentVtable;
  
  FontGlyph_GetLogicalSizeForStyleRegs(g_UiListTextStyle,0);
  control->rowHeight = extraout_EDX + 1U;
  control->rowCount = rowCount;
  control->rowSlots = rowPointers;
  maximumTextWidthPixels = 0;
  control->selectedRowSlot = rowPointers;
  (control->base).bottomOffset = (extraout_EDX + 1U) * rowCount + 1;
  if (rowCount != 0) {
    do {
      measuredTextExtent = RichTextCommandStream_MeasureRegs(g_UiListTextStyle,*rowPointers);
      if (maximumTextWidthPixels < measuredTextExtent.widthPixels) {
        maximumTextWidthPixels = measuredTextExtent.widthPixels;
      }
      rowPointers = rowPointers + 1;
    } while (extraout_ECX != 1);
  }
  control_00 = (control->base).parent;
  parentVtable = control_00->vtable;
  (control->base).rightOffset = maximumTextWidthPixels + 6;
  (control->base).leftOffset = 0;
  (control->base).topOffset = 0;
  (*parentVtable->layout)(control_00);
  return;
}

/* Address: 0x004BC490.
   Ownership: ui/controls/text.
   Purpose: Draws a wrapped rich-text control inside a clip rectangle, resolving either a localized resource or
   direct command stream, then draws intersecting children.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_DrawWrappedBlockCf
   [assets/text/richtext], UiContainer_DrawIntersectingChildren [ui/controls/layout].
*/
void UiWrappedTextControl_DrawClipped
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiNodeBase *control)

{
  UiNodeBase *commandStream;
  uint uVar1;
  dword packedStyle;
  dword extraout_EDX;
  undefined1 in_CF;
  
  (*g_GraphicsFramebufferBeginAccess)();
  if (!(bool)in_CF) {
    if (((uint)control[1].nextSibling & 0x40) == 0) {
      control[1].firstChild = (UiNodeBase *)control->layoutWidth;
    }
    uVar1 = g_UiTextStyleNormal;
    if (((uint)control[1].nextSibling & 0x100) == 0) {
      control[1].vtable = (UiNodeVtable *)((uint)control[1].vtable & 0xffffff);
    }
    else {
      uVar1 = g_UiTextStyleNormal & 0xffffff;
    }
    if (((uint)control[1].nextSibling & 0x200) == 0) {
      control[1].vtable = (UiNodeVtable *)((uint)control[1].vtable & 0xff00ffff);
    }
    else {
      uVar1 = uVar1 & 0xff00ffff;
    }
    packedStyle = (uVar1 | (uint)control[1].vtable) & 0xffff0000;
    commandStream = control[1].parent;
    if (((uint)control[1].nextSibling & 0x10) == 0) {
      commandStream = (UiNodeBase *)TextResource_Resolve((TextResourceId)commandStream);
      packedStyle = extraout_EDX;
    }
    RichTextCommandStream_DrawWrappedBlockCf
              (clipTop,clipLeft,clipBottom,clipRight,packedStyle,(word *)commandStream,
               (UiPixelExtent)control[1].firstChild,control->top,control->left);
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
void UiNineSlicePanelControl_DrawTextureFrameAndChildren
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiNodeBase *control)

{
  int iVar1;
  sdword arg5;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  int extraout_ECX_06;
  int extraout_ECX_07;
  int extraout_ECX_08;
  int extraout_ECX_09;
  int extraout_ECX_10;
  int extraout_ECX_11;
  int extraout_ECX_12;
  int extraout_ECX_13;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  bool bVar7;
  qword qVar8;
  qword qVar9;
  qword qVar10;
  qword qVar11;
  qword qVar12;
  qword qVar13;
  qword qVar14;
  qword qVar15;
  undefined8 uVar16;
  
  bVar7 = false;
  if ((control->nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    (*g_GraphicsFramebufferBeginAccess)();
    if (!bVar7) {
      qVar8 = (*g_GraphicsTextureSourceGetLogicalSize)
                        ((dword)control[1].parent,
                         (GraphicsTextureSourceAsset *)control[1].firstChild);
      iVar6 = (int)(qVar8 >> 0x20);
      qVar9 = (*g_GraphicsTextureSourceGetLogicalSize)
                        (extraout_ECX + 1,(GraphicsTextureSourceAsset *)control[1].firstChild);
      qVar10 = (*g_GraphicsTextureSourceGetLogicalSize)
                         (extraout_ECX_00 + 1,(GraphicsTextureSourceAsset *)control[1].firstChild);
      qVar11 = (*g_GraphicsTextureSourceGetLogicalSize)
                         (extraout_ECX_01 + 1,(GraphicsTextureSourceAsset *)control[1].firstChild);
      qVar12 = (*g_GraphicsTextureSourceGetLogicalSize)
                         (extraout_ECX_02 + 1,(GraphicsTextureSourceAsset *)control[1].firstChild);
      iVar1 = (int)qVar12;
      qVar12 = (*g_GraphicsTextureSourceGetLogicalSize)
                         (extraout_ECX_03 + 1,(GraphicsTextureSourceAsset *)control[1].firstChild);
      iVar2 = (int)(qVar12 >> 0x20);
      qVar13 = (*g_GraphicsTextureSourceGetLogicalSize)
                         (extraout_ECX_04 + 1,(GraphicsTextureSourceAsset *)control[1].firstChild);
      iVar3 = (int)(qVar13 >> 0x20);
      qVar14 = (*g_GraphicsTextureSourceGetLogicalSize)
                         (extraout_ECX_05 + 1,(GraphicsTextureSourceAsset *)control[1].firstChild);
      iVar4 = control->bottom;
      iVar5 = control->right - (int)qVar9;
      qVar15 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                         (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,
                          extraout_ECX_06 - 7,(GraphicsTextureSourceAsset *)control[1].firstChild,
                          g_FramebufferAccess);
      qVar15 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                         (clipTop,clipLeft,clipBottom,clipRight,(sdword)(qVar15 >> 0x20),iVar5,
                          extraout_ECX_07 + 1,(GraphicsTextureSourceAsset *)control[1].firstChild,
                          g_FramebufferAccess);
      uVar16 = (*g_GraphicsTextureSourceBlitTiledSourceAlpha)
                         (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,iVar5,
                          (sdword)(qVar15 >> 0x20),(int)qVar15 + (int)qVar8,extraout_ECX_08 + 1,
                          (GraphicsTextureSourceAsset *)control[1].firstChild,g_FramebufferAccess);
      iVar4 = iVar4 - iVar2;
      arg5 = (*g_GraphicsTextureSourceBlitTiledSourceAlpha)
                       (clipTop,clipLeft,clipBottom,clipRight,iVar4,-0x80000000,
                        (int)((ulonglong)uVar16 >> 0x20) + iVar6,(int)uVar16 - (int)qVar8,
                        extraout_ECX_09 + 1,(GraphicsTextureSourceAsset *)control[1].firstChild,
                        g_FramebufferAccess);
      qVar8 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                        (clipTop,clipLeft,clipBottom,clipRight,iVar4,arg5,extraout_ECX_10 + 2,
                         (GraphicsTextureSourceAsset *)control[1].firstChild,g_FramebufferAccess);
      iVar5 = (iVar5 + (int)qVar9) - iVar1;
      iVar4 = (iVar4 + iVar2) - iVar3;
      (*g_GraphicsTextureSourceBlitTiledSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,iVar4,-0x80000000,
                 ((int)(qVar8 >> 0x20) - iVar6) + (int)(qVar9 >> 0x20),iVar5,extraout_ECX_11 - 1,
                 (GraphicsTextureSourceAsset *)control[1].firstChild,g_FramebufferAccess);
      iVar6 = (iVar5 + iVar1) - (int)qVar13;
      qVar8 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                        (clipTop,clipLeft,clipBottom,clipRight,iVar4,iVar6,extraout_ECX_12 + 2,
                         (GraphicsTextureSourceAsset *)control[1].firstChild,g_FramebufferAccess);
      iVar4 = (iVar4 + iVar3) - (int)(qVar14 >> 0x20);
      uVar16 = (*g_GraphicsTextureSourceBlitTiledSourceAlpha)
                         (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,iVar6,iVar4,
                          (int)qVar8 + (int)qVar12,extraout_ECX_13 + 1,
                          (GraphicsTextureSourceAsset *)control[1].firstChild,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitTiledSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,iVar4,(iVar6 + (int)qVar13) - iVar1,
                 (int)((ulonglong)uVar16 >> 0x20) + (int)(qVar10 >> 0x20),
                 ((int)uVar16 - (int)qVar12) + (int)qVar11,(dword)control[1].vtable,
                 (GraphicsTextureSourceAsset *)control[1].firstChild,g_FramebufferAccess);
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
void UiFormattedContainer_RelocateWithPatchedTextPayloads
               (UiSerializedRelocationDelta relocationDelta,UiNodeBase *control)

{
  word *stream;
  undefined8 patchedStreamCursorPair;
  
  if ((control->nodeFlags & UI_NODE_TOOLTIP_ELIGIBLE) != 0) {
    stream = TextResource_Resolve(control[-1].nodeFlags);
    patchedStreamCursorPair = RichTextCommandStream_PatchPayloadBySelector(0,&control[1].top,stream)
    ;
    patchedStreamCursorPair =
         RichTextCommandStream_PatchPayloadBySelector
                   (1,&control[1].bottomOffset,(word *)patchedStreamCursorPair);
    if (((uint)control[1].nextSibling & 2) != 0) {
      RichTextCommandStream_PatchPayloadBySelector(2,control + 2,(word *)patchedStreamCursorPair);
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
void UiFormattedContainer_DrawClipped
               (int clipTop,int clipLeft,int clipBottom,int clipRight,UiNodeBase *control)

{
  UiNodeBase *pUVar1;
  UiNodeBase *pUVar2;
  uint uVar3;
  dword arg0;
  int extraout_ECX;
  dword arg6;
  int extraout_ECX_00;
  int iVar4;
  UiNodeBase *pUVar5;
  dword arg6_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  dword arg6_01;
  dword arg6_02;
  word *arg5;
  int iVar6;
  int iVar7;
  int iVar8;
  bool bVar9;
  qword qVar10;
  int iStack_28;
  
  bVar9 = false;
  if ((control[1].parent == (UiNodeBase *)0x0) || ((*g_GraphicsFramebufferBeginAccess)(), bVar9))
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
  iVar8 = control->left;
  iVar6 = control->right;
  (*g_GraphicsTextureSourceBlitSourceAlpha)
            (clipTop,clipLeft,clipBottom,clipRight,control->top,iVar8,control[1].left,
             (GraphicsTextureSourceAsset *)control[1].vtable,g_FramebufferAccess);
  qVar10 = (*g_GraphicsTextureSourceGetLogicalSize)
                     (arg0,(GraphicsTextureSourceAsset *)control[1].vtable);
  iVar8 = iVar8 + (int)qVar10;
  qVar10 = (*g_GraphicsTextureSourceGetLogicalSize)
                     (extraout_ECX + 2,(GraphicsTextureSourceAsset *)control[1].vtable);
  iVar6 = iVar6 - (int)qVar10;
  (*g_GraphicsTextureSourceBlitSourceAlpha)
            (clipTop,clipLeft,clipBottom,clipRight,control->top,iVar6,arg6,
             (GraphicsTextureSourceAsset *)control[1].vtable,g_FramebufferAccess);
  if (iVar8 < iVar6) {
    GraphicsTextureSource_BlitTiledSourceAlpha
              (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,iVar6,control->top,iVar8,
               extraout_ECX_00 - 1,(GraphicsTextureSourceAsset *)control[1].vtable,
               g_FramebufferAccess);
    pUVar1 = control[1].firstChild;
    iVar4 = control[1].layoutHeight;
    iVar6 = iVar6 - iVar8;
    while( true ) {
      for (; (iVar4 < (int)pUVar1 || (iVar4 < (int)control[1].parent)); iVar4 = iVar4 << 2) {
      }
      if (((uint)control[1].nextSibling & 2) == 0) goto UiFormattedContainer_UseResolvedScaleRange;
      if ((int)control[1].nodeFlags <= iVar4) break;
      iVar4 = iVar4 << 2;
    }
    iStack_28 = (int)(((longlong)(int)control[1].nodeFlags * (longlong)iVar6) / (longlong)iVar4);
UiFormattedContainer_UseResolvedScaleRange:
    pUVar2 = control[1].parent;
    pUVar5 = control[1].parent;
    if ((((uint)control[1].nextSibling & 2) != 0) && ((int)control[1].nodeFlags < (int)pUVar5)) {
      pUVar5 = (UiNodeBase *)control[1].nodeFlags;
    }
    uVar3 = (uint)(((longlong)(int)pUVar1 * 100) / (longlong)(int)pUVar5);
    if (((uint)control[1].nextSibling & 1) == 0) {
      iVar7 = 3;
      if ((((0x4f < uVar3) && (iVar7 = 6, 0x53 < uVar3)) && (iVar7 = 9, 0x57 < uVar3)) &&
         ((iVar7 = 0xc, 0x5b < uVar3 && (iVar7 = 0xf, 0x5f < uVar3)))) {
UiFormattedContainer_SelectMaximumTextureVariant:
        iVar7 = 0x12;
      }
    }
    else {
      iVar7 = 0x12;
      if (((((7 < uVar3) && (iVar7 = 0xf, 0xf < uVar3)) && (iVar7 = 0xc, 0x17 < uVar3)) &&
          ((((iVar7 = 9, 0x1f < uVar3 && (iVar7 = 6, 0x27 < uVar3)) &&
            ((iVar7 = 3, 0x55 < uVar3 && ((iVar7 = 6, 0x57 < uVar3 && (iVar7 = 9, 0x59 < uVar3))))))
           && (iVar7 = 0xc, 0x5b < uVar3)))) && (iVar7 = 0xf, 0x5d < uVar3))
      goto UiFormattedContainer_SelectMaximumTextureVariant;
    }
    if (control[1].firstChild != (UiNodeBase *)0x0) {
      qVar10 = (*g_GraphicsTextureSourceGetLogicalSize)
                         (iVar7 + control[1].left,(GraphicsTextureSourceAsset *)control[1].vtable);
      iVar8 = iVar8 - (int)qVar10;
      qVar10 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                         (clipTop,clipLeft,clipBottom,clipRight,control->top,iVar8,arg6_00,
                          (GraphicsTextureSourceAsset *)control[1].vtable,g_FramebufferAccess);
      iVar8 = iVar8 + (int)qVar10;
      iVar7 = (int)(((longlong)(int)pUVar1 * (longlong)iVar6) / (longlong)iVar4) + iVar8;
      GraphicsTextureSource_BlitTiledSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,iVar7,control->top,iVar8,
                 extraout_ECX_01 + 1,(GraphicsTextureSourceAsset *)control[1].vtable,
                 g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,control->top,iVar7,extraout_ECX_02 + 1,
                 (GraphicsTextureSourceAsset *)control[1].vtable,g_FramebufferAccess);
    }
    iVar6 = iVar8 + (int)(((longlong)(int)pUVar2 * (longlong)iVar6) / (longlong)iVar4);
    qVar10 = (*g_GraphicsTextureSourceGetLogicalSize)
                       (control[1].left + 0x15,(GraphicsTextureSourceAsset *)control[1].vtable);
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,control->top,iVar6 - ((int)qVar10 >> 1),arg6_01
               ,(GraphicsTextureSourceAsset *)control[1].vtable,g_FramebufferAccess);
    if (((uint)control[1].nextSibling & 2) != 0) {
      qVar10 = (*g_GraphicsTextureSourceGetLogicalSize)
                         (control[1].left + 0x15,(GraphicsTextureSourceAsset *)control[1].vtable);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,control->top,
                 (iVar8 + iStack_28) - ((int)qVar10 >> 1),arg6_02,
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
    (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control[1].nodeFlags,arg5);
  }
  return;
}

/* Address: 0x00516D10.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00516CC0[2]@00516CC0.
   Cross-module calls: SelectionPanel_RenderArmyRuntimeMetrics [gameplay/selection/runtime],
   UiContainer_DrawIntersectingChildren [ui/controls/layout].
*/
void UiArmyMetricsPanel_DrawTextureMetricsAndChildren
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiNodeBase *control)

{
  void *pvVar1;
  int arg1;
  int arg3;
  sdword arg5;
  GraphicsTextureSourceAsset *extraout_ECX;
  int arg0;
  int iVar2;
  uint uVar3;
  int arg2;
  uint uVar4;
  uint arg4;
  bool bVar5;
  qword qVar6;
  
  if ((control->nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    arg3 = control->left;
    if (control->left < clipRight) {
      arg3 = clipRight;
    }
    arg2 = control->top;
    if (control->top < clipBottom) {
      arg2 = clipBottom;
    }
    arg1 = control->right;
    if (clipLeft < control->right) {
      arg1 = clipLeft;
    }
    arg0 = control->bottom;
    if (clipTop < control->bottom) {
      arg0 = clipTop;
    }
    uVar4 = control->top;
    if ((GraphicsTextureSourceAsset *)control[1].parent != (GraphicsTextureSourceAsset *)0x0) {
      qVar6 = (*g_GraphicsTextureSourceGetLogicalSize)
                        ((dword)control[1].vtable,(GraphicsTextureSourceAsset *)control[1].parent);
      iVar2 = control->layoutHeight - (int)(qVar6 >> 0x20);
      if (((uint)control[1].nextSibling & 8) != 0) {
        uVar4 = uVar4 + iVar2;
      }
      uVar3 = iVar2 >> 1;
      bVar5 = ((uint)control[1].nextSibling & 4) != 0;
      arg4 = uVar4;
      if (bVar5) {
        arg4 = uVar4 + uVar3;
      }
      bVar5 = bVar5 && CARRY4(uVar4,uVar3);
      (*g_GraphicsFramebufferBeginAccess)();
      if (!bVar5) {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (arg0,arg1,arg2,arg3,arg4,arg5,(dword)control[1].vtable,
                   (GraphicsTextureSourceAsset *)control[1].parent,g_FramebufferAccess);
        (*g_GraphicsFramebufferEndAccess)();
        pvVar1 = g_SelectionPanelData;
        g_SelectionPanelData = pvVar1;
        if (control[1].left != 0) {
          g_SelectionPanelTextureSource = g_InfoPanelTextureSource;
          g_SelectionPanelData = g_InfoPanelData;
          SelectionPanel_RenderArmyRuntimeMetrics
                    (clipTop,clipLeft,clipBottom,clipRight,control->bottom,control->right,
                     control->top,control->left,(ArmyRuntimeSlot *)control[1].left);
          g_SelectionPanelTextureSource = extraout_ECX;
          g_SelectionPanelData = pvVar1;
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
void UiSoftwareTexturePreviewControl_DrawScaledTextureAndChildren
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiNodeBase *control)

{
  bool bVar1;
  
  if (((control->nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (bVar1 = false, control[1].firstChild != (UiNodeBase *)0x0)) {
    (*g_GraphicsFramebufferBeginAccess)();
    if (!bVar1) {
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
void UiSoftwareTexturePreviewControl_EnqueueActionOnPrimaryPress(void)

{
  void *in_stack_00000010;
  
  UiActionQueue_Enqueue(*(UiActionId *)((int)in_stack_00000010 + 0x5c),in_stack_00000010);
  return;
}

/* Address: 0x005191B0.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00518C90[6]@00518C90.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime].
*/
void UiSoftwareTexturePreviewControl_EnqueueActionOnSecondaryPress(void)

{
  void *in_stack_00000010;
  
  UiActionQueue_Enqueue(*(UiActionId *)((int)in_stack_00000010 + 0x5c),in_stack_00000010);
  return;
}

/* Address: 0x005191D0.
   Ownership: ui/controls/text.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00518C90[12]@00518C90.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime], UiKeyboardFocus_MoveNext [ui/controls/input].
*/
void UiSoftwareTexturePreviewControl_HandleKeyboardActivationCf
               (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
               UiNodeBase *control)

{
  if (keyCode != 0x10002) {
    UiActionQueue_Enqueue(control[1].left,control);
    return;
  }
  UiKeyboardFocus_MoveNext();
  return;
}

/* Address: 0x004B0150.
   Ownership: ui/controls/text.
   Purpose: Stores the pointer coordinates, hit-tests the active root when no pointer capture exists, accepts only
   nodes with nodeFlags 0x100, reloads the delay countdown on target changes, and clears the previous tooltip text.
   Local calls: UiTooltip_PrepareTargetText.
*/
void UiTooltip_UpdateHoverTarget(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX)

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
void UiNumericTextControl_RebuildTextFromValue(UiNumericTextControl *control)

{
  uint uVar1;
  sbyte sVar2;
  uint uVar3;
  uint uVar4;
  word *arg5;
  
  arg5 = control->textBuffer;
  uVar1 = control->currentValue;
  arg5[0] = 0;
  arg5[1] = 0;
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
  if (((control->editStateFlags & UI_NUMERIC_TEXT_SIGNED_VALUE) != 0) && ((int)uVar1 < 0)) {
    *arg5 = 0x2d;
    uVar1 = -uVar1;
    arg5 = control->textBuffer + 1;
  }
  if ((control->editStateFlags & UI_NUMERIC_TEXT_HEXADECIMAL_FORMAT) == 0) {
    (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,uVar1,arg5);
  }
  else {
    uVar3 = 0x1f;
    if (uVar1 != 0) {
      for (; uVar1 >> uVar3 == 0; uVar3 = uVar3 - 1) {
      }
    }
    if (uVar1 != 0) {
      sVar2 = (sbyte)(uVar3 & 0x1c);
      uVar3 = (uVar3 & 0x1c) >> 2;
      uVar1 = uVar1 >> sVar2 | uVar1 << 0x20 - sVar2;
      do {
        while( true ) {
          uVar4 = (uVar1 >> 0x1c) + 0x30;
          if (0x39 < uVar4) break;
          *arg5 = (word)uVar4;
          uVar3 = uVar3 - 1;
          uVar1 = uVar1 << 4;
          arg5 = arg5 + 1;
          if (uVar3 == 0)
          goto UiNumericTextControl_RebuildTextFromValue_UpdateRangeValidityAfterFormatting;
        }
        *arg5 = (ushort)(uVar1 >> 0x1c) + 0x37;
        uVar3 = uVar3 - 1;
        uVar1 = uVar1 << 4;
        arg5 = arg5 + 1;
      } while (uVar3 != 0);
    }
    else {
      *arg5 = 0x30;
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
void UiNumericTextControl_ParseAndCommitValue(UiNumericTextControl *control)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  word *pwVar5;
  
  pwVar5 = control->textBuffer;
  iVar4 = 1;
  if (*pwVar5 != 0) {
    if (*pwVar5 == 0x2d) {
      iVar4 = -1;
      pwVar5 = control->textBuffer + 1;
    }
    if ((control->editStateFlags & UI_NUMERIC_TEXT_HEXADECIMAL_FORMAT) == 0) {
      uVar1 = 0;
      for (; uVar2 = (uint)*pwVar5, uVar2 != 0; pwVar5 = pwVar5 + 1) {
        if ((uVar2 < 0x30) || (9 < uVar2 - 0x30))
        goto UiNumericTextControl_ParseAndCommitValue_ClearValidityAndReturnAfterParseReject;
        uVar1 = uVar1 * 10 + (uVar2 - 0x30);
      }
    }
    else {
      uVar1 = 0;
      for (; uVar2 = (uint)*pwVar5, uVar2 != 0; pwVar5 = pwVar5 + 1) {
        uVar3 = uVar2 - 0x30;
        if ((uVar2 < 0x30) ||
           ((9 < uVar3 &&
            ((uVar3 = uVar2 - 0x37, uVar3 < 10 ||
             ((0xf < uVar3 && ((uVar3 = uVar2 - 0x57, uVar3 < 10 || (0xf < uVar3))))))))))
        goto UiNumericTextControl_ParseAndCommitValue_ClearValidityAndReturnAfterParseReject;
        uVar1 = uVar1 << 4 | uVar3 & 0xf;
      }
    }
    if ((-1 < iVar4) ||
       (uVar1 = -uVar1, (control->editStateFlags & UI_NUMERIC_TEXT_SIGNED_VALUE) != 0)) {
      control->currentValue = uVar1;
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
void UiTooltip_PrepareTargetText(UiNodeBase *node)

{
  word *commandStream;
  
  if (node != (UiNodeBase *)0x0) {
    commandStream = (word *)node[-1].nodeFlags;
    if ((node->nodeFlags & UI_NODE_TOOLTIP_REFERENCE_DIRECT_UTF16) == 0) {
      commandStream = TextResource_Resolve((TextResourceId)commandStream);
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
void UiNumericTextControl_UpdateRangeValidity(UiNumericTextControl *control)

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
undefined8
UiTextEditControl_MeasurePrefixWidth(UiTextCodeUnitCount prefixLength,UiTextEditControl *control)

{
  int iVar1;
  int extraout_ECX;
  undefined4 in_EDX;
  uint uVar2;
  
  uVar2 = 0;
  iVar1 = 0;
  if (prefixLength != 0) {
    do {
      if (control->textPrefix6C[uVar2] == 0) break;
      iVar1 = FontGlyph_GetLogicalSizeForStyleRegs
                        (g_UiTextEditActiveTextStyle,(uint)control->textPrefix6C[uVar2]);
      uVar2 = uVar2 + 1;
      iVar1 = extraout_ECX + iVar1;
    } while (uVar2 < prefixLength);
  }
  return CONCAT44(in_EDX,iVar1);
}

/* Address: 0x004B6790.
   Ownership: ui/controls/text.
   Purpose: Converts a pointer X coordinate to the nearest UTF-16 cursor index, accounting for control left,
   horizontal scroll, and the optional left decoration selected by state flag 8.
   Cross-module calls: FontGlyph_GetLogicalSizeForStyleRegs [assets/text/resources].
*/
undefined8
UiTextEditControl_FindCursorIndexAtX(UiPixelCoordinate pointerX,UiTextEditControl *control)

{
  int iVar1;
  int extraout_ECX;
  undefined4 in_EDX;
  int currentTextIndex;
  int measuredPrefixWidthPixels;
  
  if ((control->editStateFlags & UI_TEXT_EDIT_DRAW_FRAMED_CHROME) != 0) {
    (*g_GraphicsTextureSourceGetLogicalSize)(0x6a,g_UiWindowTextureSource);
  }
  measuredPrefixWidthPixels = 0;
  iVar1 = 0;
  do {
    currentTextIndex = iVar1;
    if (control->textPrefix6C[currentTextIndex] == 0) break;
    iVar1 = FontGlyph_GetLogicalSizeForStyleRegs
                      (g_UiTextEditActiveTextStyle,(uint)control->textPrefix6C[currentTextIndex]);
    measuredPrefixWidthPixels = measuredPrefixWidthPixels + iVar1;
    iVar1 = currentTextIndex + 1;
  } while (measuredPrefixWidthPixels < extraout_ECX);
  return CONCAT44(in_EDX,currentTextIndex);
}

/* Address: 0x004B7010.
   Ownership: ui/controls/text.
   Purpose: Calls the installed DOS 8.3 path validator with flags derived from control state bits 1 and 2, then
   updates validity bit 1 from the validator's CF result.
*/
void UiPathTextControl_UpdateDos83Validity(UiPathTextEditControl *control)

{
  bool pathRejected;
  
  pathRejected = false;
  (*g_FileSystemValidateDos83Path)(control->editStateFlags >> 1 & 3,(byte *)control->pathBuffer);
  if (pathRejected) {
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
undefined4 UiTextControl_UpdateNonEmptyValidity(UiTextEditControl *control)

{
  undefined4 in_EAX;
  
  if (control->textPrefix6C[0] == 0) {
    control->editStateFlags = control->editStateFlags & ~UI_TEXT_EDIT_VALUE_VALID;
  }
  else {
    control->editStateFlags = control->editStateFlags | UI_TEXT_EDIT_VALUE_VALID;
  }
  return in_EAX;
}

/* Address: 0x004BB570.
   Ownership: ui/controls/text.
   Purpose: Expands both rich-text streams into fixed buffers, then compares them with the ASCII-case-insensitive
   UTF-16 comparator. EAX and EDX remain preserved.
   Cross-module calls: RichTextCommandStream_CopyExpandedCf [assets/text/richtext].
*/
undefined8 UiPointerList_CompareExpandedTextFlags(word *rightText,word *leftText)

{
  undefined4 in_EAX;
  undefined4 in_EDX;
  
  RichTextCommandStream_CopyExpandedCf(0x400,(word *)&g_UiPointerListExpandedLeftTextUtf16,leftText)
  ;
  RichTextCommandStream_CopyExpandedCf
            (0x400,(word *)&g_UiPointerListExpandedRightTextUtf16,rightText);
  (*(code *)g_Utf16StringCompareAsciiCaseInsensitiveFlagsCf)
            (&g_UiPointerListExpandedRightTextUtf16,&g_UiPointerListExpandedLeftTextUtf16);
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x004B5D00.
   Ownership: ui/controls/text.
   Purpose: Recomputes the edit control content rectangle, measures the cursor and active prefix, accounts for
   window decorations, and clamps the horizontal scroll offset so the active glyph remains visible.
   Local calls: UiTextEditControl_MeasurePrefixWidth.
   Cross-module calls: FontGlyph_GetLogicalSizeActiveRegs [assets/text/resources].
*/
undefined8 UiTextEditControl_RecomputeLayoutAndClampScroll(UiTextEditControl *control)

{
  undefined4 in_EAX;
  int iVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  qword qVar6;
  int extraout_ECX_01;
  undefined4 in_EDX;
  int iVar2;
  UiPixelOffset UVar3;
  UiPixelOffset UVar4;
  undefined8 uVar5;
  
  (control->base).layoutWidth = (control->base).right - (control->base).left;
  (control->base).layoutHeight = (control->base).bottom - (control->base).top;
  uVar5 = UiTextEditControl_MeasurePrefixWidth(control->cursorIndex,control);
  UVar3 = (UiPixelOffset)uVar5;
  iVar1 = 0;
  UVar4 = UVar3;
  if (extraout_ECX != 0) {
    iVar1 = FontGlyph_GetLogicalSizeActiveRegs((uint)control->textPrefix6C[extraout_ECX + -1]);
    UVar4 = UVar3 - iVar1;
    iVar1 = extraout_ECX_00;
  }
  iVar2 = UVar3 - (control->base).layoutWidth;
  if (control->textPrefix6C[iVar1] != 0) {
    iVar1 = FontGlyph_GetLogicalSizeActiveRegs((uint)control->textPrefix6C[iVar1]);
    iVar2 = iVar2 + iVar1;
  }
  UiTextEditControl_MeasurePrefixWidth(0x10,control);
  qVar6._0_4_ = (*g_GraphicsTextureSourceGetLogicalSize)(0x8a,g_UiWindowTextureSource);
  UVar3 = iVar2 + (int)(qword)qVar6;
  iVar1 = qVar6._4_4_ + (int)(qword)qVar6;
  if ((control->editStateFlags & UI_TEXT_EDIT_DRAW_FRAMED_CHROME) != 0) {
    qVar6._0_4_ = (*g_GraphicsTextureSourceGetLogicalSize)(0x6a,g_UiWindowTextureSource);
    iVar1 = extraout_ECX_01 + (int)(qword)qVar6 * 2;
    UVar3 = UVar3 + (int)(qword)qVar6 * 2;
  }
  if ((control->base).layoutWidth < iVar1) {
    if ((int)UVar4 < (int)control->horizontalScrollPixels) {
      control->horizontalScrollPixels = UVar4;
    }
    else if ((int)control->horizontalScrollPixels < (int)UVar3) {
      control->horizontalScrollPixels = UVar3;
    }
  }
  else {
    control->horizontalScrollPixels = 0;
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x004B2E60.
   Ownership: ui/controls/text.
   Purpose: Draws the button skin for current selectable state, resolves textResourceId, measures and draws the
   UTF-16 text using packedTextStyle, and honors clipping and suppression flags.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_MeasureRegs
   [assets/text/richtext], RichTextCommandStream_DrawSingleLine [assets/text/richtext].
*/
void UiTextButtonControl_DrawClipped
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiTextButtonControl *control)

{
  word *commandStream;
  dword arg6;
  dword arg0;
  int extraout_ECX;
  int baselineY;
  int extraout_ECX_00;
  sdword extraout_ECX_01;
  int extraout_ECX_02;
  sdword sVar1;
  sdword arg5;
  int extraout_ECX_03;
  sdword extraout_ECX_04;
  int extraout_ECX_05;
  int extraout_EDX;
  sdword sVar2;
  int iVar3;
  uint uVar4;
  int arg5_00;
  bool bVar5;
  RichTextExtentRegs RVar6;
  qword qVar7;
  undefined8 uVar8;
  
  bVar5 = &stack0xffffffe4 < (undefined1 *)0x4;
  (*g_GraphicsFramebufferBeginAccess)();
  if (bVar5) {
    return;
  }
  arg6 = 0x46;
  if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
    arg6 = 0x48;
  }
  if ((((control->selectable).stateFlags & UI_SELECTABLE_TOGGLE_ON_ACTIVATION) != 0) &&
     (arg6 = arg6 - 6, ((control->selectable).stateFlags & 0x40) != 0)) {
    arg6 = 0x44;
  }
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    if (((control->selectable).stateFlags & 0x400) != 0)
    goto UiTextButtonControl_DrawClipped_EndFramebufferAccessAndReturn;
    arg6 = arg6 + 1;
  }
  (*g_GraphicsTextureSourceBlitSourceAlpha)
            (clipTop,clipLeft,clipBottom,clipRight,(control->selectable).base.top,
             (control->selectable).base.left,arg6,g_UiWindowTextureSource,g_FramebufferAccess);
  (*g_GraphicsTextureSourceGetLogicalSize)(arg0,g_UiWindowTextureSource);
  commandStream = TextResource_Resolve(control->textResourceId);
  RVar6 = RichTextCommandStream_MeasureRegs(control->packedTextStyle,commandStream);
  iVar3 = (int)(extraout_EDX - RVar6.heightPixels) >> 1;
  uVar4 = g_UiTextStyleDisabled;
  if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (uVar4 = g_UiTextStyleNormal, ((control->selectable).stateFlags & 0x40) != 0)) {
    uVar4 = g_UiTextStyleAlternate;
  }
  if (((control->selectable).stateFlags & 0x100) == 0) {
    control->packedTextStyle = control->packedTextStyle & 0xff0000;
  }
  else {
    uVar4 = uVar4 & 0xffffff;
  }
  if (((control->selectable).stateFlags & 0x200) == 0) {
    control->packedTextStyle = control->packedTextStyle & 0xff000000;
  }
  else {
    uVar4 = uVar4 & 0xff00ffff;
  }
  uVar4 = uVar4 | control->packedTextStyle;
  if ((((control->selectable).base.nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) == 0) ||
     (((control->selectable).stateFlags & 0x1000) != 0)) {
    RichTextCommandStream_DrawSingleLine
              (clipTop,clipLeft,clipBottom,clipRight,uVar4,commandStream,
               iVar3 + (control->selectable).base.top,extraout_ECX + (control->selectable).base.left
              );
  }
  else {
    baselineY = extraout_ECX + (control->selectable).base.left;
    iVar3 = iVar3 + (control->selectable).base.top;
    qVar7 = (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                      (clipTop,clipLeft,clipBottom,clipRight,iVar3,baselineY + -2,0x7f000000,0x86,
                       g_UiWindowTextureSource,g_FramebufferAccess);
    sVar1 = (sdword)(qVar7 >> 0x20);
    qVar7 = (*g_GraphicsTextureSourceGetLogicalSize)(0x88,g_UiWindowTextureSource);
    (*g_GraphicsTextureSourceGetLogicalSize)(0x86,g_UiWindowTextureSource);
    (*g_GraphicsTextureSourceGetLogicalSize)(0x87,g_UiWindowTextureSource);
    arg5_00 = ((extraout_ECX_00 + 6) - (int)qVar7) + RVar6.widthPixels;
    qVar7 = (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                      (clipTop,clipLeft,clipBottom,clipRight,sVar1,arg5_00,0x7f000000,0x88,
                       g_UiWindowTextureSource,g_FramebufferAccess);
    sVar2 = (sdword)(qVar7 >> 0x20);
    sVar1 = extraout_ECX_01;
    if (clipLeft < arg5_00) {
      arg5_00 = clipLeft;
    }
    do {
      qVar7 = (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                        (clipTop,arg5_00,clipBottom,clipRight,sVar2,sVar1,0x7f000000,0x87,
                         g_UiWindowTextureSource,g_FramebufferAccess);
      sVar2 = (sdword)(qVar7 >> 0x20);
      sVar1 = extraout_ECX_02 + (int)qVar7;
    } while (sVar1 < arg5_00);
    uVar8 = RichTextCommandStream_DrawSingleLine
                      (clipTop,clipLeft,clipBottom,clipRight,uVar4,commandStream,iVar3,baselineY);
    qVar7 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                      (clipTop,clipLeft,clipBottom,clipRight,(sdword)((ulonglong)uVar8 >> 0x20),arg5
                       ,0x86,g_UiWindowTextureSource,g_FramebufferAccess);
    sVar2 = (sdword)(qVar7 >> 0x20);
    qVar7 = (*g_GraphicsTextureSourceGetLogicalSize)(0x88,g_UiWindowTextureSource);
    (*g_GraphicsTextureSourceGetLogicalSize)(0x86,g_UiWindowTextureSource);
    (*g_GraphicsTextureSourceGetLogicalSize)(0x87,g_UiWindowTextureSource);
    iVar3 = ((extraout_ECX_03 + 6) - (int)qVar7) + RVar6.widthPixels;
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,sVar2,iVar3,0x88,g_UiWindowTextureSource,
               g_FramebufferAccess);
    sVar1 = extraout_ECX_04;
    if (clipLeft < iVar3) {
      iVar3 = clipLeft;
    }
    do {
      qVar7 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                        (clipTop,iVar3,clipBottom,clipRight,sVar2,sVar1,0x87,g_UiWindowTextureSource
                         ,g_FramebufferAccess);
      sVar1 = extraout_ECX_05 + (int)qVar7;
    } while (sVar1 < iVar3);
  }
UiTextButtonControl_DrawClipped_EndFramebufferAccessAndReturn:
  (*g_GraphicsFramebufferEndAccess)();
  return;
}
