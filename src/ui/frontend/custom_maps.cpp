#include <algorithm>
#include <array>
#include <cstring>
#include <optional>
#include <thandor/assets/scenario/custom_project.h>
#include <thandor/assets/text/resources.h>
#include <thandor/gameplay/session/level_new.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/thandor.h>
#include <thandor/ui/frontend/custom_maps.h>
#include <thandor/ui/frontend/mission_briefing.h>
#include <thandor/ui/frontend/scenario_selection.h>
namespace {
constexpr unsigned ActionPage = 0x7e, CustomAlias = 0x10000,
                   DescriptionId = 0x7e0000;
struct Row {
  uint16_t title[128], planet[32], size[32], players[12], author[128];
};
struct UiExtra {
  UiFramedTextButtonControl button;
  UiListControl list;
  uint32_t tail[11];
  UiFocusProxyControl headers[5];
  uint16_t headerText[5][32];
  uint16_t description[2048];
};
UiExtra *ui{};
Row *rows{};
Ptr32<void> *pointers{};
std::vector<custom_maps::Project> catalog;
std::vector<std::string> errors;
std::optional<custom_maps::Project> active;
std::array<uint16_t, 256> levelPath{};
uint16_t *gameTexts{};
bool menu{};
uint32_t chosenFaction{};
UiActionHandlerPage actions{};
// Escape rich-text opcodes and control characters in user metadata. Encode
// UTF-8 via the filesystem converter so non-ASCII file names and authors use
// UTF-16 on Windows as well.
template <size_t N> void text(uint16_t (&out)[N], const std::string &input) {
  std::u16string value;
  try {
    value = std::filesystem::path(
                std::u8string(reinterpret_cast<const char8_t *>(input.data()),
                              input.size()))
                .u16string();
  } catch (...) {
    value = u"?";
  }
  size_t n = std::min(value.size(), N - 1);
  for (size_t i = 0; i < n; ++i)
    out[i] = (value[i] >= 0x8000 || value[i] < 32)
                 ? (value[i] == '\n' ? 10 : '?')
                 : value[i];
  out[n] = 0;
}
void toggleStart(bool enabled) {
  auto *root = &g_FrontendRootNode->frontendRoot.root.base;
  if (enabled)
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_START_SELECTED_GAME, root);
  else
    UiNodeList_SuppressActionId(FRONTEND_ACTION_START_SELECTED_GAME, root);
}
void freeRows() {
  if (rows)
    g_MemoryApi.free(rows);
  if (pointers)
    g_MemoryApi.free(pointers);
  rows = nullptr;
  pointers = nullptr;
}
void unselect() {
  for (auto *b : {&g_FrontendRootNode->loadGameTabButton,
                  &g_FrontendRootNode->singleGameTabButton,
                  &g_FrontendRootNode->campaignsTabButton})
    b->selectable.stateFlags &= ~UI_SELECTABLE_SELECTED_OR_CHECKED;
}
} // namespace
bool CustomMaps_MenuActive() { return menu; }
void CustomMaps_InstallUi() {
  CustomMaps_ReleaseUi();
  void *p = nullptr;
  if (g_MemoryApi.alloc(sizeof(UiExtra), &p) != 0)
    return;
  ui = new (p) UiExtra{};
  auto *root = g_FrontendRootNode;
  ui->button = root->campaignsTabButton;
  auto &b = ui->button.selectable.base;
  b.topOffset += 32;
  b.bottomOffset += 32;
  b.nextSibling = root->campaignsTabButton.selectable.base.nextSibling;
  root->campaignsTabButton.selectable.base.nextSibling = &b;
  ui->button.textResourceId = TEXT_ID_CUSTOM_MAPS;
  ui->button.selectable.actionId = ActionPage * 256;
  ui->button.selectable.stateFlags &= ~UI_SELECTABLE_SELECTED_OR_CHECKED;
  ui->list = root->missionsList;
  ui->list.base.firstChild = UI_NODE_NONE;
  ui->list.base.nextSibling = UI_NODE_NONE;
  ui->list.rowSlots = nullptr;
  ui->list.rowCount = 0;
  ui->list.selectedRowSlot = nullptr;
  ui->list.actionId = ActionPage * 256 + 1;
  ui->list.columnCount = 5;
  const int widths[] = {134, 60, 61, 65, 144};
  const size_t offsets[] = {offsetof(Row, title), offsetof(Row, planet),
                            offsetof(Row, size), offsetof(Row, players),
                            offsetof(Row, author)};
  std::array<UiListColumn, 5> columns{};
  for (unsigned i = 0; i < 5; ++i)
    columns[i] = {widths[i], uint32_t(offsets[i])};
  std::memcpy(reinterpret_cast<unsigned char *>(&ui->list) +
                  offsetof(UiListControl, columns),
              columns.data(), sizeof(columns));
  const char *names[] = {"Map", "Planet", "Size", "Players", "Author"};
  int left = -174;
  for (unsigned i = 0; i < 5; ++i) {
    ui->headers[i] = root->missionsLabel;
    auto &header = ui->headers[i].base;
    header.leftOffset = left;
    header.rightOffset = left + widths[i];
    left += widths[i];
    header.nextSibling = i + 1 < 5 ? &ui->headers[i + 1].base
                                   : &root->missionDescriptionText.base;
    ui->headers[i].text = THANDOR_PTR32_BITS(DescriptionId + 1 + i);
    text(ui->headerText[i], names[i]);
  }
  actions.handlers[0] = CustomMaps_Open;
  actions.handlers[1] = [](void *) { CustomMaps_SelectRow(); };
  UiActionHandlers_SetPage(ActionPage, &actions);
  UiRootStack_Relayout();
}
void CustomMaps_ReleaseUi() {
  menu = false;
  freeRows();
  catalog.clear();
  errors.clear();
  if (ui) {
    g_MemoryApi.free(ui);
    ui = nullptr;
  }
  UiActionHandlers_SetPage(ActionPage, nullptr);
}
void CustomMaps_LeaveMenu() {
  menu = false;
  if (ui && g_FrontendRootNode) {
    g_FrontendRootNode->missionsScroller.base.firstChild =
        &g_FrontendRootNode->missionsList.base;
    g_FrontendRootNode->missionsLabel.text = THANDOR_PTR32_BITS(0x215b);
    g_FrontendRootNode->missionsLabel.base.nextSibling =
        &g_FrontendRootNode->missionDescriptionText.base;
    ui->button.selectable.stateFlags &= ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    if (g_SessionNetworkRoleFlags != SESSION_NETWORK_ROLE_LOCAL)
      ui->button.selectable.base.nodeFlags |= UI_NODE_SUPPRESSED;
    else
      ui->button.selectable.base.nodeFlags &= ~UI_NODE_SUPPRESSED;
  }
  // Only called when another scenario type is selected, after a running game
  // has ended.
  active.reset();
  levelPath.fill(0);
}
void CustomMaps_Open(void *) {
  if (!ui || g_SessionNetworkRoleFlags != SESSION_NETWORK_ROLE_LOCAL)
    return;
  menu = true;
  unselect();
  ui->button.selectable.stateFlags |= UI_SELECTABLE_SELECTED_OR_CHECKED;
  auto *root = g_FrontendRootNode;
  UiPageStack_SetActiveIndex(
      SCENARIO_SELECTION_TAB_SINGLE_GAMES,
      UiLayoutContainerControl_AsPageStack(&root->gameSelectTabStack));
  root->missionsScroller.base.firstChild = &ui->list.base;
  root->missionsLabel.text = THANDOR_PTR32_BITS(TEXT_RESOURCE_ID_NONE);
  root->missionsLabel.base.nextSibling = &ui->headers[0].base;
  ui->list.rowSlots = nullptr;
  ui->list.selectedRowSlot = nullptr;
  ui->list.rowCount = 0;
  freeRows();
  catalog.clear();
  errors.clear();
  const auto directory =
      std::filesystem::path(std::u16string(
          reinterpret_cast<const char16_t *>(g_ExecutableDirectoryUtf16))) /
      "maps";
  try {
    for (const auto &file : custom_maps::discover(directory)) {
      custom_maps::Project p;
      std::string error;
      try {
        p = custom_maps::Project::load(file);
        p.assets.clear();
      } catch (const std::exception &e) {
        p.file = file;
        p.title = file.stem().string();
        p.author = "Unknown";
        error = e.what();
        Thandor_Log("Custom maps: rejected %s: %s", file.string().c_str(),
                    e.what());
      }
      catalog.push_back(std::move(p));
      errors.push_back(std::move(error));
    }
  } catch (const std::exception &e) {
    Thandor_Log("Custom maps scan: %s", e.what());
  }
  if (catalog.empty()) {
    text(ui->description, "No .thmap files found. Put editor projects in the "
                          "maps folder beside thandor.exe.");
    root->missionDescriptionText.text = THANDOR_PTR32_BITS(DescriptionId);
    toggleStart(false);
    UiRootStack_Relayout();
    return;
  }
  void *r = nullptr;
  void *ptr = nullptr;
  if (catalog.size() > 4096 ||
      g_MemoryApi.alloc(uint32_t(catalog.size() * sizeof(Row)), &r) != 0 ||
      g_MemoryApi.alloc(uint32_t(catalog.size() * sizeof(Ptr32<void>)), &ptr) !=
          0) {
    if (r)
      g_MemoryApi.free(r);
    text(ui->description, "Unable to allocate the custom-map list.");
    root->missionDescriptionText.text = THANDOR_PTR32_BITS(DescriptionId);
    toggleStart(false);
    return;
  }
  rows = static_cast<Row *>(r);
  pointers = static_cast<Ptr32<void> *>(ptr);
  for (size_t i = 0; i < catalog.size(); ++i) {
    const auto &p = catalog[i];
    rows[i] = {};
    text(rows[i].title, p.title);
    text(rows[i].author, p.author);
    const auto *planet =
        TextResource_Resolve(p.planet + TEXT_ID_LEVEL_COLUMN50_BASE);
    size_t unit = 0;
    while (unit + 1 < std::size(rows[i].planet) && planet[unit]) {
      rows[i].planet[unit] = planet[unit];
      ++unit;
    }
    rows[i].planet[unit] = 0;
    if (unit == 0)
      text(rows[i].planet, "Planet " + std::to_string(p.planet + 1));
    text(rows[i].size, errors[i].empty() ? std::to_string(p.width) + "x" +
                                               std::to_string(p.height)
                                         : "Invalid");
    text(rows[i].players, std::to_string(p.total_factions));
    pointers[i] = &rows[i];
  }
  UiPointerList_InitializeColumnLayout(catalog.size(), pointers,
                                       UiListControl_AsPointerList(&ui->list));
  UiPointerList_SelectColumnListIndex(0,
                                      UiListControl_AsPointerList(&ui->list));
  CustomMaps_SelectRow();
  UiRootStack_Relayout();
}
void CustomMaps_SelectRow() {
  if (!ui || !menu)
    return;
  bool confirmed = false;
  const auto row = UiPointerList_GetSelectedIndexAndConfirmed(
      UiListControl_AsPointerList(&ui->list), &confirmed);
  if (row >= catalog.size()) {
    toggleStart(false);
    return;
  }
  const auto &p = catalog[row];
  text(ui->description, errors[row].empty()
                            ? (p.texts.size() > 1 ? p.texts[1] : p.title)
                            : "Cannot load this map: " + errors[row]);
  g_FrontendRootNode->missionDescriptionText.text =
      THANDOR_PTR32_BITS(DescriptionId);
  toggleStart(errors[row].empty());
  if (confirmed && errors[row].empty())
    CustomMaps_Start();
}
void CustomMaps_Start() {
  if (!menu || !ui)
    return;
  const auto index = UiPointerList_GetSelectedIndexAndConfirmed(
      UiListControl_AsPointerList(&ui->list), nullptr);
  if (index >= catalog.size() || !errors[index].empty())
    return;
  try {
    auto project = custom_maps::Project::load(catalog[index].file);
    const auto &level = project.assets.at(project.level_entry);
    uint32_t error = 0;
    if (!NewLevel_ValidateImage(reinterpret_cast<LevelAssetRuntimePrefix *>(
                                    const_cast<std::byte *>(level.data())),
                                static_cast<uint32_t>(level.size()), 1, &error))
      throw std::runtime_error("Invalid game level image (" +
                               std::to_string(error) + ")");
    if (!gameTexts) {
      void *p = nullptr;
      if (g_MemoryApi.alloc(16 * 2048 * 2, &p) != 0)
        throw std::runtime_error("Not enough memory for map text");
      gameTexts = static_cast<uint16_t *>(p);
    }
    // Own the text in the low-address arena because the recovered UI stores
    // 32-bit pointers.
    for (unsigned i = 0; i < 16; ++i) {
      auto &line = *reinterpret_cast<uint16_t (*)[2048]>(gameTexts + i * 2048);
      text(line, i == 0 ? project.title : project.texts[i]);
      size_t length = 0;
      while (length < 2046 && line[length])
        ++length;
      for (size_t j = length + 1; j > 0; --j)
        line[j] = line[j - 1];
      line[0] = FRONTEND_TEXT_STYLE_NORMAL;
    }
    text(*reinterpret_cast<uint16_t (*)[256]>(levelPath.data()),
         project.level_entry);
    active = std::move(project);
    chosenFaction = active->default_human_faction;
    g_FrontendPlayerRuntimeBlockCount = 1;
    Thandor_Log("Custom maps: launching %s, %ux%u, %u project assets; no "
                "numbered catalog slot",
                active->file.filename().string().c_str(), active->width,
                active->height, static_cast<unsigned>(active->assets.size()));
    Resource_Release(g_FrontendLoadedCampaignAsset);
    g_FrontendLoadedCampaignAsset = nullptr;
    FrontendScenarioSession_LoadOrRequestLevelAsset(0, 0, 0, 0);
  } catch (const std::exception &e) {
    text(ui->description, std::string("Cannot load this map: ") + e.what());
    g_FrontendRootNode->missionDescriptionText.text =
        THANDOR_PTR32_BITS(DescriptionId);
    toggleStart(false);
  }
}
const uint16_t *CustomMaps_ActiveLevelPath() {
  return active ? levelPath.data() : nullptr;
}
const std::vector<std::byte> *CustomMaps_FindAsset(const uint16_t *path) {
  if (!active)
    return nullptr;
  try {
    size_t n = 0;
    while (n < 246 && path[n])
      ++n;
    if (n == 246)
      return nullptr;
    auto u8 = std::filesystem::path(
                  std::u16string(reinterpret_cast<const char16_t *>(path), n))
                  .generic_u8string();
    auto key = custom_maps::normalized_path(
        std::string(reinterpret_cast<const char *>(u8.data()), u8.size()));
    auto i = active->assets.find(key);
    return i == active->assets.end() ? nullptr : &i->second;
  } catch (...) {
    return nullptr;
  }
}
bool CustomMaps_TryResolveText(uint32_t id, uint16_t **out) {
  if (ui && menu && id == DescriptionId) {
    *out = ui->description;
    return true;
  }
  if (ui && menu && id > DescriptionId && id <= DescriptionId + 5) {
    *out = ui->headerText[id - DescriptionId - 1];
    return true;
  }
  if (!active || !gameTexts)
    return false;
  if (id == CustomAlias + TEXT_ID_LEVEL_TITLE_BASE) {
    *out = gameTexts;
    return true;
  }
  const unsigned first = CustomAlias * TEXT_ID_LEVEL_DESCRIPTION_STRIDE +
                         TEXT_ID_LEVEL_DESCRIPTION_BASE;
  if (id >= first && id < first + 15) {
    *out = gameTexts + (id - first + 1) * 2048;
    return true;
  }
  return false;
}

void CustomMaps_EnsureAiFactions() {
  if (!active || g_SessionNetworkRoleFlags != SESSION_NETWORK_ROLE_LOCAL)
    return;
  for (uint32_t f = 1; f <= active->total_factions; ++f)
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[f] =
        FACTION_RUNTIME_LIFECYCLE_ACTIVE;
}
void CustomMaps_LockFactionModes() {
  if (!active || g_SessionNetworkRoleFlags != SESSION_NETWORK_ROLE_LOCAL ||
      !g_FrontendRootNode)
    return;
  for (uint32_t f = 1; f <= active->total_factions; ++f) {
    g_FrontendRootNode
        ->NodeAt<UiFramedTextButtonControl>(
            g_FrontendTaskAssignmentControlOffsets.playerControls
                .offsets[f - 1])
        ->selectable.stateFlags |= FRONTEND_CONTROL_INACTIVE;
    if (!active->human_allowed[f])
      g_FrontendRootNode
          ->NodeAt<UiTextButtonControl>(g_FrontendTaskAssignmentControlOffsets
                                            .selectionRows.offsets[f - 1])
          ->selectable.stateFlags |= FRONTEND_CONTROL_INACTIVE;
  }
}
void CustomMaps_InitializeFactionChoice() {
  if (!active || g_SessionNetworkRoleFlags != SESSION_NETWORK_ROLE_LOCAL)
    return;
  CustomMaps_EnsureAiFactions();
  FrontendFactionSetup_ChooseFaction(
      g_LocalPlayerRuntimeId, 0, 0,
      static_cast<int>(chosenFaction ? chosenFaction
                                     : active->default_human_faction) -
          1);
  Thandor_Log("Custom maps: default player faction %u, all other active "
              "factions assigned to AI",
              active->default_human_faction);
}

void CustomMaps_RecordFactionChoice(uint32_t faction) {
  if (active && g_SessionNetworkRoleFlags == SESSION_NETWORK_ROLE_LOCAL &&
      faction >= 1 && faction <= active->total_factions &&
      active->human_allowed[faction]) {
    chosenFaction = faction;
    Thandor_Log("Custom maps: player faction %u, %u computer factions", faction,
                active->total_factions - 1);
  }
}
