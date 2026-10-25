/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/settings/original_mode.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/settings/original_mode.h>

/* STUB (base commit of experimental/original-mode): the switch is always off and not yet saved. The core package
   replaces this file with the thandor.ini-backed implementation; the feature packages only call the header. */

static OriginalModeChangeListener *s_OriginalModeListeners[8];
static int s_OriginalModeListenerCount;
static bool s_OriginalModeOn;

bool OriginalMode_IsOn()
{
    return s_OriginalModeOn;
}

void OriginalMode_Set(bool on)
{
    if (on == s_OriginalModeOn) {
        return;
    }
    s_OriginalModeOn = on;
    for (int index = 0; index < s_OriginalModeListenerCount; index++) {
        s_OriginalModeListeners[index](on);
    }
}

bool OtFeature_Active(OtFeature feature)
{
    (void)feature;
    return !s_OriginalModeOn;
}

void OriginalMode_AddChangeListener(OriginalModeChangeListener *listener)
{
    if (s_OriginalModeListenerCount < 8) {
        s_OriginalModeListeners[s_OriginalModeListenerCount++] = listener;
    }
}
