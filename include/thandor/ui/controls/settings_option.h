/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/settings_option.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_SETTINGS_OPTION_H
#define THANDOR_UI_CONTROLS_SETTINGS_OPTION_H

#include <thandor/core/types.h>
#include <thandor/core/settings/persistent.h>
#include <thandor/ui/controls/selectable.h>
#include <thandor/ui/frontend/types.h>

/* Shared bodies of the frontend and in-game settings handlers that store
   a checkbox as one bit of a persistent option word or a slider value as a persistent setting. */

/* Stores the checkbox state as bit of the option word at setting: reads the word (defaultValue when it is not
   saved), asks whether the control is selected, runs sideEffect(selected) and writes the word with the bit set
   or cleared. Read and IsSelected have no side effects, so callers that asked them in the other order behave
   the same. */
template<typename SideEffect>
inline void PersistentOption_ApplyCheckbox(UiSelectableControl *control,PersistentSettingsByteOffset setting,
          uint32_t bit,SideEffect sideEffect,PersistentSettingsValue defaultValue = 0)

{
  uint32_t optionFlags;
  Bool8 isSelected;

  optionFlags = PersistentSettings_Read(defaultValue,setting);
  isSelected = (Bool8)UiSelectableControl_IsSelected(control);
  sideEffect(isSelected);
  PersistentSettings_Write(isSelected ? optionFlags | bit : optionFlags & ~bit,setting);
}

/* The same without a side effect. */
inline void PersistentOption_ApplyCheckbox(UiSelectableControl *control,PersistentSettingsByteOffset setting,
          uint32_t bit)

{
  PersistentOption_ApplyCheckbox(control,setting,bit,[](Bool8) {});
}

/* The two above for a bit of an option-flag enum (PersistentSoundOptionFlags and the like): the same dword
   operations on its bits. */
template<ThandorFlagEnum E,typename SideEffect>
inline void PersistentOption_ApplyCheckbox(UiSelectableControl *control,PersistentSettingsByteOffset setting,E bit,
          SideEffect sideEffect,E defaultValue = E{})

{
  PersistentOption_ApplyCheckbox(control,setting,ToBits(bit),sideEffect,ToBits(defaultValue));
}

template<ThandorFlagEnum E>
inline void PersistentOption_ApplyCheckbox(UiSelectableControl *control,PersistentSettingsByteOffset setting,E bit)

{
  PersistentOption_ApplyCheckbox(control,setting,ToBits(bit));
}

/* Saves the slider's bound value as setting and returns it (for the callers that also apply it at once). */
inline PersistentSettingsValue PersistentOption_StoreSlider(UiSettingsValueControl *control,
          PersistentSettingsByteOffset setting)

{
  PersistentSettingsValue value;

  value = control->boundValue;
  PersistentSettings_Write(value,setting);
  return value;
}

#endif /* THANDOR_UI_CONTROLS_SETTINGS_OPTION_H */
