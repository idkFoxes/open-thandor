/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/campaign_tab.cpp
 * Project code (not in the original game)
 */

/* The campaign tab with the campaign map (docs/plans/campaign_map.md 5.5, 6.1, 6.2, 7.1-7.4).

   Page structure. The campaign page of gameSelectTabStack is one sibling chain: campaignsScroller (with
   campaignsList), campaignsLabel, campaignDescriptionText, then the nodes of the map (campaignButtons[6],
   campaignMap and the four texts). While the map is shown, the page starts at campaignButtons[0], so the old
   list, its label and its description are neither drawn nor hit; they stay filled and selected as before. The
   list page is restored (page head campaignsScroller, map nodes suppressed and hidden) before every rebuild of
   the list and whenever the map cannot be shown, so the original code always acts on the original page.

   Campaigns. The list rows are grouped by the campaign fingerprint (5.6); each group is one button, standing for
   its base row. A group's graph, layout and node states are built from the base row's .cgn (read natively),
   the level info and the progress file, and kept in this module's static storage while the page is shown (the
   map control keeps pointers to them).

   Network games. Selections travel as the original select command (ScenarioCatalog_SelectCampaignAndShow
   Description) with the list row; a host of open-thandor puts the selected mission's level id into its unused
   first argument, which every peer receives (an original peer ignores it). Clients show the host's selection
   with their own progress and cannot select or start themselves. */

#include <thandor/ui/frontend/campaign_tab.h>

#include <stdio.h>
#include <string.h>
#include <map>
#include <optional>
#include <string>
#include <vector>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/assets/text/richtext.h>
#include <thandor/gameplay/session/campaign_progress.h>
#include <thandor/gameplay/session/campaign_start.h>
#include <thandor/ui/frontend/campaign_level_info.h>
#include <thandor/ui/frontend/campaign_map.h>
#include <thandor/ui/frontend/campaign_map_control.h>
#include <thandor/ui/frontend/scenario_selection.h>

/* code units of ScenarioCatalogDisplayRecord.identifier (the .cgn base name of a campaign row) */
static constexpr size_t CAMPAIGN_TAB_ROW_NAME_UNITS =
    sizeof(ScenarioCatalogDisplayRecord::identifier) / sizeof(ScenarioCatalogDisplayRecord::identifier[0]);
/* the button row (plan 6.1): template offsets of its left and right edge, and the gap between two buttons */
static constexpr int32_t CAMPAIGN_TAB_BUTTON_ROW_LEFT = -176;
static constexpr int32_t CAMPAIGN_TAB_BUTTON_ROW_RIGHT = 304;
static constexpr int32_t CAMPAIGN_TAB_BUTTON_GAP = 6;
/* room a label keeps inside its button: the frame on both sides, and the check mark of a finished campaign */
static constexpr int32_t CAMPAIGN_TAB_BUTTON_TEXT_PADDING = 12;
static constexpr int32_t CAMPAIGN_TAB_BUTTON_CHECK_MARK_ROOM = 10;
/* the map frame of the template (plan 6.1), used until the node has been laid out */
static constexpr int32_t CAMPAIGN_TAB_MAP_FRAME_WIDTH = 480;
static constexpr int32_t CAMPAIGN_TAB_MAP_FRAME_HEIGHT = 161;
/* the campaign page of gameSelectTabStack */
static constexpr int CAMPAIGN_TAB_PAGE_INDEX = SCENARIO_SELECTION_TAB_CAMPAIGNS;

/* One campaign of the button row: a group of list rows with the same fingerprint. */
struct CampaignTabCampaign {
  UiListRowIndex baseRow;                   /* the list row of the group's base campaign */
  std::string name;                         /* the base row's .cgn base name: the progress section key */
  uint32_t titleIndex;                      /* the base row's campaign title index */
  std::vector<uint8_t> assetBytes;          /* the base campaign's .cgn (checked with CampaignAsset_Fits) */
  std::vector<UiListRowIndex> memberRows;   /* every list row of the group, the base row included */
  CampaignGraph graph;
  CampaignMapLayout layout;
  std::vector<CampaignMapNodeState> nodeStates;
  int doneCount;

  const CampaignAsset *Asset() const { return reinterpret_cast<const CampaignAsset *>(assetBytes.data()); }
};

/* Module data. */

/* the progress file as read when the list was last rebuilt */
static CampaignProgress s_CampaignTabProgress;
/* the campaigns of the button row while the map is shown (the map control points into the selected one) */
static std::vector<CampaignTabCampaign> s_CampaignTabCampaigns;
/* the selected campaign (index into s_CampaignTabCampaigns, -1 none) and its selected mission (node index) */
static int s_CampaignTabSelected = -1;
static int s_CampaignTabSelectedNode = -1;
/* the base name of the campaign selected last in this process (plan 7.1; not saved) */
static std::string s_CampaignTabRememberedName;
/* the bytes of each catalog .cgn read so far, keyed by its lowercased path (nullopt: cannot be read) */
static std::map<std::u16string,std::optional<std::vector<uint8_t>>> s_CampaignTabCgnCache;
/* what the map shows while it has no campaign (the control never points at freed storage) */
static const CampaignGraph s_CampaignTabEmptyGraph{};
static const CampaignMapLayout s_CampaignTabEmptyLayout{};

static bool CampaignTab_IsNetworked()
{
  return (g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) != SESSION_NETWORK_ROLE_LOCAL;
}

static bool CampaignTab_IsClient()
{
  return (g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL;
}

static void CampaignTab_SetSuppressed(UiNodeBase *node,bool suppressed)
{
  if (suppressed) {
    node->nodeFlags = node->nodeFlags | UI_NODE_SUPPRESSED;
  }
  else {
    node->nodeFlags = node->nodeFlags & ~UI_NODE_SUPPRESSED;
  }
}

static void CampaignTab_SetText(UiFocusProxyControl *label,TextResourceId textId)
{
  label->text = THANDOR_PTR32_BITS(textId);
}

/* The map nodes of the campaign page, in page order after the buttons. */
static UiNodeBase *const *CampaignTab_MapNodes(FrontendUiImage *ui,size_t *outCount)
{
  static UiNodeBase *nodes[5];

  nodes[0] = &ui->campaignMap.panel.base;
  nodes[1] = &ui->campaignMissionTitleText.base;
  nodes[2] = &ui->campaignTitleText.base;
  nodes[3] = &ui->campaignMissionDescriptionText.base;
  nodes[4] = &ui->campaignMissionStatusText.base;
  *outCount = 5;
  return nodes;
}

/* true while the campaign page starts at the button row (the map is shown) */
static bool CampaignTab_MapShown(FrontendUiImage *ui)
{
  return ui->gameSelectTabStack.pages[CAMPAIGN_TAB_PAGE_INDEX].get() == &ui->campaignButtons[0].selectable.base;
}

/* Makes head the first node of the campaign page; the shown page keeps its keyboard focus rules like a page switch
   (UiPageStack_SetActiveIndex), and the page is laid out again (a page left out of the page stack misses the
   layouts made meanwhile). */
static void CampaignTab_SetPageHead(FrontendUiImage *ui,UiNodeBase *head)
{
  UiLayoutContainerControl<3> *stack = &ui->gameSelectTabStack;
  bool pageShown;

  if (stack->pages[CAMPAIGN_TAB_PAGE_INDEX].get() == head) {
    return;
  }
  pageShown = stack->base.firstChild.get() == stack->pages[CAMPAIGN_TAB_PAGE_INDEX].get();
  if (pageShown) {
    UiNodeSubtree_ReleaseKeyboardFocus(&stack->base);
  }
  stack->pages[CAMPAIGN_TAB_PAGE_INDEX] = head;
  if (pageShown) {
    stack->base.firstChild = head;
    UiNodeSubtree_AcquireKeyboardFocusDefaults(&stack->base);
  }
  UiLayoutContainerControl_LayoutChildren(UiLayoutContainerControl_AsPageStack(stack));
  UiNode_InvalidateRoot(&stack->base);
}

void FrontendCampaignTab_ShowList()
{
  FrontendUiImage *ui = g_FrontendRootNode;
  UiNodeBase *const *mapNodes;
  size_t mapNodeCount;

  if (ui == nullptr) {
    return;
  }
  for (UiFramedTextButtonControl &button : ui->campaignButtons) {
    CampaignTab_SetSuppressed(&button.selectable.base,true);
    button.selectable.stateFlags = button.selectable.stateFlags & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
  }
  mapNodes = CampaignTab_MapNodes(ui,&mapNodeCount);
  for (size_t index = 0; index < mapNodeCount; index++) {
    CampaignTab_SetSuppressed(mapNodes[index],true);
  }
  FrontendCampaignMapControl_SetGraph(&ui->campaignMap,&s_CampaignTabEmptyGraph,&s_CampaignTabEmptyLayout,{});
  CampaignTab_SetPageHead(ui,&ui->campaignsScroller.base);
  s_CampaignTabCampaigns.clear();
  s_CampaignTabSelected = -1;
  s_CampaignTabSelectedNode = -1;
}

/* The decoded .cgn of a catalog row (cached per process; the files do not change while the game runs). */
static const std::optional<std::vector<uint8_t>> &CampaignTab_ReadRowCampaign(const uint16_t *rowName)
{
  uint16_t name[CAMPAIGN_TAB_ROW_NAME_UNITS + 1] = {};
  uint16_t path[THANDOR_PATH_CAPACITY] = {};
  std::u16string key;

  memcpy(name,rowName,CAMPAIGN_TAB_ROW_NAME_UNITS * sizeof(uint16_t));
  /* level\<name>.cgn as FrontendScenarioSession_LoadOrRequestCampaignBundle builds it */
  WidePath_CombineDirectoryAndLeaf(path,name,g_ScenarioLevelDirectoryUtf16);
  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_CGN,path,THANDOR_PATH_CAPACITY);
  for (const uint16_t *unit = path; *unit != 0; unit++) {
    key.push_back(static_cast<char16_t>((*unit >= 'A' && *unit <= 'Z') ? *unit + ('a' - 'A') : *unit));
  }
  auto cached = s_CampaignTabCgnCache.find(key);
  if (cached != s_CampaignTabCgnCache.end()) {
    return cached->second;
  }
  std::optional<std::vector<uint8_t>> bytes = CampaignLevelInfo_ReadEntryNative(path);
  if (bytes.has_value() &&
      !CampaignAsset_Fits(reinterpret_cast<const CampaignAsset *>(bytes->data()),static_cast<uint32_t>(bytes->size()))) {
    bytes.reset();
  }
  if (!bytes.has_value()) {
    Thandor_Log("campaign tab: catalog campaign \"%ls\" cannot be read",reinterpret_cast<const wchar_t *>(path));
  }
  return s_CampaignTabCgnCache.emplace(key,std::move(bytes)).first->second;
}

/* The row's .cgn base name as an ASCII section key ('?' for other characters, so it fails the name check). */
static std::string CampaignTab_RowName(const ScenarioCatalogDisplayRecord *row)
{
  std::string name;

  for (size_t unit = 0; unit < CAMPAIGN_TAB_ROW_NAME_UNITS && row->identifier[unit] != 0 && row->identifier[unit] != '.';
       unit++) {
    name.push_back(row->identifier[unit] < 0x80 ? static_cast<char>(row->identifier[unit]) : '?');
  }
  return name;
}

static const ScenarioCatalogDisplayRecord *CampaignTab_ListRow(FrontendUiImage *ui,UiListRowIndex row)
{
  return static_cast<const ScenarioCatalogDisplayRecord *>(ui->campaignsList.rowSlots[row].get());
}

/* Graph, layout and node states of one campaign from its base asset and the progress. */
static void CampaignTab_BuildCampaign(CampaignTabCampaign &campaign,int32_t frameWidth,int32_t frameHeight)
{
  const CampaignAsset *asset = campaign.Asset();
  std::vector<CampaignLevelInfo> levelInfo;
  const CampaignProgressSection *section =
      CampaignProgress_FindSectionOfCampaign(s_CampaignTabProgress,asset,campaign.name);

  for (const CampaignLevelFileInfo &file : CampaignLevelInfo_Read(asset)) {
    levelInfo.push_back(CampaignLevelFileInfo_ToGraphInfo(file));
  }
  campaign.graph = CampaignGraph_Build(asset,levelInfo);
  campaign.layout = CampaignMap_Layout(campaign.graph,frameWidth,frameHeight);
  campaign.nodeStates.clear();
  campaign.doneCount = 0;
  for (const CampaignGraphNode &node : campaign.graph.nodes) {
    CampaignMapNodeState state = CAMPAIGN_MAP_NODE_LOCKED;
    if ((section != nullptr) && CampaignProgressSection_IsLevelDone(*section,node.levelId)) {
      state = CAMPAIGN_MAP_NODE_DONE;
      campaign.doneCount++;
    }
    else if (CampaignProgress_IsLevelUnlocked(section,asset,node.levelId)) {
      state = CAMPAIGN_MAP_NODE_UNLOCKED;
    }
    campaign.nodeStates.push_back(state);
  }
}

/* The mission a campaign opens with (plan 7.1): the first unlocked mission that is not done, otherwise the last
   done one, otherwise the first mission. */
static int CampaignTab_DefaultNode(const CampaignTabCampaign &campaign)
{
  int lastDone = -1;

  for (size_t index = 0; index < campaign.nodeStates.size(); index++) {
    if (campaign.nodeStates[index] == CAMPAIGN_MAP_NODE_UNLOCKED) {
      return static_cast<int>(index);
    }
    if (campaign.nodeStates[index] == CAMPAIGN_MAP_NODE_DONE) {
      lastDone = static_cast<int>(index);
    }
  }
  return (lastDone >= 0) ? lastDone : campaign.graph.firstNode;
}

/* Plain Latin-1 text of a text resource as UTF-16 (rich-text commands dropped, leading blanks trimmed). */
static std::u16string CampaignTab_PlainText(TextResourceId textId)
{
  char narrow[96];
  const char *cursor = narrow;
  std::u16string text;

  narrow[0] = '\0';
  RichTextCommandStream_CopyToNarrow(sizeof(narrow),reinterpret_cast<uint8_t *>(narrow),TextResource_Resolve(textId));
  while (*cursor == ' ') {
    cursor++;
  }
  for (; *cursor != '\0'; cursor++) {
    text.push_back(static_cast<char16_t>(static_cast<unsigned char>(*cursor)));
  }
  return text;
}

static uint32_t CampaignTab_TextWidth(std::u16string text)
{
  return RichTextCommandStream_MeasureLine(g_UiTextStyleNormal,reinterpret_cast<uint16_t *>(text.data())).widthPixels;
}

/* Sets the labels "<short name> <done>/<visible>" (plan 5.5), the check marks and the widths of the buttons: the
   row is split by the label widths, every label cut to an even share of the row so the row always fits. */
static void CampaignTab_FillButtons(FrontendUiImage *ui)
{
  const int count = static_cast<int>(s_CampaignTabCampaigns.size());
  const int32_t rowWidth = CAMPAIGN_TAB_BUTTON_ROW_RIGHT - CAMPAIGN_TAB_BUTTON_ROW_LEFT;
  const int32_t available = rowWidth - CAMPAIGN_TAB_BUTTON_GAP * (count - 1);
  std::vector<int32_t> needed;
  int32_t neededTotal = 0;

  for (int index = 0; index < count; index++) {
    const CampaignTabCampaign &campaign = s_CampaignTabCampaigns[index];
    const bool finished = campaign.doneCount == static_cast<int>(campaign.graph.nodes.size());
    const int32_t room = CAMPAIGN_TAB_BUTTON_TEXT_PADDING + (finished ? CAMPAIGN_TAB_BUTTON_CHECK_MARK_ROOM : 0);
    uint16_t baseName[CAMPAIGN_TAB_ROW_NAME_UNITS + 1] = {};
    char count8[32];
    std::u16string name;
    std::u16string counts;
    std::u16string label;
    TextResourceId shortName;

    memcpy(baseName,CampaignTab_ListRow(ui,campaign.baseRow)->identifier,CAMPAIGN_TAB_ROW_NAME_UNITS * sizeof(uint16_t));
    /* the catalog name keeps the dot of its cut extension ("tutorial."); the short-name table wants the base name */
    for (size_t unit = 0; unit < CAMPAIGN_TAB_ROW_NAME_UNITS && baseName[unit] != 0; unit++) {
      if (baseName[unit] == '.') {
        baseName[unit] = 0;
        break;
      }
    }
    shortName = TextResource_CampaignShortName(baseName);
    name = CampaignTab_PlainText((shortName != TEXT_RESOURCE_ID_NONE) ? shortName
                                                                        : TEXT_ID_CAMPAIGN_TITLE_BASE + campaign.titleIndex);
    snprintf(count8,sizeof(count8)," %d/%d",campaign.doneCount,static_cast<int>(campaign.graph.nodes.size()));
    for (const char *cursor = count8; *cursor != '\0'; cursor++) {
      counts.push_back(static_cast<char16_t>(*cursor));
    }
    /* a long title (Q8: campaigns without a short name) is cut to an even share of the row */
    label = name + counts;
    while (!name.empty() && (static_cast<int32_t>(CampaignTab_TextWidth(label)) + room > available / count)) {
      name.pop_back();
      while (!name.empty() && name.back() == u' ') {
        name.pop_back();
      }
      label = name + u"." + counts;
    }
    UiFramedTextButtonControl &button = ui->campaignButtons[index];
    button.textResourceId = TextResource_SetCampaignButtonLabel(index,reinterpret_cast<const uint16_t *>(label.c_str()));
    UiFramedTextButton_SetCheckMark(&button,finished);
    needed.push_back(static_cast<int32_t>(CampaignTab_TextWidth(label)) + room);
    neededTotal += needed.back();
  }
  /* the space left over is shared evenly; the last button ends at the row's right edge */
  int32_t left = CAMPAIGN_TAB_BUTTON_ROW_LEFT;
  const int32_t extra = (neededTotal < available) ? (available - neededTotal) / count : 0;
  for (int index = 0; index < count; index++) {
    UiFramedTextButtonControl &button = ui->campaignButtons[index];
    int32_t width = (neededTotal < available) ? needed[index] + extra : available / count;
    if (index == count - 1) {
      width = CAMPAIGN_TAB_BUTTON_ROW_RIGHT - left;
    }
    button.selectable.base.leftOffset = left;
    button.selectable.base.rightOffset = left + width;
    CampaignTab_SetSuppressed(&button.selectable.base,false);
    left += width + CAMPAIGN_TAB_BUTTON_GAP;
  }
  for (int index = count; index < FRONTEND_CAMPAIGN_BUTTON_COUNT; index++) {
    UiFramedTextButtonControl &button = ui->campaignButtons[index];
    CampaignTab_SetSuppressed(&button.selectable.base,true);
    button.selectable.stateFlags = button.selectable.stateFlags & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    button.textResourceId = TEXT_RESOURCE_ID_NONE;
    UiFramedTextButton_SetCheckMark(&button,false);
  }
}

/* Selected frame on the selected campaign's button only (radio buttons). */
static void CampaignTab_SelectButton(FrontendUiImage *ui)
{
  for (int index = 0; index < FRONTEND_CAMPAIGN_BUTTON_COUNT; index++) {
    UiSelectableStateFlags &flags = ui->campaignButtons[index].selectable.stateFlags;
    flags = (index == s_CampaignTabSelected) ? (flags | UI_SELECTABLE_SELECTED_OR_CHECKED)
                                             : (flags & ~UI_SELECTABLE_SELECTED_OR_CHECKED);
  }
  UiNode_InvalidateRoot(&ui->gameSelectTabStack.base);
}

/* What the selected mission allows and says (plan 5.5, 7.2, 7.4). */
struct CampaignTabMissionStatus {
  bool startable;
  TextResourceId templateId; /* TEXT_RESOURCE_ID_NONE: no status line */
  int referenceNode;         /* the mission named in the line, -1 none */
};

/* The node a carry target takes its units from: the snapshot's source, else the source of a carry edge into it,
   else a record that carries into it; -1 when none is on the map. */
static int CampaignTab_CarrySourceNode(const CampaignTabCampaign &campaign,int node,const CampaignProgressCarry *carry)
{
  const CampaignAsset *asset = campaign.Asset();
  const int32_t levelId = campaign.graph.nodes[node].levelId;
  int source;

  if ((carry != nullptr) && ((source = CampaignGraph_FindNode(campaign.graph,carry->sourceLevelId)) >= 0)) {
    return source;
  }
  for (const CampaignGraphEdge &edge : campaign.graph.edges) {
    if ((edge.to == node) && edge.carryOver) {
      return edge.from;
    }
  }
  for (int32_t record = 0; record < asset->levelRecordCount; record++) {
    const CampaignLevelRecord &levelRecord = asset->levels[record];
    for (int selection = 0; selection < CAMPAIGN_END_SELECTION_COUNT; selection++) {
      if ((levelRecord.successorLevelIds[selection] == levelId) && (levelRecord.levelId != levelId) &&
          ((levelRecord.carryOverMask >> selection & 1) != 0) && ((levelRecord.skipMask >> selection & 1) == 0) &&
          ((source = CampaignGraph_FindNode(campaign.graph,levelRecord.levelId)) >= 0)) {
        return source;
      }
    }
  }
  return -1;
}

static CampaignTabMissionStatus CampaignTab_MissionStatus(const CampaignTabCampaign &campaign,int node)
{
  const CampaignAsset *asset = campaign.Asset();
  const int32_t levelId = campaign.graph.nodes[node].levelId;
  const CampaignMapNodeState state = campaign.nodeStates[node];
  const CampaignProgressSection *section =
      CampaignProgress_FindSectionOfCampaign(s_CampaignTabProgress,asset,campaign.name);
  const CampaignProgressCarry *carry =
      (section != nullptr) ? CampaignProgressSection_FindValidCarry(*section,asset,levelId) : nullptr;
  const bool carryTarget = CampaignAsset_IsCarryTarget(asset,levelId);
  const bool networked = CampaignTab_IsNetworked();
  CampaignTabMissionStatus status{false,TEXT_RESOURCE_ID_NONE,-1};
  const CampaignGraphEdge *predecessor = nullptr;

  /* the path the mission follows on: from a done mission when there is one */
  for (const CampaignGraphEdge &edge : campaign.graph.edges) {
    if ((edge.to == node) && (edge.from != node) &&
        ((predecessor == nullptr) ||
         ((campaign.nodeStates[edge.from] == CAMPAIGN_MAP_NODE_DONE) &&
          (campaign.nodeStates[predecessor->from] != CAMPAIGN_MAP_NODE_DONE)))) {
      predecessor = &edge;
    }
  }
  if (state == CAMPAIGN_MAP_NODE_LOCKED) {
    if (predecessor != nullptr) {
      status.templateId = TEXT_ID_CAMPAIGN_STATUS_LOCKED;
      status.referenceNode = predecessor->from;
    }
    return status;
  }
  if (carryTarget && (networked || (carry == nullptr))) {
    status.referenceNode = CampaignTab_CarrySourceNode(campaign,node,carry);
    if (status.referenceNode >= 0) {
      status.templateId = networked ? TEXT_ID_CAMPAIGN_STATUS_NETWORK_CARRY : TEXT_ID_CAMPAIGN_STATUS_NEEDS_CARRY;
    }
    return status;
  }
  status.startable = true;
  if (state == CAMPAIGN_MAP_NODE_DONE) {
    status.templateId = TEXT_ID_CAMPAIGN_STATUS_DONE;
    if (carryTarget && ((status.referenceNode = CampaignTab_CarrySourceNode(campaign,node,carry)) >= 0)) {
      status.templateId = TEXT_ID_CAMPAIGN_STATUS_DONE_CARRY;
    }
    return status;
  }
  if ((node == campaign.graph.firstNode) || (predecessor == nullptr)) {
    status.templateId = TEXT_ID_CAMPAIGN_STATUS_UNLOCKED_FIRST;
    return status;
  }
  /* a branch: not the lowest win selection of the mission it follows */
  status.templateId = TEXT_ID_CAMPAIGN_STATUS_UNLOCKED;
  status.referenceNode = predecessor->from;
  for (const CampaignGraphEdge &edge : campaign.graph.edges) {
    if ((edge.from == predecessor->from) && (edge.to != edge.from) && (edge.selection < predecessor->selection)) {
      status.templateId = TEXT_ID_CAMPAIGN_STATUS_UNLOCKED_BRANCH;
      break;
    }
  }
  return status;
}

static uint16_t *CampaignTab_MissionTitle(const CampaignTabCampaign &campaign,int node)
{
  const int32_t titleIndex = campaign.graph.nodes[node].titleTextIndex;

  return (titleIndex >= 0) ? TextResource_Resolve(TEXT_ID_LEVEL_TITLE_BASE + titleIndex) : nullptr;
}

/* The selected mission's heading, description and status line, and the Start button (not for clients). */
static void CampaignTab_ShowMission(FrontendUiImage *ui)
{
  UiNodeBase *root = &ui->frontendRoot.root.base;
  bool startable = false;

  CampaignTab_SetText(&ui->campaignMissionTitleText,TEXT_RESOURCE_ID_NONE);
  CampaignTab_SetText(&ui->campaignMissionDescriptionText,TEXT_RESOURCE_ID_NONE);
  CampaignTab_SetText(&ui->campaignMissionStatusText,TEXT_RESOURCE_ID_NONE);
  if ((s_CampaignTabSelected >= 0) && (s_CampaignTabSelectedNode >= 0)) {
    const CampaignTabCampaign &campaign = s_CampaignTabCampaigns[s_CampaignTabSelected];
    const CampaignGraphNode &node = campaign.graph.nodes[s_CampaignTabSelectedNode];
    const CampaignTabMissionStatus status = CampaignTab_MissionStatus(campaign,s_CampaignTabSelectedNode);

    CampaignTab_SetText(&ui->campaignMissionTitleText,
                        TextResource_SetCampaignMapLine(TEXT_ID_CAMPAIGN_MAP_HEADING_LINE,
                                                        TEXT_ID_CAMPAIGN_MISSION_HEADING,
                                                        static_cast<uint32_t>(node.missionNumber),
                                                        CampaignTab_MissionTitle(campaign,s_CampaignTabSelectedNode)));
    if (node.titleTextIndex >= 0) {
      CampaignTab_SetText(&ui->campaignMissionDescriptionText,
                          static_cast<TextResourceId>(TEXT_ID_LEVEL_DESCRIPTION_BASE +
                                                      TEXT_ID_LEVEL_DESCRIPTION_STRIDE * node.titleTextIndex));
    }
    if (status.templateId != TEXT_RESOURCE_ID_NONE) {
      const int reference = (status.referenceNode >= 0) ? status.referenceNode : s_CampaignTabSelectedNode;
      CampaignTab_SetText(&ui->campaignMissionStatusText,
                          TextResource_SetCampaignMapLine(TEXT_ID_CAMPAIGN_MAP_STATUS_LINE,status.templateId,
                                                          static_cast<uint32_t>(campaign.graph.nodes[reference].missionNumber),
                                                          CampaignTab_MissionTitle(campaign,reference)));
    }
    startable = status.startable;
  }
  if (!CampaignTab_IsClient()) {
    if (startable) {
      UiNodeList_UnsuppressActionId(FRONTEND_ACTION_START_SELECTED_GAME,root);
    }
    else {
      UiNodeList_SuppressActionId(FRONTEND_ACTION_START_SELECTED_GAME,root);
    }
  }
  UiNode_InvalidateRoot(&ui->gameSelectTabStack.base);
}

/* Shows campaign index (its button, title and map) with mission node (-1: the default mission). */
static void CampaignTab_ShowCampaign(FrontendUiImage *ui,int index,int node)
{
  CampaignTabCampaign &campaign = s_CampaignTabCampaigns[index];

  s_CampaignTabSelected = index;
  if ((node < 0) || (node >= static_cast<int>(campaign.graph.nodes.size()))) {
    node = CampaignTab_DefaultNode(campaign);
  }
  s_CampaignTabSelectedNode = node;
  s_CampaignTabRememberedName = campaign.name;
  CampaignTab_SelectButton(ui);
  CampaignTab_SetText(&ui->campaignTitleText,TEXT_ID_CAMPAIGN_TITLE_BASE + campaign.titleIndex);
  FrontendCampaignMapControl_SetGraph(&ui->campaignMap,&campaign.graph,&campaign.layout,campaign.nodeStates);
  FrontendCampaignMapControl_SetSelectedNode(&ui->campaignMap,node);
  CampaignTab_ShowMission(ui);
}

/* The campaign (index into s_CampaignTabCampaigns) a list row belongs to, -1 none. */
static int CampaignTab_CampaignOfRow(UiListRowIndex row)
{
  for (size_t index = 0; index < s_CampaignTabCampaigns.size(); index++) {
    for (UiListRowIndex member : s_CampaignTabCampaigns[index].memberRows) {
      if (member == row) {
        return static_cast<int>(index);
      }
    }
  }
  return -1;
}

/* Node callback of the map control: a click selects a mission, a second click within the double-click time
   asks to start it (plan 7.2). */
static void CampaignTab_OnMapNode(int nodeIndex,bool startRequested)
{
  FrontendUiImage *ui = g_FrontendRootNode;

  if ((ui == nullptr) || !CampaignTab_MapShown(ui) || (s_CampaignTabSelected < 0)) {
    return;
  }
  const CampaignTabCampaign &campaign = s_CampaignTabCampaigns[s_CampaignTabSelected];
  if (CampaignTab_IsClient()) {
    /* clients only watch: the host's selection stays */
    FrontendCampaignMapControl_SetSelectedNode(&ui->campaignMap,s_CampaignTabSelectedNode);
    return;
  }
  if ((nodeIndex < 0) || (nodeIndex >= static_cast<int>(campaign.graph.nodes.size()))) {
    return;
  }
  FrontendCampaignMapControl_SetSelectedNode(&ui->campaignMap,nodeIndex);
  if (nodeIndex != s_CampaignTabSelectedNode) {
    s_CampaignTabSelectedNode = nodeIndex;
    CampaignTab_ShowMission(ui);
    if (CampaignTab_IsNetworked()) {
      /* the clients show the host's mission: the level id in the select command's unused first argument */
      FrontendCommand_Issue<ScenarioCatalog_SelectCampaignAndShowDescription>
                (static_cast<uint32_t>(campaign.graph.nodes[nodeIndex].levelId),0,campaign.baseRow);
    }
  }
  if (startRequested && CampaignTab_MissionStatus(campaign,nodeIndex).startable) {
    FrontendScenarioSelection_ActivateSelectedRecord(&ui->gameSelectStartButton);
  }
}

void FrontendCampaignTab_Refresh()
{
  FrontendUiImage *ui = g_FrontendRootNode;
  UiListControl *list;
  std::vector<CampaignGroupCandidate> candidates;
  std::vector<const std::vector<uint8_t> *> assets;
  std::vector<CampaignGroup> groups;
  int32_t frameWidth;
  int32_t frameHeight;

  if (ui == nullptr) {
    return;
  }
  list = &ui->campaignsList;
  FrontendCampaignTab_ShowList();
  if ((list->rowSlots.get() == nullptr) || (list->rowCount == 0)) {
    return;
  }
  /* every row must be readable, or the list stays (a campaign must not become unreachable) */
  for (UiListRowIndex row = 0; row < list->rowCount; row++) {
    const ScenarioCatalogDisplayRecord *record = CampaignTab_ListRow(ui,row);
    const std::optional<std::vector<uint8_t>> &bytes = CampaignTab_ReadRowCampaign(record->identifier);
    if (!bytes.has_value()) {
      return;
    }
    const CampaignAsset *asset = reinterpret_cast<const CampaignAsset *>(bytes->data());
    candidates.push_back(CampaignGroupCandidate{CampaignAsset_Fingerprint(asset),asset->firstLevelId,
                                                static_cast<int32_t>(record->titleTextResourceId)});
    assets.push_back(&*bytes);
  }
  groups = CampaignCatalog_GroupByFingerprint(candidates);
  if (groups.size() > static_cast<size_t>(FRONTEND_CAMPAIGN_BUTTON_COUNT)) {
    Thandor_Log("campaign tab: %u campaigns, more than %d buttons; the campaigns list is shown",
                static_cast<unsigned>(groups.size()),FRONTEND_CAMPAIGN_BUTTON_COUNT);
    return;
  }
  s_CampaignTabProgress = CampaignProgress_LoadCurrent();
  frameWidth = ui->campaignMap.panel.base.right - ui->campaignMap.panel.base.left;
  frameHeight = ui->campaignMap.panel.base.bottom - ui->campaignMap.panel.base.top;
  if ((frameWidth <= 0) || (frameHeight <= 0)) {
    frameWidth = CAMPAIGN_TAB_MAP_FRAME_WIDTH;
    frameHeight = CAMPAIGN_TAB_MAP_FRAME_HEIGHT;
  }
  std::vector<CampaignTabCampaign> campaigns;
  for (const CampaignGroup &group : groups) {
    CampaignTabCampaign campaign{};
    const UiListRowIndex baseRow = static_cast<UiListRowIndex>(group.baseIndex);

    campaign.baseRow = baseRow;
    campaign.name = CampaignTab_RowName(CampaignTab_ListRow(ui,baseRow));
    campaign.titleIndex = CampaignTab_ListRow(ui,baseRow)->titleTextResourceId;
    campaign.assetBytes = *assets[group.baseIndex];
    for (size_t member : group.memberIndices) {
      campaign.memberRows.push_back(static_cast<UiListRowIndex>(member));
    }
    if (!CampaignProgress_IsValidName(campaign.name)) {
      Thandor_Log("campaign tab: campaign name \"%s\" is not a progress key; the campaigns list is shown",
                  campaign.name.c_str());
      return;
    }
    CampaignTab_BuildCampaign(campaign,frameWidth,frameHeight);
    if (campaign.graph.firstNode < 0) {
      Thandor_Log("campaign tab: campaign \"%s\" has no first mission; the campaigns list is shown",
                  campaign.name.c_str());
      return;
    }
    campaigns.push_back(std::move(campaign));
  }
  s_CampaignTabCampaigns = std::move(campaigns);
  /* the map nodes instead of the list (the list keeps its rows and selection) */
  CampaignTab_SetPageHead(ui,&ui->campaignButtons[0].selectable.base);
  CampaignTab_FillButtons(ui);
  {
    UiNodeBase *const *mapNodes;
    size_t mapNodeCount;
    mapNodes = CampaignTab_MapNodes(ui,&mapNodeCount);
    for (size_t index = 0; index < mapNodeCount; index++) {
      CampaignTab_SetSuppressed(mapNodes[index],false);
    }
  }
  FrontendCampaignMapControl_SetCallback(&ui->campaignMap,CampaignTab_OnMapNode);
  UiLayoutContainerControl_LayoutChildren(UiLayoutContainerControl_AsPageStack(&ui->gameSelectTabStack));
  const int selected =
      CampaignTab_CampaignOfRow(UiPointerList_GetSelectedIndexAndConfirmed(UiListControl_AsPointerList(list),nullptr));
  CampaignTab_ShowCampaign(ui,(selected >= 0) ? selected : 0,-1);
}

void FrontendCampaignTab_SelectRow(UiListRowIndex selectionIndex,uint32_t levelId)
{
  FrontendUiImage *ui = g_FrontendRootNode;
  int index;

  if ((ui == nullptr) || !CampaignTab_MapShown(ui) || ((index = CampaignTab_CampaignOfRow(selectionIndex)) < 0)) {
    return;
  }
  CampaignTab_ShowCampaign(ui,index,
                           (levelId != 0) ? CampaignGraph_FindNode(s_CampaignTabCampaigns[index].graph,
                                                                   static_cast<int32_t>(levelId))
                                          : -1);
}

UiListRowIndex FrontendCampaignTab_InitialRow()
{
  FrontendUiImage *ui = g_FrontendRootNode;

  if ((ui == nullptr) || CampaignTab_IsNetworked() || !CampaignTab_MapShown(ui) ||
      s_CampaignTabRememberedName.empty()) {
    return 0;
  }
  for (const CampaignTabCampaign &campaign : s_CampaignTabCampaigns) {
    if (campaign.name == s_CampaignTabRememberedName) {
      return campaign.baseRow;
    }
  }
  return 0;
}

bool FrontendCampaignTab_StartLevel(uint32_t *outStartLevelId)
{
  FrontendUiImage *ui = g_FrontendRootNode;

  *outStartLevelId = 0;
  if ((ui == nullptr) || !CampaignTab_MapShown(ui) || (s_CampaignTabSelected < 0) || (s_CampaignTabSelectedNode < 0)) {
    return true;
  }
  const CampaignTabCampaign &campaign = s_CampaignTabCampaigns[s_CampaignTabSelected];
  if (!CampaignTab_MissionStatus(campaign,s_CampaignTabSelectedNode).startable) {
    Thandor_Log("campaign tab: mission %d cannot be started",campaign.graph.nodes[s_CampaignTabSelectedNode].missionNumber);
    return false;
  }
  const int32_t levelId = campaign.graph.nodes[s_CampaignTabSelectedNode].levelId;
  const UiListRowIndex row =
      UiPointerList_GetSelectedIndexAndConfirmed(UiListControl_AsPointerList(&ui->campaignsList),nullptr);
  /* the first mission of the base row starts as the original does; a start-point copy's row (selected by an
     original host) gets the level explicitly, its own first level differs */
  *outStartLevelId = ((levelId == campaign.Asset()->firstLevelId) && (row == campaign.baseRow))
                         ? 0 : static_cast<uint32_t>(levelId);
  return true;
}

void FrontendCampaignTab_SelectCampaignButton(UiNodeBase *sourceNode)
{
  FrontendUiImage *ui = g_FrontendRootNode;
  int index = -1;

  if ((ui == nullptr) || !CampaignTab_MapShown(ui)) {
    return;
  }
  for (int button = 0; button < FRONTEND_CAMPAIGN_BUTTON_COUNT; button++) {
    if (sourceNode == &ui->campaignButtons[button].selectable.base) {
      index = button;
    }
  }
  if ((index < 0) || (index >= static_cast<int>(s_CampaignTabCampaigns.size())) || CampaignTab_IsClient()) {
    CampaignTab_SelectButton(ui); /* clients only watch: the selection stays the host's */
    return;
  }
  /* the pressed button alone shows selected until the select command arrives (network games) */
  for (int button = 0; button < FRONTEND_CAMPAIGN_BUTTON_COUNT; button++) {
    if (button != index) {
      ui->campaignButtons[button].selectable.stateFlags =
          ui->campaignButtons[button].selectable.stateFlags & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    }
  }
  UiNode_InvalidateRoot(&ui->gameSelectTabStack.base);
  FrontendCommand_Issue<ScenarioCatalog_SelectCampaignAndShowDescription>(0,0,s_CampaignTabCampaigns[index].baseRow);
}

void FrontendCampaignTab_BindNodes(FrontendUiImage *ui)
{
  for (UiFramedTextButtonControl &button : ui->campaignButtons) {
    button.activationSound = g_UiButtonSoundVoiceSets7[3]; /* as the tab buttons */
  }
  /* "Mission N: <title>" in the selected style (plan 6.2) */
  ui->campaignMissionTitleText.labelFlags = ui->campaignMissionTitleText.labelFlags | UI_LABEL_OWN_STYLE_PALETTE;
  ui->campaignMissionTitleText.styleOverride = g_UiTextStyleSelected & UI_TEXT_STYLE_PALETTE_BYTE;
  FrontendCampaignMapControl_SetCallback(&ui->campaignMap,CampaignTab_OnMapNode);
}
