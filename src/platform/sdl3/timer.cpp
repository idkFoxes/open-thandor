/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/sdl3/timer.cpp
 * Project code (not in the original game)
 */

/* SDL3 backend: the periodic timers of g_TimerRegisterPeriodic / g_TimerUnregisterPeriodic on SDL timers instead
   of WinMM's timeSetEvent. Like the original's TimerSystem_RegisterPeriodic there are 32 slots, the period is 1000 / frequency ms
   (truncated) and the callbacks run on a timer thread (SDL's). The period is kept without drift: each callback
   asks SDL for the time left to its next due point, as a periodic WinMM timer fires on a fixed grid. */

#include <thandor/platform/sdl3/sdl_objects.h>

#include <SDL3/SDL_timer.h>

#include <array>
#include <atomic>
#include <memory>
#include <mutex>
#include <vector>

#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

namespace {

constexpr std::size_t kTimerSlotCount = 32; /* the original's 32 timer callback slots */
constexpr Uint64 kNanosecondsPerMillisecond = 1000000;

/* One registration. It stays allocated until the backend ends, so a callback still running on the timer thread
   never sees freed memory after its timer was stopped. */
struct TimerRegistration {
  TimerCallbackProc *callback = nullptr;
  Uint64 periodNs = 0;
  Uint64 nextDueNs = 0; /* only touched by the timer thread after the start */
  std::atomic<bool> active{true};
};

struct TimerSlot {
  TimerRegistration *registration = nullptr;
  SDL_TimerID timerId = 0;
};

std::mutex s_timerMutex;
std::array<TimerSlot, kTimerSlotCount> s_timerSlots;
std::vector<std::unique_ptr<TimerRegistration>> s_registrations;

Uint64 SDLCALL TimerDispatch(void *userdata, SDL_TimerID /*timerId*/, Uint64 /*interval*/)
{
  auto &registration = *static_cast<TimerRegistration *>(userdata);
  if (!registration.active.load()) {
    return 0; /* stopped: SDL removes the timer */
  }
  registration.callback();
  registration.nextDueNs += registration.periodNs;
  const Uint64 now = SDL_GetTicksNS();
  if (registration.nextDueNs + registration.periodNs < now) {
    registration.nextDueNs = now + registration.periodNs; /* far behind (e.g. a debugger stop): no burst */
  }
  return (registration.nextDueNs > now) ? registration.nextDueNs - now : 1;
}

} // namespace

void SdlTimer_RegisterPeriodic(TimerFrequencyHz frequencyHz,TimerCallbackProc *callback)
{
  if (frequencyHz == 0) {
    return; /* timeSetEvent would get a division by zero */
  }
  const Uint64 periodNs = static_cast<Uint64>(1000 / frequencyHz) * kNanosecondsPerMillisecond;
  const std::scoped_lock lock(s_timerMutex);
  for (TimerSlot &slot : s_timerSlots) {
    if (slot.registration != nullptr) {
      continue;
    }
    auto registration = std::make_unique<TimerRegistration>();
    registration->callback = callback;
    registration->periodNs = periodNs;
    registration->nextDueNs = SDL_GetTicksNS() + periodNs;
    slot.registration = registration.get();
    slot.timerId = SDL_AddTimerNS(periodNs, TimerDispatch, registration.get());
    if (slot.timerId == 0) {
      Thandor_Log("SDL_AddTimerNS failed: %s", SDL_GetError());
    }
    s_registrations.push_back(std::move(registration));
    return;
  }
  /* all slots taken: ignored, as in the original */
}

void SdlTimer_UnregisterPeriodic(TimerCallbackProc *callback)
{
  const std::scoped_lock lock(s_timerMutex);
  for (TimerSlot &slot : s_timerSlots) {
    if ((slot.registration != nullptr) && (slot.registration->callback == callback)) {
      slot.registration->active.store(false);
      SDL_RemoveTimer(slot.timerId);
      slot = TimerSlot{};
      return;
    }
  }
}

void SdlTimer_Shutdown(void)
{
  const std::scoped_lock lock(s_timerMutex);
  for (TimerSlot &slot : s_timerSlots) {
    if (slot.registration != nullptr) {
      slot.registration->active.store(false);
      SDL_RemoveTimer(slot.timerId);
      slot = TimerSlot{};
    }
  }
  /* s_registrations stays: a callback may still be finishing on the timer thread */
}
