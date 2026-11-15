/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/selftest/campaignmap_selftest.cpp
 * Project code (not in the original game)
 */

/* OPEN_THANDOR_SELFTEST=campaignmap: the campaign graph, fingerprint, grouping and map layout
   (src/ui/frontend/campaign_graph.cpp) on the four stock campaigns, built from embedded copies of their level
   records (LEVEL.PCK level\tutorial.cgn, hansolo.cgn, nimm2.cgn, luke.cgn, dumped with tools/data/cgn.py) and the
   measured level info (hansolo m0 and m19 have no level file; the win selections are 1, plus 2 on hansolo m4 and
   m12). No game data is needed. The expected node positions and edge polylines are those of the approved mockup v4
   (ot-scratch campaign-mockup tools\compose.py page_v3: frame 464,248..943,408), in mockup pixels. Also checks the
   fingerprints (equal to zlib.crc32 over the same bytes of the real files; the start-point copies hansolo4..21 and
   nimm2_2..5 have their base campaign's), the grouping of copies and a cycle. One line per failed check, a line per
   campaign and "campaignmap: <n> of <m> checks failed". */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/selftest/selftest.h>
#include <thandor/ui/frontend/campaign_map.h>

namespace {

/* mockup v4 map frame: left/top pixel and size */
constexpr int32_t MOCKUP_FRAME_X = 464;
constexpr int32_t MOCKUP_FRAME_Y = 248;
constexpr int32_t MOCKUP_FRAME_WIDTH = 943 - 464 + 1;
constexpr int32_t MOCKUP_FRAME_HEIGHT = 408 - 248 + 1;

struct CampaignMapTestRecord {
    int32_t levelId;
    int32_t successorLevelIds[8];
    uint32_t carryOverMask;
    uint32_t skipMask;
    const char16_t *levelFileName;
};

const CampaignMapTestRecord s_Tutorial[] = {
    {1, {0, 2, 0, 0, 0, 0, 0, 0}, 0x2, 0x1, u"tutorial\\t00_tut"},
    {2, {0, 3, 0, 0, 0, 0, 0, 0}, 0x2, 0x1, u"tutorial\\t01_tut"},
    {3, {3, 0, 0, 0, 0, 0, 0, 0}, 0x0, 0x1, u"tutorial\\t02_tut"},
};

const CampaignMapTestRecord s_Hansolo[] = {
    {1, {1, 2, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"hansolo\\s00_tut"},
    {2, {2, 3, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"hansolo\\s01_insel"},
    {3, {3, 4, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"hansolo\\s02_schlucht"},
    {4, {4, 5, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"hansolo\\s03_erobern"},
    {5, {5, 6, 7, 0, 0, 0, 0, 0}, 0x0, 0x0, u"hansolo\\s04_konflikt"},
    {6, {6, 8, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"hansolo\\s05_trigelb"},
    {7, {7, 8, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"hansolo\\s06_trirot"},
    {8, {8, 9, 0, 0, 0, 0, 0, 0}, 0x2, 0x1, u"hansolo\\s07_landunt"},
    {9, {9, 10, 0, 0, 0, 0, 0, 0}, 0x0, 0x1, u"hansolo\\s08_instand"},
    {10, {10, 11, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"hansolo\\s09_krieg"},
    {11, {11, 12, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"hansolo\\s10_flug"},
    {12, {12, 13, 0, 0, 0, 0, 0, 0}, 0x2, 0x1, u"hansolo\\s11_warn"},
    {13, {13, 14, 15, 0, 0, 0, 0, 0}, 0x0, 0x1, u"hansolo\\s12_ueber"},
    {14, {14, 18, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"hansolo\\s13_kfeuer"},
    {15, {15, 16, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"hansolo\\s14_feind"},
    {16, {18, 17, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"hansolo\\s15_bombe"},
    {17, {17, 19, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"hansolo\\s16_trommel"},
    {18, {18, 19, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"hansolo\\s17_heftig"},
    {19, {19, 21, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"hansolo\\s18_flucht"},
    {20, {20, 21, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"hansolo\\s19_forschung"},
    {21, {21, 22, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"hansolo\\s20_posten"},
    {22, {22, 23, 0, 0, 0, 0, 0, 0}, 0x2, 0x1, u"hansolo\\s21_Marsch"},
    {23, {23, 24, 0, 0, 0, 0, 0, 0}, 0x0, 0x1, u"hansolo\\s22_laser"},
    {24, {24, 25, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"hansolo\\s23_tritod"},
    {25, {25, 26, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"hansolo\\s24_indust"},
    {26, {25, 0, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"hansolo\\s25_finale"},
};

const CampaignMapTestRecord s_Nimm2[] = {
    {1, {1, 2, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"nimm2\\ersterkontakt"},
    {2, {2, 3, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"nimm2\\eisigehöhen"},
    {3, {3, 4, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"nimm2\\kometenhagel"},
    {4, {4, 5, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"nimm2\\sumpfland"},
    {5, {5, 0, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"nimm2\\paradis"},
};

const CampaignMapTestRecord s_Luke[] = {
    {1, {1, 2, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"luke\\l00_inselkampf"},
    {2, {2, 3, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"luke\\l01_dejavu"},
    {3, {3, 4, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"luke\\l02_gebirgsjäger"},
    {4, {4, 0, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"luke\\l03_dasnest"},
};

/* expected node: mission number and centre in mockup pixels */
struct CampaignMapTestNode {
    int32_t mission;
    int32_t x;
    int32_t y;
};

/* expected edge: missions, carry-over, polyline in mockup pixels */
struct CampaignMapTestEdge {
    int32_t from;
    int32_t to;
    bool carryOver;
    int32_t pointCount;
    CampaignMapPoint points[CAMPAIGN_MAP_EDGE_MAX_POINTS];
};

const CampaignMapTestNode s_HansoloNodes[] = {
    {1, 492, 288}, {2, 535, 288}, {3, 578, 288}, {4, 621, 288}, {5, 664, 269}, {6, 664, 307},
    {7, 707, 288}, {8, 750, 288}, {9, 793, 288}, {10, 836, 288}, {11, 879, 288},
    {12, 492, 368}, {13, 535, 349}, {14, 535, 387}, {15, 578, 387}, {16, 621, 387}, {17, 621, 349},
    {18, 664, 368}, {20, 707, 368}, {21, 750, 368}, {22, 793, 368}, {23, 836, 368}, {24, 879, 368},
    {25, 922, 368},
};

const CampaignMapTestEdge s_HansoloEdges[] = {
    {1, 2, false, 2, {{506, 288}, {521, 288}}},
    {2, 3, false, 2, {{549, 288}, {564, 288}}},
    {3, 4, false, 2, {{592, 288}, {607, 288}}},
    {4, 5, false, 4, {{635, 288}, {642, 288}, {642, 269}, {650, 269}}},
    {4, 6, false, 4, {{635, 288}, {642, 288}, {642, 307}, {650, 307}}},
    {5, 7, false, 4, {{678, 269}, {685, 269}, {685, 288}, {693, 288}}},
    {6, 7, false, 4, {{678, 307}, {685, 307}, {685, 288}, {693, 288}}},
    {7, 8, true, 2, {{721, 288}, {736, 288}}},
    {8, 9, false, 2, {{764, 288}, {779, 288}}},
    {9, 10, false, 2, {{807, 288}, {822, 288}}},
    {10, 11, false, 2, {{850, 288}, {865, 288}}},
    {11, 12, true, 6, {{893, 288}, {900, 288}, {900, 326}, {471, 326}, {471, 368}, {478, 368}}},
    {12, 13, false, 4, {{506, 368}, {513, 368}, {513, 349}, {521, 349}}},
    {12, 14, false, 4, {{506, 368}, {513, 368}, {513, 387}, {521, 387}}},
    {13, 17, false, 2, {{549, 349}, {607, 349}}},
    {14, 15, false, 2, {{549, 387}, {564, 387}}},
    {15, 16, false, 2, {{592, 387}, {607, 387}}},
    {16, 18, false, 4, {{635, 387}, {642, 387}, {642, 368}, {650, 368}}},
    {17, 18, false, 4, {{635, 349}, {642, 349}, {642, 368}, {650, 368}}},
    {18, 20, false, 2, {{678, 368}, {693, 368}}},
    {20, 21, false, 2, {{721, 368}, {736, 368}}},
    {21, 22, true, 2, {{764, 368}, {779, 368}}},
    {22, 23, false, 2, {{807, 368}, {822, 368}}},
    {23, 24, false, 2, {{850, 368}, {865, 368}}},
    {24, 25, false, 2, {{893, 368}, {908, 368}}},
};

/* one row centred in the frame (mockup v4 Kyrrdon: step 86 at y 328) */
const CampaignMapTestNode s_TutorialNodes[] = {{1, 617, 328}, {2, 703, 328}, {3, 789, 328}};
const CampaignMapTestEdge s_TutorialEdges[] = {
    {1, 2, true, 2, {{631, 328}, {689, 328}}},
    {2, 3, true, 2, {{717, 328}, {775, 328}}},
};
const CampaignMapTestNode s_Nimm2Nodes[] = {{1, 531, 328}, {2, 617, 328}, {3, 703, 328}, {4, 789, 328}, {5, 875, 328}};
const CampaignMapTestEdge s_Nimm2Edges[] = {
    {1, 2, false, 2, {{545, 328}, {603, 328}}},
    {2, 3, false, 2, {{631, 328}, {689, 328}}},
    {3, 4, false, 2, {{717, 328}, {775, 328}}},
    {4, 5, false, 2, {{803, 328}, {861, 328}}},
};
const CampaignMapTestNode s_LukeNodes[] = {{1, 574, 328}, {2, 660, 328}, {3, 746, 328}, {4, 832, 328}};
const CampaignMapTestEdge s_LukeEdges[] = {
    {1, 2, false, 2, {{588, 328}, {646, 328}}},
    {2, 3, false, 2, {{674, 328}, {732, 328}}},
    {3, 4, false, 2, {{760, 328}, {818, 328}}},
};

struct CampaignMapTestCampaign {
    const char *name;
    int32_t firstLevelId;
    const CampaignMapTestRecord *records;
    int32_t recordCount;
    uint32_t fingerprint; /* zlib.crc32 over the same bytes of the real .cgn */
    int32_t rowCount;
    bool hasCarryEdge;
    const CampaignMapTestNode *nodes;
    int32_t nodeCount;
    const CampaignMapTestEdge *edges;
    int32_t edgeCount;
};

#define CAMPAIGNMAP_COUNT(array) static_cast<int32_t>(sizeof(array) / sizeof((array)[0]))

const CampaignMapTestCampaign s_Campaigns[] = {
    {"tutorial", 1, s_Tutorial, CAMPAIGNMAP_COUNT(s_Tutorial), 0x5ca97f55u, 1, true, s_TutorialNodes,
     CAMPAIGNMAP_COUNT(s_TutorialNodes), s_TutorialEdges, CAMPAIGNMAP_COUNT(s_TutorialEdges)},
    {"hansolo", 2, s_Hansolo, CAMPAIGNMAP_COUNT(s_Hansolo), 0x1f265d16u, 2, true, s_HansoloNodes,
     CAMPAIGNMAP_COUNT(s_HansoloNodes), s_HansoloEdges, CAMPAIGNMAP_COUNT(s_HansoloEdges)},
    {"nimm2", 1, s_Nimm2, CAMPAIGNMAP_COUNT(s_Nimm2), 0xe43bcb55u, 1, false, s_Nimm2Nodes,
     CAMPAIGNMAP_COUNT(s_Nimm2Nodes), s_Nimm2Edges, CAMPAIGNMAP_COUNT(s_Nimm2Edges)},
    {"luke", 1, s_Luke, CAMPAIGNMAP_COUNT(s_Luke), 0xdb1ee09bu, 1, false, s_LukeNodes, CAMPAIGNMAP_COUNT(s_LukeNodes),
     s_LukeEdges, CAMPAIGNMAP_COUNT(s_LukeEdges)},
};

unsigned s_CampaignMapChecks;
unsigned s_CampaignMapFailures;

void CampaignMapTest_Check(bool passed, const char *campaign, const char *what, int32_t value)
{
    s_CampaignMapChecks++;
    if (!passed) {
        s_CampaignMapFailures++;
        Thandor_Log("campaignmap FAILED: %s: %s (%d)", campaign, what, value);
    }
}

/* A campaign asset in native memory: header plus records (dword-aligned storage). */
struct CampaignMapTestAsset {
    std::vector<uint32_t> storage;

    CampaignAsset *Asset() { return reinterpret_cast<CampaignAsset *>(storage.data()); }
};

CampaignMapTestAsset CampaignMapTest_MakeAsset(int32_t firstLevelId, const CampaignMapTestRecord *records,
                                               int32_t recordCount)
{
    CampaignMapTestAsset test;
    size_t bytes = offsetof(CampaignAsset, levels) + sizeof(CampaignLevelRecord) * static_cast<size_t>(recordCount);
    test.storage.assign((bytes + 3) / 4, 0);
    CampaignAsset *asset = test.Asset();
    asset->firstLevelId = firstLevelId;
    asset->levelRecordCount = recordCount;
    for (int32_t i = 0; i < recordCount; i++) {
        CampaignLevelRecord &record = asset->levels[i];
        record.levelId = records[i].levelId;
        memcpy(record.successorLevelIds, records[i].successorLevelIds, sizeof record.successorLevelIds);
        record.carryOverMask = records[i].carryOverMask;
        record.skipMask = records[i].skipMask;
        record.endMovieNumbers[0] = 1; /* not part of the fingerprint */
        for (int32_t unit = 0; unit < 58 && records[i].levelFileName[unit] != 0; unit++) {
            record.levelFileName[unit] = static_cast<uint16_t>(records[i].levelFileName[unit]);
        }
    }
    return test;
}

/* The measured level info: hansolo s00_tut and s19_forschung have no level file; the win selections are 1, plus 2
   on the branch levels (hansolo ids 5 and 13). */
std::vector<CampaignLevelInfo> CampaignMapTest_LevelInfo(const char *campaign, const CampaignMapTestRecord *records,
                                                         int32_t recordCount)
{
    std::vector<CampaignLevelInfo> info;
    bool hansolo = strcmp(campaign, "hansolo") == 0;
    for (int32_t i = 0; i < recordCount; i++) {
        int32_t id = records[i].levelId;
        bool exists = !(hansolo && (id == 1 || id == 20));
        uint32_t wins = hansolo && (id == 5 || id == 13) ? 0x6u : 0x2u;
        info.push_back(CampaignLevelInfo{exists, 100 + i, exists ? wins : 0u});
    }
    return info;
}

void CampaignMapTest_Campaign(const CampaignMapTestCampaign &test)
{
    char what[160];
    CampaignMapTestAsset asset = CampaignMapTest_MakeAsset(test.firstLevelId, test.records, test.recordCount);
    std::vector<CampaignLevelInfo> info = CampaignMapTest_LevelInfo(test.name, test.records, test.recordCount);
    CampaignFingerprint fingerprint = CampaignAsset_Fingerprint(asset.Asset());
    CampaignGraph graph = CampaignGraph_Build(asset.Asset(), info);
    CampaignMapLayout layout = CampaignMap_Layout(graph, MOCKUP_FRAME_WIDTH, MOCKUP_FRAME_HEIGHT);

    CampaignMapTest_Check(fingerprint.crc32 == test.fingerprint, test.name, "fingerprint crc32",
                          static_cast<int32_t>(fingerprint.crc32));
    CampaignMapTest_Check(fingerprint.recordCount == test.recordCount, test.name, "fingerprint record count",
                          fingerprint.recordCount);
    CampaignMapTest_Check(graph.firstNode >= 0 && graph.nodes[graph.firstNode].missionNumber == 1, test.name,
                          "first node is mission 1", graph.firstNode);
    CampaignMapTest_Check(static_cast<int32_t>(graph.nodes.size()) == test.nodeCount, test.name, "node count",
                          static_cast<int32_t>(graph.nodes.size()));
    CampaignMapTest_Check(static_cast<int32_t>(graph.edges.size()) == test.edgeCount, test.name, "edge count",
                          static_cast<int32_t>(graph.edges.size()));
    CampaignMapTest_Check(graph.hasCarryEdge == test.hasCarryEdge, test.name, "carry legend", graph.hasCarryEdge);
    CampaignMapTest_Check(layout.rowCount == test.rowCount, test.name, "row count", layout.rowCount);

    for (int32_t i = 0; i < test.nodeCount; i++) {
        const CampaignMapTestNode &expected = test.nodes[i];
        int32_t node = -1;
        for (size_t n = 0; n < graph.nodes.size(); n++) {
            if (graph.nodes[n].missionNumber == expected.mission) {
                node = static_cast<int32_t>(n);
            }
        }
        snprintf(what, sizeof what, "mission %d on the map", expected.mission);
        CampaignMapTest_Check(node >= 0, test.name, what, node);
        if (node < 0) {
            continue;
        }
        const CampaignMapNodeLayout &actual = layout.nodes[node];
        snprintf(what, sizeof what, "mission %d at (%d,%d), expected (%d,%d)", expected.mission,
                 MOCKUP_FRAME_X + actual.x, MOCKUP_FRAME_Y + actual.y, expected.x, expected.y);
        CampaignMapTest_Check(MOCKUP_FRAME_X + actual.x == expected.x && MOCKUP_FRAME_Y + actual.y == expected.y,
                              test.name, what, expected.mission);
        snprintf(what, sizeof what, "mission %d title index from the level info", expected.mission);
        CampaignMapTest_Check(graph.nodes[node].titleTextIndex == 100 + graph.nodes[node].recordIndex, test.name, what,
                              graph.nodes[node].titleTextIndex);
    }

    for (int32_t i = 0; i < test.edgeCount; i++) {
        const CampaignMapTestEdge &expected = test.edges[i];
        int32_t found = -1;
        for (size_t e = 0; e < graph.edges.size(); e++) {
            if (graph.nodes[graph.edges[e].from].missionNumber == expected.from &&
                graph.nodes[graph.edges[e].to].missionNumber == expected.to) {
                found = static_cast<int32_t>(e);
            }
        }
        snprintf(what, sizeof what, "edge %d -> %d", expected.from, expected.to);
        CampaignMapTest_Check(found >= 0, test.name, what, found);
        if (found < 0) {
            continue;
        }
        snprintf(what, sizeof what, "edge %d -> %d carry-over", expected.from, expected.to);
        CampaignMapTest_Check(graph.edges[found].carryOver == expected.carryOver, test.name, what,
                              graph.edges[found].carryOver);
        const CampaignMapEdgeLayout &actual = layout.edges[found];
        bool same = actual.pointCount == expected.pointCount;
        for (int32_t p = 0; same && p < expected.pointCount; p++) {
            same = MOCKUP_FRAME_X + actual.points[p].x == expected.points[p].x &&
                   MOCKUP_FRAME_Y + actual.points[p].y == expected.points[p].y;
        }
        snprintf(what, sizeof what, "edge %d -> %d polyline", expected.from, expected.to);
        CampaignMapTest_Check(same, test.name, what, actual.pointCount);
    }

    Thandor_Log("campaignmap: %s fingerprint %08x %d, %d nodes, %d edges, %d columns, %d rows", test.name,
                static_cast<unsigned>(fingerprint.crc32), fingerprint.recordCount,
                static_cast<int32_t>(graph.nodes.size()), static_cast<int32_t>(graph.edges.size()), layout.columnCount,
                layout.rowCount);
}

/* The start-point copies (other firstLevelId, other success movies) share the fingerprint; the grouping picks the
   lowest firstLevelId as the base, whatever the row order. */
void CampaignMapTest_Grouping()
{
    CampaignMapTestAsset hansolo = CampaignMapTest_MakeAsset(2, s_Hansolo, CAMPAIGNMAP_COUNT(s_Hansolo));
    CampaignMapTestAsset hansolo16 = CampaignMapTest_MakeAsset(16, s_Hansolo, CAMPAIGNMAP_COUNT(s_Hansolo));
    CampaignMapTestAsset nimm2 = CampaignMapTest_MakeAsset(1, s_Nimm2, CAMPAIGNMAP_COUNT(s_Nimm2));
    CampaignMapTestAsset nimm2_3 = CampaignMapTest_MakeAsset(3, s_Nimm2, CAMPAIGNMAP_COUNT(s_Nimm2));
    CampaignMapTestAsset luke = CampaignMapTest_MakeAsset(1, s_Luke, CAMPAIGNMAP_COUNT(s_Luke));
    CampaignMapTestAsset tutorial = CampaignMapTest_MakeAsset(1, s_Tutorial, CAMPAIGNMAP_COUNT(s_Tutorial));
    hansolo16.Asset()->levels[3].endMovieNumbersVariant[1] = 0;
    hansolo16.Asset()->levels[4].endMovieNumbersVariant[1] = 0;

    CampaignFingerprint hansoloPrint = CampaignAsset_Fingerprint(hansolo.Asset());
    CampaignMapTest_Check(CampaignAsset_Fingerprint(hansolo16.Asset()) == hansoloPrint, "grouping",
                          "hansolo16 has the hansolo fingerprint", 0);

    /* rows sorted by name as in the campaign list: hansolo, hansolo16, luke, nimm2, nimm2_3, tutorial; then the
       copy first */
    const CampaignGroupCandidate rows[] = {
        {CampaignAsset_Fingerprint(hansolo16.Asset()), 16, 5},
        {hansoloPrint, 2, 1},
        {CampaignAsset_Fingerprint(luke.Asset()), 1, 12},
        {CampaignAsset_Fingerprint(nimm2_3.Asset()), 3, 9},
        {CampaignAsset_Fingerprint(nimm2.Asset()), 1, 7},
        {CampaignAsset_Fingerprint(tutorial.Asset()), 1, 0},
        {hansoloPrint, 2, 3}, /* same first level, higher title index: not the base */
    };
    std::vector<CampaignGroup> groups = CampaignCatalog_GroupByFingerprint(rows);
    CampaignMapTest_Check(groups.size() == 4, "grouping", "four campaigns", static_cast<int32_t>(groups.size()));
    if (groups.size() == 4) {
        CampaignMapTest_Check(groups[0].baseIndex == 1 && groups[0].memberIndices == std::vector<size_t>{0, 1, 6},
                              "grouping", "hansolo base and members", static_cast<int32_t>(groups[0].baseIndex));
        CampaignMapTest_Check(groups[1].baseIndex == 2 && groups[1].memberIndices.size() == 1, "grouping", "luke",
                              static_cast<int32_t>(groups[1].baseIndex));
        CampaignMapTest_Check(groups[2].baseIndex == 4 && groups[2].memberIndices == std::vector<size_t>{3, 4},
                              "grouping", "nimm2 base and members", static_cast<int32_t>(groups[2].baseIndex));
        CampaignMapTest_Check(groups[3].baseIndex == 5, "grouping", "tutorial", static_cast<int32_t>(groups[3].baseIndex));
    }
}

/* A cycle 1 -> 2 -> 3 -> 1 is broken at its back edge (3 -> 1, not drawn); a level only a loss reaches is a node
   without a path; the unknown-trigger fallback takes selections 1..7. */
void CampaignMapTest_Synthetic()
{
    const CampaignMapTestRecord records[] = {
        {1, {1, 2, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"x\\a"},
        {2, {2, 3, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"x\\b"},
        {3, {4, 1, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"x\\c"},
        {4, {4, 0, 0, 0, 0, 0, 0, 0}, 0x0, 0x0, u"x\\d"},
    };
    CampaignMapTestAsset asset = CampaignMapTest_MakeAsset(1, records, CAMPAIGNMAP_COUNT(records));
    std::vector<CampaignLevelInfo> info = {
        {true, 0, 0x2u}, {true, 0, CAMPAIGN_WIN_SELECTIONS_UNKNOWN}, {true, 0, 0x2u}, {true, 0, 0x2u}};
    CampaignGraph graph = CampaignGraph_Build(asset.Asset(), info);
    CampaignMapLayout layout = CampaignMap_Layout(graph, MOCKUP_FRAME_WIDTH, MOCKUP_FRAME_HEIGHT);

    CampaignMapTest_Check(graph.nodes.size() == 4, "synthetic", "loss-only level is a node",
                          static_cast<int32_t>(graph.nodes.size()));
    CampaignMapTest_Check(graph.edges.size() == 3, "synthetic", "edges 1->2, 2->3, 3->1",
                          static_cast<int32_t>(graph.edges.size()));
    if (graph.edges.size() == 3 && layout.nodes.size() == 4) {
        CampaignMapTest_Check(layout.edges[2].pointCount == 0, "synthetic", "back edge not drawn",
                              layout.edges[2].pointCount);
        CampaignMapTest_Check(layout.nodes[0].column == 0 && layout.nodes[1].column == 1 &&
                                  layout.nodes[2].column == 2 && layout.columnCount == 3,
                              "synthetic", "columns after the cycle break", layout.columnCount);
    }
    CampaignMapTest_Check(CampaignGraph_FindNode(graph, 3) == 2 && CampaignGraph_FindNode(graph, 9) == -1,
                          "synthetic", "find node", CampaignGraph_FindNode(graph, 3));
}

} // namespace

void Thandor_SelfTestCampaignMap()
{
    s_CampaignMapChecks = 0;
    s_CampaignMapFailures = 0;
    for (const CampaignMapTestCampaign &campaign : s_Campaigns) {
        CampaignMapTest_Campaign(campaign);
    }
    CampaignMapTest_Grouping();
    CampaignMapTest_Synthetic();
    Thandor_Log("campaignmap: %u of %u checks failed", s_CampaignMapFailures, s_CampaignMapChecks);
}
