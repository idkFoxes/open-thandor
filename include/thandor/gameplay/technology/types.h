/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/technology/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_TECHNOLOGY_TYPES_H
#define THANDOR_GAMEPLAY_TECHNOLOGY_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/gameplay/ai/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/ui/controls/types.h>

/* Types (split out by tools/dev/split_types.py). */

typedef struct TechnologyCategoryMasks TechnologyCategoryMasks, *PTechnologyCategoryMasks;
typedef struct TechnologyRecord TechnologyRecord, *PTechnologyRecord;
typedef struct TechnologyAssetHeader TechnologyAssetHeader, *PTechnologyAssetHeader;
typedef struct TechnologyAsset TechnologyAsset, *PTechnologyAsset;

enum {
    TECHNOLOGY_CATEGORY_A=0,
    TECHNOLOGY_CATEGORY_B=1,
    TECHNOLOGY_CATEGORY_C=2,
    TECHNOLOGY_CATEGORY_D=3
};
typedef int TechnologyCategory;

typedef uint32_t TechnologyId;

struct TechnologyCategoryMasks {
    uint32_t category2[8]; 
    uint32_t category3[8]; 
};

struct TechnologyRecord {
    uint32_t prerequisiteMasks[8]; 
    TechnologyXeniteCostQ4 xeniteCostQ4; 
    TechnologyEnergyCostQ4 energyCostQ4; 
    TechnologyResearchDurationQ5 researchDurationQ5; 
    PckTechnologyIdCatalog dependencyTechnologyIndex; 
    UiTextResourceId completionMessageResourceId; 
    TechnologyCategory category; 
    AiTechnologyCandidateScore baseCandidateScore; 
    uint32_t reserved3C; 
};

struct TechnologyAssetHeader {
    struct GeneratedAssetCommonPrefix common; 
    uint8_t reservedB0_1FF[336]; 
};

struct TechnologyAsset {
    struct TechnologyAssetHeader header; 
    struct TechnologyRecord records[1]; 
};

#endif /* THANDOR_GAMEPLAY_TECHNOLOGY_TYPES_H */
