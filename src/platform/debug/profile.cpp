/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/debug/profile.cpp
 * Project code (not in the original game)
 */

/* Step profiler (OPEN_THANDOR_STEP_PROFILE, see profile.h). Only reads the clock. */

#include <algorithm>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <thandor/thandor.h>
#include <thandor/platform/debug/hooks.h>
#include <thandor/platform/debug/profile.h>

namespace {

using Clock = std::chrono::steady_clock;

const char *const SECTION_NAMES[DEBUG_PROFILE_SECTION_COUNT] = {
  "army_update", "plan_route", "group_routes", "yield_replan", "find_blocking", "handle_blocked", "arrival_test",
  "corridor_step", "drive_improved", "formation_slots", "grid_copy", "plan_retry", "plan_direct", "propagate",
  "yield_cap", "yield_budget", "lockstep",
};

struct SectionStats {
  uint64_t calls = 0;
  int64_t totalNs = 0;
  int64_t maxStepNs = 0;
  uint64_t maxStepCalls = 0;
};

struct Window {
  std::vector<int64_t> stepNs;
  std::vector<int64_t> frameNs;
  SectionStats sections[DEBUG_PROFILE_SECTION_COUNT];
  unsigned firstTick = 0;
  unsigned lastTick = 0;
};

bool s_opened = false;
bool s_enabled = false;
int s_interval = 100;
FILE *s_file = nullptr;
FILE *s_rawFile = nullptr;
Clock::time_point s_stepStart;
Clock::time_point s_frameStart;
bool s_frameRunning = false;
bool s_inStep = false;
int s_depth[DEBUG_PROFILE_SECTION_COUNT];
Clock::time_point s_enterTime[DEBUG_PROFILE_SECTION_COUNT];
int64_t s_stepSectionNs[DEBUG_PROFILE_SECTION_COUNT];
uint64_t s_stepSectionCalls[DEBUG_PROFILE_SECTION_COUNT];
Window s_window;
Window s_total;

bool Enabled()
{
  if (!s_opened) {
    const char *value = getenv("OPEN_THANDOR_STEP_PROFILE");
    const char *path = getenv("OPEN_THANDOR_STEP_PROFILE_FILE");
    const char *raw = getenv("OPEN_THANDOR_STEP_PROFILE_RAW");
    s_opened = true;
    if (value != nullptr && atoi(value) > 0) {
      s_enabled = true;
      s_interval = atoi(value) == 1 ? 100 : atoi(value);
      s_file = fopen((path != nullptr && path[0] != '\0') ? path : "stepprofile.txt", "w");
      if (raw != nullptr && raw[0] != '\0') {
        s_rawFile = fopen(raw, "w");
      }
      Thandor_Log("test aid: step profile every %d steps", s_interval);
    }
  }
  return s_enabled;
}

int64_t Percentile(std::vector<int64_t> values, int percent)
{
  size_t index;

  if (values.empty()) {
    return 0;
  }
  std::sort(values.begin(), values.end());
  index = (values.size() * (size_t)percent) / 100;
  if (index >= values.size()) {
    index = values.size() - 1;
  }
  return values[index];
}

int64_t Mean(const std::vector<int64_t> &values)
{
  int64_t sum = 0;

  if (values.empty()) {
    return 0;
  }
  for (int64_t value : values) {
    sum += value;
  }
  return sum / (int64_t)values.size();
}

void WriteWindow(char kind, const Window &window)
{
  size_t steps = window.stepNs.size();

  if (s_file == nullptr || steps == 0) {
    return;
  }
  fprintf(s_file, "%c %u %u %u step %lld %lld %lld frame %lld %lld %lld\n", kind, window.firstTick,
          window.lastTick, (unsigned)steps, (long long)(Mean(window.stepNs) / 1000),
          (long long)(Percentile(window.stepNs, 99) / 1000),
          (long long)(*std::max_element(window.stepNs.begin(), window.stepNs.end()) / 1000),
          (long long)(Mean(window.frameNs) / 1000), (long long)(Percentile(window.frameNs, 99) / 1000),
          window.frameNs.empty() ? 0LL
                                 : (long long)(*std::max_element(window.frameNs.begin(), window.frameNs.end()) / 1000));
  for (int section = 0; section < DEBUG_PROFILE_SECTION_COUNT; section++) {
    const SectionStats &stats = window.sections[section];
    if (stats.calls == 0) {
      continue;
    }
    fprintf(s_file, "  %c%s %s %.2f %.1f %lld %llu\n", kind, "S", SECTION_NAMES[section],
            (double)stats.calls / (double)steps, (double)stats.totalNs / 1000.0 / (double)steps,
            (long long)(stats.maxStepNs / 1000), (unsigned long long)stats.maxStepCalls);
  }
}

void ResetWindow(Window &window)
{
  window.stepNs.clear();
  window.frameNs.clear();
  for (SectionStats &stats : window.sections) {
    stats = SectionStats();
  }
}

}  // namespace

void DebugProfile_SessionStarted()
{
  if (!Enabled()) {
    return;
  }
  ResetWindow(s_window);
  ResetWindow(s_total);
  if (s_file != nullptr) {
    fprintf(s_file, "# session started\n");
    fflush(s_file);
  }
}

void DebugProfile_StepBegin()
{
  if (!Enabled()) {
    return;
  }
  s_inStep = true;
  s_stepStart = Clock::now();
}

void DebugProfile_StepEnd(unsigned simulationTick)
{
  int64_t stepNs;

  if (!s_enabled || !s_inStep) {
    return;
  }
  stepNs = std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - s_stepStart).count();
  /* the lockstep right before the step (commands, their route plans) belongs to it */
  stepNs += s_stepSectionNs[DEBUG_PROFILE_LOCKSTEP];
  s_inStep = false;
  for (Window *window : {&s_window, &s_total}) {
    if (window->stepNs.empty()) {
      window->firstTick = simulationTick;
    }
    window->lastTick = simulationTick;
    window->stepNs.push_back(stepNs);
    for (int section = 0; section < DEBUG_PROFILE_SECTION_COUNT; section++) {
      SectionStats &stats = window->sections[section];
      stats.calls += s_stepSectionCalls[section];
      stats.totalNs += s_stepSectionNs[section];
      stats.maxStepNs = std::max(stats.maxStepNs, s_stepSectionNs[section]);
      stats.maxStepCalls = std::max(stats.maxStepCalls, s_stepSectionCalls[section]);
    }
  }
  if (s_rawFile != nullptr) {
    fprintf(s_rawFile, "%u %lld", simulationTick, (long long)(stepNs / 1000));
    for (int section = 0; section < DEBUG_PROFILE_SECTION_COUNT; section++) {
      fprintf(s_rawFile, " %lld", (long long)(s_stepSectionNs[section] / 1000));
    }
    fprintf(s_rawFile, "\n");
  }
  for (int section = 0; section < DEBUG_PROFILE_SECTION_COUNT; section++) {
    s_stepSectionNs[section] = 0;
    s_stepSectionCalls[section] = 0;
  }
  if ((int)s_window.stepNs.size() >= s_interval) {
    WriteWindow('C', s_total);
    WriteWindow('W', s_window);
    if (s_file != nullptr) {
      fflush(s_file);
    }
    if (s_rawFile != nullptr) {
      fflush(s_rawFile);
    }
    ResetWindow(s_window);
  }
}

void DebugProfile_FrameBegin()
{
  if (!Enabled()) {
    return;
  }
  s_frameStart = Clock::now();
  s_frameRunning = true;
}

void DebugProfile_FrameEnd()
{
  int64_t frameNs;

  if (!s_enabled || !s_frameRunning) {
    return;
  }
  s_frameRunning = false;
  frameNs = std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - s_frameStart).count();
  s_window.frameNs.push_back(frameNs);
  s_total.frameNs.push_back(frameNs);
}

void DebugProfile_Enter(int section)
{
  if (!s_enabled || (unsigned)section >= (unsigned)DEBUG_PROFILE_SECTION_COUNT) {
    return;
  }
  if (s_depth[section]++ == 0) {
    s_enterTime[section] = Clock::now();
  }
}

void DebugProfile_Leave(int section)
{
  if (!s_enabled || (unsigned)section >= (unsigned)DEBUG_PROFILE_SECTION_COUNT || s_depth[section] == 0) {
    return;
  }
  s_stepSectionCalls[section]++;
  if (--s_depth[section] == 0) {
    s_stepSectionNs[section] +=
         std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - s_enterTime[section]).count();
  }
}
