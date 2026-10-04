/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/root_stack.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/root_stack.h>
#include <thandor/thandor.h>

/* Implementation ownership: ui/controls/root_stack. */

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
