/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/chat.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/chat.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Adds a chat line to the shared recent-text history and rebuilds the frontend chat history box from its five
   newest entries.
*/
void FrontendRecentTextHistory_InsertAndRebuild5(uint16_t *text)

{
  RecentTextHistoryPointerList *output;
  
  /* lineCount + textLines of the chat history box form the pointer list. */
  output = (RecentTextHistoryPointerList *)
           &((UiConditionalActionControl *)FRONTEND_UI(g_FrontendRootNode,chatMessageHistory))->lineCount;
  RecentTextHistory_Insert(text);
  RecentTextHistory_SortAndBuildPointerList(5,output); /* the box shows five lines */
}

/* Handler of action 0x200E (slot 14 of g_FrontendUiActionHandlersPage20.handlers00_54), a click on the chat
   strip at the top left: drops the oldest chat lines until four are left, then one more (a click removes the
   oldest line shown), and rebuilds the strip's pointer list of at most five lines.
*/
void FrontendRecentText_TrimAndSortTopFive(UiNodeBase *source)

{
  uint32_t currentEntryCount;
  
  for (currentEntryCount = ((UiConditionalActionControl *)source)->lineCount; 4 < currentEntryCount;
      currentEntryCount--) {
    RecentTextHistory_RemoveOldest();
  }
  RecentTextHistory_RemoveOldest();
  RecentTextHistory_SortAndBuildPointerList
            (5,(RecentTextHistoryPointerList *)&((UiConditionalActionControl *)source)->lineCount);
}
