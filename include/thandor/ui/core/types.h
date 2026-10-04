/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/core/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CORE_TYPES_H
#define THANDOR_UI_CORE_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/core/types.h>
#include <thandor/graphics/core/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/network/protocol/types.h>
#include <thandor/ui/controls/types.h>

/* Types (split from generated/types.h by tools/dev/split_types.py). */

typedef struct RecentTextHistorySlot RecentTextHistorySlot, *PRecentTextHistorySlot;
typedef struct UiRootCallbacks UiRootCallbacks, *PUiRootCallbacks;
typedef struct UiDirtyRectEntry UiDirtyRectEntry, *PUiDirtyRectEntry;
typedef struct UiActionHandlerPage UiActionHandlerPage, *PUiActionHandlerPage;
typedef struct UiActionQueueEntry UiActionQueueEntry, *PUiActionQueueEntry;
typedef struct UiRuntimeRecord UiRuntimeRecord, *PUiRuntimeRecord;
typedef struct PcxPreview64 PcxPreview64, *PPcxPreview64;
typedef struct PcxRgb24 PcxRgb24, *PPcxRgb24;

using UiStopMessageCode = int;

using UiActionHandlerPageIndex = uint32_t;

using RecentTextSerialCounter = uint32_t;

using RecentTextHistoryEntryLimit = uint32_t;

using UiDirtyRectCount = uint32_t;

using UiActionQueueUsedBytes = uint32_t;

struct RecentTextHistorySlot {
    uint16_t text[128]; 
};

struct UiRootCallbacks {
    Ptr32<Bool8 (struct UiRootNode *)> vetoClose; // Optional close/pop callback. Returning true vetoes removal of the root; false permits the pop.
    Ptr32<void (struct UiRootNode *)> frameUpdate; // Optional per-frame callback invoked by UiFrame_Update while this root is active.
    Ptr32<Bool8 (struct UiRootNode *)> method08; // Caller-cleanup root method invoked with UiRootNode *; mixed convention is intentional.
    Ptr32<Bool8 (UiKeyboardStateMask, UiActionId, struct UiRootNode *)> keyboardFallback; // Optional root-level keyboard fallback used after focused controls decline an event. The bool result conveys handling/traversal state.
    Ptr32<int (struct UiRootNode *)> pointerMissPolicy; // Signed return policy; nonnegative stops pointer root traversal, negative continues to previousRoot.
};

struct UiDirtyRectEntry {
    Ptr32<struct UiRootNode> rootNode; 
    Ptr32<struct UiRootNode> rootNodeCopy; 
    GraphicsScreenCoordinate left; 
    GraphicsScreenCoordinate top; 
    GraphicsScreenCoordinate right; 
    GraphicsScreenCoordinate bottom; 
};

struct UiActionHandlerPage {
    Ptr32<void (void *)> handlers[256]; 
};

struct UiActionQueueEntry {
    UiActionId actionId; 
    Ptr32<void> source; 
};

struct UiRuntimeRecord {
    struct UiTransferPacketHeader packetHeader; 
    uint8_t payload[240]; // Packet payload after the 16-byte header (chunk offset/size/data, ping tick, ...).
};

struct PcxRgb24 {
    ColorChannelByte red; 
    ColorChannelByte green; 
    ColorChannelByte blue; 
};

struct PcxPreview64 {
    struct PcxRgb24 palette[256]; 
    uint8_t pixels[4096]; 
};
using UiRuntimePostUnlockCallbackProc = void ();

#endif /* THANDOR_UI_CORE_TYPES_H */
