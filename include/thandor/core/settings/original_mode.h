/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/settings/original_mode.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_SETTINGS_ORIGINAL_MODE_H
#define THANDOR_CORE_SETTINGS_ORIGINAL_MODE_H

/* Not in the original: the "Original Thandor" master switch. While it is on, every open-thandor addition listed
   in OtFeature behaves as in the original game, whatever its own setting says; the own settings keep their values
   and apply again when the switch is turned off. Bug and crash fixes are not features and stay active. */

enum OtFeature {
    OT_FEATURE_IMPROVED_PATHING,    /* improved pathfinding (world/pathing/mode.h); takes effect with the next session */
    OT_FEATURE_OBJECTIVE_OVERLAY,   /* in-game objective overlay (Z) */
    OT_FEATURE_AUTOSAVE,            /* autosave every minute of local play */
    OT_FEATURE_CAMPAIGN_MAP,        /* campaign map in the frontend instead of the original campaign list */
    OT_FEATURE_MOVIE_VOLUME_BOOST,  /* loudness boost of the quiet movies */
    OT_FEATURE_CUSTOM_MAPS,         /* user maps from maps\ in the single-player list */
    OT_FEATURE_KEY_BINDINGS,        /* user key bindings ([keys]); off = the original keys */
    OT_FEATURE_COUNT
};

/* True while "Original Thandor" is on. */
bool OriginalMode_IsOn();

/* Turns the switch on or off (saved to thandor.ini) and notifies the change listeners when it changed. */
void OriginalMode_Set(bool on);

/* True when the feature may act: the switch is off. The feature's own setting is checked by its owner as before. */
bool OtFeature_Active(OtFeature feature);

/* Called after the switch changed (at most 8 listeners; e.g. key bindings re-apply their tables). */
typedef void OriginalModeChangeListener(bool originalModeOn);
void OriginalMode_AddChangeListener(OriginalModeChangeListener *listener);

#endif /* THANDOR_CORE_SETTINGS_ORIGINAL_MODE_H */
