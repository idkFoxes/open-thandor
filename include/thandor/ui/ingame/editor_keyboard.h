/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/editor_keyboard.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_EDITOR_KEYBOARD_H
#define THANDOR_UI_INGAME_EDITOR_KEYBOARD_H

#include <thandor/core/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* Info texts the world view cycles through with Ctrl+I (worldViewCyclingInfoText holds the text resource id) */
inline constexpr int32_t TEXT_ID_WORLD_VIEW_INFO_FIRST = 0x112;
inline constexpr int32_t TEXT_ID_WORLD_VIEW_INFO_LAST = 0x117;
/* Step of the editor's light direction and field origin hotkeys (Ctrl/Shift + arrow keys) */
inline constexpr int32_t EDITOR_ADJUST_STEP = 0x400;

void InGameUiRootKeyboardFallback_DispatchCommandByCodeAndModifierFlags
          (uint32_t keyboardStateMask,uint32_t keyboardEventCode,UiRootNode *uiRoot);

extern UiCommandRuntimeRecordPrefix *g_UiHoverSelectionRecord;

extern uint32_t g_UiCommandModeGArmyAssetId;
extern PckArmyAssetIdCatalog g_UiCommandMode4ArmyAssetId;

#endif /* THANDOR_UI_INGAME_EDITOR_KEYBOARD_H */
