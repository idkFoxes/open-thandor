/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/render/light_transitions.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/render/light_transitions.h>
#include <thandor/thandor.h>

/* Starts fading out a dynamic light (shading record) over fadeOutTicks: a negative transition duration
   makes InterpolationStateTable_Advance256ByTicks shrink the radius to zero and then free the light. A
   light still fading in keeps its current fraction; a zero duration switches the light off at once.
*/
void InterpolationState_SetNegatedTargetAndRescaleProgress
          (GraphicsTransitionTickCount fadeOutTicks,GraphicsShadingRuntimeRecord *shadingRecord)

{
  GraphicsTransitionTickCount negatedDuration;
  GraphicsTransitionTickCount fadeInDuration;
  GraphicsTransitionTickCount rescaledElapsed;
  PackedRgb24 switchOffValue; /* the value the switch-off path stores into all four fields */

  if (shadingRecord == nullptr) {
    return;
  }
  negatedDuration = -fadeOutTicks;
  /* Original quirk: the switch-off path stores -fadeOutTicks (not 0) when it is reached without the
     fade-in rescale, i.e. when the light was already fading out or fadeOutTicks <= 0. */
  switchOffValue = (PackedRgb24)negatedDuration;
  if ((negatedDuration < 0) && (-1 < shadingRecord->radiusTransitionDurationTicks)) {
    if (shadingRecord->radiusTransitionDurationTicks == 0) {
      /* steady light: elapsed runs from -fadeOutTicks up to 0 */
      shadingRecord->radiusTransitionDurationTicks = negatedDuration;
      shadingRecord->radiusTransitionElapsedTicks = negatedDuration;
      return;
    }
    /* fading in: exchange in the new duration, rescale elapsed to keep the reached radius
       fraction */
    fadeInDuration = shadingRecord->radiusTransitionDurationTicks;
    shadingRecord->radiusTransitionDurationTicks = negatedDuration;
    rescaledElapsed = (int)(((int64_t)negatedDuration * (int64_t)shadingRecord->radiusTransitionElapsedTicks)
                            / (int64_t)fadeInDuration);
    shadingRecord->radiusTransitionElapsedTicks = rescaledElapsed;
    if (rescaledElapsed != 0) {
      return;
    }
    switchOffValue = 0;
  }
  /* switch-off path */
  ((PackedRgb24 *)&shadingRecord->squaredRadiusQ24)[1] = switchOffValue; /* high dword */
  ((PackedRgb24 *)&shadingRecord->squaredRadiusQ24)[0] = switchOffValue;
  shadingRecord->packedColorRgbActive = switchOffValue;
  shadingRecord->targetRadiusQ12 = switchOffValue;
}

/* Advances the radius transitions of all 256 dynamic lights (shading records) by elapsedTicks. An active
   light in transition gets radius = target * elapsed / duration (squared for the shading pass); a finished
   fade-in becomes steady, a finished fade-out (negative duration) frees the light.
*/
void InterpolationStateTable_Advance256ByTicks(GraphicsElapsedTickCount elapsedTicks)

{
  int durationTicks;
  int currentRadius;
  int remainingCount;
  GraphicsShadingRuntimeRecord *shadingRecord;

  shadingRecord = g_GraphicsShadingRuntimeRecords;
  for (remainingCount = GRAPHICS_SHADING_RUNTIME_RECORD_COUNT; remainingCount != 0; remainingCount--) {
    durationTicks = shadingRecord->radiusTransitionDurationTicks;
    if ((shadingRecord->packedColorRgbActive != 0) && (durationTicks != 0)) {
      /* the radius uses the elapsed time from before this step */
      currentRadius = (int)(((int64_t)shadingRecord->targetRadiusQ12 *
                    (int64_t)shadingRecord->radiusTransitionElapsedTicks) / (int64_t)durationTicks);
      shadingRecord->radiusTransitionElapsedTicks = shadingRecord->radiusTransitionElapsedTicks + elapsedTicks;
      shadingRecord->squaredRadiusQ24 = (int64_t)currentRadius * (int64_t)currentRadius;
      if (durationTicks < 0) {
        if ((uint32_t)shadingRecord->radiusTransitionElapsedTicks < INTERPOLATION_SIGN_BIT) { /* elapsed >= 0 */
          /* fade-out finished: free the light */
          shadingRecord->squaredRadiusQ24 = 0;
          shadingRecord->targetRadiusQ12 = 0;
          shadingRecord->packedColorRgbActive = 0;
          shadingRecord->radiusTransitionElapsedTicks = 0;
          shadingRecord->radiusTransitionDurationTicks = 0;
        }
      }
      else if (durationTicks < shadingRecord->radiusTransitionElapsedTicks) {
        /* fade-in complete: the light stays at its last radius */
        shadingRecord->radiusTransitionElapsedTicks = 0;
        shadingRecord->radiusTransitionDurationTicks = 0;
      }
    }
    shadingRecord++;
  }
}
