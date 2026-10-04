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
  std::atomic<bool> running{false}; /* the callback is executing on the timer thread */
};

struct TimerSlot {
  TimerRegistration *registration = nullptr;
  SDL_TimerID timerId = 0;
};

std::mutex s_timerMutex;
std::array<TimerSlot, kTimerSlotCount> s_timerSlots;
std::vector<std::unique_ptr<TimerRegistration>> s_registrations;
thread_local bool t_inTimerDispatch = false; /* true on the timer thread while a callback runs */

Uint64 SDLCALL TimerDispatch(void *userdata, SDL_TimerID /*timerId*/, Uint64 /*interval*/)
{
  auto &registration = *static_cast<TimerRegistration *>(userdata);
  /* running is raised before active is checked (both sequentially consistent): a stopping thread that has
     cleared active either is seen here, or sees running and waits for the callback to end */
  registration.running.store(true);
  if (!registration.active.load()) {
    registration.running.store(false);
    return 0; /* stopped: SDL removes the timer */
  }
  t_inTimerDispatch = true;
  registration.callback();
  t_inTimerDispatch = false;
  registration.running.store(false);
  registration.nextDueNs += registration.periodNs;
  const Uint64 now = SDL_GetTicksNS();
  if (registration.nextDueNs + registration.periodNs < now) {
    registration.nextDueNs = now + registration.periodNs; /* far behind (e.g. a debugger stop): no burst */
  }
  return (registration.nextDueNs > now) ? registration.nextDueNs - now : 1;
}

/* Waits until a stopped registration's callback is no longer running, so the caller may free the data it
   touches. SDL_RemoveTimer does not wait for a callback in progress. Called without s_timerMutex held (a
   callback may register or unregister timers itself). On the timer thread no other callback runs at the same
   time (SDL has one timer thread), and a callback stopping its own timer must not wait for itself. */
void WaitWhileCallbackRunning(const TimerRegistration *registration)
{
  if ((registration == nullptr) || t_inTimerDispatch) {
    return;
  }
  while (registration->running.load()) {
    SDL_Delay(0);
  }
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
  TimerRegistration *stopped = nullptr;
  {
    const std::scoped_lock lock(s_timerMutex);
    for (TimerSlot &slot : s_timerSlots) {
      if ((slot.registration != nullptr) && (slot.registration->callback == callback)) {
        stopped = slot.registration;
        stopped->active.store(false);
        SDL_RemoveTimer(slot.timerId);
        slot = TimerSlot{};
        break;
      }
    }
  }
  /* the callers free the callback's data right after this returns (e.g. UiRuntime_Shutdown, the record ring) */
  WaitWhileCallbackRunning(stopped);
}

void SdlTimer_Shutdown(void)
{
  std::array<TimerRegistration *, kTimerSlotCount> stopped{};
  {
    const std::scoped_lock lock(s_timerMutex);
    for (std::size_t slotIndex = 0; slotIndex < kTimerSlotCount; slotIndex++) {
      TimerSlot &slot = s_timerSlots[slotIndex];
      if (slot.registration != nullptr) {
        stopped[slotIndex] = slot.registration;
        slot.registration->active.store(false);
        SDL_RemoveTimer(slot.timerId);
        slot = TimerSlot{};
      }
    }
  }
  /* the arena is freed after this (Runtime_Shutdown): no callback may still be inside it */
  for (const TimerRegistration *registration : stopped) {
    WaitWhileCallbackRunning(registration);
  }
  /* s_registrations stays allocated anyway: SDL may still be about to dispatch a removed timer */
}
