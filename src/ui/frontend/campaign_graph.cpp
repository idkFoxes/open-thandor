/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/campaign_graph.cpp
 * Project code (not in the original game)
 */

/* Campaign graph, fingerprint, base-campaign grouping and map layout of the campaign tab
   (docs/plans/campaign_map.md 5.1-5.3, 5.6; include/thandor/ui/frontend/campaign_map.h). Pure functions on native
   memory: no arena, no files, no game state. */

#include <thandor/ui/frontend/campaign_map.h>
#include <algorithm>
#include <array>
#include <map>
#include <tuple>
#include <thandor/platform/bootstrap/image.h>

namespace {

constexpr std::array<uint32_t, 256> MakeCrc32Table()
{
    std::array<uint32_t, 256> table{};
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t value = i;
        for (int bit = 0; bit < 8; bit++) {
            value = (value & 1u) != 0 ? (value >> 1) ^ 0xEDB88320u : value >> 1;
        }
        table[i] = value;
    }
    return table;
}

constexpr std::array<uint32_t, 256> CRC32_TABLE = MakeCrc32Table();

/* Running CRC32 (IEEE, zlib's crc32): start with 0xFFFFFFFF, xor the result with 0xFFFFFFFF. */
struct Crc32 {
    uint32_t state = 0xFFFFFFFFu;

    void Byte(uint8_t value) { state = CRC32_TABLE[(state ^ value) & 0xFFu] ^ (state >> 8); }

    void Dword(uint32_t value)
    {
        for (int shift = 0; shift < 32; shift += 8) {
            Byte(static_cast<uint8_t>(value >> shift));
        }
    }

    void Unit(uint16_t value)
    {
        Byte(static_cast<uint8_t>(value));
        Byte(static_cast<uint8_t>(value >> 8));
    }

    uint32_t Result() const { return state ^ 0xFFFFFFFFu; }
};

int32_t CampaignAsset_RecordCount(const CampaignAsset *campaignAsset)
{
    return campaignAsset->levelRecordCount > 0 ? campaignAsset->levelRecordCount : 0;
}

} // namespace

CampaignFingerprint CampaignAsset_Fingerprint(const CampaignAsset *campaignAsset)
{
    Crc32 crc;
    int32_t recordCount = CampaignAsset_RecordCount(campaignAsset);

    for (int32_t i = 0; i < recordCount; i++) {
        const CampaignLevelRecord &record = campaignAsset->levels[i];
        crc.Dword(static_cast<uint32_t>(record.levelId));
        for (int32_t successor : record.successorLevelIds) {
            crc.Dword(static_cast<uint32_t>(successor));
        }
        crc.Dword(record.carryOverMask);
        crc.Dword(record.skipMask);
        for (uint16_t unit : record.levelFileName) {
            if (unit == 0) {
                break;
            }
            crc.Unit(unit);
        }
        crc.Unit(0);
    }
    return CampaignFingerprint{crc.Result(), recordCount};
}

std::vector<CampaignGroup> CampaignCatalog_GroupByFingerprint(std::span<const CampaignGroupCandidate> candidates)
{
    std::vector<CampaignGroup> groups;

    for (size_t i = 0; i < candidates.size(); i++) {
        const CampaignGroupCandidate &candidate = candidates[i];
        auto group = std::find_if(groups.begin(), groups.end(), [&](const CampaignGroup &existing) {
            return candidates[existing.baseIndex].fingerprint == candidate.fingerprint;
        });
        if (group == groups.end()) {
            groups.push_back(CampaignGroup{i, {i}});
            continue;
        }
        group->memberIndices.push_back(i);
        const CampaignGroupCandidate &base = candidates[group->baseIndex];
        if (std::tie(candidate.firstLevelId, candidate.titleIndex) < std::tie(base.firstLevelId, base.titleIndex)) {
            group->baseIndex = i;
        }
    }
    return groups;
}

CampaignGraph CampaignGraph_Build(const CampaignAsset *campaignAsset, std::span<const CampaignLevelInfo> levelInfo)
{
    CampaignGraph graph{};
    int32_t recordCount = CampaignAsset_RecordCount(campaignAsset);
    std::map<int32_t, int32_t> recordOfLevel; /* first record of each level id whose file exists */
    std::vector<bool> reached(static_cast<size_t>(recordCount), false);
    std::vector<int32_t> pending;
    std::vector<int32_t> nodeOfRecord(static_cast<size_t>(recordCount), -1);

    graph.firstLevelId = campaignAsset->firstLevelId;
    graph.firstNode = -1;
    graph.hasCarryEdge = false;

    std::map<int32_t, int32_t> firstRecordOfLevel;
    for (int32_t i = 0; i < recordCount; i++) {
        firstRecordOfLevel.emplace(campaignAsset->levels[i].levelId, i);
    }
    for (const auto &[levelId, recordIndex] : firstRecordOfLevel) {
        if (levelId > 0 && static_cast<size_t>(recordIndex) < levelInfo.size() && levelInfo[recordIndex].fileExists) {
            recordOfLevel.emplace(levelId, recordIndex);
        }
    }

    /* reachability from the first level over every successor of existing levels (5.1) */
    auto start = recordOfLevel.find(graph.firstLevelId);
    if (start != recordOfLevel.end()) {
        reached[start->second] = true;
        pending.push_back(start->second);
    }
    while (!pending.empty()) {
        int32_t recordIndex = pending.back();
        pending.pop_back();
        for (int32_t successor : campaignAsset->levels[recordIndex].successorLevelIds) {
            auto found = recordOfLevel.find(successor);
            if (found != recordOfLevel.end() && !reached[found->second]) {
                reached[found->second] = true;
                pending.push_back(found->second);
            }
        }
    }

    for (int32_t i = 0; i < recordCount; i++) {
        if (!reached[i]) {
            continue;
        }
        const CampaignLevelRecord &record = campaignAsset->levels[i];
        nodeOfRecord[i] = static_cast<int32_t>(graph.nodes.size());
        if (record.levelId == graph.firstLevelId) {
            graph.firstNode = nodeOfRecord[i];
        }
        graph.nodes.push_back(CampaignGraphNode{record.levelId, i, record.levelId - graph.firstLevelId + 1,
                                                levelInfo[i].titleTextIndex});
    }

    /* win edges (5.2) */
    for (int32_t from = 0; from < static_cast<int32_t>(graph.nodes.size()); from++) {
        int32_t recordIndex = graph.nodes[from].recordIndex;
        const CampaignLevelRecord &record = campaignAsset->levels[recordIndex];
        uint32_t winMask = levelInfo[recordIndex].winSelectionMask;
        size_t firstEdgeOfNode = graph.edges.size();
        for (int32_t selection = 0; selection < CAMPAIGN_END_SELECTION_COUNT; selection++) {
            if ((winMask >> selection & 1u) == 0) {
                continue;
            }
            auto found = recordOfLevel.find(record.successorLevelIds[selection]);
            if (found == recordOfLevel.end() || nodeOfRecord[found->second] < 0 ||
                nodeOfRecord[found->second] == from) {
                continue;
            }
            int32_t to = nodeOfRecord[found->second];
            bool carryOver = (record.carryOverMask >> selection & 1u) != 0 && (record.skipMask >> selection & 1u) == 0;
            auto same = std::find_if(graph.edges.begin() + static_cast<std::ptrdiff_t>(firstEdgeOfNode),
                                     graph.edges.end(), [to](const CampaignGraphEdge &edge) { return edge.to == to; });
            if (same != graph.edges.end()) {
                same->carryOver = same->carryOver || carryOver;
            } else {
                graph.edges.push_back(CampaignGraphEdge{from, to, selection, carryOver});
            }
        }
    }
    for (const CampaignGraphEdge &edge : graph.edges) {
        graph.hasCarryEdge = graph.hasCarryEdge || edge.carryOver;
    }
    return graph;
}

int32_t CampaignGraph_FindNode(const CampaignGraph &graph, int32_t levelId)
{
    for (size_t i = 0; i < graph.nodes.size(); i++) {
        if (graph.nodes[i].levelId == levelId) {
            return static_cast<int32_t>(i);
        }
    }
    return -1;
}

namespace {

/* Marks the edges that close a cycle (target on the depth-first stack), starting from the first node and then
   from every node not yet visited, in node order. */
std::vector<bool> CampaignMap_FindBackEdges(const CampaignGraph &graph,
                                            const std::vector<std::vector<int32_t>> &outEdges)
{
    enum : uint8_t { UNVISITED, ON_STACK, DONE };
    size_t nodeCount = graph.nodes.size();
    std::vector<bool> backEdge(graph.edges.size(), false);
    std::vector<uint8_t> state(nodeCount, UNVISITED);
    std::vector<std::pair<int32_t, size_t>> stack; /* node, next out-edge position */
    std::vector<int32_t> roots;

    if (graph.firstNode >= 0) {
        roots.push_back(graph.firstNode);
    }
    for (size_t i = 0; i < nodeCount; i++) {
        roots.push_back(static_cast<int32_t>(i));
    }
    for (int32_t root : roots) {
        if (state[root] != UNVISITED) {
            continue;
        }
        state[root] = ON_STACK;
        stack.emplace_back(root, 0);
        while (!stack.empty()) {
            auto &[node, position] = stack.back();
            if (position == outEdges[node].size()) {
                state[node] = DONE;
                stack.pop_back();
                continue;
            }
            int32_t edgeIndex = outEdges[node][position++];
            int32_t to = graph.edges[edgeIndex].to;
            if (state[to] == ON_STACK) {
                backEdge[edgeIndex] = true;
                Thandor_Log("campaign map: cycle broken at the path from mission %d to mission %d",
                            graph.nodes[node].missionNumber, graph.nodes[to].missionNumber);
            } else if (state[to] == UNVISITED) {
                state[to] = ON_STACK;
                stack.emplace_back(to, 0);
            }
        }
    }
    return backEdge;
}

} // namespace

CampaignMapLayout CampaignMap_Layout(const CampaignGraph &graph, int32_t frameWidth, int32_t frameHeight)
{
    CampaignMapLayout layout{};
    int32_t nodeCount = static_cast<int32_t>(graph.nodes.size());
    std::vector<std::vector<int32_t>> outEdges(static_cast<size_t>(nodeCount));
    std::vector<std::vector<int32_t>> inEdges(static_cast<size_t>(nodeCount)); /* forward edges only */
    std::vector<int32_t> column(static_cast<size_t>(nodeCount), 0);
    std::vector<int32_t> laneOffset(static_cast<size_t>(nodeCount), 0);
    std::vector<int32_t> order; /* topological order */

    layout.nodes.resize(static_cast<size_t>(nodeCount));
    layout.edges.resize(graph.edges.size());
    if (nodeCount == 0) {
        return layout;
    }

    for (size_t e = 0; e < graph.edges.size(); e++) {
        outEdges[graph.edges[e].from].push_back(static_cast<int32_t>(e));
    }
    std::vector<bool> backEdge = CampaignMap_FindBackEdges(graph, outEdges);
    for (size_t e = 0; e < graph.edges.size(); e++) {
        if (!backEdge[e]) {
            inEdges[graph.edges[e].to].push_back(static_cast<int32_t>(e));
        }
    }
    auto forwardSuccessorCount = [&](int32_t node) {
        int32_t count = 0;
        for (int32_t e : outEdges[node]) {
            count += backEdge[e] ? 0 : 1;
        }
        return count;
    };

    /* topological order (Kahn, lowest node first) and the longest path from the sources */
    {
        std::vector<int32_t> remaining(static_cast<size_t>(nodeCount));
        std::vector<bool> placed(static_cast<size_t>(nodeCount), false);
        for (int32_t v = 0; v < nodeCount; v++) {
            remaining[v] = static_cast<int32_t>(inEdges[v].size());
        }
        while (static_cast<int32_t>(order.size()) < nodeCount) {
            int32_t next = 0;
            while (next < nodeCount && (placed[next] || remaining[next] != 0)) {
                next++;
            }
            if (next == nodeCount) {
                break; /* cannot happen once the back edges are removed */
            }
            placed[next] = true;
            order.push_back(next);
            for (int32_t e : outEdges[next]) {
                if (!backEdge[e]) {
                    int32_t to = graph.edges[e].to;
                    remaining[to]--;
                    column[to] = std::max(column[to], column[next] + 1);
                }
            }
        }
    }
    /* as late as possible: a node whose only successor is a merge node moves to the column before the merge */
    for (auto it = order.rbegin(); it != order.rend(); ++it) {
        int32_t v = *it;
        if (forwardSuccessorCount(v) != 1) {
            continue;
        }
        for (int32_t e : outEdges[v]) {
            int32_t to = graph.edges[e].to;
            if (!backEdge[e] && inEdges[to].size() >= 2) {
                column[v] = std::max(column[v], column[to] - 1);
            }
        }
    }

    int32_t columnCount = 0;
    for (int32_t v = 0; v < nodeCount; v++) {
        columnCount = std::max(columnCount, column[v] + 1);
    }
    std::vector<std::vector<int32_t>> columnNodes(static_cast<size_t>(columnCount));
    for (int32_t v = 0; v < nodeCount; v++) {
        columnNodes[column[v]].push_back(v);
    }

    /* lanes, column by column (predecessors are always in an earlier column) */
    for (std::vector<int32_t> &nodes : columnNodes) {
        if (nodes.size() == 1) {
            int32_t v = nodes[0];
            if (inEdges[v].size() == 1) {
                int32_t predecessor = graph.edges[inEdges[v][0]].from;
                if (forwardSuccessorCount(predecessor) == 1 && column[predecessor] == column[v] - 1) {
                    laneOffset[v] = laneOffset[predecessor];
                }
            }
            continue;
        }
        auto key = [&](int32_t v) {
            if (inEdges[v].empty()) {
                return std::make_tuple(0, 0, v);
            }
            const CampaignGraphEdge &edge = graph.edges[inEdges[v][0]];
            return std::make_tuple(laneOffset[edge.from], edge.selection, v);
        };
        std::sort(nodes.begin(), nodes.end(), [&](int32_t a, int32_t b) { return key(a) < key(b); });
        int32_t count = static_cast<int32_t>(nodes.size());
        for (int32_t i = 0; i < count; i++) {
            laneOffset[nodes[i]] = (2 * i - (count - 1)) * CAMPAIGN_MAP_LANE;
        }
    }

    /* rows */
    const int32_t halfNode = CAMPAIGN_MAP_NODE_WIDTH / 2;
    const int32_t lastPixel = frameWidth - 1;
    int32_t lastRowColumns = (lastPixel - CAMPAIGN_MAP_FIRST_COLUMN_X - halfNode - CAMPAIGN_MAP_WRAP) /
                             CAMPAIGN_MAP_COLUMN_STEP + 1;
    int32_t wrappedRowColumns = (lastPixel - CAMPAIGN_MAP_FIRST_COLUMN_X - halfNode - 2 * CAMPAIGN_MAP_WRAP) /
                                CAMPAIGN_MAP_COLUMN_STEP + 1;
    lastRowColumns = std::max(lastRowColumns, 1);
    wrappedRowColumns = std::max(wrappedRowColumns, 1);
    std::vector<int32_t> rowOfColumn(static_cast<size_t>(columnCount), 0);
    std::vector<int32_t> columnX(static_cast<size_t>(columnCount), 0);
    std::vector<int32_t> rowY;

    if (columnCount <= lastRowColumns) {
        int32_t step = std::min(CAMPAIGN_MAP_SINGLE_ROW_MAX_STEP, std::max(frameWidth, 1) / columnCount);
        int32_t firstX = lastPixel / 2 - (columnCount - 1) * step / 2;
        for (int32_t c = 0; c < columnCount; c++) {
            columnX[c] = firstX + c * step;
        }
        rowY.push_back((frameHeight - 1) / 2);
    } else {
        int32_t c = 0;
        while (c < columnCount) {
            int32_t remainingColumns = columnCount - c;
            int32_t take = remainingColumns <= lastRowColumns ? remainingColumns : wrappedRowColumns;
            int32_t row = static_cast<int32_t>(rowY.size());
            rowY.push_back(CAMPAIGN_MAP_FIRST_ROW_Y + row * CAMPAIGN_MAP_ROW_STEP);
            for (int32_t i = 0; i < take; i++, c++) {
                rowOfColumn[c] = row;
                columnX[c] = CAMPAIGN_MAP_FIRST_COLUMN_X + i * CAMPAIGN_MAP_COLUMN_STEP;
            }
        }
    }
    layout.columnCount = columnCount;
    layout.rowCount = static_cast<int32_t>(rowY.size());

    for (int32_t v = 0; v < nodeCount; v++) {
        int32_t row = rowOfColumn[column[v]];
        layout.nodes[v] = CampaignMapNodeLayout{columnX[column[v]], rowY[row] + laneOffset[v], column[v], row};
    }

    /* edge polylines */
    for (size_t e = 0; e < graph.edges.size(); e++) {
        CampaignMapEdgeLayout &edgeLayout = layout.edges[e];
        edgeLayout = CampaignMapEdgeLayout{};
        if (backEdge[e]) {
            continue;
        }
        const CampaignMapNodeLayout &a = layout.nodes[graph.edges[e].from];
        const CampaignMapNodeLayout &b = layout.nodes[graph.edges[e].to];
        int32_t outX = a.x + halfNode;
        int32_t inX = b.x - halfNode;
        auto add = [&](int32_t x, int32_t y) { edgeLayout.points[edgeLayout.pointCount++] = CampaignMapPoint{x, y}; };
        if (b.row != a.row) {
            int32_t gapY = (rowY[a.row] + rowY[a.row + 1]) / 2 - 2;
            add(outX, a.y);
            add(outX + CAMPAIGN_MAP_WRAP, a.y);
            add(outX + CAMPAIGN_MAP_WRAP, gapY);
            add(inX - CAMPAIGN_MAP_WRAP, gapY);
            add(inX - CAMPAIGN_MAP_WRAP, b.y);
            add(inX, b.y);
        } else if (a.y == b.y) {
            add(outX, a.y);
            add(inX, b.y);
        } else {
            int32_t middleX = (outX + inX) / 2;
            add(outX, a.y);
            add(middleX, a.y);
            add(middleX, b.y);
            add(inX, b.y);
        }
    }
    return layout;
}
