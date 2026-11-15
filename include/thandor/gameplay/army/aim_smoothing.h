/*
 * Open Thandor - shared weapon/turret aim stepping.
 *
 * This is deliberately a small value-type helper so tooling (the integrated map editor and
 * Simulation Lab) executes the same recovered aim state machine as the live game without
 * constructing pointer-heavy runtime overlays. The live runtime wrappers in model_rules.cpp
 * remain responsible for dirty flags and storage layout.
 */
#ifndef THANDOR_GAMEPLAY_ARMY_AIM_SMOOTHING_H
#define THANDOR_GAMEPLAY_ARMY_AIM_SMOOTHING_H

#include <stdint.h>

struct ArmyAimYawStepResult {
    uint32_t angle16;
    int32_t velocity16;
    uint8_t dirty;
    uint8_t outsideTolerance;
};

struct ArmyAimPitchStepResult {
    uint32_t angle16;
    int32_t velocity16;
    uint32_t clampedTarget16;
    uint32_t runtimeReturnValue;
    uint8_t dirty;
};

inline ArmyAimYawStepResult ArmyAim_StepYawTowardTarget(
    uint32_t currentAngle16, int32_t currentVelocity16, uint32_t targetAngle16,
    int32_t rateLimitAnglePerTick, int32_t accelerationAnglePerTick,
    uint32_t stepTicks, uint32_t toleranceAngle16) noexcept
{
    constexpr uint32_t kMask = 0xFFFFU;
    constexpr uint32_t kHalfTurn = 0x8000U;
    constexpr uint32_t kFullTurn = 0x10000U;

    uint32_t angle = currentAngle16 & kMask;
    targetAngle16 &= kMask;
    const uint32_t delta = (targetAngle16 - angle) & kMask;
    const int32_t step = currentVelocity16 * static_cast<int32_t>(stepTicks);
    int32_t velocity = currentVelocity16;
    bool snap = false;

    if (delta <= kHalfTurn) {
        if (step < 0) {
            velocity = 0;
        } else if (delta <= static_cast<uint32_t>(step)) {
            snap = true;
        } else {
            angle = (angle + static_cast<uint32_t>(step)) & kMask;
            const int32_t accelerated = currentVelocity16 +
                static_cast<int32_t>(stepTicks) * accelerationAnglePerTick;
            velocity = accelerated < rateLimitAnglePerTick ? accelerated : rateLimitAnglePerTick;
        }
    } else if (step > 0) {
        velocity = 0;
    } else if (static_cast<uint32_t>(step + static_cast<int32_t>(kFullTurn)) <= delta) {
        snap = true;
    } else {
        angle = (angle + static_cast<uint32_t>(step)) & kMask;
        const int32_t accelerated = currentVelocity16 -
            static_cast<int32_t>(stepTicks) * accelerationAnglePerTick;
        velocity = accelerated > -rateLimitAnglePerTick ? accelerated : -rateLimitAnglePerTick;
    }

    bool dirty = false;
    bool outside = false;
    if (snap) {
        velocity = 0;
        if (targetAngle16 != (currentAngle16 & kMask)) {
            angle = targetAngle16;
            dirty = true;
        }
    } else {
        dirty = true;
        angle &= kMask;
        const uint32_t residual = (angle - targetAngle16) & kMask;
        outside = toleranceAngle16 < residual && residual < kFullTurn - toleranceAngle16;
    }
    return {angle, velocity, static_cast<uint8_t>(dirty), static_cast<uint8_t>(outside)};
}

inline ArmyAimPitchStepResult ArmyAim_StepPitchTowardTarget(
    uint32_t currentAngle, int32_t currentVelocity, uint32_t targetAngle,
    int32_t rateLimitAnglePerTick, int32_t accelerationAnglePerTick,
    int32_t minimumPitchAngle, int32_t maximumPitchAngle,
    uint32_t stepTicks) noexcept
{
    constexpr uint32_t kMask = 0xFFFFU;
    uint32_t pitchAngle = currentAngle;
    uint32_t clampedTarget = targetAngle;
    if (maximumPitchAngle < static_cast<int32_t>(targetAngle)) {
        clampedTarget = static_cast<uint32_t>(maximumPitchAngle);
    }
    if (static_cast<int32_t>(clampedTarget) < minimumPitchAngle) {
        clampedTarget = static_cast<uint32_t>(minimumPitchAngle);
    }

    const int32_t pitchStep = currentVelocity * static_cast<int32_t>(stepTicks);
    int32_t velocity = currentVelocity;
    bool snap = true;
    if (clampedTarget != pitchAngle) {
        if (static_cast<int32_t>(pitchAngle) <= static_cast<int32_t>(clampedTarget)) {
            if (pitchStep < 0) {
                velocity = 0;
                snap = false;
            } else if (pitchStep < static_cast<int32_t>(clampedTarget - pitchAngle)) {
                pitchAngle = pitchAngle + static_cast<uint32_t>(pitchStep);
                const int32_t accelerated = currentVelocity +
                    static_cast<int32_t>(stepTicks) * accelerationAnglePerTick;
                velocity = accelerated < rateLimitAnglePerTick ? accelerated : rateLimitAnglePerTick;
                snap = false;
            }
        } else if (pitchStep > 0) {
            velocity = 0;
            snap = false;
        } else if (static_cast<int32_t>(clampedTarget - pitchAngle) < pitchStep) {
            pitchAngle = pitchAngle + static_cast<uint32_t>(pitchStep);
            const int32_t accelerated = currentVelocity -
                static_cast<int32_t>(stepTicks) * accelerationAnglePerTick;
            velocity = accelerated > -rateLimitAnglePerTick ? accelerated : -rateLimitAnglePerTick;
            snap = false;
        }
    }

    bool dirty = false;
    uint32_t runtimeReturn = 0;
    if (!snap) {
        dirty = true;
        runtimeReturn = (pitchAngle - clampedTarget) & kMask;
    } else {
        velocity = 0;
        if (clampedTarget != currentAngle) {
            pitchAngle = clampedTarget;
            dirty = true;
        }
        runtimeReturn = clampedTarget;
    }
    return {pitchAngle, velocity, clampedTarget, runtimeReturn, static_cast<uint8_t>(dirty)};
}

#endif
