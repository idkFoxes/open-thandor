/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/campaign_map.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_UI_FRONTEND_CAMPAIGN_MAP_H
#define THANDOR_UI_FRONTEND_CAMPAIGN_MAP_H

/* The mission map of the campaign tab (docs/plans/campaign_map.md, section 5): the campaign graph built from a
   loaded campaign asset (.cgn) and the per-level info, the campaign fingerprint, the grouping of start-point
   copies into one base campaign, and the pixel layout of the map. Everything here is pure and lives in native
   memory (std::vector); nothing touches the arena, the game state or game files. */

#include <stddef.h>
#include <stdint.h>
#include <span>
#include <vector>
#include <thandor/assets/scenario/catalog.h>

/* Number of end selections of a campaign level record (successorLevelIds, carryOverMask, skipMask). */
inline constexpr int CAMPAIGN_END_SELECTION_COUNT = 8;

/* CampaignLevelInfo.winSelectionMask when the level's end triggers could not be read: every selection 1..7 counts
   as a win (5.2 fallback). */
inline constexpr uint32_t CAMPAIGN_WIN_SELECTIONS_UNKNOWN = 0xFEu;

/* What the map needs from a level file (5.4), one per campaign level record, in record order. Filled by the level
   info reader (CM6, CampaignLevelInfo_Read) from the LEV header; a plain struct so the graph never reads files. */
struct CampaignLevelInfo {
    bool fileExists;           /* the level file was found (package entry or loose file) and its header is valid */
    int32_t titleTextIndex;    /* LEV +0x170 (LevelAssetHeader.titleTextResourceIndex); -1 when unknown */
    uint32_t winSelectionMask; /* bit s: an active end trigger with movieVariantSelector 1 ends with selection s;
                                  CAMPAIGN_WIN_SELECTIONS_UNKNOWN when the triggers could not be read */
};

/* The campaign fingerprint (4.4, 5.6): CRC32 (IEEE, as zlib) over every level record without the header and
   without the end movies and exit zones, plus the record count. Per record, in record order: levelId,
   successorLevelIds[8], carryOverMask, skipMask (little-endian dwords), then the UTF-16LE level file name up to its
   terminator (at most 58 units) and one 0 unit. The start-point copies (hansolo4/8/11/16/21, nimm2_2..5) have the
   fingerprint of their base campaign because they differ only in the header and the success movies. */
struct CampaignFingerprint {
    uint32_t crc32;
    int32_t recordCount;

    bool operator==(const CampaignFingerprint &) const = default;
};

/* The caller has checked the asset with CampaignAsset_Fits (levelRecordCount records follow the header). */
CampaignFingerprint CampaignAsset_Fingerprint(const CampaignAsset *campaignAsset);

/* One campaign catalog row for the grouping (5.6). */
struct CampaignGroupCandidate {
    CampaignFingerprint fingerprint;
    int32_t firstLevelId;
    int32_t titleIndex; /* campaign title index of the catalog record */
};

/* Rows with the same fingerprint are one campaign. The base row has the lowest firstLevelId, then the lowest title
   index, then the lowest row index. Groups are in the order of their first row; members in row order. */
struct CampaignGroup {
    size_t baseIndex;
    std::vector<size_t> memberIndices;
};

std::vector<CampaignGroup> CampaignCatalog_GroupByFingerprint(std::span<const CampaignGroupCandidate> candidates);

/* A mission of the map (5.1): a level record whose file exists and that the first level reaches through the
   successor tables (any selection, through existing levels only). */
struct CampaignGraphNode {
    int32_t levelId;
    int32_t recordIndex;    /* index into campaignAsset->levels and into the level info */
    int32_t missionNumber;  /* levelId - firstLevelId + 1 */
    int32_t titleTextIndex; /* from the level info */
};

/* A path of the map (5.2): from -> successorLevelIds[selection] for a win selection whose successor is another
   node. Two win selections that lead to the same node give one edge (the lower selection; carryOver if either
   carries). */
struct CampaignGraphEdge {
    int32_t from;      /* node index */
    int32_t to;        /* node index */
    int32_t selection; /* end selection */
    bool carryOver;    /* carryOverMask bit set and skipMask bit clear: drawn as a double line */
};

struct CampaignGraph {
    int32_t firstLevelId;
    int32_t firstNode;                    /* node of firstLevelId, -1 when it has no level file (empty graph) */
    std::vector<CampaignGraphNode> nodes; /* in record order */
    std::vector<CampaignGraphEdge> edges; /* by source node, then selection */
    bool hasCarryEdge;                    /* the legend item Einheitenuebernahme is shown */
};

/* Builds the graph of a campaign asset (pass the base campaign of its group, 5.6, so the mission numbers start at
   1). levelInfo holds one entry per level record in record order; records without an entry count as missing. The
   caller has checked the asset with CampaignAsset_Fits. Records with a duplicate levelId after the first are
   ignored, like the engine's record lookup that stops at the first match. */
CampaignGraph CampaignGraph_Build(const CampaignAsset *campaignAsset, std::span<const CampaignLevelInfo> levelInfo);

/* Node index of a level id, -1 when the level is not on the map. */
int32_t CampaignGraph_FindNode(const CampaignGraph &graph, int32_t levelId);

/* Map measures (5.3, mockup v4, logical pixels). Positions are relative to the top-left pixel of the map frame
   (the frame's border is its first and last pixel line); a node centred at (x, y) covers x - 14 .. x + 13,
   y - 8 .. y + 7. */
inline constexpr int32_t CAMPAIGN_MAP_NODE_WIDTH = 28;
inline constexpr int32_t CAMPAIGN_MAP_NODE_HEIGHT = 16;
inline constexpr int32_t CAMPAIGN_MAP_COLUMN_STEP = 43;      /* multi-row layout */
inline constexpr int32_t CAMPAIGN_MAP_FIRST_COLUMN_X = 28;   /* multi-row layout */
inline constexpr int32_t CAMPAIGN_MAP_FIRST_ROW_Y = 40;      /* multi-row layout */
inline constexpr int32_t CAMPAIGN_MAP_ROW_STEP = 80;         /* multi-row layout */
inline constexpr int32_t CAMPAIGN_MAP_LANE = 19;             /* two nodes in a column sit at -19 and +19 */
inline constexpr int32_t CAMPAIGN_MAP_WRAP = 7;              /* wrap stub length and the right margin of a row */
inline constexpr int32_t CAMPAIGN_MAP_SINGLE_ROW_MAX_STEP = 86;
inline constexpr int32_t CAMPAIGN_MAP_EDGE_MAX_POINTS = 6;

struct CampaignMapPoint {
    int32_t x;
    int32_t y;

    bool operator==(const CampaignMapPoint &) const = default;
};

struct CampaignMapNodeLayout {
    int32_t x;      /* node centre, frame-relative */
    int32_t y;
    int32_t column; /* column over all rows (longest path) */
    int32_t row;
};

/* An axis-aligned polyline from the right edge of the source node to the left edge of the target node: 2 points
   (same height), 4 (branch elbow at the middle of the gap) or 6 (wrap: right, down to 2 px above the middle of the
   row gap, left, down, right). pointCount 0: a back edge that breaks a cycle (not drawn). */
struct CampaignMapEdgeLayout {
    int32_t pointCount;
    CampaignMapPoint points[CAMPAIGN_MAP_EDGE_MAX_POINTS];
};

struct CampaignMapLayout {
    std::vector<CampaignMapNodeLayout> nodes; /* parallel to graph.nodes */
    std::vector<CampaignMapEdgeLayout> edges; /* parallel to graph.edges */
    int32_t columnCount;
    int32_t rowCount;
};

/* Lays the graph out in a frame of the given size (5.3):
   - column = longest path over the edges (a cycle is broken at its back edge, one log line); a node whose only
     successor is a merge node (two or more predecessors) moves right to the column before that merge;
   - nodes of one column are spread over lanes 2 * LANE apart, centred on the row line, ordered by the lane of their
     predecessor, then by the selection of the edge that leads there; a column with one node keeps the lane of its
     only predecessor when that predecessor has no other successor and sits in the column before, otherwise it is
     on the row line;
   - when all columns fit one row (the node plus WRAP inside the frame at COLUMN_STEP spacing), one row centred in
     the frame with a step of min(SINGLE_ROW_MAX_STEP, frameWidth / columns); otherwise rows at FIRST_ROW_Y +
     n * ROW_STEP, FIRST_COLUMN_X + i * COLUMN_STEP, each row but the last also keeping room for its wrap stub.
   Stock data gives the mockup v4 positions (self-test campaignmap). */
CampaignMapLayout CampaignMap_Layout(const CampaignGraph &graph, int32_t frameWidth, int32_t frameHeight);

#endif /* THANDOR_UI_FRONTEND_CAMPAIGN_MAP_H */
