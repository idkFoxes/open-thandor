/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/debug/script.cpp
 * Project code (not in the original game)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thandor/platform/system/win32.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/test_aids.h>
#include <thandor/platform/debug/script.h>
#include <thandor/platform/debug/autoshot.h>

/* Test aid: OPEN_THANDOR_SCRIPT=<file> replays timed input from a text file, one command per line:
     <ms> click <x> <y> [hold]  left press at framebuffer pixel x,y, release after hold ms (120)
     <ms> rclick <x> <y>    the same with the right button
     <ms> move <x> <y>      pointer motion
     <ms> drag <x> <y> <x2> <y2>  left press at x,y, motion to x2,y2 with the button held, release there
     <ms> wheel <x> <y> <n>  pointer at x,y, then n mouse wheel notches (positive: up, negative: down)
     <ms> key <vk>          key press and release (Windows virtual-key code, decimal)
     <ms> keydown <vk> / keyup <vk>  press or release only (held modifiers: keydown 17, key 37, keyup 17)
     <ms> type <text>       types the rest of the line (ASCII), one character per tick, as the window procedure
                            delivers it: space as VK_SPACE key-down/up (it gets no WM_CHAR), letters and digits
                            as key-down (swallowed by text edits), Keyboard_OnChar (the WM_CHAR) and key-up,
                            other characters as Keyboard_OnChar only
     <ms> shot              save the framebuffer now (shots\script_NNNN.bmp)
     <ms> quit              end the process
     <ms> layout <w> <h>    the following coordinates are for a w x h screen; they are moved by half
                            the difference to the current resolution (dialogs and the view are centred)
     <ms> clickuntilingame <x> <y> [interval]  left click x,y every interval ms (3000) until the level runs
     <ms> clickuntilnextlevel <x> <y> [interval]  the same until the next in-game session starts (test build:
                            campaign level change); the times of the following lines count from then
     <ms> ingame            wait until the level has loaded and the game runs; the times of the
                            following lines count from that moment
   <ms> counts from the first message pump. Pointer events go into the same ring DirectInput fills. */
volatile unsigned g_TestAidInGameFrames;
volatile unsigned g_TestAidSessionCount;

/* Port-only test aid (no original address). Moves the pointer to framebuffer pixel x,y with button mask buttons
   (LEFT/RIGHT of GraphicsCursorButtonState) and appends a pointer event of that type to the 256-entry ring
   g_CursorInputEvents, as the SDL3 input backend does for real mouse input, so scripted clicks
   reach the UI through the normal event path. */
static void DebugScript_PushCursorEvent(GraphicsCursorEventType type, uint32_t buttons, int x, int y,
                                        UiPointerWheelDelta wheelDelta = 0)
{
  uint32_t index = g_CursorInputWriteIndex;
  uint32_t next = index + 1;
  if (255 < next) { /* wrap around the ring */
    next = 0;
  }
  g_MouseX = x;
  g_MouseY = y;
  g_MouseButtonMask = buttons;
  g_CursorInputEvents[index].eventType = type;
  g_CursorInputEvents[index].buttonState = buttons;
  g_CursorInputEvents[index].pointerX = x;
  g_CursorInputEvents[index].pointerY = y;
  g_CursorInputEvents[index].wheelDelta = wheelDelta;
  g_CursorInputEvents[index].clockValue = g_CursorInputClockValue;
  g_CursorInputWriteIndex = next;
}

void DebugScript_Tick(void)
{
  static FILE *script;
  static int state = -1;
  static unsigned start;
  static unsigned due;
  static char command[32];
  static int x;
  static int y;
  static int hold;
  static int extra;
  static int pendingRelease;
  static uint32_t releaseButton;
  static unsigned releaseAt;
  static int releaseX;
  static int releaseY;
  static int layoutWidth;
  static int layoutHeight;
  static char typeText[128];
  static int typePosition = -1;
  char line[128];
  unsigned now;
  /* state: -1 not started, 0 no (more) script, 1 read the next line, 2 a line waits for its time */
  if (state < 0) {
    const char *path = getenv("OPEN_THANDOR_SCRIPT");
    state = 0;
    if (path != NULL && (script = fopen(path, "r")) != NULL) {
      state = 1;
      start = Thandor_TickCount();
      Thandor_Log("script: %s", path);
    }
  }
  now = Thandor_TickCount() - start;
  {
    /* Report when the in-game session stops producing frames (the level ended and the frontend runs again). */
    static unsigned lastFrames;
    static unsigned framesChangedAt;
    static int sessionEndReported;
    unsigned tick = Thandor_TickCount();
    if (g_TestAidInGameFrames != lastFrames) {
      lastFrames = g_TestAidInGameFrames;
      framesChangedAt = tick;
      sessionEndReported = 0;
    }
    else if (lastFrames != 0 && !sessionEndReported && tick - framesChangedAt > 3000) {
      sessionEndReported = 1;
      Thandor_Log("script: in-game session ended after %u frames", lastFrames);
    }
  }
  if (pendingRelease && now >= releaseAt) {
    DebugScript_PushCursorEvent(releaseButton == 1 ? LEFT_RELEASE : RIGHT_RELEASE, 0, releaseX, releaseY);
    pendingRelease = 0;
  }
  if (typePosition >= 0) {
    /* one character of a "type" line per tick, so the 64-entry keyboard ring cannot overflow */
    int character = (unsigned char)typeText[typePosition];
    if (character == 0) {
      typePosition = -1;
    }
    else {
      typePosition++;
      if (character == ' ') {
        Keyboard_OnKeyDown(VK_SPACE);
        Keyboard_OnKeyUp(VK_SPACE);
      }
      else {
        int virtualKey = 0;
        if ((character >= '0' && character <= '9') || (character >= 'A' && character <= 'Z')) {
          virtualKey = character;
        }
        else if (character >= 'a' && character <= 'z') {
          virtualKey = character - ('a' - 'A');
        }
        if (virtualKey != 0) {
          Keyboard_OnKeyDown(virtualKey);
        }
        Keyboard_OnChar(character);
        if (virtualKey != 0) {
          Keyboard_OnKeyUp(virtualKey);
        }
      }
      return;
    }
  }
  if (state == 0) {
    return;
  }
  for (;;) {
    if (state == 1) {
      if (fgets(line, sizeof line, script) == NULL) {
        state = 0;
        fclose(script);
        return;
      }
      command[0] = 0;
      x = y = hold = extra = 0;
      if (sscanf(line, "%u %31s %d %d %d %d", &due, command, &x, &y, &hold, &extra) < 2) {
        continue;
      }
      if (strcmp(command, "type") == 0) {
        /* the text is the rest of the line after "type " (without the line end) */
        const char *text = strstr(line, "type") + 4;
        size_t length;
        if (*text == ' ' || *text == '\t') {
          text++;
        }
        length = strcspn(text, "\r\n");
        if (length >= sizeof typeText) {
          length = sizeof typeText - 1;
        }
        memcpy(typeText, text, length);
        typeText[length] = 0;
      }
      state = 2;
    }
    if (now < due || pendingRelease || typePosition >= 0) {
      return;
    }
    if (strcmp(command, "ingame") == 0) {
      if (g_TestAidInGameFrames == 0) {
        return;
      }
      start = Thandor_TickCount();
      now = 0;
      state = 1;
      Thandor_Log("script: in game, times restart at 0");
      continue;
    }
    if (strcmp(command, "clickuntilingame") == 0 || strcmp(command, "clickuntilnextlevel") == 0) {
      /* click x,y every `hold` ms (default 3000) until the level runs (clickuntilingame) or until the next
         in-game session has started (clickuntilnextlevel, test build: end movie, results, briefing of the next
         campaign level); this line stays pending until then */
      static unsigned targetSession;
      static int armed;
      int nextLevel = strcmp(command, "clickuntilnextlevel") == 0;
      int clickX = x;
      int clickY = y;
      int reached;
      if (nextLevel && !armed) {
        targetSession = g_TestAidSessionCount + 1;
        armed = 1;
      }
      reached = nextLevel ? g_TestAidSessionCount >= targetSession : g_TestAidInGameFrames != 0;
      if (reached) {
        armed = 0;
        state = 1;
        if (nextLevel) {
          start = Thandor_TickCount();
          now = 0;
          Thandor_Log("script: next level started (session %u), times restart at 0", g_TestAidSessionCount);
        }
        continue;
      }
      if (layoutWidth > 0) {
        clickX += ((int)g_FramebufferWidth - layoutWidth) / 2;
        clickY += ((int)g_FramebufferHeight - layoutHeight) / 2;
      }
      Thandor_Log("script: %u ms click %d %d (until in game)", now, clickX, clickY);
      DebugScript_PushCursorEvent(MOTION_OR_WHEEL, 0, clickX, clickY);
      DebugScript_PushCursorEvent(LEFT_PRESS, 1, clickX, clickY);
      pendingRelease = 1;
      releaseButton = 1;
      releaseX = clickX;
      releaseY = clickY;
      releaseAt = now + 120;
      due = now + (hold > 0 ? (unsigned)hold : 3000);
      return;
    }
    state = 1;
    if (strcmp(command, "layout") == 0) {
      layoutWidth = x;
      layoutHeight = y;
      continue;
    }
    if (layoutWidth > 0) {
      x += ((int)g_FramebufferWidth - layoutWidth) / 2;
      y += ((int)g_FramebufferHeight - layoutHeight) / 2;
    }
    Thandor_Log("script: %u ms %s %d %d", now, command, x, y);
    if (strcmp(command, "click") == 0 || strcmp(command, "rclick") == 0) {
      int right = command[0] == 'r';
      /* g_MouseButtonMask bits: 1 left, 4 right */
      DebugScript_PushCursorEvent(MOTION_OR_WHEEL, 0, x, y);
      DebugScript_PushCursorEvent(right ? RIGHT_PRESS : LEFT_PRESS, right ? 4 : 1, x, y);
      pendingRelease = 1;
      releaseButton = right ? 4 : 1;
      /* the next line is read before the release is due and overwrites x, y */
      releaseX = x;
      releaseY = y;
      releaseAt = now + (hold > 0 ? (unsigned)hold : 120);
    }
    else if (strcmp(command, "move") == 0) {
      DebugScript_PushCursorEvent(MOTION_OR_WHEEL, 0, x, y);
    }
    else if (strcmp(command, "wheel") == 0) {
      DebugScript_PushCursorEvent(MOTION_OR_WHEEL, 0, x, y);
      DebugScript_PushCursorEvent(MOTION_OR_WHEEL, 0, x, y, hold);
    }
    else if (strcmp(command, "drag") == 0) {
      /* drag x y x2 y2: press the left button at x,y, move with it held to x2,y2 and release there */
      int toX = hold;
      int toY = extra;
      if (layoutWidth > 0) {
        toX += ((int)g_FramebufferWidth - layoutWidth) / 2;
        toY += ((int)g_FramebufferHeight - layoutHeight) / 2;
      }
      DebugScript_PushCursorEvent(MOTION_OR_WHEEL, 0, x, y);
      DebugScript_PushCursorEvent(LEFT_PRESS, 1, x, y);
      DebugScript_PushCursorEvent(MOTION_OR_WHEEL, 1, (x + toX) / 2, (y + toY) / 2);
      DebugScript_PushCursorEvent(MOTION_OR_WHEEL, 1, toX, toY);
      pendingRelease = 1;
      releaseButton = 1;
      releaseX = toX;
      releaseY = toY;
      releaseAt = now + 200;
    }
    else if (strcmp(command, "key") == 0) {
      Keyboard_OnKeyDown(x);
      Keyboard_OnKeyUp(x);
      if (g_TestAidInGameFrames != 0) {
        Thandor_Log("script: simulation step ticks now %u", (unsigned)g_InGameSimulationStepTicks);
      }
    }
    /* held keys for combinations such as Alt+P: keydown 18, key 80, keyup 18 */
    else if (strcmp(command, "keydown") == 0) {
      Keyboard_OnKeyDown(x);
    }
    else if (strcmp(command, "keyup") == 0) {
      Keyboard_OnKeyUp(x);
    }
    else if (strcmp(command, "type") == 0) {
      Thandor_Log("script: type \"%s\"", typeText);
      typePosition = 0;
      return;
    }
    else if (strcmp(command, "shot") == 0) {
      DebugAutoShot_SaveNow();
    }
    else if (strcmp(command, "quit") == 0) {
      ExitProcess(0);
    }
  }
}
