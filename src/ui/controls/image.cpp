/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/image.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/image.h>
#include <thandor/thandor.h>

/* Implementation ownership: ui/controls/image. */

/* layout of g_UiImageControlVtable (image toggles of the in-game resource panel): lays out the children
   relative to the parent's rectangle instead of the control's own by swapping the parent's edges in for
   the call; afterwards the own rectangle is restored and its size stored as layoutWidth/layoutHeight.
*/
void UiImageControl_LayoutChildrenToParent(UiImageControl *control)

{
  int32_t *edgeField;
  int savedLeft;
  int savedTop;
  int savedRight;
  int savedBottom;
  UiNodeBase *parentNode;
  int32_t parentTop;
  int32_t parentRight;
  int32_t parentBottom;
  
  parentNode = (control->selectable).base.parent;
  parentTop = parentNode->top;
  parentRight = parentNode->right;
  parentBottom = parentNode->bottom;
  LOCK();
  edgeField = &(control->selectable).base.left;
  savedLeft = *edgeField;
  *edgeField = parentNode->left;
  UNLOCK();
  LOCK();
  edgeField = &(control->selectable).base.top;
  savedTop = *edgeField;
  *edgeField = parentTop;
  UNLOCK();
  LOCK();
  edgeField = &(control->selectable).base.right;
  savedRight = *edgeField;
  *edgeField = parentRight;
  UNLOCK();
  LOCK();
  edgeField = &(control->selectable).base.bottom;
  savedBottom = *edgeField;
  *edgeField = parentBottom;
  UNLOCK();
  UiContainer_LayoutChildren((UiNodeBase *)control);
  (control->selectable).base.left = savedLeft;
  (control->selectable).base.top = savedTop;
  (control->selectable).base.right = savedRight;
  (control->selectable).base.bottom = savedBottom;
  (control->selectable).base.layoutWidth = savedRight - savedLeft;
  (control->selectable).base.layoutHeight = savedBottom - savedTop;
  return;
}
