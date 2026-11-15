/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/selftest/mousering_selftest.cpp
 * Project code (not in the original game)
 */

/* OPEN_THANDOR_SELFTEST=mousering: the mouse event ring when it fills up between two UI frames (a burst of motion
   during a hitch) and the release of held buttons on focus loss. The events go through the real backend handlers
   (HandleMouseEvent, HandleFocusLost, HandleWindowDeactivated) and are read back with the real consumer
   (GraphicsCursor_ConsumeNextInputEvent), so the write and the read side of the ring are covered together.
     not full     events arrive unchanged, in order
     full         merges motion, keeps every button event, the newest position and the wheel sum
     no motion    a ring of nothing but button events keeps the last event of each button
     focus loss   every held button gets a release, the mask is cleared
   Ends with "mousering: ok, N checks" or "mousering: FAILED, ...". */

#include <SDL3/SDL_events.h>

#include <cstdint>
#include <vector>

#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/sdl3/sdl_objects.h>
#include <thandor/platform/selftest/selftest.h>

namespace {

using thandor::sdl3::HandleFocusLost;
using thandor::sdl3::HandleMouseEvent;
using thandor::sdl3::HandleWindowDeactivated;

constexpr int kRingEntries = CURSOR_INPUT_EVENT_RING_SIZE - 1; /* read == write means empty */

int s_checks = 0;
int s_failures = 0;

void Check(bool ok, const char *what)
{
  s_checks++;
  if (!ok) {
    s_failures++;
    Thandor_Log("mousering %s: FAILED", what);
  }
}

void ResetRing()
{
  g_CursorInputWriteIndex = 0;
  g_CursorInputReadIndex = 0;
  g_MouseButtonMask = CURSOR_BUTTON_NONE;
  g_MouseX = 0;
  g_MouseY = 0;
}

void Motion()
{
  SDL_Event event = {};
  event.type = SDL_EVENT_MOUSE_MOTION;
  event.motion.xrel = 1.0f;
  HandleMouseEvent(event);
}

void Wheel()
{
  SDL_Event event = {};
  event.type = SDL_EVENT_MOUSE_WHEEL;
  event.wheel.integer_y = 1;
  event.wheel.direction = SDL_MOUSEWHEEL_NORMAL;
  HandleMouseEvent(event);
}

void Button(Uint8 button, bool down)
{
  SDL_Event event = {};
  event.type = down ? SDL_EVENT_MOUSE_BUTTON_DOWN : SDL_EVENT_MOUSE_BUTTON_UP;
  event.button.button = button;
  event.button.down = down;
  HandleMouseEvent(event);
}

std::vector<CursorPointerEvent> Drain()
{
  std::vector<CursorPointerEvent> events;
  CursorPointerEvent event;
  while (g_GraphicsCursorConsumeEvent(&event)) {
    events.push_back(event);
  }
  return events;
}

int Count(const std::vector<CursorPointerEvent> &events, GraphicsCursorEventType type)
{
  int count = 0;
  for (const CursorPointerEvent &event : events) {
    if (event.eventType == static_cast<uint32_t>(type)) {
      count++;
    }
  }
  return count;
}

bool Is(const CursorPointerEvent &event, GraphicsCursorEventType type)
{
  return event.eventType == static_cast<uint32_t>(type);
}

void TestNotFull()
{
  ResetRing();
  Button(SDL_BUTTON_LEFT, true);
  Motion();
  Button(SDL_BUTTON_LEFT, false);
  const std::vector<CursorPointerEvent> events = Drain();
  Check(events.size() == 3, "not full: count");
  if (events.size() == 3) {
    Check(Is(events[0], LEFT_PRESS) && Is(events[1], MOTION_OR_WHEEL) && Is(events[2], LEFT_RELEASE), "not full: order");
    Check(events[1].pointerX == 1, "not full: position");
  }
}

/* The reported bug: a burst of motion fills the ring while a button is held, then the release arrives. */
void TestFullKeepsRelease()
{
  ResetRing();
  Button(SDL_BUTTON_LEFT, true);
  for (int i = 0; i < 700; i++) {
    Motion();
  }
  Button(SDL_BUTTON_LEFT, false);
  const std::vector<CursorPointerEvent> events = Drain();
  Check(events.size() == kRingEntries, "full: ring stays filled, not read as empty");
  Check(Count(events, LEFT_PRESS) == 1, "full: press kept");
  Check(Count(events, LEFT_RELEASE) == 1, "full: release kept");
  if (events.size() >= 2) {
    Check(Is(events.front(), LEFT_PRESS), "full: press first");
    Check(Is(events.back(), LEFT_RELEASE), "full: release last");
    Check(events.back().pointerX == 700, "full: release has the newest position");
    Check(Is(events[events.size() - 2], MOTION_OR_WHEEL) && (events[events.size() - 2].pointerX == 700),
          "full: newest motion kept");
    Check(events.back().buttonState == CURSOR_BUTTON_NONE, "full: release state");
  }
  Check(g_MouseButtonMask == CURSOR_BUTTON_NONE, "full: mask released");
}

/* Motion with different button states next to it cannot merge: the oldest motion goes, button events stay. */
void TestFullWithoutMergeableMotion()
{
  ResetRing();
  /* LP M LR M ... : 255 entries, 64 presses, 64 releases, 127 motions, no two motions adjacent */
  for (int i = 0; i < 64; i++) {
    Button(SDL_BUTTON_LEFT, true);
    Motion();
    Button(SDL_BUTTON_LEFT, false);
    if (i < 63) {
      Motion();
    }
  }
  Button(SDL_BUTTON_RIGHT, true);
  Button(SDL_BUTTON_RIGHT, false);
  const std::vector<CursorPointerEvent> events = Drain();
  Check(events.size() == kRingEntries, "no merge: count");
  Check(Count(events, LEFT_PRESS) == 64, "no merge: presses kept");
  Check(Count(events, LEFT_RELEASE) == 64, "no merge: releases kept");
  Check(Count(events, RIGHT_PRESS) == 1, "no merge: new press kept");
  Check(Count(events, RIGHT_RELEASE) == 1, "no merge: new release kept");
  Check(Count(events, MOTION_OR_WHEEL) == 125, "no merge: two oldest motions dropped");
  if (events.size() >= 2) {
    Check(Is(events[events.size() - 2], RIGHT_PRESS) && Is(events.back(), RIGHT_RELEASE), "no merge: order");
  }
}

/* Nothing but button events: only an older event of a type that occurs again may go, the last one stays. */
void TestFullOfButtonEvents()
{
  ResetRing();
  /* LP LR ... LP: 128 presses and 127 releases */
  for (int i = 0; i < 127; i++) {
    Button(SDL_BUTTON_LEFT, true);
    Button(SDL_BUTTON_LEFT, false);
  }
  Button(SDL_BUTTON_LEFT, true);
  Button(SDL_BUTTON_LEFT, false);
  const std::vector<CursorPointerEvent> events = Drain();
  Check(events.size() == kRingEntries, "buttons only: count");
  Check((Count(events, LEFT_PRESS) == 127) && (Count(events, LEFT_RELEASE) == 128), "buttons only: oldest press went");
  if (!events.empty()) {
    Check(Is(events.back(), LEFT_RELEASE), "buttons only: final release kept");
    Check(events.back().buttonState == CURSOR_BUTTON_NONE, "buttons only: final state");
  }
  Check(g_MouseButtonMask == CURSOR_BUTTON_NONE, "buttons only: mask");
}

void TestWheelSumSurvivesMerge()
{
  ResetRing();
  for (int i = 0; i < 256; i++) {
    Wheel();
  }
  const std::vector<CursorPointerEvent> events = Drain();
  Check(events.size() == kRingEntries, "wheel: count");
  int notches = 0;
  for (const CursorPointerEvent &event : events) {
    notches += event.wheelDelta;
  }
  Check(notches == 256, "wheel: sum");
}

void TestFocusLossReleasesHeldButtons()
{
  ResetRing();
  Button(SDL_BUTTON_LEFT, true);
  Button(SDL_BUTTON_RIGHT, true);
  HandleFocusLost();
  Check(g_MouseButtonMask == CURSOR_BUTTON_NONE, "focus lost: mask cleared");
  std::vector<CursorPointerEvent> events = Drain();
  Check(events.size() == 4, "focus lost: one release per held button");
  if (events.size() == 4) {
    Check(Is(events[0], LEFT_PRESS) && Is(events[1], RIGHT_PRESS), "focus lost: presses first");
    Check(Is(events[2], LEFT_RELEASE) && (events[2].buttonState == RIGHT), "focus lost: left release");
    Check(Is(events[3], RIGHT_RELEASE) && (events[3].buttonState == CURSOR_BUTTON_NONE), "focus lost: right release");
  }

  ResetRing();
  HandleFocusLost();
  Check(Drain().empty(), "focus lost: nothing held, no event");

  ResetRing();
  Button(SDL_BUTTON_MIDDLE, true);
  HandleWindowDeactivated();
  Check(g_MouseButtonMask == CURSOR_BUTTON_NONE, "deactivated: mask cleared");
  events = Drain();
  Check((events.size() == 2) && Is(events[1], MIDDLE_RELEASE), "deactivated: middle release");
}

} // namespace

void Thandor_SelfTestMouseRing()
{
  g_FramebufferWidth = 800;
  g_FramebufferHeight = 600;
  TestNotFull();
  TestFullKeepsRelease();
  TestFullWithoutMergeableMotion();
  TestFullOfButtonEvents();
  TestWheelSumSurvivesMerge();
  TestFocusLossReleasesHeldButtons();
  ResetRing();
  if (s_failures == 0) {
    Thandor_Log("mousering: ok, %d checks", s_checks);
  }
  else {
    Thandor_Log("mousering: FAILED, %d of %d checks", s_failures, s_checks);
  }
}
