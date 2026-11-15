#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>
struct UiNodeBase;
void CustomMaps_InstallUi();
void CustomMaps_ReleaseUi();
void CustomMaps_Open(void *);
void CustomMaps_LeaveMenu();
bool CustomMaps_MenuActive();
void CustomMaps_SelectRow();
void CustomMaps_Start();
const uint16_t *CustomMaps_ActiveLevelPath();
const std::vector<std::byte> *CustomMaps_FindAsset(const uint16_t *path);
bool CustomMaps_TryResolveText(uint32_t id, uint16_t **text);

void CustomMaps_InitializeFactionChoice();
void CustomMaps_EnsureAiFactions();
void CustomMaps_LockFactionModes();

void CustomMaps_RecordFactionChoice(uint32_t faction);
