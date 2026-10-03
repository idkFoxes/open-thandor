/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/session/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

__declspec(align(8)) SessionNetworkRoleFlags g_SessionNetworkRoleFlags = 0;

/* uint32_t network lockstep interval in simulation steps (2 * the frontend speed slider value); sent in the join ack */
__declspec(align(4)) uint32_t g_SessionNetworkTickInterval = 2;

__declspec(align(16)) UiRootCallbacks g_InGameUiRootCallbacks = {
    .frameUpdate = THANDOR_FN(InGameUiRoot_UpdateFrame),
    .keyboardFallback = THANDOR_FN(InGameHotkeys_DispatchCommandByFlags)};

__declspec(align(16)) InGameUiImage g_InGameRuntimeDefaultImageTemplate = {
        { /* +0000 inGameRootPanel g_UiPanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x17C), .parent = UI_TEMPLATE_NO_LINK,
            .vtable = THANDOR_PTR(&g_UiPanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {0},
        { /* +0058 chatInputPageStack g_UiLayoutContainerControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9DC),
            .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0xFFFFFFFF, 0x000000B0},
        { /* +00B0 chatInputTextEdit g_UiRequiredTextEditControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x58),
            .vtable = THANDOR_PTR(&g_UiRequiredTextEditControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topOffset = -24,
            .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000E00, 0x00001024, 0x00000000, 0x00000030},
        { /* +017C primaryPageStack g_UiLayoutContainerControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000003, 0x00000880, 0x000001D8, 0x000040AC},
        { /* +01D8 endMovieView g_UiImageActionControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x240), .firstChild = UI_TEMPLATE_LINK(0x2F8), .parent = UI_TEMPLATE_LINK(0x17C),
            .vtable = THANDOR_PTR(&g_UiImageActionControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topAnchorQ31 = 0x10000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x70000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000005, 0x00000000, 0x00000000, 0x00000000, 0x00001009, 0x00001009, 0x00000320},
        { /* +0240 endMovieLetterboxTop g_UiFillPanelControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x29C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x17C),
            .vtable = THANDOR_PTR(&g_UiFillPanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x10000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00000000, 0xFF000000},
        { /* +029C endMovieLetterboxBottom g_UiFillPanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x17C),
            .vtable = THANDOR_PTR(&g_UiFillPanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topAnchorQ31 = 0x70000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00000000, 0xFF000000},
        { /* +02F8 endMoviePageStack g_UiLayoutContainerControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1D8),
            .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0xFFFFFFFF, 0x00000350},
        { /* +0350 resultsScreenPanel g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x3AC), .parent = UI_TEMPLATE_LINK(0x2F8),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000045},
        { /* +03AC resultsChartPageStack g_UiLayoutContainerControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x584), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x350),
            .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000003, 0x00000408, 0x00000484, 0x00000500},
        { /* +0408 resultsChart1 g_FrontendResultsTableVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3AC),
            .vtable = THANDOR_PTR(&g_FrontendResultsTableVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -306, .topOffset = -118, .rightOffset = 305, .bottomOffset = 90,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, (uint32_t)FrontendResultsGraph_DrawFactionWeightSumColumn,
            0x00000006, 0x00000000, 0x00000062, 0x00000055, 0x00000007, 0x00000002, 0x00000006, 0x00000003,
            0x00000004, 0x00000005},
        { /* +0484 resultsChart2 g_FrontendResultsTableVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3AC),
            .vtable = THANDOR_PTR(&g_FrontendResultsTableVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -306, .topOffset = -118, .rightOffset = 305, .bottomOffset = 90,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, (uint32_t)FrontendResultsGraph_DrawFactionWeightLane0Column,
            0x00000006, 0x00000000, 0x00000062, 0x00000055, 0x00000002, 0x00000008, 0x00000009, 0x0000000A,
            0x0000000B, 0x00000003},
        { /* +0500 resultsChart3 g_FrontendResultsTableVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3AC),
            .vtable = THANDOR_PTR(&g_FrontendResultsTableVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -306, .topOffset = -118, .rightOffset = 305, .bottomOffset = 90,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, (uint32_t)FrontendResultsGraph_DrawFactionWeightLane1Column,
            0x00000008, 0x00000000, 0x00000062, 0x00000055, 0x00000002, 0x0000000C, 0x0000000D, 0x0000000E,
            0x0000000F, 0x00000010, 0x00000011, 0x00000004},
        { /* +0584 resultsTabMilitary g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5E4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x350),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -304, .topOffset = 136, .rightOffset = -192, .bottomOffset = 160,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000081, 0x0000101C, 0x000021B1},
        { /* +05E4 resultsTabEconomy g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x644), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x350),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -160, .topOffset = 136, .rightOffset = -48, .bottomOffset = 160,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000081, 0x0000101C, 0x000021B0},
        { /* +0644 resultsTabThird g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x6A4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x350),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -16, .topOffset = 136, .rightOffset = 96, .bottomOffset = 160,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000083, 0x0000101C, 0x000021AF},
        { /* +06A4 resultsContinueButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x704), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x350),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 192, .topOffset = 136, .rightOffset = 304, .bottomOffset = 160,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x0000008C, 0x0000101B, 0x000021AE},
        { /* +0704 resultsSecondaryExitButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x764), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x350),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 192, .topOffset = 104, .rightOffset = 304, .bottomOffset = 128,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000080, 0x00001025, 0x000021C5},
        { /* +0764 resultsChartModeButtonA g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x7C4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x350),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -304, .topOffset = 104, .rightOffset = -192, .bottomOffset = 128,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000083, 0x00001026, 0x000021C6},
        { /* +07C4 resultsChartModeButtonB g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x824), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x350),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -160, .topOffset = 104, .rightOffset = -48, .bottomOffset = 128,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000081, 0x00001026, 0x000021C7},
        { /* +0824 resultsSummaryText g_UiListOffsetControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x350),
            .vtable = THANDOR_PTR(&g_UiListOffsetControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -256, .topOffset = -160, .rightOffset = 256, .bottomOffset = -144,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x000001B0, 0x000021C0},
        { /* +0880 levelMovieView g_UiImageActionControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x8E4), .parent = UI_TEMPLATE_LINK(0x17C),
            .vtable = THANDOR_PTR(&g_UiImageActionControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000001, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
        { /* +08E4 playerStatusBox g_UiConditionalActionControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x880),
            .vtable = THANDOR_PTR(&g_UiConditionalActionControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -192, .topOffset = -32, .rightOffset = 192, .bottomOffset = 32,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000, (uint32_t)&g_InGamePlayerStatusTextSlots,
            (uint32_t)((uint8_t *)&g_InGamePlayerStatusTextSlots + 0x80),
            (uint32_t)((uint8_t *)&g_InGamePlayerStatusTextSlots + 0x100),
            (uint32_t)((uint8_t *)&g_InGamePlayerStatusTextSlots + 0x180),
            (uint32_t)((uint8_t *)&g_InGamePlayerStatusTextSlots + 0x200),
            (uint32_t)((uint8_t *)&g_InGamePlayerStatusTextSlots + 0x280),
            (uint32_t)((uint8_t *)&g_InGamePlayerStatusTextSlots + 0x300),
            (uint32_t)((uint8_t *)&g_InGamePlayerStatusTextSlots + 0x380)},
        { /* +0960 messageHistoryPanel g_UiConditionalActionControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x4530), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9DC),
            .vtable = THANDOR_PTR(&g_UiConditionalActionControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 16, .rightOffset = 432, .bottomOffset = 58,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x0000100F},
        { /* +09DC worldViewArea g_UiLayoutContainerControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x17C),
            .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000001, 0x00000A30},
        { /* +0A30 worldView g_FrontendModelPointerContextVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xBD0), .firstChild = UI_TEMPLATE_LINK(0x2384), .parent = UI_TEMPLATE_LINK(0x9DC),
            .vtable = THANDOR_PTR(&g_FrontendModelPointerContextVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00034600},
        { /* +0BD0 gameWindowPageStack g_UiLayoutContainerControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x960), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9DC),
            .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000009, 0xFFFFFFFF, 0x00001BE0, 0x00000E6C, 0x00000C44, 0x00000CA0, 0x00000CFC, 0x00000D58,
            0x00000DB4, 0x00000E10},
        { /* +0C44 gameMenuWindow g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x24F4), .parent = UI_TEMPLATE_LINK(0xBD0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -236, .topOffset = -166, .rightOffset = 212, .bottomOffset = 166,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000005, 0x00000000, 0x00000000, 0x00000002},
        { /* +0CA0 quitGameWindow g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x3034), .parent = UI_TEMPLATE_LINK(0xBD0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -236, .topOffset = -166, .rightOffset = 212, .bottomOffset = 166,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000005, 0x00000000, 0x00000000, 0x00000002},
        { /* +0CFC saveGameWindow g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x2B94), .parent = UI_TEMPLATE_LINK(0xBD0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -236, .topOffset = -166, .rightOffset = 212, .bottomOffset = 166,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000005, 0x00000000, 0x00000000, 0x00000002},
        { /* +0D58 graphicsSettingsWindow g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x3210), .parent = UI_TEMPLATE_LINK(0xBD0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -236, .topOffset = -166, .rightOffset = 212, .bottomOffset = 166,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000005, 0x00000000, 0x00000000, 0x00000002},
        { /* +0DB4 audioSettingsWindow g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x38E0), .parent = UI_TEMPLATE_LINK(0xBD0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -236, .topOffset = -166, .rightOffset = 212, .bottomOffset = 166,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000005, 0x00000000, 0x00000000, 0x00000002},
        { /* +0E10 missionHelpWindow g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0xEC8), .parent = UI_TEMPLATE_LINK(0xBD0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -236, .topOffset = -166, .rightOffset = 212, .bottomOffset = 166,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000005, 0x00000000, 0x00000000, 0x00000003},
        { /* +0E6C technologyWindow g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x1420), .parent = UI_TEMPLATE_LINK(0xBD0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -32, .topOffset = -132, .rightOffset = 32, .bottomOffset = 132,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000005},
        { /* +0EC8 missionHelpTitle g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xF24), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xE10),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -208, .topOffset = -148, .rightOffset = 208, .bottomOffset = -124,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000001, 0x00000000, 0x000021CC},
        { /* +0F24 missionHelpCloseButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xF84), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xE10),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 96, .topOffset = 128, .rightOffset = 208, .bottomOffset = 152,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000088, 0x00001020, 0x000021CD},
        { /* +0F84 missionHelpBriefingTab g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xFE4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xE10),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 96, .rightOffset = 208, .bottomOffset = 24,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000083, 0x00001021, 0x000021CE},
        { /* +0FE4 missionHelpKeyboardTab g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x1044), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xE10),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 96, .topOffset = 32, .rightOffset = 208, .bottomOffset = 56,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000081, 0x00001022, 0x000021CF},
        { /* +1044 missionHelpMouseTab g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x10A4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xE10),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 96, .topOffset = 64, .rightOffset = 208, .bottomOffset = 88,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000081, 0x00001023, 0x000021D0},
        { /* +10A4 missionHelpTabPageStack g_UiLayoutContainerControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xE10),
            .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -208, .topOffset = -118, .rightOffset = 80, .bottomOffset = 152,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000003, 0x00001100, 0x000011EC, 0x00001334},
        { /* +1100 missionBriefingScroll g_UiScrollableControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x1190), .parent = UI_TEMPLATE_LINK(0x10A4),
            .vtable = THANDOR_PTR(&g_UiScrollableControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x000004A0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0000000F,
            0x0000000F},
        { /* +1190 missionBriefingText g_UiListOffsetControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1100),
            .vtable = THANDOR_PTR(&g_UiListOffsetControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 3, .topOffset = 3, .rightOffset = 266, .bottomOffset = 6,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000040, 0x00000109, 0x000021CE},
        { /* +11EC keyboardHelpScroll g_UiScrollableControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x127C), .parent = UI_TEMPLATE_LINK(0x10A4),
            .vtable = THANDOR_PTR(&g_UiScrollableControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x000004A0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0000000F,
            0x0000000F},
        { /* +127C keyboardHelpKeyColumn g_UiListOffsetControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x12D8), .parent = UI_TEMPLATE_LINK(0x11EC),
            .vtable = THANDOR_PTR(&g_UiListOffsetControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 3, .topOffset = 3, .rightOffset = 266, .bottomOffset = 6,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000040, 0x00000109, 0x00002400},
        { /* +12D8 keyboardHelpDescriptionColumn g_UiListOffsetControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x127C),
            .vtable = THANDOR_PTR(&g_UiListOffsetControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 97,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000040, 0x000000A8, 0x00002401},
        { /* +1334 mouseHelpScroll g_UiScrollableControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x13C4), .parent = UI_TEMPLATE_LINK(0x10A4),
            .vtable = THANDOR_PTR(&g_UiScrollableControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x000004A0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0000000F,
            0x0000000F},
        { /* +13C4 mouseHelpText g_UiListOffsetControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1334),
            .vtable = THANDOR_PTR(&g_UiListOffsetControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 3, .topOffset = 3, .rightOffset = 266, .bottomOffset = 6,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000040, 0x00000109, 0x00002402},
        { /* +1420 technologyTitle g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x147C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xE6C),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 20, .topOffset = 12, .rightOffset = -20, .bottomOffset = 28,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x0000217C},
        { /* +147C technologyCloseButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x14DC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xE6C),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 20, .topOffset = -36, .rightOffset = 132, .bottomOffset = -12,
            .topAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000088, 0x00001011, 0x0000217D},
        { /* +14DC technologyResearchButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x1544), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xE6C),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -132, .topOffset = -36, .rightOffset = -20, .bottomOffset = -12,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000084, 0x00001013, 0x0000217E},
        {.tooltipText = (uint16_t *)0x00300000},
        { /* +1544 technologyAreaTab1 g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x15AC), .firstChild = UI_TEMPLATE_LINK(0x1814), .parent = UI_TEMPLATE_LINK(0xE6C),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 11, .topOffset = -44, .rightOffset = 2, .bottomOffset = -44,
            .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x12492492, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x320},
        {
            0x00001491, 0x00001014, 0x0000217F},
        {.tooltipText = (uint16_t *)0x00300000},
        { /* +15AC technologyAreaTab2 g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x1614), .firstChild = UI_TEMPLATE_LINK(0x1870), .parent = UI_TEMPLATE_LINK(0xE6C),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 9, .topOffset = -44, .bottomOffset = -44,
            .leftAnchorQ31 = 0x12492492, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x24924925, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x320},
        {
            0x00001491, 0x00001015, 0x0000217F},
        {.tooltipText = (uint16_t *)0x00300000},
        { /* +1614 technologyAreaTab3 g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x167C), .firstChild = UI_TEMPLATE_LINK(0x18CC), .parent = UI_TEMPLATE_LINK(0xE6C),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 7, .topOffset = -44, .rightOffset = -2, .bottomOffset = -44,
            .leftAnchorQ31 = 0x24924925, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x36D36D37, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x320},
        {
            0x00001491, 0x00001016, 0x0000217F},
        {.tooltipText = (uint16_t *)0x00300000},
        { /* +167C technologyAreaTab4 g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x16E4), .firstChild = UI_TEMPLATE_LINK(0x1928), .parent = UI_TEMPLATE_LINK(0xE6C),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 5, .topOffset = -44, .rightOffset = -5, .bottomOffset = -44,
            .leftAnchorQ31 = 0x36D36D37, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x49249249, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x320},
        {
            0x00001491, 0x00001017, 0x0000217F},
        {.tooltipText = (uint16_t *)0x00300000},
        { /* +16E4 technologyAreaTab5 g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x174C), .firstChild = UI_TEMPLATE_LINK(0x1984), .parent = UI_TEMPLATE_LINK(0xE6C),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 2, .topOffset = -44, .rightOffset = -7, .bottomOffset = -44,
            .leftAnchorQ31 = 0x49249249, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x5B6DB6DB, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x320},
        {
            0x00001491, 0x00001018, 0x0000217F},
        {.tooltipText = (uint16_t *)0x00300000},
        { /* +174C technologyAreaTab6 g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x17B4), .firstChild = UI_TEMPLATE_LINK(0x19E0), .parent = UI_TEMPLATE_LINK(0xE6C),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topOffset = -44, .rightOffset = -9, .bottomOffset = -44,
            .leftAnchorQ31 = 0x5B6DB6DB, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x6DB6DB6E, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x320},
        {
            0x00001491, 0x00001019, 0x0000217F},
        {.tooltipText = (uint16_t *)0x00300000},
        { /* +17B4 technologyAreaTab7 g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x1A98), .firstChild = UI_TEMPLATE_LINK(0x1A3C), .parent = UI_TEMPLATE_LINK(0xE6C),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -2, .topOffset = -44, .rightOffset = -11, .bottomOffset = -44,
            .leftAnchorQ31 = 0x6DB6DB6E, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x320},
        {
            0x00001491, 0x0000101A, 0x0000217F},
        { /* +1814 technologyAreaTab1Icon g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1544),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000005},
        { /* +1870 technologyAreaTab2Icon g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x15AC),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000005},
        { /* +18CC technologyAreaTab3Icon g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1614),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000005},
        { /* +1928 technologyAreaTab4Icon g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x167C),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000005},
        { /* +1984 technologyAreaTab5Icon g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x16E4),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000005},
        { /* +19E0 technologyAreaTab6Icon g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x174C),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000005},
        { /* +1A3C technologyAreaTab7Icon g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x17B4),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000005},
        { /* +1A98 technologyDescriptionFrame g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x1AF4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xE6C),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 20, .topOffset = 36, .rightOffset = 20, .bottomOffset = -132,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {0},
        { /* +1AF4 technologyDescriptionScroll g_UiScrollableControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x1B84), .parent = UI_TEMPLATE_LINK(0xE6C),
            .vtable = THANDOR_PTR(&g_UiScrollableControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 36, .topOffset = 36, .rightOffset = -12, .bottomOffset = -52,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x000004A0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0000000F,
            0x0000000F},
        { /* +1B84 technologyDescriptionText g_UiListOffsetControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1AF4),
            .vtable = THANDOR_PTR(&g_UiListOffsetControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 3, .topOffset = 3, .rightOffset = 310, .bottomOffset = 6,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000040, 0x00000133, 0x0000217F},
        { /* +1BE0 messageWindow g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x1C3C), .parent = UI_TEMPLATE_LINK(0xBD0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -228, .topOffset = -100, .rightOffset = 228, .bottomOffset = 100,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000005, 0x00000000, 0x00000000, 0x00000001},
        { /* +1C3C messageWindowTitle g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x1C98), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1BE0),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 20, .topOffset = 12, .rightOffset = -20, .bottomOffset = 28,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00002166},
        { /* +1C98 messageTextEdit g_UiRequiredTextEditControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x1D64), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1BE0),
            .vtable = THANDOR_PTR(&g_UiRequiredTextEditControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 12, .topOffset = 28, .rightOffset = -12, .bottomOffset = 45,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000408, 0xFFFFFFFF, 0x00000000, 0x00000030},
        { /* +1D64 messageCancelButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x1DC4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1BE0),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -132, .topOffset = -36, .rightOffset = -20, .bottomOffset = -12,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000088, 0x00001002, 0x00002168},
        { /* +1DC4 messageSendAndCloseButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x1E24), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1BE0),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -132, .topOffset = -68, .rightOffset = -20, .bottomOffset = -44,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000084, 0x00001005, 0x00002169},
        { /* +1E24 messageSendButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x1E84), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1BE0),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -132, .topOffset = -100, .rightOffset = -20, .bottomOffset = -76,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000080, 0x00001004, 0x00002167},
        { /* +1E84 messageRecipientPlayersTab g_UiTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x1EE4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1BE0),
            .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 20, .topOffset = 60, .rightOffset = 132, .bottomOffset = 84,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000080, 0x00001006, 0x0000216B},
        { /* +1EE4 messageRecipientGroupsTab g_UiTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x1F44), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1BE0),
            .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 20, .topOffset = 84, .rightOffset = 132, .bottomOffset = 108,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000080, 0x00001007, 0x0000216C},
        { /* +1F44 messageRecipientAllTab g_UiTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x1FA4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1BE0),
            .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 20, .topOffset = 108, .rightOffset = 132, .bottomOffset = 132,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000082, 0x00001008, 0x0000216A},
        { /* +1FA4 messageRecipientPageStack g_UiLayoutContainerControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1BE0),
            .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0x00001FFC, 0xFFFFFFFF},
        { /* +1FFC messageRecipientScroll g_UiScrollableControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x208C), .parent = UI_TEMPLATE_LINK(0x1FA4),
            .vtable = THANDOR_PTR(&g_UiScrollableControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -80, .topOffset = 56, .rightOffset = 80, .bottomOffset = -12,
            .leftAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x000004A0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0000000F,
            0x0000000F},
        { /* +208C messageRecipientList g_UiPanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x20E4), .parent = UI_TEMPLATE_LINK(0x1FFC),
            .vtable = THANDOR_PTR(&g_UiPanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightOffset = 136, .bottomOffset = 168,
            .layoutWidth = -1, .layoutHeight = -1},
        {0},
        { /* +20E4 messageRecipientCheckbox1 g_UiTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x2144), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x208C),
            .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 4, .rightOffset = -4, .bottomOffset = 28,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000491, 0xFFFFFFFF, 0x0000216D},
        { /* +2144 messageRecipientCheckbox2 g_UiTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x21A4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x208C),
            .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 28, .rightOffset = -4, .bottomOffset = 52,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000490, 0xFFFFFFFF, 0x0000216E},
        { /* +21A4 messageRecipientCheckbox3 g_UiTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x2204), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x208C),
            .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 52, .rightOffset = -4, .bottomOffset = 76,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000490, 0xFFFFFFFF, 0x0000216F},
        { /* +2204 messageRecipientCheckbox4 g_UiTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x2264), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x208C),
            .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 76, .rightOffset = -4, .bottomOffset = 100,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000490, 0xFFFFFFFF, 0x00002170},
        { /* +2264 messageRecipientCheckbox5 g_UiTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x22C4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x208C),
            .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 100, .rightOffset = -4, .bottomOffset = 124,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000490, 0xFFFFFFFF, 0x00002171},
        { /* +22C4 messageRecipientCheckbox6 g_UiTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x2324), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x208C),
            .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 124, .rightOffset = -4, .bottomOffset = 148,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000490, 0xFFFFFFFF, 0x00002172},
        { /* +2324 messageRecipientCheckbox7 g_UiTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x208C),
            .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 148, .rightOffset = -4, .bottomOffset = 172,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000490, 0xFFFFFFFF, 0x00002173},
        { /* +2384 worldViewCyclingInfoText g_UiCommandVisibilitySingleLineTextVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x23E0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xA30),
            .vtable = THANDOR_PTR(&g_UiCommandVisibilitySingleLineTextVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topOffset = -16,
            .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00000112},
        { /* +23E0 worldViewStatusTextA g_UiCommandVisibilitySingleLineTextVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x243C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xA30),
            .vtable = THANDOR_PTR(&g_UiCommandVisibilitySingleLineTextVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightOffset = -96, .bottomOffset = 16,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000801, 0x00000000, 0x000021D1},
        { /* +243C worldViewStatusTextB g_UiCommandVisibilitySingleLineTextVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x2498), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xA30),
            .vtable = THANDOR_PTR(&g_UiCommandVisibilitySingleLineTextVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 96, .bottomOffset = 16,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00001001, 0x00000000, 0x000021D5},
        { /* +2498 worldViewWrappedStatusText g_UiCommandVisibilityWrappedTextVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xA30),
            .vtable = THANDOR_PTR(&g_UiCommandVisibilityWrappedTextVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000010},
        { /* +24F4 gameMenuTitle g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x2550), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC44),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -224, .topOffset = -148, .rightOffset = 224, .bottomOffset = -124,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000001, 0x00000000, 0x00002123},
        { /* +2550 gameMenuSaveButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x25B0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC44),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -208, .topOffset = -80, .rightOffset = -96, .bottomOffset = -56,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000080, 0x0000120E, 0x0000214D},
        { /* +25B0 gameMenuQuitButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x2610), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC44),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -208, .topOffset = -48, .rightOffset = -96, .bottomOffset = -24,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000080, 0x00001200, 0x00002148},
        { /* +2610 gameMenuGraphicsButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x2670), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC44),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -208, .topOffset = -16, .rightOffset = -96, .bottomOffset = 8,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000080, 0x00001202, 0x00002121},
        { /* +2670 gameMenuAudioButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x26D0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC44),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -208, .topOffset = 16, .rightOffset = -96, .bottomOffset = 40,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000080, 0x00001203, 0x00002122},
        { /* +26D0 rightButtonNoScrollCheckbox g_UiTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x2730), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC44),
            .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -19, .topOffset = 60, .rightOffset = 208, .bottomOffset = 84,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000091, 0x00001216, 0x000021C8},
        { /* +2730 scrollSpeedGroup g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x28AC), .firstChild = UI_TEMPLATE_LINK(0x278C), .parent = UI_TEMPLATE_LINK(0xC44),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -24, .topOffset = 88, .rightOffset = 208, .bottomOffset = 160,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00002844, 0x000021C9},
        { /* +278C scrollSpeedMinLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x27E8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x2730),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topOffset = -28,
            .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x000021CA},
        { /* +27E8 scrollSpeedMaxLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x2844), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x2730),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topOffset = -28,
            .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0x00000000, 0x000021CB},
        { /* +2844 scrollSpeedSlider g_UiRangeSliderControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x2730),
            .vtable = THANDOR_PTR(&g_UiRangeSliderControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topOffset = 24, .bottomOffset = -24,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000004, 0x00000008, 0x00000080, 0x00000020, 0x00000001, 0x00001217},
        { /* +28AC autoCameraGroup g_UiTitledWindowControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x29C0), .firstChild = UI_TEMPLATE_LINK(0x2900), .parent = UI_TEMPLATE_LINK(0xC44),
            .vtable = THANDOR_PTR(&g_UiTitledWindowControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -24, .topOffset = -120, .rightOffset = 208, .bottomOffset = -58,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x0000215F},
        { /* +2900 autoZoomOffCheckbox g_UiTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x2960), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x28AC),
            .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 3, .topOffset = 3, .rightOffset = -3, .bottomOffset = 27,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000091, 0x00001212, 0x00002160},
        { /* +2960 autoRotationOffCheckbox g_UiTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x28AC),
            .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 3, .topOffset = 27, .rightOffset = -3, .bottomOffset = 51,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000091, 0x00001213, 0x00002161},
        { /* +29C0 cameraLinkGroup g_UiTitledWindowControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x2B34), .firstChild = UI_TEMPLATE_LINK(0x2A14), .parent = UI_TEMPLATE_LINK(0xC44),
            .vtable = THANDOR_PTR(&g_UiTitledWindowControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -24, .topOffset = -42, .rightOffset = 208, .bottomOffset = 44,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00002162},
        { /* +2A14 linkRotationZoomCheckbox g_UiTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x2A74), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x29C0),
            .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 3, .topOffset = 3, .rightOffset = -3, .bottomOffset = 27,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000091, 0x00001214, 0x00002163},
        { /* +2A74 linkRotationTiltCheckbox g_UiTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x2AD4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x29C0),
            .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 3, .topOffset = 27, .rightOffset = -3, .bottomOffset = 51,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000091, 0x00001215, 0x00002164},
        { /* +2AD4 hidePanelCheckbox g_UiTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x29C0),
            .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 3, .topOffset = 51, .rightOffset = -3, .bottomOffset = 75,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000091, 0x0000121B, 0x00002165},
        { /* +2B34 gameMenuCloseButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC44),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -208, .topOffset = 128, .rightOffset = -96, .bottomOffset = 152,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x0000008C, 0x00001201, 0x0000211F},
        { /* +2B94 saveGameBackButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x2BF4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xCFC),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -208, .topOffset = 128, .rightOffset = -96, .bottomOffset = 152,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000088, 0x00001218, 0x0000214F},
        { /* +2BF4 saveGameTitle g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x2C50), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xCFC),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -208, .topOffset = -148, .rightOffset = 208, .bottomOffset = -124,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000001, 0x00000000, 0x0000214E},
        { /* +2C50 saveGameSaveButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x2CB0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xCFC),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 96, .topOffset = 128, .rightOffset = 208, .bottomOffset = 152,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000084, 0x00001210, 0x0000214D},
        { /* +2CB0 saveGameDeleteButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x2D10), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xCFC),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -32, .topOffset = 128, .rightOffset = 80, .bottomOffset = 152,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000080, 0x00001219, 0x00002153},
        { /* +2D10 saveGameListScroll g_UiScrollableControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x2E1C), .firstChild = UI_TEMPLATE_LINK(0x2DA0), .parent = UI_TEMPLATE_LINK(0xCFC),
            .vtable = THANDOR_PTR(&g_UiScrollableControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -208, .topOffset = -116, .rightOffset = 208, .bottomOffset = 48,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x000004A0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0000000F,
            0x0000000F},
        { /* +2DA0 saveGameList g_UiListControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x2D10),
            .vtable = THANDOR_PTR(&g_UiListControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x0000000A, 0x00000000, 0x00000000, 0x00000000, 0x0000120F, 0x00000000, 0x00000002, 0x00000000,
            0x00000100, 0x00000000, 0x00000089, 0x000000C0},
        { /* +2E1C saveGameListHeaderLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x2E78), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xCFC),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -200, .topOffset = -132, .rightOffset = 200, .bottomOffset = -116,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000004, 0x00000000, 0x00002150},
        { /* +2E78 saveGameDescriptionText g_UiListOffsetControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x2ED4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xCFC),
            .vtable = THANDOR_PTR(&g_UiListOffsetControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -204, .topOffset = 56, .rightOffset = 204, .bottomOffset = 71,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000004, 0x00000000, 0x0000215D},
        { /* +2ED4 saveNameEntryStack g_UiLayoutContainerControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xCFC),
            .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0xFFFFFFFF, 0x00002F2C},
        { /* +2F2C saveNameEdit g_UiRequiredTextEditControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x2FD8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x2ED4),
            .vtable = THANDOR_PTR(&g_UiRequiredTextEditControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -208, .topOffset = 95, .rightOffset = 208, .bottomOffset = 112,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x0000040C, 0x00001211, 0x00000000, 0x00000020},
        { /* +2FD8 saveNameLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x2ED4),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -200, .topOffset = 79, .rightOffset = 200, .bottomOffset = 95,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000004, 0x00000000, 0x00002152},
        { /* +3034 quitMenuBackButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x3094), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xCA0),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -208, .topOffset = 128, .rightOffset = -96, .bottomOffset = 152,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000088, 0x00001218, 0x00002149},
        { /* +3094 quitMenuTitleLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x30F0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xCA0),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -208, .topOffset = -148, .rightOffset = 208, .bottomOffset = -124,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000001, 0x00000000, 0x00002144},
        { /* +30F0 quitMenuAbortMissionButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x3150), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xCA0),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 96, .topOffset = 128, .rightOffset = 208, .bottomOffset = 152,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000084, 0x0000101D, 0x0000214A},
        { /* +3150 quitMenuSurrenderButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x31B0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xCA0),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 96, .topOffset = 96, .rightOffset = 208, .bottomOffset = 120,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000080, 0x0000101E, 0x0000214B},
        { /* +31B0 quitMenuRestartMissionButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xCA0),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 96, .topOffset = 64, .rightOffset = 208, .bottomOffset = 88,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000080, 0x00001027, 0x0000214C},
        { /* +3210 graphicsOptionsBackButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x3270), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xD58),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -208, .topOffset = 128, .rightOffset = -96, .bottomOffset = 152,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x0000008C, 0x00001218, 0x0000211F},
        { /* +3270 graphicsOptionsTitleLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x32CC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xD58),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -208, .topOffset = -148, .rightOffset = 208, .bottomOffset = -124,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000001, 0x00000000, 0x0000212E},
        { /* +32CC shadingEnabledCheckbox g_UiTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x332C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xD58),
            .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -204, .topOffset = -112, .rightOffset = -12, .bottomOffset = -88,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000091, 0x00001204, 0x0000212F},
        { /* +332C shadingLevelGroup g_UiTitledWindowControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x35F0), .firstChild = UI_TEMPLATE_LINK(0x3380), .parent = UI_TEMPLATE_LINK(0xD58),
            .vtable = THANDOR_PTR(&g_UiTitledWindowControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -208, .topOffset = -80, .rightOffset = -8, .bottomOffset = 78,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00002130},
        { /* +3380 shadingLevel32x32Button g_UiNumericPairTextButtonVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x33E8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x332C),
            .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 3, .topOffset = 3, .rightOffset = -3, .bottomOffset = 27,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000481, 0x00001205, 0x00002133, 0x00000000, 0x00000000, 0x00000020, 0x00000020},
        { /* +33E8 shadingLevel32x64Button g_UiNumericPairTextButtonVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x3450), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x332C),
            .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 3, .topOffset = 27, .rightOffset = -3, .bottomOffset = 51,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000481, 0x00001205, 0x00002133, 0x00000000, 0x00000000, 0x00000020, 0x00000040},
        { /* +3450 shadingLevel32x128Button g_UiNumericPairTextButtonVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x34B8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x332C),
            .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 3, .topOffset = 51, .rightOffset = -3, .bottomOffset = 75,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000481, 0x00001205, 0x00002133, 0x00000000, 0x00000000, 0x00000020, 0x00000080},
        { /* +34B8 shadingLevel64x64Button g_UiNumericPairTextButtonVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x3520), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x332C),
            .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 3, .topOffset = 75, .rightOffset = -3, .bottomOffset = 99,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000481, 0x00001205, 0x00002133, 0x00000000, 0x00000000, 0x00000040, 0x00000040},
        { /* +3520 shadingLevel64x128Button g_UiNumericPairTextButtonVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x3588), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x332C),
            .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 3, .topOffset = 99, .rightOffset = -3, .bottomOffset = 123,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000481, 0x00001205, 0x00002133, 0x00000000, 0x00000000, 0x00000040, 0x00000080},
        { /* +3588 shadingLevel128x128Button g_UiNumericPairTextButtonVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x332C),
            .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 3, .topOffset = 123, .rightOffset = -3, .bottomOffset = 147,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000481, 0x00001205, 0x00002133, 0x00000000, 0x00000000, 0x00000080, 0x00000080},
        { /* +35F0 modelDetailGroup g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x376C), .firstChild = UI_TEMPLATE_LINK(0x364C), .parent = UI_TEMPLATE_LINK(0xD58),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 8, .topOffset = -112, .rightOffset = 208, .bottomOffset = -40,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00003704, 0x00002131},
        { /* +364C modelDetailMinLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x36A8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x35F0),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topOffset = -28,
            .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00002134},
        { /* +36A8 modelDetailMaxLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x3704), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x35F0),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topOffset = -28,
            .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0x00000000, 0x00002135},
        { /* +3704 modelDetailSlider g_UiRangeSliderControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x35F0),
            .vtable = THANDOR_PTR(&g_UiRangeSliderControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topOffset = 24, .bottomOffset = -24,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000004, 0x00004000, 0x00040000, 0x00010000, 0x00001000, 0x00001206},
        { /* +376C textureQualityGroup g_UiTitledWindowControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x37C0), .parent = UI_TEMPLATE_LINK(0xD58),
            .vtable = THANDOR_PTR(&g_UiTitledWindowControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 8, .topOffset = -8, .rightOffset = 208, .bottomOffset = 78,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00002132},
        { /* +37C0 textureQualityLowButton g_UiTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x3820), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x376C),
            .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 3, .topOffset = 3, .rightOffset = -3, .bottomOffset = 27,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000081, 0x00001207, 0x00002136},
        { /* +3820 textureQualityMediumButton g_UiTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x3880), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x376C),
            .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 3, .topOffset = 27, .rightOffset = -3, .bottomOffset = 51,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000081, 0x00001207, 0x00002137},
        { /* +3880 textureQualityHighButton g_UiTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x376C),
            .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 3, .topOffset = 51, .rightOffset = -3, .bottomOffset = 75,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000081, 0x00001207, 0x00002138},
        { /* +38E0 soundOptionsBackButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x3940), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xDB4),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -208, .topOffset = 128, .rightOffset = -96, .bottomOffset = 152,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x0000008C, 0x00001218, 0x0000211F},
        { /* +3940 soundOptionsTitleLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x399C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xDB4),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -208, .topOffset = -148, .rightOffset = 208, .bottomOffset = -124,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000001, 0x00000000, 0x0000213A},
        { /* +399C musicEnabledCheckbox g_UiTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x39FC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xDB4),
            .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -204, .topOffset = -112, .rightOffset = -12, .bottomOffset = -88,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000091, 0x00001209, 0x0000213B},
        { /* +39FC effectsEnabledCheckbox g_UiTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x3A5C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xDB4),
            .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -204, .topOffset = -80, .rightOffset = -12, .bottomOffset = -56,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000091, 0x00001208, 0x0000213C},
        { /* +3A5C reverseStereoCheckbox g_UiTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x3ABC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xDB4),
            .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -204, .topOffset = -16, .rightOffset = -12, .bottomOffset = 8,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000091, 0x0000120A, 0x0000213D},
        { /* +3ABC effectsVolumeGroup g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x3C38), .firstChild = UI_TEMPLATE_LINK(0x3B18), .parent = UI_TEMPLATE_LINK(0xDB4),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 8, .topOffset = -120, .rightOffset = 208, .bottomOffset = -52,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00003BD0, 0x0000213E},
        { /* +3B18 effectsVolumeMinLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x3B74), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3ABC),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topOffset = -32,
            .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00002141},
        { /* +3B74 effectsVolumeMaxLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x3BD0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3ABC),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topOffset = -32,
            .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0x00000000, 0x00002142},
        { /* +3BD0 effectsVolumeSlider g_UiRangeSliderControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3ABC),
            .vtable = THANDOR_PTR(&g_UiRangeSliderControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topOffset = 20, .bottomOffset = -24,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000004, 0x00000000, 0x00008000, 0x00008000, 0x00000800, 0x0000120B},
        { /* +3C38 movieVolumeGroup g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x3DB4), .firstChild = UI_TEMPLATE_LINK(0x3C94), .parent = UI_TEMPLATE_LINK(0xDB4),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 8, .topOffset = -52, .rightOffset = 208, .bottomOffset = 16,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00003D4C, 0x0000213F},
        { /* +3C94 movieVolumeMinLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x3CF0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3C38),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topOffset = -32,
            .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00002141},
        { /* +3CF0 movieVolumeMaxLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x3D4C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3C38),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topOffset = -32,
            .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0x00000000, 0x00002142},
        { /* +3D4C movieVolumeSlider g_UiRangeSliderControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3C38),
            .vtable = THANDOR_PTR(&g_UiRangeSliderControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topOffset = 20, .bottomOffset = -24,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000004, 0x00000000, 0x00008000, 0x00008000, 0x00000800, 0x0000120C},
        { /* +3DB4 musicVolumeGroup g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x3F30), .firstChild = UI_TEMPLATE_LINK(0x3E10), .parent = UI_TEMPLATE_LINK(0xDB4),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 8, .topOffset = 84, .rightOffset = 208, .bottomOffset = 152,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00003EC8, 0x00002140},
        { /* +3E10 musicVolumeMinLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x3E6C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3DB4),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topOffset = -32,
            .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00002141},
        { /* +3E6C musicVolumeMaxLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x3EC8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3DB4),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topOffset = -32,
            .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0x00000000, 0x00002142},
        { /* +3EC8 musicVolumeSlider g_UiRangeSliderControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3DB4),
            .vtable = THANDOR_PTR(&g_UiRangeSliderControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topOffset = 20, .bottomOffset = -24,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000004, 0x00000000, 0x00008000, 0x00008000, 0x00000800, 0x0000120D},
        { /* +3F30 messageMovieVolumeGroup g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x3F8C), .parent = UI_TEMPLATE_LINK(0xDB4),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 8, .topOffset = 16, .rightOffset = 208, .bottomOffset = 84,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00004044, 0x00002143},
        { /* +3F8C messageMovieVolumeMinLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x3FE8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3F30),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topOffset = -32,
            .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00002141},
        { /* +3FE8 messageMovieVolumeMaxLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x4044), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3DB4),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topOffset = -32,
            .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0x00000000, 0x00002142},
        { /* +4044 messageMovieVolumeSlider g_UiRangeSliderControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3F30),
            .vtable = THANDOR_PTR(&g_UiRangeSliderControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topOffset = 20, .bottomOffset = -24,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000004, 0x00000000, 0x00008000, 0x00008000, 0x00000800, 0x0000121A},
        { /* +40AC sidePanelStack g_UiLayoutContainerControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x9DC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x17C),
            .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0x00004104, 0xFFFFFFFF},
        { /* +4104 sidePanelFrameLeftEdge g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x4160), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x40AC),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000020},
        { /* +4160 sidePanelFrameRightEdge g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x41BC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x40AC),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000020, 0x00000000, 0x00000000, 0x00000001},
        { /* +41BC sidePanelFrameTopCap g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x4218), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x40AC),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000020, 0x00000000, 0x00000000, 0x00000002},
        { /* +4218 sidePanelFrameMenuBar g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x4274), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x40AC),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000020, 0x00000000, 0x00000000, 0x00000003},
        { /* +4274 sidePanelFrameInfoSection g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x42D0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x40AC),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000020, 0x00000000, 0x00000000, 0x00000004},
        { /* +42D0 sidePanelFrameBottomCap g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x432C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x40AC),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000020, 0x00000000, 0x00000000, 0x00000005},
        { /* +432C sidePanelMenuButtonStack g_UiLayoutContainerControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x9A1C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x40AC),
            .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0x00004388, 0xFFFFFFFF, 0x00180000},
        { /* +4388 inGameMenuButton g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x4400), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x432C),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000611, 0x00001003, 0x00000000, 0x00000000, 0x00000000, 0x0000000B, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00180007},
        { /* +4400 missionObjectivesButton g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x4478), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x432C),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000611, 0x0000101F, 0x00000000, 0x00000000, 0x00000000, 0x00000008, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00180008},
        { /* +4478 countdownDisplayPanel g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x44D4), .parent = UI_TEMPLATE_LINK(0x432C),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x100},
        {
            0x00000000, 0x00000000, 0x00000000, 0x00000009},
        { /* +44D4 countdownText g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4478),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000015, 0x00000000, (uint32_t)&g_InGameCountdownTextUtf16},
        { /* +4530 resourceBarModeStack g_UiLayoutContainerControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x4644), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9DC),
            .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000003, 0x0000458C, 0x000045E8, 0xFFFFFFFF},
        { /* +458C resourcePanel g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x4758), .parent = UI_TEMPLATE_LINK(0x4530),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00000000, 0x00000006},
        { /* +45E8 editorTabStripA g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x4BA4), .parent = UI_TEMPLATE_LINK(0x4530),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00000000, 0x00000026},
        { /* +4644 gamePanelsModeStack g_UiLayoutContainerControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x58), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9DC),
            .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000003, 0x000046A0, 0x000046FC, 0xFFFFFFFF},
        { /* +46A0 gamePanelsArea g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x4D0C), .parent = UI_TEMPLATE_LINK(0x4644),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x40},
        {
            0x00000000, 0x00000000, 0x00000000, 0x00000007},
        { /* +46FC editorTabStripB g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x98B8), .parent = UI_TEMPLATE_LINK(0x4644),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00000000, 0x00000027},
        { /* +4758 resourcePanelImageToggle8 g_UiImageControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x4820), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x458C),
            .vtable = THANDOR_PTR(&g_UiImageControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
        {
            0x00000060, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000008},
        { /* +47C4 resourcePanelImageToggle8Popup g_UiNineSlicePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4758),
            .vtable = THANDOR_PTR(&g_UiNineSlicePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00000010, 0x00000020},
        { /* +4820 resourcePanelImageToggle9 g_UiImageControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x48EC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x458C),
            .vtable = THANDOR_PTR(&g_UiImageControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
        {
            0x00000060, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000009},
        { /* +488C resourcePanelImageToggle9Popup g_UiNineSlicePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4820),
            .vtable = THANDOR_PTR(&g_UiNineSlicePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00000010, 0x00000020, 0x00180005},
        { /* +48EC resourcePanelIconButton g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x4964), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x458C),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000611, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x0000000A, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00180011},
        { /* +4964 xeniteGauge g_UiFormattedContainerVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x49FC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x458C),
            .vtable = THANDOR_PTR(&g_UiFormattedContainerVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x100},
        {
            0x00000001, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00004000, 0x00180012},
        { /* +49FC tritiumGauge g_UiFormattedContainerVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x4A94), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x458C),
            .vtable = THANDOR_PTR(&g_UiFormattedContainerVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x100},
        {
            0x00000001, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00004000, 0x00180013},
        { /* +4A94 energyGauge g_UiFormattedContainerVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x4B44), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x458C),
            .vtable = THANDOR_PTR(&g_UiFormattedContainerVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x100},
        {
            0x00000002, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000200},
        { /* +4B44 xeniteAmountText g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x458C),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000312, 0x00000000, (uint32_t)&g_FrontendCurrentFactionPrimaryResourceTextUtf16,
            0x01010000, 0x00180014},
        { /* +4BA4 editorModeTabTerrainHeight g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x4C1C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x45E8),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000603, 0x00001100, 0x00000000, 0x00000000, 0x00000000, 0x00000028, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00180015},
        { /* +4C1C editorModeTabTerrainMaterial g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x4C94), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x45E8),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000601, 0x00001101, 0x00000000, 0x00000000, 0x00000000, 0x00000029, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00180016},
        { /* +4C94 editorModeTabTerrainSmoothing g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x45E8),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000601, 0x00001102, 0x00000000, 0x00000000, 0x00000000, 0x0000002A, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00180001},
        { /* +4D0C diplomacyPanel g_UiImageControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5DB4), .firstChild = UI_TEMPLATE_LINK(0x4D78), .parent = UI_TEMPLATE_LINK(0x46A0),
            .vtable = THANDOR_PTR(&g_UiImageControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x140},
        {
            0x00000060, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x0000000C},
        { /* +4D78 diplomacyFrame g_UiNineSlicePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x4DD4), .parent = UI_TEMPLATE_LINK(0x4D0C),
            .vtable = THANDOR_PTR(&g_UiNineSlicePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00000010, 0x00000020},
        { /* +4DD4 diplomacyRow1 g_UiLayoutContainerControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x4E2C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4D78),
            .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0x0000503C, 0xFFFFFFFF},
        { /* +4E2C diplomacyRow2 g_UiLayoutContainerControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x4E84), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4D78),
            .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0x00005098, 0xFFFFFFFF},
        { /* +4E84 diplomacyRow3 g_UiLayoutContainerControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x4EDC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4D78),
            .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0x000050F4, 0xFFFFFFFF},
        { /* +4EDC diplomacyRow4 g_UiLayoutContainerControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x4F34), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4D78),
            .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0x00005150, 0xFFFFFFFF},
        { /* +4F34 diplomacyRow5 g_UiLayoutContainerControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x4F8C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4D78),
            .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0x000051AC, 0xFFFFFFFF},
        { /* +4F8C diplomacyRow6 g_UiLayoutContainerControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x4FE4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4D78),
            .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0x00005208, 0xFFFFFFFF},
        { /* +4FE4 diplomacyRow7 g_UiLayoutContainerControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4D78),
            .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0x00005264, 0xFFFFFFFF},
        { /* +503C diplomacyRow1PlayerNumberLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x52C0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4DD4),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 4, .rightOffset = -4, .bottomOffset = 18,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00002191},
        { /* +5098 diplomacyRow2PlayerNumberLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x531C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4E2C),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 4, .rightOffset = -4, .bottomOffset = 18,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00002191},
        { /* +50F4 diplomacyRow3PlayerNumberLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5378), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4E84),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 4, .rightOffset = -4, .bottomOffset = 18,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00002191},
        { /* +5150 diplomacyRow4PlayerNumberLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x53D4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4EDC),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 4, .rightOffset = -4, .bottomOffset = 18,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00002191},
        { /* +51AC diplomacyRow5PlayerNumberLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5430), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4F34),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 4, .rightOffset = -4, .bottomOffset = 18,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00002191},
        { /* +5208 diplomacyRow6PlayerNumberLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x548C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4F8C),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 4, .rightOffset = -4, .bottomOffset = 18,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00002191},
        { /* +5264 diplomacyRow7PlayerNumberLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x54E8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4FE4),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 4, .rightOffset = -4, .bottomOffset = 18,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00002191},
        { /* +52C0 diplomacyRow1FactionLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5544), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4DD4),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 4, .rightOffset = -4, .bottomOffset = 18,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0x00000000, 0x00002174},
        { /* +531C diplomacyRow2FactionLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x55A0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4E2C),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 4, .rightOffset = -4, .bottomOffset = 18,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0x00000000, 0x00002174},
        { /* +5378 diplomacyRow3FactionLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x55FC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4E84),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 4, .rightOffset = -4, .bottomOffset = 18,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0x00000000, 0x00002174},
        { /* +53D4 diplomacyRow4FactionLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5658), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4EDC),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 4, .rightOffset = -4, .bottomOffset = 18,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0x00000000, 0x00002174},
        { /* +5430 diplomacyRow5FactionLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x56B4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4F34),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 4, .rightOffset = -4, .bottomOffset = 18,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0x00000000, 0x00002174},
        { /* +548C diplomacyRow6FactionLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5710), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4F8C),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 4, .rightOffset = -4, .bottomOffset = 18,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0x00000000, 0x00002174},
        { /* +54E8 diplomacyRow7FactionLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x576C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4FE4),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 4, .rightOffset = -4, .bottomOffset = 18,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000002, 0x00000000, 0x00002174},
        { /* +5544 diplomacyRow1RelationLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x57C8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4DD4),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 18, .rightOffset = -4, .bottomOffset = 32,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x000021A3},
        { /* +55A0 diplomacyRow2RelationLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5824), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4E2C),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 18, .rightOffset = -4, .bottomOffset = 32,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x000021A3},
        { /* +55FC diplomacyRow3RelationLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5880), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4E84),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 18, .rightOffset = -4, .bottomOffset = 32,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x000021A3},
        { /* +5658 diplomacyRow4RelationLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x58DC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4EDC),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 18, .rightOffset = -4, .bottomOffset = 32,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x000021A3},
        { /* +56B4 diplomacyRow5RelationLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5938), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4F34),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 18, .rightOffset = -4, .bottomOffset = 32,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x000021A3},
        { /* +5710 diplomacyRow6RelationLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5994), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4F8C),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 18, .rightOffset = -4, .bottomOffset = 32,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x000021A3},
        { /* +576C diplomacyRow7RelationLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x59F0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4FE4),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 18, .rightOffset = -4, .bottomOffset = 32,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x000021A3},
        { /* +57C8 diplomacyRow1PlayerNameLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5A4C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4DD4),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 32, .rightOffset = -4, .bottomOffset = 46,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000010, 0x00000000, (uint32_t)&g_EmptyFrontendPlayerNameUtf16},
        { /* +5824 diplomacyRow2PlayerNameLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5AC8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4E2C),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 32, .rightOffset = -4, .bottomOffset = 46,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000010, 0x00000000, (uint32_t)&g_EmptyFrontendPlayerNameUtf16},
        { /* +5880 diplomacyRow3PlayerNameLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5B44), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4E84),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 32, .rightOffset = -4, .bottomOffset = 46,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000010, 0x00000000, (uint32_t)&g_EmptyFrontendPlayerNameUtf16},
        { /* +58DC diplomacyRow4PlayerNameLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5BC0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4EDC),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 32, .rightOffset = -4, .bottomOffset = 46,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000010, 0x00000000, (uint32_t)&g_EmptyFrontendPlayerNameUtf16},
        { /* +5938 diplomacyRow5PlayerNameLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5C3C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4F34),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 32, .rightOffset = -4, .bottomOffset = 46,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000010, 0x00000000, (uint32_t)&g_EmptyFrontendPlayerNameUtf16},
        { /* +5994 diplomacyRow6PlayerNameLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5CB8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4F8C),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 32, .rightOffset = -4, .bottomOffset = 46,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000010, 0x00000000, (uint32_t)&g_EmptyFrontendPlayerNameUtf16},
        { /* +59F0 diplomacyRow7PlayerNameLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5D34), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4FE4),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 32, .rightOffset = -4, .bottomOffset = 46,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000010, 0x00000000, (uint32_t)&g_EmptyFrontendPlayerNameUtf16},
        { /* +5A4C diplomacyRow1RelationButton g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4DD4),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001012, 0x00000000, 0x000000A9, 0x00000000, 0x00000021},
        { /* +5AC8 diplomacyRow2RelationButton g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4E2C),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001012, 0x00000000, 0x000000A9, 0x00000000, 0x00000021},
        { /* +5B44 diplomacyRow3RelationButton g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4E84),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001012, 0x00000000, 0x000000A9, 0x00000000, 0x00000021},
        { /* +5BC0 diplomacyRow4RelationButton g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4EDC),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001012, 0x00000000, 0x000000A9, 0x00000000, 0x00000021},
        { /* +5C3C diplomacyRow5RelationButton g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4F34),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001012, 0x00000000, 0x000000A9, 0x00000000, 0x00000021},
        { /* +5CB8 diplomacyRow6RelationButton g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4F8C),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001012, 0x00000000, 0x000000A9, 0x00000000, 0x00000021},
        { /* +5D34 diplomacyRow7RelationButton g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4FE4),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001012, 0x00000000, 0x000000A9, 0x00000000, 0x00000021, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00180002},
        { /* +5DB4 buildCatalogPanel g_UiImageControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x7680), .firstChild = UI_TEMPLATE_LINK(0x5E20), .parent = UI_TEMPLATE_LINK(0x46A0),
            .vtable = THANDOR_PTR(&g_UiImageControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x140},
        {
            0x00000060, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x0000000D},
        { /* +5E20 buildCatalogFrame g_UiNineSlicePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x5E7C), .parent = UI_TEMPLATE_LINK(0x5DB4),
            .vtable = THANDOR_PTR(&g_UiNineSlicePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00000018, 0x00000022},
        { /* +5E7C buildCatalogEntry00 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5EFC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +5EFC buildCatalogEntry01 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5F7C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +5F7C buildCatalogEntry02 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5FFC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +5FFC buildCatalogEntry03 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x607C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +607C buildCatalogEntry04 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x60FC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +60FC buildCatalogEntry05 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x617C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +617C buildCatalogEntry06 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x61FC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +61FC buildCatalogEntry07 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x627C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +627C buildCatalogEntry08 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x62FC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +62FC buildCatalogEntry09 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x637C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +637C buildCatalogEntry10 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x63FC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +63FC buildCatalogEntry11 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x647C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +647C buildCatalogEntry12 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x64FC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +64FC buildCatalogEntry13 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x657C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +657C buildCatalogEntry14 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x65FC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +65FC buildCatalogEntry15 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x667C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +667C buildCatalogEntry16 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x66FC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +66FC buildCatalogEntry17 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x677C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +677C buildCatalogEntry18 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x67FC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +67FC buildCatalogEntry19 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x687C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +687C buildCatalogEntry20 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x68FC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +68FC buildCatalogEntry21 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x697C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +697C buildCatalogEntry22 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x69FC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +69FC buildCatalogEntry23 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x6A7C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +6A7C buildCatalogEntry24 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x6AFC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +6AFC buildCatalogEntry25 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x6B7C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +6B7C buildCatalogEntry26 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x6BFC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +6BFC buildCatalogEntry27 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x6C7C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +6C7C buildCatalogEntry28 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x6CFC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +6CFC buildCatalogEntry29 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x6D7C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +6D7C buildCatalogEntry30 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x6DFC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +6DFC buildCatalogEntry31 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x6E7C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +6E7C buildCatalogEntry32 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x6EFC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +6EFC buildCatalogEntry33 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x6F7C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +6F7C buildCatalogEntry34 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x6FFC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +6FFC buildCatalogEntry35 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x707C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +707C buildCatalogEntry36 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x70FC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +70FC buildCatalogEntry37 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x717C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +717C buildCatalogEntry38 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x71FC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +71FC buildCatalogEntry39 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x727C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +727C buildCatalogEntry40 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x72FC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +72FC buildCatalogEntry41 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x737C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +737C buildCatalogEntry42 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x73FC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +73FC buildCatalogEntry43 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x747C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +747C buildCatalogEntry44 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x74FC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +74FC buildCatalogEntry45 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x757C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +757C buildCatalogEntry46 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x75FC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +75FC buildCatalogEntry47 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5E20),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100B, 0x00000000, 0x00000000, 0x00000000, 0x00000023, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00180003},
        { /* +7680 specialBuildCatalogPanel g_UiImageControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x8C4C), .firstChild = UI_TEMPLATE_LINK(0x76EC), .parent = UI_TEMPLATE_LINK(0x46A0),
            .vtable = THANDOR_PTR(&g_UiImageControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x140},
        {
            0x00000060, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x0000000E},
        { /* +76EC specialBuildCatalogFrame g_UiNineSlicePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x7748), .parent = UI_TEMPLATE_LINK(0x7680),
            .vtable = THANDOR_PTR(&g_UiNineSlicePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00000018, 0x00000022},
        { /* +7748 specialBuildCatalogEntry00 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x77C8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +77C8 specialBuildCatalogEntry01 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x7848), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +7848 specialBuildCatalogEntry02 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x78C8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +78C8 specialBuildCatalogEntry03 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x7948), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +7948 specialBuildCatalogEntry04 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x79C8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +79C8 specialBuildCatalogEntry05 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x7A48), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +7A48 specialBuildCatalogEntry06 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x7AC8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +7AC8 specialBuildCatalogEntry07 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x7B48), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +7B48 specialBuildCatalogEntry08 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x7BC8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +7BC8 specialBuildCatalogEntry09 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x7C48), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +7C48 specialBuildCatalogEntry10 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x7CC8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +7CC8 specialBuildCatalogEntry11 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x7D48), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +7D48 specialBuildCatalogEntry12 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x7DC8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +7DC8 specialBuildCatalogEntry13 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x7E48), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +7E48 specialBuildCatalogEntry14 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x7EC8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +7EC8 specialBuildCatalogEntry15 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x7F48), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +7F48 specialBuildCatalogEntry16 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x7FC8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +7FC8 specialBuildCatalogEntry17 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x8048), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +8048 specialBuildCatalogEntry18 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x80C8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +80C8 specialBuildCatalogEntry19 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x8148), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +8148 specialBuildCatalogEntry20 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x81C8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +81C8 specialBuildCatalogEntry21 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x8248), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +8248 specialBuildCatalogEntry22 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x82C8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +82C8 specialBuildCatalogEntry23 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x8348), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +8348 specialBuildCatalogEntry24 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x83C8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +83C8 specialBuildCatalogEntry25 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x8448), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +8448 specialBuildCatalogEntry26 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x84C8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +84C8 specialBuildCatalogEntry27 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x8548), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +8548 specialBuildCatalogEntry28 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x85C8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +85C8 specialBuildCatalogEntry29 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x8648), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +8648 specialBuildCatalogEntry30 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x86C8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +86C8 specialBuildCatalogEntry31 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x8748), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +8748 specialBuildCatalogEntry32 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x87C8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +87C8 specialBuildCatalogEntry33 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x8848), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +8848 specialBuildCatalogEntry34 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x88C8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +88C8 specialBuildCatalogEntry35 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x8948), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +8948 specialBuildCatalogEntry36 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x89C8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +89C8 specialBuildCatalogEntry37 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x8A48), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +8A48 specialBuildCatalogEntry38 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x8AC8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +8AC8 specialBuildCatalogEntry39 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x8B48), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +8B48 specialBuildCatalogEntry40 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x8BC8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +8BC8 specialBuildCatalogEntry41 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x76EC),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x0000100C, 0x00000000, 0x00000000, 0x00000000, 0x00000023, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00180004},
        { /* +8C4C armyStockPanel g_UiImageControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x8CB8), .parent = UI_TEMPLATE_LINK(0x46A0),
            .vtable = THANDOR_PTR(&g_UiImageControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x140},
        {
            0x00000060, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x0000000F},
        { /* +8CB8 armyStockFrame g_UiNineSlicePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x8D14), .parent = UI_TEMPLATE_LINK(0x8C4C),
            .vtable = THANDOR_PTR(&g_UiNineSlicePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00000018, 0x00000022},
        { /* +8D14 armyStockSlot00 g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x8D90), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x8CB8),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001001, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +8D90 armyStockSlot01 g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x8E0C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x8CB8),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001001, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +8E0C armyStockSlot02 g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x8E88), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x8CB8),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001001, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +8E88 armyStockSlot03 g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x8F04), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x8CB8),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001001, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +8F04 armyStockSlot04 g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x8F80), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x8CB8),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001001, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +8F80 armyStockSlot05 g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x8FFC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x8CB8),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001001, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +8FFC armyStockSlot06 g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x9078), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x8CB8),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001001, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +9078 armyStockSlot07 g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x90F4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x8CB8),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001001, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +90F4 armyStockSlot08 g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x9170), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x8CB8),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001001, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +9170 armyStockSlot09 g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x91EC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x8CB8),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001001, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +91EC armyStockSlot10 g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x9268), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x8CB8),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001001, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +9268 armyStockSlot11 g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x92E4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x8CB8),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001001, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +92E4 armyStockSlot12 g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x9360), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x8CB8),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001001, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +9360 armyStockSlot13 g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x93DC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x8CB8),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001001, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +93DC armyStockSlot14 g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x9458), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x8CB8),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001001, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +9458 armyStockSlot15 g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x94D4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x8CB8),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001001, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +94D4 armyStockSlot16 g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x9550), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x8CB8),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001001, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +9550 armyStockSlot17 g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x95CC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x8CB8),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001001, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +95CC armyStockSlot18 g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x9648), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x8CB8),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001001, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +9648 armyStockSlot19 g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x96C4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x8CB8),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001001, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +96C4 armyStockSlot20 g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x9740), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x8CB8),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001001, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +9740 armyStockSlot21 g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x97BC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x8CB8),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001001, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +97BC armyStockSlot22 g_UiCommandSpriteButtonWithDetailsVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x9838), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x8CB8),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonWithDetailsVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001001, 0x00000000, 0x00000000, 0x00000000, 0x00000023},
        { /* +9838 armyStockSlot23 g_UiCatalogEntryControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x8CB8),
            .vtable = THANDOR_PTR(&g_UiCatalogEntryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000A60, 0x00001001, 0x00000000, 0x00000000, 0x00000000, 0x00000023, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00180019},
        { /* +98B8 editorModeTabRegion g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x9930), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x46FC),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000601, 0x00001104, 0x00000000, 0x00000000, 0x00000000, 0x0000002B, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00180017},
        { /* +9930 editorModeTabUnitPlacement g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x99A8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x46FC),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000601, 0x00001105, 0x00000000, 0x00000000, 0x00000000, 0x0000002C, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00180018},
        { /* +99A8 editorModeTabObjectPlacement g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x46FC),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000601, 0x00001106, 0x00000000, 0x00000000, 0x00000000, 0x0000002D},
        { /* +9A1C minimapView g_UiSelectionGeometryControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x9A8C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x40AC),
            .vtable = THANDOR_PTR(&g_UiSelectionGeometryControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00012000, 0x00008000, 0x00000800, 0x00002000, 0x00000000, 0x00001000},
        { /* +9A8C modePreviewPageStack g_UiLayoutContainerControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x9EE0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x40AC),
            .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000008, 0x00009AFC, 0x00009B60, 0x00009BBC, 0x00009C18, 0x00009C74, 0x00009D2C, 0xFFFFFFFF,
            0x00009DE4},
        { /* +9AFC notificationTargetButton g_UiImageActionControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9A8C),
            .vtable = THANDOR_PTR(&g_UiImageActionControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00000000, 0x00000025, 0x0000100D, 0x0000100E},
        { /* +9B60 heightToolPreview g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9A8C),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00000000, 0x00000025},
        { /* +9BBC materialToolSelectedSwatch g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9A8C),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000009},
        { /* +9C18 smoothingToolPreview g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9A8C),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00000000, 0x00000025},
        { /* +9C74 unitPlacementPreviewFrame g_UiFillPanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x9CD0), .parent = UI_TEMPLATE_LINK(0x9A8C),
            .vtable = THANDOR_PTR(&g_UiFillPanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000009, 0x00000000, 0x00000000, 0xFF000000},
        { /* +9CD0 unitPlacementPreviewImage g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9C74),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000009},
        { /* +9D2C objectPlacementPreviewFrame g_UiFillPanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x9D88), .parent = UI_TEMPLATE_LINK(0x9A8C),
            .vtable = THANDOR_PTR(&g_UiFillPanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000009, 0x00000000, 0x00000000, 0xFF000000},
        { /* +9D88 objectPlacementPreviewImage g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9D2C),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000009},
        { /* +9DE4 regionToolPreview g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9A8C),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00000000, 0x00000025},
        { /* +9EE0 modeDetailPageStack g_UiLayoutContainerControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xB19C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x40AC),
            .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000008, 0x00009F50, 0x0000A678, 0x0000A6D4, 0x0000AFD0, 0x0000B02C, 0x0000B0E4, 0xFFFFFFFF,
            0x0000B140},
        { /* +9F50 selectionDetailPanel g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x9FAC), .parent = UI_TEMPLATE_LINK(0x9EE0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00000000, 0x000000A6},
        { /* +9FAC selectionDetailPageStack g_UiLayoutContainerControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9F50),
            .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000004, 0xFFFFFFFF, 0x0000A00C, 0x0000A1F8, 0x0000A140},
        { /* +A00C singleSelectionMetrics g_UiArmyMetricsPanelVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xA06C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9FAC),
            .vtable = THANDOR_PTR(&g_UiArmyMetricsPanelVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .layoutWidth = -1, .layoutHeight = -1},
        {0},
        { /* +A06C singleSelectionStatsText g_UiListOffsetControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xA0CC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9FAC),
            .vtable = THANDOR_PTR(&g_UiListOffsetControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000100, 0x00000000, 0x0018002C, 0x01000000, 0x00180006},
        { /* +A0CC singleSelectionUpgradeButton g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9FAC),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x100},
        {
            0x00000200, 0x00001010, 0x00000000, 0x000000A7, 0x00000000, 0x000000A8},
        { /* +A140 hoverItemIcon g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xA19C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9FAC),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .layoutWidth = -1, .layoutHeight = -1},
        {0},
        { /* +A19C hoverItemStatsText g_UiListOffsetControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9FAC),
            .vtable = THANDOR_PTR(&g_UiListOffsetControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000100, 0x00000000, 0x0018002C, 0x01000000},
        { /* +A1F8 multiSelectionCell00 g_UiArmyMetricsPanelVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xA258), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9FAC),
            .vtable = THANDOR_PTR(&g_UiArmyMetricsPanelVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .layoutWidth = -1, .layoutHeight = -1},
        {0},
        { /* +A258 multiSelectionCell01 g_UiArmyMetricsPanelVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xA2B8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9FAC),
            .vtable = THANDOR_PTR(&g_UiArmyMetricsPanelVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .layoutWidth = -1, .layoutHeight = -1},
        {0},
        { /* +A2B8 multiSelectionCell02 g_UiArmyMetricsPanelVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xA318), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9FAC),
            .vtable = THANDOR_PTR(&g_UiArmyMetricsPanelVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .layoutWidth = -1, .layoutHeight = -1},
        {0},
        { /* +A318 multiSelectionCell03 g_UiArmyMetricsPanelVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xA378), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9FAC),
            .vtable = THANDOR_PTR(&g_UiArmyMetricsPanelVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .layoutWidth = -1, .layoutHeight = -1},
        {0},
        { /* +A378 multiSelectionCell04 g_UiArmyMetricsPanelVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xA3D8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9FAC),
            .vtable = THANDOR_PTR(&g_UiArmyMetricsPanelVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .layoutWidth = -1, .layoutHeight = -1},
        {0},
        { /* +A3D8 multiSelectionCell05 g_UiArmyMetricsPanelVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xA438), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9FAC),
            .vtable = THANDOR_PTR(&g_UiArmyMetricsPanelVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .layoutWidth = -1, .layoutHeight = -1},
        {0},
        { /* +A438 multiSelectionCell06 g_UiArmyMetricsPanelVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xA498), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9FAC),
            .vtable = THANDOR_PTR(&g_UiArmyMetricsPanelVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .layoutWidth = -1, .layoutHeight = -1},
        {0},
        { /* +A498 multiSelectionCell07 g_UiArmyMetricsPanelVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xA4F8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9FAC),
            .vtable = THANDOR_PTR(&g_UiArmyMetricsPanelVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .layoutWidth = -1, .layoutHeight = -1},
        {0},
        { /* +A4F8 multiSelectionCell08 g_UiArmyMetricsPanelVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xA558), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9FAC),
            .vtable = THANDOR_PTR(&g_UiArmyMetricsPanelVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .layoutWidth = -1, .layoutHeight = -1},
        {0},
        { /* +A558 multiSelectionCell09 g_UiArmyMetricsPanelVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xA5B8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9FAC),
            .vtable = THANDOR_PTR(&g_UiArmyMetricsPanelVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .layoutWidth = -1, .layoutHeight = -1},
        {0},
        { /* +A5B8 multiSelectionCell10 g_UiArmyMetricsPanelVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xA618), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9FAC),
            .vtable = THANDOR_PTR(&g_UiArmyMetricsPanelVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .layoutWidth = -1, .layoutHeight = -1},
        {0},
        { /* +A618 multiSelectionCell11 g_UiArmyMetricsPanelVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9FAC),
            .vtable = THANDOR_PTR(&g_UiArmyMetricsPanelVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .layoutWidth = -1, .layoutHeight = -1},
        {0},
        { /* +A678 heightToolPanel g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9EE0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00000000, 0x000000A6},
        { /* +A6D4 materialPalettePanel g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xA730), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9EE0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00000000, 0x000000A6},
        { /* +A730 materialSwatch00 g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xA7E8), .firstChild = UI_TEMPLATE_LINK(0xA78C), .parent = UI_TEMPLATE_LINK(0x9EE0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x2AAAAAAA, .bottomAnchorQ31 = 0x20000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000009},
        { /* +A78C materialSwatch00Selector g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xA730),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000003, 0x00001110, 0xFFFFFFFF},
        { /* +A7E8 materialSwatch01 g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xA8A0), .firstChild = UI_TEMPLATE_LINK(0xA844), .parent = UI_TEMPLATE_LINK(0x9EE0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x2AAAAAAA, .rightAnchorQ31 = 0x55555555, .bottomAnchorQ31 = 0x20000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000009},
        { /* +A844 materialSwatch01Selector g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xA7E8),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000001, 0x00001110, 0xFFFFFFFF},
        { /* +A8A0 materialSwatch02 g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xA958), .firstChild = UI_TEMPLATE_LINK(0xA8FC), .parent = UI_TEMPLATE_LINK(0x9EE0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x55555555, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x20000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000009},
        { /* +A8FC materialSwatch02Selector g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xA8A0),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000001, 0x00001110, 0xFFFFFFFF},
        { /* +A958 materialSwatch03 g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xAA10), .firstChild = UI_TEMPLATE_LINK(0xA9B4), .parent = UI_TEMPLATE_LINK(0x9EE0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topAnchorQ31 = 0x20000000, .rightAnchorQ31 = 0x2AAAAAAA, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000009},
        { /* +A9B4 materialSwatch03Selector g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xA958),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000001, 0x00001110, 0xFFFFFFFF},
        { /* +AA10 materialSwatch04 g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xAAC8), .firstChild = UI_TEMPLATE_LINK(0xAA6C), .parent = UI_TEMPLATE_LINK(0x9EE0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x2AAAAAAA, .topAnchorQ31 = 0x20000000, .rightAnchorQ31 = 0x55555555, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000009},
        { /* +AA6C materialSwatch04Selector g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xAA10),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000001, 0x00001110, 0xFFFFFFFF},
        { /* +AAC8 materialSwatch05 g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xAB80), .firstChild = UI_TEMPLATE_LINK(0xAB24), .parent = UI_TEMPLATE_LINK(0x9EE0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x55555555, .topAnchorQ31 = 0x20000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000009},
        { /* +AB24 materialSwatch05Selector g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xAAC8),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000001, 0x00001110, 0xFFFFFFFF},
        { /* +AB80 materialSwatch06 g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xAC38), .firstChild = UI_TEMPLATE_LINK(0xABDC), .parent = UI_TEMPLATE_LINK(0x9EE0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x2AAAAAAA, .bottomAnchorQ31 = 0x60000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000009},
        { /* +ABDC materialSwatch06Selector g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xAB80),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000001, 0x00001110, 0xFFFFFFFF},
        { /* +AC38 materialSwatch07 g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xACF0), .firstChild = UI_TEMPLATE_LINK(0xAC94), .parent = UI_TEMPLATE_LINK(0x9EE0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x2AAAAAAA, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x55555555, .bottomAnchorQ31 = 0x60000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000009},
        { /* +AC94 materialSwatch07Selector g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xAC38),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000001, 0x00001110, 0xFFFFFFFF},
        { /* +ACF0 materialSwatch08 g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xADA8), .firstChild = UI_TEMPLATE_LINK(0xAD4C), .parent = UI_TEMPLATE_LINK(0x9EE0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x55555555, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x60000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000009},
        { /* +AD4C materialSwatch08Selector g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xACF0),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000001, 0x00001110, 0xFFFFFFFF},
        { /* +ADA8 materialSwatch09 g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xAE60), .firstChild = UI_TEMPLATE_LINK(0xAE04), .parent = UI_TEMPLATE_LINK(0x9EE0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .topAnchorQ31 = 0x60000000, .rightAnchorQ31 = 0x2AAAAAAA, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000009},
        { /* +AE04 materialSwatch09Selector g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xADA8),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000001, 0x00001110, 0xFFFFFFFF},
        { /* +AE60 materialSwatch10 g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xAF18), .firstChild = UI_TEMPLATE_LINK(0xAEBC), .parent = UI_TEMPLATE_LINK(0x9EE0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x2AAAAAAA, .topAnchorQ31 = 0x60000000, .rightAnchorQ31 = 0x55555555, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000009},
        { /* +AEBC materialSwatch10Selector g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xAE60),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000001, 0x00001110, 0xFFFFFFFF},
        { /* +AF18 materialSwatch11 g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0xAF74), .parent = UI_TEMPLATE_LINK(0x9EE0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x55555555, .topAnchorQ31 = 0x60000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000009},
        { /* +AF74 materialSwatch11Selector g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xAF18),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000001, 0x00001110, 0xFFFFFFFF},
        { /* +AFD0 smoothingToolPanel g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9EE0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00000000, 0x000000A6},
        { /* +B02C unitPlacementPanel g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0xB088), .parent = UI_TEMPLATE_LINK(0x9EE0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00000000, 0x000000A6},
        { /* +B088 unitPlacementStatsText g_UiListOffsetControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB02C),
            .vtable = THANDOR_PTR(&g_UiListOffsetControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000100, 0x00000000, 0x0018002C, 0x01000000},
        { /* +B0E4 objectPlacementPanel g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9EE0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00000000, 0x000000A6},
        { /* +B140 regionToolPanel g_UiImagePanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x9EE0),
            .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x00000000, 0x000000A6},
        { /* +B19C modeCommandPageStack g_UiLayoutContainerControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x40AC),
            .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000008, 0x0000B210, 0x0000B610, 0x0000B7F0, 0x0000B9D0, 0x0000BC28, 0x0000BD90, 0xFFFFFFFF,
            0x0000BEF8, 0x00180009},
        { /* +B210 selectionGroupButton0 g_UiCommandSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xB290), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000200, 0x0000100A, 0x00000000, 0x0000002E, 0x00000000, 0x0000002F, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0018000A},
        { /* +B290 selectionGroupButton1 g_UiCommandSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xB310), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000200, 0x0000100A, 0x00000000, 0x00000032, 0x00000000, 0x00000033, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0018000B},
        { /* +B310 selectionGroupButton2 g_UiCommandSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xB390), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000200, 0x0000100A, 0x00000000, 0x00000036, 0x00000000, 0x00000037, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0018000C},
        { /* +B390 selectionGroupButton3 g_UiCommandSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xB410), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000200, 0x0000100A, 0x00000000, 0x0000003A, 0x00000000, 0x0000003B, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0018000D},
        { /* +B410 selectionGroupButton4 g_UiCommandSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xB490), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000200, 0x0000100A, 0x00000000, 0x0000003E, 0x00000000, 0x0000003F, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0018000E},
        { /* +B490 selectionGroupButton5 g_UiCommandSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xB510), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000200, 0x0000100A, 0x00000000, 0x00000042, 0x00000000, 0x00000043, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0018000F},
        { /* +B510 selectionGroupButton6 g_UiCommandSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xB590), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000200, 0x0000100A, 0x00000000, 0x00000046, 0x00000000, 0x00000047, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00180010},
        { /* +B590 selectionGroupButton7 g_UiCommandSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiCommandSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000200, 0x0000100A, 0x00000000, 0x0000004A, 0x00000000, 0x0000004B, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0018001A},
        { /* +B610 heightToolOption0 g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xB688), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000203, 0x00001108, 0x00000000, 0x0000007E, 0x00000000, 0x0000007F, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x0018001B},
        { /* +B688 heightToolOption1 g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xB700), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000201, 0x00001109, 0x00000000, 0x00000082, 0x00000000, 0x00000083, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x0018001C},
        { /* +B700 heightToolOption2 g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xB778), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000201, 0x0000110A, 0x00000000, 0x00000086, 0x00000000, 0x00000087, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x0018001D},
        { /* +B778 heightToolOption3 g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000201, 0x0000110B, 0x00000000, 0x0000008A, 0x00000000, 0x0000008B, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x0018001E},
        { /* +B7F0 materialToolOption0 g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xB868), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000203, 0x0000110C, 0x00000000, 0x0000008E, 0x00000000, 0x0000008F, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x0018001F},
        { /* +B868 materialToolOption1 g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xB8E0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000201, 0x0000110D, 0x00000000, 0x00000092, 0x00000000, 0x00000093, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00180020},
        { /* +B8E0 materialToolOption2 g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xB958), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000201, 0x0000110E, 0x00000000, 0x00000096, 0x00000000, 0x00000097, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x0018001D},
        { /* +B958 materialToolOption3 g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000201, 0x0000110F, 0x00000000, 0x0000008A, 0x00000000, 0x0000008B, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00180021},
        { /* +B9D0 smoothingToolOption0 g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xBA48), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000203, 0x00001117, 0x00000000, 0x0000009A, 0x00000000, 0x0000009B, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00180022},
        { /* +BA48 smoothingToolOption1 g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xBAC0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000201, 0x00001118, 0x00000000, 0x0000009E, 0x00000000, 0x0000009F, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00180023},
        { /* +BAC0 smoothingToolOption2 g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xBB38), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000201, 0x00001119, 0x00000000, 0x000000A0, 0x00000000, 0x000000A1, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00180024},
        { /* +BB38 smoothingRelaxGatedButton g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xBBB0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000200, 0x0000111A, 0x00000000, 0x00000080, 0x00000000, 0x00000081, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00180025},
        { /* +BBB0 smoothingRelaxLandButton g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000200, 0x0000111B, 0x00000000, 0x00000084, 0x00000000, 0x00000085, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00180028},
        { /* +BC28 unitPlacementOption0 g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xBCA0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000203, 0x00001111, 0x00000000, 0x00000090, 0x00000000, 0x00000091, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00180029},
        { /* +BCA0 unitPlacementOption2 g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xBD18), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000201, 0x00001113, 0x00000000, 0x00000094, 0x00000000, 0x00000095, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x0018002A},
        { /* +BD18 unitPlacementOption1 g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000201, 0x00001112, 0x00000000, 0x00000098, 0x00000000, 0x00000099, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x0018002B},
        { /* +BD90 objectPlacementOption0 g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xBE08), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000203, 0x00001114, 0x00000000, 0x0000009C, 0x00000000, 0x0000009D, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00180029},
        { /* +BE08 objectPlacementOption2 g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xBE80), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000201, 0x00001116, 0x00000000, 0x00000094, 0x00000000, 0x00000095, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x0018002A},
        { /* +BE80 objectPlacementOption1 g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000201, 0x00001115, 0x00000000, 0x00000098, 0x00000000, 0x00000099, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00180026},
        { /* +BEF8 regionToolOption0 g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xBF70), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000203, 0x0000111C, 0x00000000, 0x00000088, 0x00000000, 0x00000089, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00180027},
        { /* +BF70 regionToolOption1 g_UiSpriteButtonControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB19C),
            .vtable = THANDOR_PTR(&g_UiSpriteButtonControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
        {
            0x00000201, 0x0000111D, 0x00000000, 0x0000008C, 0x00000000, 0x0000008D},
};

__declspec(align(4)) InGameRuntimeRoot *g_InGameRuntimeRoot = 0;

/* L"flm\\ende0000.flm" */
uint16_t g_SessionEndMoviePathUtf16[17] = {'f', 'l', 'm', '\\', 'e', 'n', 'd', 'e', '0', '0', '0', '0', '.', 'f', 'l', 'm', 0};

/* L"flm\\ende0001.flm" */
uint16_t g_FlmEnde0001FlmPathUtf16[17] = {'f', 'l', 'm', '\\', 'e', 'n', 'd', 'e', '0', '0', '0', '1', '.', 'f', 'l', 'm', 0};

uint32_t g_SessionNetworkTickCounter = 0;

TerrainRegionCollectionCount g_TerrainRegionCollectionStoredCount = 0;

TerrainRegionCollectionCount g_TerrainRegionCollectionVisitedCount = 0;

uint32_t g_TerrainRegionCollectionEntries = 0;

SelectionPlayerRuntimeBlock *g_SelectionPlayerBlocks = 0;

uint32_t g_InGameActiveEffectVoice = 0;

uint32_t g_InGameEffectsEnabled = 0;

uint32_t g_InGameActiveMusicVoice = 0;

uint32_t g_InGameMusicNextTrackCountdown = 0;

uint16_t g_InGameCountdownTextUtf16[8] = {0};

uint32_t g_InGameNetworkTickCountdown = 0;

uint32_t g_InGameStateTickSpinLock = 0;

uint32_t g_EndMovieVariantIndex = 0;

uint16_t *g_EndMoviePath = 0;

static uint32_t g_HostCommandBatchSyncSentThisInterval = 0;

static void *g_InGameFactionScratchBufferSetA8[8] = {0};

static void *g_InGameFactionScratchBufferSetB8[8] = {0};

/* uint32_t[24] energy allocation priority per model runtime class (0 = none, up to 0x12); gameplay/session/runtime.c energy distribution */
static const uint32_t g_FactionEnergyAllocationPriorityByModelClass[24] = {
    /*  0 */ 0, 0, 0, 0, 256, 768, 1024, 1280, 1536, 1792, 512, 4608, 0, 2048, 4096, 0,
    /* 16 */ 0, 0, 0, 0, 0, 0, 2048, 256};

/* the notification movie path; the three digits at [9] are overwritten with
   the movie number before it is opened (gameplay/session/runtime.c) */
/* L"flm\\movie000.flm" */
static uint16_t g_FlmMovie000FlmPathUtf16[17] = {'f', 'l', 'm', '\\', 'm', 'o', 'v', 'i', 'e', '0', '0', '0', '.', 'f', 'l', 'm', 0};

static int32_t g_InGamePendingSimulationTicks = 0;

static uint32_t g_InGameSessionNotificationTimeoutTicks = 0;

static uint32_t g_EndGameResultsCurrentMusicTrackId = 0;

static WorldObjectRecord *g_InGameWorldObjectRecords = 0;

static uint32_t g_InGameWorldRuntimeDwordArray256[256] = {0};

/* 15 command records and the terminator
   record (commandCode 0) that ends the dispatcher's scan */
static const UiCommandDispatchRecord g_EndGameResultsCommandDispatchRecords_00_Code00030071_Modifier30[16] = {
    /*  0 */ {.commandCode = 0x30071, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x567870},
    /*  1 */ {.commandCode = 0x10001, .continuationEntryAddress = 0x567270},
    /*  2 */ {.commandCode = 0x30063, .continuationEntryAddress = 0x5674B0},
    /*  3 */ {.commandCode = 0x10000, .continuationEntryAddress = 0x567410},
    /*  4 */ {.commandCode = 0x20001, .continuationEntryAddress = 0x567460},
    /*  5 */ {.commandCode = 0x20002, .continuationEntryAddress = 0x5673A0},
    /*  6 */ {.commandCode = 0x20004, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x567340},
    /*  7 */ {.commandCode = 0x30070, .continuationEntryAddress = 0x5675E0},
    /*  8 */ {.commandCode = 0x30067, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x567660},
    /*  9 */ {.commandCode = 0x30067, .continuationEntryAddress = 0x567620},
    /* 10 */ {.commandCode = 0x10002, .continuationEntryAddress = 0x5676A0},
    /* 11 */ {.commandCode = 0x30070, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x5677E0},
    /* 12 */ {.commandCode = 0x30078, .modifierClassFlags = 0x3C, .continuationEntryAddress = 0x567200},
    /* 13 */ {.commandCode = 0x30065, .modifierClassFlags = 0x3C, .continuationEntryAddress = 0x567230},
    /* 14 */ {.commandCode = 0x3007A, .modifierClassFlags = 0x3C, .continuationEntryAddress = 0x5671E0},
    /* 15 */ {.commandCode = 0x0, .modifierClassFlags = 0x90909090, .continuationEntryAddress = 0x90909090}}; /* commandCode 0, the rest is the original's NOP fill */

static uint8_t g_InGameSessionStartedNetworked = 0;

InGameSimulationStepBatchTicks g_InGameSimulationStepTicks = 0;

/* Implementation ownership: gameplay/session/runtime. */

/* Failure exit of InGameRuntime_RunSessionUntilExit: releases what the session set up and reports the error. */
static Bool8 InGameRuntime_FailSession(uint32_t sessionError,uint32_t *outError)

{
  InGameRuntime_ShutdownAndReleaseResources();
  *outError = sessionError;
  return false;
}


/* Runs one in-game session from the frontend: starts a new level or loads a saved game (bit 0 of
   loadExistingSessionFlag), then renders frames until the session is closed, the end movie is due or the local
   player left, tears the session down along the matching path and returns true. A failed start or an emptied UI
   root stack returns false with the error code in *outError, which the caller hands to the fatal-error dispatcher.
*/
Bool8 InGameRuntime_RunSessionUntilExit(LevelAssetRuntimePrefix *levelAsset,
          FrontendBooleanState32 loadExistingSessionFlag,uint16_t *levelPathUtf16,uint32_t *outError)

{
  uint32_t startupError;
  Bool8 started;

  if ((loadExistingSessionFlag & 1U) == 0) {
    started = InGameRuntime_InitializeNewSession(levelAsset,levelPathUtf16,&startupError);
  }
  else {
    started = InGameRuntime_InitializeLoadedSession(levelPathUtf16,&startupError);
  }
  if (!started) {
    return InGameRuntime_FailSession(startupError,outError);
  }
  DebugHook_SessionStarted();
  do {
    DebugHook_SessionFrameBegin();
    /* two pending simulation ticks are consumed per rendered frame, clamped at zero */
    g_InGamePendingSimulationTicks = g_InGamePendingSimulationTicks - 2;
    if ((int)g_InGamePendingSimulationTicks < 0) {
      g_InGamePendingSimulationTicks = 0;
    }
    UiRootStack_InvalidateAll();
    UiFrame_ProcessAndPresent();
    DebugHook_SessionFrameEnd();
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_SESSION_CLOSED) != 0) {
      g_SoundStopAllVoices();
      g_TimerUnregisterPeriodic(InGameRuntime_ProcessQueuedSessionNotificationTimer);
      GridScratch_ReleaseBuffers();
      g_EndMovieSelectionIndex = 0;
      OldUnitRuntime_RebuildScenarioReplayTables();
      UiRuntime_SetSynchronizationHooks(NULL,NULL);
      UiRootStack_PopUntilWindowTextureBoundary();
      InGameRuntime_ShutdownAndReleaseResources();
      return true;
    }
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING) != 0) {
      g_SoundStopAllVoices();
      g_TimerUnregisterPeriodic(InGameRuntime_ProcessQueuedSessionNotificationTimer);
      GridScratch_ReleaseBuffers();
      /* FrontendSession_PeriodicTick keeps running under the in-game tick lock while the end movie plays */
      UiRuntime_SetSynchronizationHooks(FrontendSession_PeriodicTick,(RuntimeSpinLockValue *)&g_InGameStateTickSpinLock);
      Frontend_PlaySelectedEndMovie();
      OldUnitRuntime_RebuildScenarioReplayTables();
      UiRuntime_SetSynchronizationHooks(NULL,NULL);
      UiRootStack_PopUntilWindowTextureBoundary();
      InGameRuntime_ShutdownAndReleaseResources();
      g_FrontendScenarioPathScratchUtf16[0] = 0;
      return true;
    }
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_LOCAL_PLAYER_LEFT) != 0) {
      g_SoundStopAllVoices();
      g_TimerUnregisterPeriodic(InGameRuntime_ProcessQueuedSessionNotificationTimer);
      GridScratch_ReleaseBuffers();
      UiRuntime_SetSynchronizationHooks(NULL,NULL);
      InGameRuntime_ShutdownAndReleaseResources();
      g_FrontendScenarioPathScratchUtf16[0] = 0;
      return true;
    }
  } while (g_UiRootNode != UI_ROOT_STACK_END);
  return InGameRuntime_FailSession(FATAL_ERROR_GENERAL_FAILURE,outError);
}


/* Placement overlay: grey the field and mark where the pending army asset fits; refreshed every 8th simulation
   tick, removed once the placement ends. */
static void InGameUiRoot_UpdatePlacementOverlay(InGameRuntimeRootFrameView *inGameRoot)

{
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PLACEMENT_OVERLAY_SHOWN) == 0) {
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING) != 0) {
      g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_PLACEMENT_OVERLAY_SHOWN;
      FieldGrid_SetAllCellOverlayColors(INGAME_PLACEMENT_OVERLAY_ARGB,(inGameRoot->worldRuntime).fieldGrid);
      WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries
                (THANDOR_PTR(g_InGamePendingPlacementArmyAsset),&inGameRoot->worldRuntime);
    }
  }
  else if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING) == 0) {
    g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & ~UI_COMMAND_RUNTIME_FLAG_PLACEMENT_OVERLAY_SHOWN;
    FieldGrid_SetAllCellOverlayColors(ARGB8888_OPAQUE_WHITE,(inGameRoot->worldRuntime).fieldGrid);
  }
  else if ((g_GameFactionRuntimeImage.tail.simulationTick & 7) == 0) {
    FieldGrid_SetAllCellOverlayColors(INGAME_PLACEMENT_OVERLAY_ARGB,(inGameRoot->worldRuntime).fieldGrid);
    WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries
              (THANDOR_PTR(g_InGamePendingPlacementArmyAsset),&inGameRoot->worldRuntime);
  }
}


/* Keeps the camera target within 16 cells of the field: clamps the grid position, converts it back to world
   coordinates and moves target and camera by the difference. */
static void InGameUiRoot_KeepCameraTargetNearField(InGameRuntimeRootFrameView *inGameRoot)

{
  WorldRuntimeContext *worldRuntime;
  WorldMotionState *cameraMotion;
  FieldGridAsset *cameraFieldGrid;
  FieldGridCoordinates targetGridPosition;
  int64_t projectedProduct;
  int targetColumnQ12;
  int targetRowQ12;
  int gridColumn;
  int gridRow;
  int outOfBoundsAxisCount;
  int deltaX;
  int deltaY;

  worldRuntime = &inGameRoot->worldRuntime;
  cameraMotion = &worldRuntime->motion;
  cameraFieldGrid = worldRuntime->fieldGrid;
  outOfBoundsAxisCount = 0;
  targetGridPosition = FieldGrid_WorldToGridQ12(cameraMotion->targetPositionYQ12,cameraMotion->targetPositionXQ12);
  targetRowQ12 = targetGridPosition.rowQ12;
  targetColumnQ12 = targetGridPosition.columnQ12;
  gridColumn = (targetColumnQ12 >> Q12_SHIFT) - 8;
  gridRow = (targetRowQ12 >> Q12_SHIFT) - 8;
  if (gridColumn < -16) {
    targetColumnQ12 = -8 * Q12_ONE;
    outOfBoundsAxisCount = 1;
  }
  else if ((int)cameraFieldGrid->gridWidth < gridColumn) {
    outOfBoundsAxisCount = 1;
    targetColumnQ12 = (cameraFieldGrid->gridWidth + 8) * Q12_ONE;
  }
  if (gridRow < -16) {
    targetRowQ12 = -8 * Q12_ONE;
    outOfBoundsAxisCount++;
  }
  else if ((int)cameraFieldGrid->gridHeight < gridRow) {
    outOfBoundsAxisCount++;
    targetRowQ12 = (cameraFieldGrid->gridHeight + 8) * Q12_ONE;
  }
  if (outOfBoundsAxisCount != 0) {
    projectedProduct = (int64_t)(targetRowQ12 + targetColumnQ12 * 2) * FIELD_GRID_WORLD_COLUMN_STEP_X;
    deltaX = FIXED_PRODUCT_SHR(projectedProduct,13) - cameraMotion->targetPositionXQ12;
    deltaY = FIXED_PRODUCT_SHR(((int64_t)targetRowQ12 * -1999),Q12_SHIFT) - cameraMotion->targetPositionYQ12;
    cameraMotion->targetPositionXQ12 = cameraMotion->targetPositionXQ12 + deltaX;
    cameraMotion->targetPositionYQ12 = cameraMotion->targetPositionYQ12 + deltaY;
    cameraMotion->positionXQ12 = cameraMotion->positionXQ12 + deltaX;
    cameraMotion->positionYQ12 = cameraMotion->positionYQ12 + deltaY;
    WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  }
}


/* Ambient effect sounds (only called with the effects sound option on). Every 8th frame the spatial sound gains of
   all world objects are recomputed. g_InGameEffectsEnabled counts frames until the next ambient effect sound: at 0
   it waits for the current one to end and starts a new delay of 1..64 frames; when it counts down to 0 one of the
   four level effects plays. */
static void InGameUiRoot_UpdateEffectSounds
          (InGameRuntimeRootFrameView *inGameRoot,InGamePresentationTick currentPresentationTick)

{
  WorldRuntimeContext *worldRuntime;
  WorldOwnerListNode *ownerNode;
  uint32_t randomValue;
  uint32_t effectsGain;
  IDirectSoundBuffer *playedVoice;

  worldRuntime = &inGameRoot->worldRuntime;
  if ((currentPresentationTick & 7) == 0) {
    SpatialSoundPool_ClearDesiredGains();
    for (ownerNode = worldRuntime->ownerListHead; ownerNode != NULL; ownerNode = ownerNode->nextNode) {
      /* the callback of the node's owner class (model, shot or effect) */
      (*(&g_RuntimeMaintenanceCallbackPhases.audioRefresh.army)[ownerNode->ownerClassId])(worldRuntime,ownerNode);
    }
    SpatialSoundPool_ApplyDesiredGains();
    InGameSelectionDetailPanel_Rebuild();
  }
  if (g_InGameEffectsEnabled == 0) {
    if (g_SoundIsVoicePlaying((IDirectSoundBuffer *)g_InGameActiveEffectVoice)) {
      g_InGameActiveEffectVoice = NULL;
      randomValue = Random_NextPrimary();
      g_InGameEffectsEnabled = (randomValue & INGAME_AMBIENT_SOUND_DELAY_MASK) + 1;
    }
  }
  else {
    g_InGameEffectsEnabled--;
    if (g_InGameEffectsEnabled == 0) {
      effectsGain = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_EFFECTS_GAIN);
      randomValue = Random_NextPrimary();
      if (g_SoundPlayOneShot
                    (effectsGain,effectsGain,
                     g_InGameLevelEffectVoiceSets[randomValue & 3],&playedVoice)) {
        g_InGameActiveEffectVoice = (uint32_t)playedVoice;
      }
    }
  }
}


/* Music (with the music sound option on): same delay scheme as the ambient effect sounds, then the best-suited of
   the four level tracks plays; nothing plays when no track scores above 0. */
static void InGameUiRoot_UpdateMusic(WorldRuntimeContext *worldRuntime)

{
  uint32_t soundOptionFlags;
  InGameLevelConditionStorage *levelConditionStorage;
  uint32_t randomValue;
  uint32_t trackIndex;
  uint32_t trackScore;
  uint32_t bestTrackIndex;
  uint32_t bestTrackScore;
  uint32_t musicGain;
  LevelMusicSampleNumber selectedMusicTrackId;
  IDirectSoundBuffer *playedVoice;

  soundOptionFlags = PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
  if ((soundOptionFlags & PERSISTENT_SOUND_OPTION_MUSIC) == 0) {
    return;
  }
  if (g_InGameMusicNextTrackCountdown == 0) {
    if (g_SoundIsVoicePlaying((IDirectSoundBuffer *)g_InGameActiveMusicVoice)) {
      g_InGameActiveMusicVoice = NULL;
      randomValue = Random_NextPrimary();
      g_InGameMusicNextTrackCountdown = (randomValue & INGAME_AMBIENT_SOUND_DELAY_MASK) + 1;
    }
    return;
  }
  g_InGameMusicNextTrackCountdown--;
  if (g_InGameMusicNextTrackCountdown != 0) {
    return;
  }
  bestTrackScore = 0;
  bestTrackIndex = 0;
  for (trackIndex = 0; trackIndex < 4; trackIndex++) {
    trackScore = InGameMusic_ComputeTrackSuitabilityScore
                      ((levelConditionStorage->levelImage).worldSettings.musicSampleNumbers[trackIndex],worldRuntime);
    if ((int)bestTrackScore < (int)trackScore) {
      bestTrackIndex = trackIndex;
      bestTrackScore = trackScore;
    }
  }
  if (bestTrackScore != 0) {
    selectedMusicTrackId = (levelConditionStorage->levelImage).worldSettings.musicSampleNumbers[bestTrackIndex];
    musicGain = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MUSIC_GAIN);
    g_EndGameResultsCurrentMusicTrackId = selectedMusicTrackId;
    if (g_SoundPlayOneShot
                  (musicGain,musicGain,g_InGameLevelMusicVoiceSets[bestTrackIndex],
                   &playedVoice)) {
      g_InGameActiveMusicVoice = (uint32_t)playedVoice;
    }
  }
}


/* Camera keys: arrows scroll by the configured step, Page Up/Down tilt, Insert/Delete rotate, Home/End zoom
   (0x400 = 1/64 turn, 0x800 = 0.5 in Q12). */
static void InGameUiRoot_ApplyCameraKeys(InGameRuntimeRootFrameView *inGameRoot)

{
  WorldRuntimeContext *worldRuntime;
  WorldMotionState *cameraMotion;
  uint32_t scrollStep;
  AngleTurn32 clampedPitchAngle;
  UQ12 clampedTargetDistance;

  worldRuntime = &inGameRoot->worldRuntime;
  cameraMotion = &worldRuntime->motion;
  if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_LEFT] != 0) {
    scrollStep = PersistentSettings_Read(PERSISTENT_DEFAULT_CAMERA_SCROLL_STEP,PERSISTENT_SETTING_CAMERA_SCROLL_STEP);
    WorldRuntime_TranslateCameraByScreenDelta(0,-scrollStep,worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
  }
  if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_RIGHT] != 0) {
    scrollStep = PersistentSettings_Read(PERSISTENT_DEFAULT_CAMERA_SCROLL_STEP,PERSISTENT_SETTING_CAMERA_SCROLL_STEP);
    WorldRuntime_TranslateCameraByScreenDelta(0,scrollStep,worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
  }
  if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_UP] != 0) {
    scrollStep = PersistentSettings_Read(PERSISTENT_DEFAULT_CAMERA_SCROLL_STEP,PERSISTENT_SETTING_CAMERA_SCROLL_STEP);
    WorldRuntime_TranslateCameraByScreenDelta(-scrollStep,0,worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
  }
  if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_DOWN] != 0) {
    scrollStep = PersistentSettings_Read(PERSISTENT_DEFAULT_CAMERA_SCROLL_STEP,PERSISTENT_SETTING_CAMERA_SCROLL_STEP);
    WorldRuntime_TranslateCameraByScreenDelta(scrollStep,0,worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
  }
  if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_PAGE_UP] != 0) {
    clampedPitchAngle = cameraMotion->pitchAngle - INGAME_CAMERA_KEY_ANGLE_STEP;
    if ((int)clampedPitchAngle < (int)cameraMotion->minimumPitchAngle) {
      clampedPitchAngle = cameraMotion->minimumPitchAngle;
    }
    WorldRuntime_PointCameraAtTarget
              (clampedPitchAngle,cameraMotion->headingAngle,cameraMotion->targetDistanceQ12,
               cameraMotion->targetPositionZQ12,cameraMotion->targetPositionYQ12,cameraMotion->targetPositionXQ12,
               worldRuntime);
  }
  if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_PAGE_DOWN] != 0) {
    clampedPitchAngle = cameraMotion->pitchAngle + INGAME_CAMERA_KEY_ANGLE_STEP;
    if ((int)cameraMotion->maximumPitchAngle < (int)clampedPitchAngle) {
      clampedPitchAngle = cameraMotion->maximumPitchAngle;
    }
    WorldRuntime_PointCameraAtTarget
              (clampedPitchAngle,cameraMotion->headingAngle,cameraMotion->targetDistanceQ12,
               cameraMotion->targetPositionZQ12,cameraMotion->targetPositionYQ12,cameraMotion->targetPositionXQ12,
               worldRuntime);
  }
  if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_INSERT] != 0) {
    WorldRuntime_PointCameraAtTarget
              (cameraMotion->pitchAngle,
               (cameraMotion->headingAngle - INGAME_CAMERA_KEY_ANGLE_STEP) & FIXED_ANGLE16_MASK,
               cameraMotion->targetDistanceQ12,cameraMotion->targetPositionZQ12,cameraMotion->targetPositionYQ12,
               cameraMotion->targetPositionXQ12,worldRuntime);
  }
  if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_DELETE] != 0) {
    WorldRuntime_PointCameraAtTarget
              (cameraMotion->pitchAngle,
               (cameraMotion->headingAngle + INGAME_CAMERA_KEY_ANGLE_STEP) & FIXED_ANGLE16_MASK,
               cameraMotion->targetDistanceQ12,cameraMotion->targetPositionZQ12,cameraMotion->targetPositionYQ12,
               cameraMotion->targetPositionXQ12,worldRuntime);
  }
  if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_HOME] != 0) {
    clampedTargetDistance = cameraMotion->targetDistanceQ12 - INGAME_CAMERA_KEY_DISTANCE_STEP_Q12;
    if ((int)clampedTargetDistance < (int)worldRuntime->minimumCameraDistanceQ12) {
      clampedTargetDistance = worldRuntime->minimumCameraDistanceQ12;
    }
    WorldRuntime_PointCameraAtTarget
              (cameraMotion->pitchAngle,cameraMotion->headingAngle,clampedTargetDistance,
               cameraMotion->targetPositionZQ12,cameraMotion->targetPositionYQ12,cameraMotion->targetPositionXQ12,
               worldRuntime);
  }
  if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_END] != 0) {
    clampedTargetDistance = cameraMotion->targetDistanceQ12 + INGAME_CAMERA_KEY_DISTANCE_STEP_Q12;
    if ((int)worldRuntime->maximumCameraDistanceQ12 < (int)clampedTargetDistance) {
      clampedTargetDistance = worldRuntime->maximumCameraDistanceQ12;
    }
    WorldRuntime_PointCameraAtTarget
              (cameraMotion->pitchAngle,cameraMotion->headingAngle,clampedTargetDistance,
               cameraMotion->targetPositionZQ12,cameraMotion->targetPositionYQ12,cameraMotion->targetPositionXQ12,
               worldRuntime);
  }
}


/* Countdown text: the first of the 64 scheduled conditions that is a running countdown shows its remaining seconds
   as minutes:seconds; without one the timer node stays hidden. */
static void InGameUiRoot_UpdateCountdownText(InGameRuntimeRootFrameView *inGameRoot)

{
  InGameConditionSchedule *schedule;
  uint8_t *countdownText;
  uint32_t conditionIndex;
  uint32_t secondsLeft;
  uint32_t minutesByteLength;

  schedule = &g_InGameLevelRuntimeGlobalBlock.conditionStorage->schedule;
  countdownText = (uint8_t *)THANDOR_ADDR(g_InGameCountdownTextUtf16,0);
  inGameRoot->countdownPanelNodeFlags = inGameRoot->countdownPanelNodeFlags & ~UI_NODE_SUPPRESSED;
  for (conditionIndex = 0; conditionIndex < INGAME_SCHEDULED_CONDITION_COUNT; conditionIndex++) {
    if ((schedule->conditions[conditionIndex].statusAndKind.kind & INGAME_SCHEDULED_CONDITION_KIND_MASK) ==
        INGAME_SCHEDULED_CONDITION_COUNTDOWN_ELAPSED) {
      secondsLeft = schedule->conditions[conditionIndex].payload.operands[1];
      if (secondsLeft != 0) {
        minutesByteLength = g_WideNumberFormatUtf16
                           (WIDE_FORMAT_PAD_WITH_SPACE,0,2,1,secondsLeft / 60,(uint16_t *)countdownText);
        *(uint16_t *)(countdownText + minutesByteLength) = ':';
        g_WideNumberFormatUtf16
                  (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,secondsLeft % 60,
                   (uint16_t *)(countdownText + minutesByteLength + 2));
        return;
      }
    }
  }
  inGameRoot->countdownPanelNodeFlags = inGameRoot->countdownPanelNodeFlags | UI_NODE_SUPPRESSED;
}


/* Frame update of the in-game UI root for the whole session: network session upkeep, the
   placement overlay, cursor frame and edge scrolling, keeping the camera target near the field, and, unless the
   interaction subsystem is active, ambient effect sounds, music selection, the camera keys, the countdown text and
   the terrain texture refresh. Nothing but the network upkeep runs while waiting for players.
*/

void InGameUiRoot_UpdateFrame(InGameRuntimeRootFrameView *inGameRoot)

{
  WorldRuntimeContext *worldRuntime;
  UiNodeBase *hoveredNode;
  uint32_t cursorFrame;
  uint32_t edgeScrollCursorFrame;
  uint32_t soundOptionFlags;
  uint32_t activePageIndex;
  InGamePresentationTick currentPresentationTick;

  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      FrontendHostSession_TickPeerTimeoutsAndDropPlayers();
    }
  }
  else {
    FrontendClientSession_TickHostTimeout();
  }
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS) == 0) {
    InGameUiRoot_UpdatePlacementOverlay(inGameRoot);
    cursorFrame = 0;
    hoveredNode = (*((inGameRoot->rootUi).base.vtable)->hitTest)
                       (g_CursorOverrideY,g_CursorOverrideX,(UiNodeBase *)inGameRoot);
    if (hoveredNode != UI_NODE_NONE) { /* hit test found a node */
      cursorFrame = hoveredNode->vtable->pointerMove(g_CursorOverrideY,g_CursorOverrideX,hoveredNode);
    }
    g_GameFactionRuntimeImage.tail.presentationTick++;
    RecentTextHistory_SortAndBuildPointerList(8,&inGameRoot->recentTextHistory);
    currentPresentationTick = g_GameFactionRuntimeImage.tail.presentationTick;
    worldRuntime = &inGameRoot->worldRuntime;
    InGameHud_UpdateStatusCountersAndSessionPrompts();
    activePageIndex = UiPageStack_ActivePageIndex(&inGameRoot->worldViewAreaPageStack);
    /* edge scrolling only on the plain world view (no drag selection or notification jump, first page, no
       interaction node flag 8) and while cursor button bit 2 is up; its scroll-arrow frame wins over the hovered
       node's frame */
    if (((worldRuntime->runtimeFlags & (WORLD_RUNTIME_FLAG_DRAG_SELECTING | WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO)) == 0) &&
        (activePageIndex == 0) && ((worldRuntime->interaction.nodeFlags & 8) == 0) &&
        ((g_CursorButtonState & 4) == 0)) {
      edgeScrollCursorFrame = WorldRuntime_ApplyEdgeScrollAndGetCursorFrame(worldRuntime);
      if (edgeScrollCursorFrame != 0) {
        cursorFrame = edgeScrollCursorFrame;
      }
    }
    g_GraphicsCursorSetFrame(cursorFrame);
    InGameUiRoot_KeepCameraTargetNearField(inGameRoot);
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE) == 0) {
      TerrainDirectionTable_AdvanceAndRebuildVectors();
      soundOptionFlags =
           PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
      if ((soundOptionFlags & PERSISTENT_SOUND_OPTION_EFFECTS) != 0) {
        InGameUiRoot_UpdateEffectSounds(inGameRoot,currentPresentationTick);
      }
      InGameUiRoot_UpdateMusic(worldRuntime);
      if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PAUSED) != 0) {
        /* paused: no camera keys, countdown or terrain refresh */
        InGameRuntime_UpdateCursorGridAndViewScaleCache();
        return;
      }
      activePageIndex = UiPageStack_ActivePageIndex(&inGameRoot->gameWindowPageStack);
      if (activePageIndex == 2) {
        InGameTechnologyPanel_Rebuild(&inGameRoot->rootUi);
      }
      InterpolationStateTable_Advance256ByTicks(g_InGameSimulationStepTicks);
      InGameUiRoot_ApplyCameraKeys(inGameRoot);
      InGameUiRoot_UpdateCountdownText(inGameRoot);
      currentPresentationTick = g_GameFactionRuntimeImage.tail.presentationTick;
    }
    if ((currentPresentationTick & 31) == 0) {
      TerrainCompositeTexture_FillPlane1();
    }
    if ((currentPresentationTick & 3) == 0) {
      TerrainCompositeTexture_RebuildPlane0();
    }
  }
  InGameRuntime_UpdateCursorGridAndViewScaleCache();
}


/* Turns the saved form of the resource registration records (widget.hex) back into pointers, after a savegame
   load and after writing a savegame: the 1-based offsets become runtime-object, shading-record, army/shot/effect
   slot pointers, texture set and palette are re-selected per domain, and the sprite id is resolved again. Also
   restores the tail record pointer and the local player's faction assignment.
*/
void ResourceRegistrationRuntime_RebaseLoadedRecords(ResourceRegistrationRuntimeImage *runtimeImage)

{
  ResourceRegistrationRecord *registrationRecord;
  ResourceRegistrationRecord *tailRecord;
  uint32_t remainingRecords;
  uint32_t nestedCount;
  uint32_t nestedIndex;
  uint8_t *primaryPointer;
  uint8_t *secondaryPointer;
  uint8_t *nestedBasePointer;
  uint8_t *auxiliaryPointer;
  void *tailNestedPointer;
  GraphicsTextureSet *selectedTextureSet;
  GraphicsPaletteAsset *selectedPalette;
  SpriteAssetHeader *resolvedSprite;
  ArmyRuntimeSlot *payloadSlot;

  registrationRecord = runtimeImage->records;
  remainingRecords = runtimeImage->recordCount;
  /* Original quirk: the record loop tests its count only after the first record, so an image with recordCount 0
     would walk 2^32 records. */
  do {
    if ((registrationRecord->flags & RUNTIME_REGISTRATION_RECORD_ALLOCATED) != 0) {
      primaryPointer = (uint8_t *)(registrationRecord->primaryPointerOrSavedOffset).savedIdOrOffset;
      secondaryPointer = (uint8_t *)(registrationRecord->secondaryPointerOrSavedOffset).runtimePointer;
      nestedBasePointer = (uint8_t *)(registrationRecord->nestedBasePointerOrSavedOffset).runtimePointer;
      /* 1-based offsets from the runtime-object base; 0 stays NULL */
      if (primaryPointer != NULL) {
        primaryPointer = primaryPointer + (int)g_RuntimeObjectRebaseBaseMinusOne;
      }
      if (secondaryPointer != NULL) {
        secondaryPointer = secondaryPointer + (int)g_RuntimeObjectRebaseBaseMinusOne;
      }
      if (nestedBasePointer != NULL) {
        nestedBasePointer = nestedBasePointer + (int)g_RuntimeObjectRebaseBaseMinusOne;
      }
      (registrationRecord->primaryPointerOrSavedOffset).savedIdOrOffset = (uint32_t)primaryPointer;
      (registrationRecord->secondaryPointerOrSavedOffset).runtimePointer = secondaryPointer;
      (registrationRecord->nestedBasePointerOrSavedOffset).runtimePointer = nestedBasePointer;
      (registrationRecord->ownerRuntimeOrSavedOffset).runtimePointer = runtimeImage;
      auxiliaryPointer = (uint8_t *)(registrationRecord->auxiliaryPointerOrSavedOffset).savedIdOrOffset;
      nestedCount = registrationRecord->nestedCount;
      if (auxiliaryPointer != NULL) {
        /* 1-based offset from the shading records; 0 is null */
        auxiliaryPointer = (uint8_t *)(THANDOR_ADDR(g_GraphicsShadingRuntimeRecords,-1) + (int)auxiliaryPointer);
      }
      (registrationRecord->auxiliaryPointerOrSavedOffset).savedIdOrOffset = (uint32_t)auxiliaryPointer;
      /* the nested pointers are 1-based offsets from the runtime-object base as well */
      for (nestedIndex = 0; nestedIndex < nestedCount; nestedIndex++) {
        if (registrationRecord->nestedPointersOrSavedOffsets[nestedIndex].runtimePointer != NULL) {
          registrationRecord->nestedPointersOrSavedOffsets[nestedIndex].runtimePointer =
               (uint8_t *)((int)registrationRecord->nestedPointersOrSavedOffsets[nestedIndex].runtimePointer +
                       (int)g_RuntimeObjectRebaseBaseMinusOne);
        }
      }
      payloadSlot = (registrationRecord->runtimePayload).armyRuntime;
      switch(registrationRecord->domainIndex) {
      case RESOURCE_DOMAIN_ARMY_RUNTIME:
        /* textureSet holds the army graphics binding index until here */
        payloadSlot = (ArmyRuntimeSlot *)
                     ((int)payloadSlot + g_ModelRuntimeRebaseDelta);
        selectedPalette = g_ArmyGraphicsBindings[(int)registrationRecord->textureSet].paletteAsset;
        registrationRecord->textureSet = g_ArmyGraphicsBindings[(int)registrationRecord->textureSet].textureSet;
        registrationRecord->paletteAsset = selectedPalette;
        break;
      case RESOURCE_DOMAIN_SHOT_RUNTIME:
        payloadSlot = (ArmyRuntimeSlot *)
                     (g_ShotRuntimeRebaseBaseMinusOne + (int)payloadSlot);
        registrationRecord->textureSet = g_ShotTextureSet;
        registrationRecord->paletteAsset = g_ShotPalette;
        break;
      case RESOURCE_DOMAIN_EFFECT_RUNTIME:
        payloadSlot = (ArmyRuntimeSlot *)
                     (g_EffectRuntimeRebaseBaseMinusOne + (int)payloadSlot);
        selectedTextureSet = g_EffectTextureSet;
        selectedPalette = g_EffectPalette;
        /* effects flagged 2 in their model runtime use the army graphics of binding 0 */
        if ((((payloadSlot->modelRuntimeOrSavedOffset).modelRuntime)->effectModelFlags
            & 2) != 0) {
          selectedTextureSet = g_ArmyGraphicsBindings[0].textureSet;
          selectedPalette = g_ArmyGraphicsBindings[0].paletteAsset;
        }
        registrationRecord->textureSet = selectedTextureSet;
        registrationRecord->paletteAsset = selectedPalette;
      }
      (registrationRecord->runtimePayload).armyRuntime = payloadSlot;
      resolvedSprite = SpriteAssetRegistry_FindById((SpriteAssetId)registrationRecord->spriteAsset);
      registrationRecord->spriteAsset = resolvedSprite;
    }
    registrationRecord++;
    remainingRecords--;
  } while (remainingRecords != 0);
  /* the last nested slot of the last record is the saved tail record */
  tailNestedPointer = runtimeImage->records[runtimeImage->recordCount - 1].nestedPointersOrSavedOffsets
           [12].runtimePointer;
  tailRecord = NULL;
  if (tailNestedPointer != NULL) {
    tailRecord = (ResourceRegistrationRecord *)(g_RuntimeObjectRebaseBaseMinusOne + (int)tailNestedPointer);
  }
  runtimeImage->tailRecord = tailRecord;
  (g_FrontendPlayerRuntimeBlocks->factionAssignment).factionAssignmentIndex = runtimeImage->factionAssignmentIndex;
}


/* Periodic timer callback of the in-game session: counts the network tick countdown down to zero and advances the
   periodic clock while no resource registration is in progress.
*/
void __cdecl InGameRuntime_PeriodicCountdownAndClockTick(void)

{
  if (g_InGameNetworkTickCountdown != 0) {
    g_InGameNetworkTickCountdown--;
  }
  if (g_InGameResourceRegistrationBusyCount == 0) {
    g_GameFactionRuntimeImage.tail.periodicClockTick++;
  }
  return;
}

/* Keyboard fallback of the in-game UI root: looks the key up in the hotkey table (key code plus required Ctrl/Alt
   combination) and runs its action: chat, message window, menus, save, pause, game speed, side panel,
   screenshot, leaving the game and the three cheat keys (only while cheats are enabled).
*/
Bool8 InGameHotkeys_DispatchCommandByFlags(UiKeyboardStateMask modifierFlags,UiActionId commandCode,
          InGameRuntimeRootFrameView *inGameRoot)

{
  /* The record table holds the original game's continuation addresses inside this function; they are only
     used as keys here, each continuation is one case of the switch below. rt is the runtime root. */
  uint8_t *rt = (uint8_t *)inGameRoot;
  const UiCommandDispatchRecord *record = g_EndGameResultsCommandDispatchRecords_00_Code00030071_Modifier30;
  uint32_t target = 0;
  Bool8 localSession = (g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == 0;

  /* A record without modifier class matches only without Ctrl and Alt; otherwise exactly the named
     combination (Ctrl, Alt, or both) must be held. */

  for (;; record++) {
    uint32_t flags = record->modifierClassFlags;
    if (record->commandCode == 0) {
      return false;
    }
    if (record->commandCode != commandCode) {
      continue;
    }
    if (flags == 0) {
      if ((modifierFlags & (KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT)) != 0) continue;
    }
    else if ((flags & KEYBOARD_STATE_ALT) == 0) {
      if (((modifierFlags & KEYBOARD_STATE_CTRL) == 0) || ((modifierFlags & KEYBOARD_STATE_ALT) != 0)) continue;
    }
    else if ((flags & KEYBOARD_STATE_CTRL) == 0) {
      if (((modifierFlags & KEYBOARD_STATE_CTRL) != 0) || ((modifierFlags & KEYBOARD_STATE_ALT) == 0)) continue;
    }
    else {
      if (((modifierFlags & KEYBOARD_STATE_CTRL) == 0) || ((modifierFlags & KEYBOARD_STATE_ALT) == 0)) continue;
    }
    target = (uint32_t)record->continuationEntryAddress;
    break;
  }
  switch (target) {
  case 0x5671e0: /* Ctrl+Alt+Z, cheat: toggle fast build and research */
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_CHEATS_ENABLED) != 0) {
      g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags ^ UI_COMMAND_RUNTIME_FLAG_CHEAT_FAST_BUILD;
    }
    break;
  case 0x567200: /* Ctrl+Alt+X, cheat: +1000 Xenite (xeniteCurrentQ4 += 1000 << Q4_SHIFT) */
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_CHEATS_ENABLED) != 0) {
      g_GameFactionRuntimeImage.records[((WorldRuntimeContext *)INGAME_UI(rt,
           worldView))->activeFactionRuntimeIndex].xeniteCurrentQ4 += 1000 << Q4_SHIFT;
    }
    break;
  case 0x567230: /* Ctrl+Alt+E, cheat: +100 energy supply and capacity (Q4) */
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_CHEATS_ENABLED) != 0) {
      g_GameFactionRuntimeImage.records[((WorldRuntimeContext *)INGAME_UI(rt,
           worldView))->activeFactionRuntimeIndex].baselineEnergySupplyQ4 += 100 << Q4_SHIFT;
      g_GameFactionRuntimeImage.records[((WorldRuntimeContext *)INGAME_UI(rt,
           worldView))->activeFactionRuntimeIndex].energyGenerationCapacityQ4 += 100 << Q4_SHIFT;
    }
    break;
  case 0x567270: /* Enter: open the chat line */
    UiPageStack_SetActiveIndex(1,(UiPageStackControl *)INGAME_UI(rt,chatInputPageStack));
    ((UiTextEditControl *)INGAME_UI(rt,chatInputTextEdit))->cursorIndex = 0;
    ((UiTextEditControl *)INGAME_UI(rt,chatInputTextEdit))->selectionStart = 0;
    ((UiTextEditControl *)INGAME_UI(rt,chatInputTextEdit))->selectionEnd = 0;
    if (!localSession) {
      UiNodeBase *recipientTab;
      int i;
      for (i = 0; i < 24; i++) {
        ((uint32_t *)((InGameCommandTextEditControlCC *)INGAME_UI(rt,chatInputTextEdit))->textBuffer)[i] = 0;
      }
      /* Original quirk: the result is not tested; with no tab selected this is the last tab */
      UiSelectableGroup_FindVisibleSelected(&recipientTab,NULL,3,INGAME_UI(rt,messageRecipientAllTab),
                                            INGAME_UI(rt,messageRecipientGroupsTab),
                                            INGAME_UI(rt,messageRecipientPlayersTab));
      g_InGameUiActionHandlersPage10.handlers[((UiSelectableControl *)recipientTab)->actionId & 0xff]
                (recipientTab);
    }
    UiKeyboardFocus_Set(INGAME_UI(rt,chatInputTextEdit));
    break;
  case 0x567340: /* Alt+F4: game menu on the quit page */
  case 0x5673a0: /* F2: game menu on the save page (local games only) */
  case 0x567410: /* Esc: game menu */
  case 0x567460: { /* F1: mission objectives */
    UiSelectableControl *toggle;
    if ((target == 0x5673a0) && !localSession) {
      break;
    }
    toggle = (UiSelectableControl *)(target == 0x567460 ? INGAME_UI(rt,missionObjectivesButton) :
                                                     INGAME_UI(rt,inGameMenuButton));
    UiSelectableControl_SetSelected(1,toggle);
    if (((toggle->stateFlags & UI_SPRITE_BUTTON_ACTIVATION_SOUND) != 0) &&
        (((UiSpriteButtonControl *)toggle)->activationSound != NULL)) {
      g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,
                            ((UiSpriteButtonControl *)toggle)->activationSound,NULL);
    }
    if (target == 0x567460) {
      InGameMissionHelpPage_Toggle((UiNodeBase *)toggle);
      break;
    }
    InGameSettingsPage_ToggleAndSynchronizeControls(toggle);
    if (target == 0x567340) {
      InGameQuitMenu_OpenAndRefreshButtons((InGameCommandPanelSourceAddress32)INGAME_UI(rt,
           gameMenuQuitButton));
    }
    else if (target == 0x5673a0) {
      InGameSaveGamePage_RebuildCatalog(INGAME_UI(rt,gameMenuSaveButton));
    }
    break;
  }
  case 0x5674b0: { /* C: toggle the message window (network games only) */
    UiPageStackControl *stack;
    uint32_t index;
    UiNodeBase *recipientTab;
    int i;
    if (localSession) {
      break;
    }
    stack = (UiPageStackControl *)INGAME_UI(rt,gameWindowPageStack);
    index = (UiPageStack_ActivePageIndex(stack) == 1) ? 0 : 1;
    UiPageStack_SetActiveIndex(index,stack);
    INGAME_UI(rt,worldView)->nodeFlags =
         INGAME_UI(rt,worldView)->nodeFlags & ~UI_NODE_SUPPRESSED;
    if (index != 1) {
      break;
    }
    INGAME_UI(rt,worldView)->nodeFlags =
         INGAME_UI(rt,worldView)->nodeFlags | UI_NODE_SUPPRESSED;
    UiKeyboardFocus_ReleaseNode(INGAME_UI(rt,worldView));
    ((UiTextEditControl *)INGAME_UI(rt,messageTextEdit))->cursorIndex = 0;
    ((UiTextEditControl *)INGAME_UI(rt,messageTextEdit))->selectionStart = 0;
    ((UiTextEditControl *)INGAME_UI(rt,messageTextEdit))->selectionEnd = 0;
    for (i = 0; i < 24; i++) {
      ((uint32_t *)((InGameCommandTextEditControlCC *)INGAME_UI(rt,messageTextEdit))->textBuffer)[i] = 0;
    }
    /* Original quirk: the result is not tested; with no tab selected this is the last tab */
    UiSelectableGroup_FindVisibleSelected(&recipientTab,NULL,3,INGAME_UI(rt,messageRecipientAllTab),
                                          INGAME_UI(rt,messageRecipientGroupsTab),
                                          INGAME_UI(rt,messageRecipientPlayersTab));
    g_InGameUiActionHandlersPage10.handlers[((UiSelectableControl *)recipientTab)->actionId & 0xff]
              (recipientTab);
    g_KeyboardFlushEvents();
    break;
  }
  case 0x5675e0: /* P: pause */
    if (localSession) {
      InGameCommand_TogglePauseRequest(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_TOGGLE_PAUSE,0,0,0);
    }
    break;
  case 0x567620: /* G: faster */
  case 0x567660: /* Alt+G: slower */ {
    int step = (target == 0x567620) ? 1 : -1;
    if (localSession) {
      InGameSimulationSpeed_AdjustPlayerAndRecomputeMinimumTicks(g_LocalPlayerRuntimeId,0,0,step);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_ADJUST_GAME_SPEED,0,0,step);
    }
    break;
  }
  case 0x5676a0: { /* Tab: hide or show the side panel; bit 2 of the map/mouse settings remembers it */
    uint32_t settings = PersistentSettings_Read(0,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
    UiPageStackControl *stack = (UiPageStackControl *)INGAME_UI(rt,sidePanelStack);
    if (UiPageStack_ActivePageIndex(stack) != 0) {
      UiPageStack_SetActiveIndex(0,stack);
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(rt,resourceBarModeStack));
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(rt,gamePanelsModeStack));
      INGAME_UI(rt,worldViewArea)->rightOffset = INGAME_UI(rt,sidePanelFrameLeftEdge)->leftOffset;
      settings = settings & ~PERSISTENT_MAP_OPTION_SIDE_PANEL_HIDDEN;
    }
    else {
      UiPageStack_SetActiveIndex(1,stack);
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(rt,resourceBarModeStack));
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(rt,gamePanelsModeStack));
      INGAME_UI(rt,worldViewArea)->rightOffset = 0;
      settings = settings | PERSISTENT_MAP_OPTION_SIDE_PANEL_HIDDEN;
    }
    UiContainer_LayoutChildren((UiNodeBase *)rt);
    PersistentSettings_Write(settings,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
    break;
  }
  case 0x5677e0: { /* Alt+P: screenshot to the next numbered PCX file */
    GraphicsCapturedTextureSourceAsset *capture =
         g_GraphicsFramebufferCaptureRegion(g_FramebufferHeight,g_FramebufferWidth,0,0);
    void *pcxBytes;
    uint32_t pcxByteCount;
    uint32_t pcxError;
    uint16_t *digitHigh = &g_ScreenshotFileNameUtf16[6];
    uint16_t *digitLow = &g_ScreenshotFileNameUtf16[7];
    if (capture == NULL) {
      break;
    }
    if (!Pcx_EncodeCapture(capture,&pcxBytes,&pcxByteCount,&pcxError)) {
      g_MemoryApi.free(capture);
      break;
    }
    FileSystem_WriteBufferToPath(pcxByteCount,pcxBytes,
                                   g_ScreenshotFileNameUtf16);
    g_MemoryApi.free(pcxBytes);
    g_MemoryApi.free(capture);
    /* advance the two-digit number in the file name */
    *digitLow = *digitLow + 1;
    if (*digitLow > '9') {
      *digitHigh = *digitHigh + 1;
      *digitLow = *digitLow - 10;
      if (*digitHigh > '9') {
        *digitHigh = *digitHigh - 10;
      }
    }
    break;
  }
  case 0x567870: /* Alt+Q: leave the game (not as host) */
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != 0) {
      break;
    }
    if (localSession) {
      InGameCommand_HandlePlayerDeparture(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_PLAYER_DEPARTURE,0,0,0);
    }
    break;
  default:
    Thandor_Log("EndGameResults dispatch: unhandled continuation %08x",target);
    break;
  }
  return false;
}


/* Opens the movie of the notification queue head ("flm\movie%03d.flm"). When its first frame is ready, the movie
   becomes the notification button's texture source and the head's payload the active notification; a payload with
   a map target makes the button clickable. */
static void InGameNotification_StartQueueHeadMovie(InGameRuntimeRoot *inGameRoot)

{
  uint32_t notificationMovieNumber;
  MovieRuntime *notificationMovie;

  notificationMovieNumber = inGameRoot->notificationQueue[0].movieId;
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_PAD_WITH_ZERO,0,3,1,notificationMovieNumber,&g_FlmMovie000FlmPathUtf16[9]);
  if (!Movie_Open(MOVIE_OPEN_PACKAGE_ONLY,g_FlmMovie000FlmPathUtf16,NULL,NULL)) {
    return;
  }
  /* movies 100-299 and 700-899 play at the alternate movie gain */
  if ((99 < notificationMovieNumber) &&
      ((notificationMovieNumber < 300) || ((699 < notificationMovieNumber) && (notificationMovieNumber < 900)))) {
    Movie_SetAudioGainQ15(g_MovieAlternateAudioGainQ15);
  }
  if (!Movie_AdvanceFrame(&notificationMovie,NULL)) {
    return;
  }
  inGameRoot->notificationButtonTextureSource = (uint32_t)notificationMovie;
  inGameRoot->notificationButtonSubresource = 0;
  inGameRoot->activeNotificationPayload = inGameRoot->notificationQueue[0].payload;
  if (inGameRoot->notificationButtonCursorFrame == PAYLOAD_ACTIVE) {
    inGameRoot->notificationButtonCursorFrame = NOTIFICATION_INTERACTION_NONE;
  }
  g_InGameSessionNotificationTimeoutTicks = 0;
  if ((inGameRoot->activeNotificationPayload).payloadKind != NOTIFICATION_PAYLOAD_NONE) {
    inGameRoot->notificationButtonCursorFrame = PAYLOAD_ACTIVE;
  }
}


/* Pops the notification queue head: moves entries 1-3 forward and clears the last entry. */
static void InGameNotification_PopQueueHead(InGameRuntimeRoot *inGameRoot)

{
  int slot;

  for (slot = 0; slot < INGAME_NOTIFICATION_QUEUE_SLOTS - 1; slot++) {
    inGameRoot->notificationQueue[slot] = inGameRoot->notificationQueue[slot + 1];
  }
  memset(&inGameRoot->notificationQueue[INGAME_NOTIFICATION_QUEUE_SLOTS - 1],0,
         sizeof(inGameRoot->notificationQueue[INGAME_NOTIFICATION_QUEUE_SLOTS - 1]));
}


/* Periodic timer that plays the queued in-game notification movies: while one plays it advances a frame and, at
   the end, closes it and keeps the notification's map target clickable for 0x280 more ticks; otherwise it starts
   the movie of the queue head ("flm\movie%03d.flm"), makes its payload the active notification and pops the
   four-entry queue.
*/
void InGameRuntime_ProcessQueuedSessionNotificationTimer(void)

{
  GraphicsTextureSourceAsset *panelTextureSource;
  InGameRuntimeRoot *inGameRoot;

  panelTextureSource = g_InGamePanelTextureSource;
  inGameRoot = g_InGameRuntimeRoot;
  /* the target of the last notification stays clickable until the timeout runs out */
  if (g_InGameSessionNotificationTimeoutTicks != 0) {
    g_InGameSessionNotificationTimeoutTicks--;
    if ((g_InGameSessionNotificationTimeoutTicks == 0) &&
        (inGameRoot->notificationButtonCursorFrame == PAYLOAD_ACTIVE)) {
      inGameRoot->notificationButtonCursorFrame = NOTIFICATION_INTERACTION_NONE;
    }
  }
  /* notificationButtonTextureSource holds the playing movie, or the panel texture source when none plays */
  if (panelTextureSource != (GraphicsTextureSourceAsset *)inGameRoot->notificationButtonTextureSource) {
    if (!Movie_AdvanceFrame(NULL,NULL)) {
      Movie_Close();
      g_InGameSessionNotificationTimeoutTicks = 640;
      inGameRoot->notificationButtonTextureSource = (uint32_t)panelTextureSource;
      inGameRoot->notificationButtonSubresource = 37;
    }
  }
  else if (inGameRoot->notificationQueue[0].priority != 0) {
    InGameNotification_StartQueueHeadMovie(inGameRoot);
    InGameNotification_PopQueueHead(inGameRoot);
  }
  return;
}


/* Failure exit of InGameRuntime_InitializeNewSession: closes the level movie (also when it was not opened yet),
   stores the error in *outError and returns false. */
static Bool8 InGameNewSession_Fail(uint32_t error,uint32_t *outError)

{
  Movie_Close();
  *outError = error;
  return false;
}


/* Resets the session state for a new game: session counters and end-movie state, clears the player-removal packet
   area and all selection blocks, sets up every player's frontend record and selection block (not ready, command
   sync pending, fresh timeout, faction and name), and starts the periodic step timer and the UI synchronization
   hooks.
*/
static void InGameNewSession_ResetSessionState(void)

{
  uint32_t *clearCursor;
  int remainingCount;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  FrontendPlayerRuntimeRecord *frontendPlayer;
  SelectionPlayerRuntimeBlock *selectionBlock;
  PlayerRuntimeId playerId;
  uint32_t factionIndex;
  uint32_t *nameSource;
  uint32_t *nameDestination;

  g_UiCommandRuntimeFlags = UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS | UI_COMMAND_RUNTIME_FLAG_PAUSED;
  g_SessionNetworkTickCounter = 1;
  g_HostCommandBatchSyncSentThisInterval = 0;
  g_GameFactionRuntimeImage.tail.simulationTick = 1;
  g_GameFactionRuntimeImage.tail.presentationTick = 0;
  g_InGameSessionNotificationTimeoutTicks = 0;
  g_InGameReadyStateToggleFlags = 0;
  g_EndGameResultsCurrentMusicTrackId = 0;
  g_InGameSelectedTechnologyId = TEC_000_BASIC_TECHNOLOGY;
  g_InGameSessionStartedNetworked =
       (g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
       SESSION_NETWORK_ROLE_LOCAL;
  g_TextureDownsampleShift = PersistentSettings_Read(0,PERSISTENT_SETTING_TEXTURE_QUALITY);
  g_EndMovieSelectionIndex = UINT32_MAX;
  g_EndMovieVariantIndex = 0;
  g_EndMoviePath = NULL;
  /* clear the client packet buffers and all selection blocks (0x10230 dwords). The original clears the six packet
     buffers with one 0x280-byte fill over their contiguous memory range; they are separate variables here, so
     each is cleared on its own, in the original memory order. */
  memset(&g_FrontendClientPlayerRemovalPacket10007,0,sizeof(g_FrontendClientPlayerRemovalPacket10007));
  memset(g_FrontendClientPlayerCommandRecords,0,sizeof(g_FrontendClientPlayerCommandRecords));
  memset(g_FrontendClientCommandBatchPacketBuffer,0,sizeof(g_FrontendClientCommandBatchPacketBuffer));
  memset(&g_FrontendPacket10021Buffer,0,sizeof(g_FrontendPacket10021Buffer));
  memset(&g_FrontendPacket10022Buffer,0,sizeof(g_FrontendPacket10022Buffer));
  memset(&g_FrontendPacket10023Buffer,0,sizeof(g_FrontendPacket10023Buffer));
  clearCursor = (uint32_t *)g_SelectionPlayerBlocks;
  for (remainingCount = SELECTION_PLAYER_BLOCK_COUNT * sizeof(SelectionPlayerRuntimeBlock) / sizeof(uint32_t);
       remainingCount != 0; remainingCount--) {
    *clearCursor = 0;
    clearCursor++;
  }
  /* per player: not ready, command sync pending, fresh timeout; link its selection block and copy the name.
     Original quirk: the player count is tested only after the first player. */
  remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
  selectionBlock = g_SelectionPlayerBlocks;
  frontendPlayer = g_FrontendPlayerRuntimeBlocks;
  do {
    playerId = frontendPlayer->playerRuntimeId;
    (frontendPlayer->factionAssignment).readyOrWaitState = 0;
    frontendPlayer->commandSyncPending = FRONTEND_COMMAND_SYNC_PENDING;
    frontendPlayer->heartbeatExpiryTicks = 1024;
    factionIndex = (frontendPlayer->factionAssignment).factionAssignmentIndex;
    g_SelectionPlayerRuntimeBlockPointers[playerId] = selectionBlock;
    selectionBlock->factionIndex = factionIndex;
    selectionBlock->simulationStepTicks = 1;
    /* Original quirk: 20 dwords (80 bytes) are copied although both name fields hold 20 UTF-16 characters
       (40 bytes), so the copy also covers the 40 bytes behind each of them. */
    nameSource = (uint32_t *)frontendPlayer->playerName.textUtf16;
    nameDestination = (uint32_t *)selectionBlock->playerNameUtf16;
    for (remainingCount = 20; remainingCount != 0; remainingCount--) {
      *nameDestination = *nameSource;
      nameSource++;
      nameDestination++;
    }
    remainingPlayers--;
    selectionBlock++;
    frontendPlayer++;
  } while (remainingPlayers != 0);
  g_SessionTransferTimeoutTicks = 1024;
  g_InGameNetworkTickCountdown = INGAME_TIMER_TICKS_PER_SIMULATION_STEP;
  g_InGameStateTickSpinLock = 0;
  g_TimerRegisterPeriodic(INGAME_PERIODIC_TIMER_HZ,InGameRuntime_PeriodicCountdownAndClockTick);
  UiRuntime_SetSynchronizationHooks
            (InGameRuntime_UpdateSimulationAndNetworkTick,(RuntimeSpinLockValue *)&g_InGameStateTickSpinLock);
}


/* Whether the session name keeps titleChar: the characters Windows forbids in file names and '.' are dropped. */
static Bool8 InGameNewSession_IsSessionNameCharacter(uint16_t titleChar)

{
  switch (titleChar) {
  case '*':
  case '<':
  case '>':
  case '"':
  case '/':
  case '\\':
  case '.':
  case '?':
  case ':':
  case '|':
    return false;
  default:
    return true;
  }
}


/* Sets the text of the in-game template's save-name edit (saveNameEdit, 32 code units) to the level title without
   the characters dropped by InGameNewSession_IsSessionNameCharacter. The first title character is skipped and at
   most the next 31 are looked at. */
static void InGameNewSession_BuildSessionName(UiTextResourceId titleTextIndex)

{
  uint16_t *sessionNameCursor;
  uint16_t *titleSource;
  uint16_t titleChar;
  int remainingCount;

  sessionNameCursor = ((UiRequiredTextEditControl *)&g_InGameRuntimeDefaultImageTemplate.saveNameEdit)->textBuffer;
  for (remainingCount = 32; remainingCount != 0; remainingCount--) {
    *sessionNameCursor = 0;
    sessionNameCursor++;
  }
  titleSource = TextResource_Resolve(titleTextIndex + TEXT_ID_LEVEL_TITLE_BASE);
  sessionNameCursor = ((UiRequiredTextEditControl *)&g_InGameRuntimeDefaultImageTemplate.saveNameEdit)->textBuffer;
  for (remainingCount = 31; remainingCount != 0; remainingCount--) {
    titleSource++;
    titleChar = *titleSource;
    if (titleChar == 0) {
      break;
    }
    if (InGameNewSession_IsSessionNameCharacter(titleChar)) {
      *sessionNameCursor = titleChar;
      sessionNameCursor++;
    }
  }
}


/* Allocates the zeroed world object pool, the selection info panel resources of the local player and the in-game
   root (a copy of g_InGameRuntimeDefaultImageTemplate with its control tree, world callbacks and camera limits) and
   pushes the root onto the UI root stack. Returns true with the root in *outRoot; on failure returns false with
   the error in *outError.
*/
static Bool8 InGameNewSession_CreateRoot(InGameRuntimeRoot **outRoot,uint32_t *outError)

{
  void *objectPool;
  InGameRuntimeRoot *inGameRoot;
  SelectionPlayerRuntimeBlock *localPlayerBlock;
  uint32_t *clearCursor;
  uint32_t *copyCursor;
  uint32_t *templateCursor;
  int remainingCount;
  uint32_t allocationError;
  uint32_t stepError;

  /* 4 MB pool for the world objects (INGAME_WORLD_OBJECT_RECORD_COUNT records), zeroed */
  allocationError = g_MemoryApi.alloc(INGAME_WORLD_OBJECT_RECORD_COUNT * sizeof(WorldObjectRecord),&objectPool);
  if (allocationError != 0) {
    *outError = allocationError;
    return false;
  }
  g_RuntimeObjectRebaseBaseMinusOne = (uint8_t *)objectPool - 1;
  g_InGameWorldObjectRecords = (WorldObjectRecord *)objectPool;
  clearCursor = (uint32_t *)objectPool;
  for (remainingCount = INGAME_WORLD_OBJECT_RECORD_COUNT * sizeof(WorldObjectRecord) / 4; remainingCount != 0;
       remainingCount--) {
    *clearCursor = 0;
    clearCursor++;
  }
  if (!SelectionInfoPanel_InitResources
                     ((SelectionInfoEntitySlots *)
                      g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId],&stepError)) {
    *outError = stepError;
    return false;
  }
  allocationError = g_MemoryApi.alloc(sizeof(InGameRuntimeRoot),(void **)&inGameRoot);
  if (allocationError != 0) {
    *outError = allocationError;
    return false;
  }
  templateCursor = (uint32_t *)&g_InGameRuntimeDefaultImageTemplate;
  g_InGameRuntimeRoot = inGameRoot;
  /* copy the in-game root template (sizeof(InGameRuntimeRoot) / 4 dwords) */
  copyCursor = (uint32_t *)inGameRoot;
  for (remainingCount = sizeof(InGameRuntimeRoot) / 4; remainingCount != 0; remainingCount--) {
    *copyCursor = *templateCursor;
    templateCursor++;
    copyCursor++;
  }
  if (!InGameUiRuntime_InitializeControlTreeResources((UiRootNode *)inGameRoot,&stepError)) {
    *outError = stepError;
    return false;
  }
  /* world input and command callbacks, camera limits, and the step hook for the world runtime */
  /* signature differs: the overlay callback takes GraphicsBooleanState (int), the slot uint32_t */
  inGameRoot->worldOverlayCallback =
       (void (*)(uint32_t, WorldRuntimeContext *))InGameWorldOverlay_RebuildOrReleaseTransientMarkers;
  (inGameRoot->worldRuntime).selection.dispatchCommandCallback =
       InGameUiRuntime_DispatchCommandByCodeAndModifierFlags;
  (inGameRoot->worldRuntime).selection.resolveContextActionPrimaryCallback =
       InGameWorldInput_ResolveContextActionAndCursor;
  (inGameRoot->worldRuntime).selection.resolveContextActionSecondaryCallback =
       InGameWorldInput_ResolveContextActionAndCursor;
  (inGameRoot->worldRuntime).selection.beginPointerCaptureCallback =
       InGameWorldInput_BeginPointerCapture;
  (inGameRoot->worldRuntime).selection.updateDragSelectionCallback =
       InGameWorldInput_UpdateDragSelectionAndCamera;
  (inGameRoot->worldRuntime).selection.commitPointerActionCallback =
       InGameWorldInput_CommitPointerAction;
  /* signature differs: the callback takes void *, the slot WorldRuntimeContext * */
  (inGameRoot->worldRuntime).fieldRegion.clearTransientStateCallback =
       (void (*)(WorldRuntimeContext *))InGameUiRuntime_ResetNotificationButtonCursor;
  (inGameRoot->worldRuntime).selection.dispatchWorldContextActionCallback =
       InGameUiRuntime_DispatchWorldContextActionCallback;
  (inGameRoot->worldRuntime).minimumCameraDistanceQ12 = 8 * Q12_ONE;
  (inGameRoot->worldRuntime).maximumCameraDistanceQ12 = 19 * Q12_ONE;
  (inGameRoot->worldRuntime).motion.minimumPitchAngle = INGAME_CAMERA_MINIMUM_PITCH_ANGLE16;
  (inGameRoot->worldRuntime).motion.maximumPitchAngle = INGAME_CAMERA_MAXIMUM_PITCH_ANGLE16;
  (inGameRoot->worldRuntime).tickSpinLock = &g_InGameStateTickSpinLock;
  (inGameRoot->worldRuntime).simulationAndNetworkTickCallback =
       InGameRuntime_UpdateSimulationAndNetworkTick;
  localPlayerBlock = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId];
  inGameRoot->localPlayerMarkedCellCount = 0;
  inGameRoot->localPlayerMarkedCells = localPlayerBlock->markedCells;
  UiRootStack_Push(&g_InGameUiRootCallbacks,(UiRootNode *)inGameRoot);
  *outRoot = inGameRoot;
  return true;
}


/* Opens the level movie that plays while loading and shows its first frames, attaches the world arrays with the
   local player's faction, resets the game data defaults and loads the level resources, clears the notification
   queue and creates the terrain texture. Returns true on success; on failure returns false with the error in
   *outError.
*/
static Bool8 InGameNewSession_LoadWorld(LevelAssetRuntimePrefix *levelAsset,uint16_t *levelMoviePath,
                                       InGameRuntimeRoot *inGameRoot,uint32_t *outError)

{
  WorldRuntimeContext *world;
  uint16_t *loadingMoviePath;
  MovieRuntime *firstFrameMovie;
  uint32_t movieEndCode;
  PlayerRuntimeId localPlayerId;
  uint32_t localFactionIndex;
  uint32_t resetDefaultsError;
  uint32_t *clearCursor;
  int remainingCount;
  uint32_t stepError;

  world = &inGameRoot->worldRuntime;
  if (!LevelAsset_PrepareEndingMoviePath(levelMoviePath,&levelAsset->header,&loadingMoviePath,&stepError)) {
    *outError = stepError;
    return false;
  }
  if (!Movie_Open(MOVIE_OPEN_PACKAGE_ONLY,loadingMoviePath,NULL,&stepError)) {
    *outError = stepError;
    return false;
  }
  if (!Movie_AdvanceFrame(&firstFrameMovie,&movieEndCode)) {
    *outError = movieEndCode;
    return false;
  }
  inGameRoot->levelMovieRuntime = firstFrameMovie;
  g_MoviePlaybackBaseFrameGroup = 0;
  g_MoviePlaybackScheduleCounter = 0;
  g_MoviePlaybackScheduleSpan = 0;
  g_MoviePlaybackCurrentFrame = 0;
  MoviePlayback_AdvanceToFrameAndPresent(0);
  MoviePlayback_AdvanceToFrameAndPresent(1);
  RecentTextHistory_SortAndBuildPointerList(8,&inGameRoot->recentTextHistory);
  WorldRuntime_AttachObjectArray(INGAME_WORLD_OBJECT_RECORD_COUNT,g_InGameWorldObjectRecords,world);
  localPlayerId = g_LocalPlayerRuntimeId;
  localFactionIndex = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId]->factionIndex;
  levelAsset->playerSlots[6].aiClassOrMode = localFactionIndex;
  world->activeFactionRuntimeIndex = localFactionIndex;
  world->selection.activePlayerRuntimeId = localPlayerId;
  WorldRuntime_AttachAndClearDwordArray
            (INGAME_WORLD_DWORD_ARRAY_COUNT,g_InGameWorldRuntimeDwordArray256,world);
  resetDefaultsError = GameData_ResetDefaults();
  if (resetDefaultsError != 0) {
    *outError = resetDefaultsError;
    return false;
  }
  if (!InGameLevelRuntime_LoadResourcesAfterDefaultReset(levelAsset,world,&stepError)) {
    *outError = stepError;
    return false;
  }
  /* clear the four notification queue records (0x80 bytes) */
  clearCursor = (uint32_t *)inGameRoot->notificationQueue;
  for (remainingCount = 32; remainingCount != 0; remainingCount--) {
    *clearCursor = 0;
    clearCursor++;
  }
  if (!TerrainCompositeTexture_Create(&stepError)) {
    *outError = stepError;
    return false;
  }
  return true;
}


/* Sets or clears flag in the world runtime flags. */
static void InGameSession_SetWorldRuntimeFlag(WorldRuntimeContext *world,WorldRuntimeFlags flag,Bool8 enabled)

{
  if (enabled) {
    world->runtimeFlags = world->runtimeFlags | flag;
  }
  else {
    world->runtimeFlags = world->runtimeFlags & ~flag;
  }
}


/* Takes the step spin lock and finishes the world while the step is held off: sets up the shading texture,
   mirrors the shading and mouse/panel options into the world runtime flags and the panel layout, carries campaign
   units over (or resets the pending unit tables), allocates the grid scratch and rebuilds the derived terrain,
   influence, technology, build/army/command grid and lighting data. Returns true on success with the lock still
   held; on failure returns false with the error in *outError.
   Original quirk: the spin lock is not released on failure.
*/
static Bool8 InGameNewSession_FinishWorldUnderTickLock(InGameRuntimeRoot *inGameRoot,uint32_t *outError)

{
  WorldRuntimeContext *world;
  uint32_t textureDimension;
  uint32_t gridHalfSize;
  uint32_t subresourceCount;
  uint32_t linkOptionFlags;
  uint32_t mapMouseOptionFlags;
  uint32_t subsystemFailureError;
  uint32_t gridScratchError;

  world = &inGameRoot->worldRuntime;
  g_SpinLockAcquire((RuntimeSpinLockValue *)&g_InGameStateTickSpinLock);
  g_InGameSimulationStepTicks = 1;
  textureDimension =
       PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_TEXTURE_DIMENSION,PERSISTENT_SETTING_SHADING_TEXTURE_DIMENSION);
  gridHalfSize = PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_GRID_HALF_SIZE,PERSISTENT_SETTING_SHADING_GRID_HALF_SIZE);
  subresourceCount =
       PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_SUBRESOURCE_COUNT,PERSISTENT_SETTING_SHADING_SUBRESOURCE_COUNT);
  GraphicsShadingRuntime_InitializeGeneratedTexture(subresourceCount,gridHalfSize,textureDimension);
  /* mirror the shading and mouse/panel options into the world runtime flags */
  InGameSession_SetWorldRuntimeFlag
            (world,WORLD_RUNTIME_FLAG_SHADING_ENABLED,PersistentSettings_Read(1,PERSISTENT_SETTING_SHADING_ENABLED) != 0);
  linkOptionFlags = PersistentSettings_Read(0,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS);
  InGameSession_SetWorldRuntimeFlag
            (world,WORLD_RUNTIME_FLAG_LINK_ROTATION_ZOOM,(linkOptionFlags & PERSISTENT_LINK_OPTION_ROTATION_ZOOM) != 0);
  InGameSession_SetWorldRuntimeFlag
            (world,WORLD_RUNTIME_FLAG_LINK_ROTATION_TILT,(linkOptionFlags & PERSISTENT_LINK_OPTION_ROTATION_TILT) != 0);
  InGameSession_SetWorldRuntimeFlag
            (world,WORLD_RUNTIME_FLAG_HIDE_PANEL,(linkOptionFlags & PERSISTENT_LINK_OPTION_HIDE_PANEL) != 0);
  mapMouseOptionFlags = PersistentSettings_Read(0,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
  /* Original quirk: the (unreachable) failure of the optional subsystem below reports the map/mouse option flags
     as its error code, or the address of the resource bar page stack when bit 2 of them is set (left-over
     intermediate values). */
  subsystemFailureError = mapMouseOptionFlags;
  if ((mapMouseOptionFlags & 4) != 0) {
    UiPageStack_SetActiveIndex(1,&inGameRoot->sidePanelPageStack);
    subsystemFailureError = (uint32_t)&inGameRoot->resourceBarModePageStack;
    UiPageStack_SetActiveIndex(0,&inGameRoot->resourceBarModePageStack);
    UiPageStack_SetActiveIndex(0,&inGameRoot->gamePanelsModePageStack);
    inGameRoot->worldViewAreaRightOffset = 0;
    UiContainer_LayoutChildren((UiNodeBase *)inGameRoot);
  }
  if (InGameRuntime_InitializeOptionalSubsystemAlwaysSuccess((uint32_t)world->fieldGrid) != 0) {
    *outError = subsystemFailureError;
    return false;
  }
  /* a campaign carries units over from the previous level */
  if (g_FrontendLoadedCampaignAsset == 0) {
    OldUnitRuntime_ResetPendingTables();
  }
  else {
    DebugHook_CampaignCarryOver(0);
    OldUnitRuntime_MergeMasksAndReplayRecords();
    DebugHook_CampaignCarryOver(1);
  }
  if (!GridScratch_AllocateForFieldGrid(world->fieldGrid,&gridScratchError)) {
    *outError = gridScratchError;
    return false;
  }
  GridScratch_RebuildTerrainAndRuntimeClassificationMasks(world);
  GridInfluence_ClearDistanceBandsAndRefreshEntities(world->ownerListHead);
  TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks();
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
    inGameRoot->worldViewWrappedTextNodeFlags = inGameRoot->worldViewWrappedTextNodeFlags | 8;
  }
  else {
    inGameRoot->worldViewWrappedTextNodeFlags = inGameRoot->worldViewWrappedTextNodeFlags & ~8u;
  }
  InGameBuildCatalog_RebuildGrid((UiNodeBase *)inGameRoot);
  InGameSpecialBuildCatalog_RebuildGrid((UiNodeBase *)inGameRoot);
  InGameArmyStock_RebuildGrid((UiNodeBase *)inGameRoot);
  InGameOtherPlayerCommand_RebuildTargetEntries((UiNodeBase *)inGameRoot);
  WorldLightingRuntime_UpdateInterpolatedTerrainLighting();
  return true;
}


/* Reports this player as loaded, releases the step spin lock taken by InGameNewSession_FinishWorldUnderTickLock and
   shows the player-status screen while keeping the lockstep running until every player is ready; then closes the
   level movie and switches to the game page.
*/
static void InGameNewSession_ReportReadyAndWaitForPlayers(InGameRuntimeRoot *inGameRoot)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_PLAYER_READY,0,0,0);
  }
  g_SpinLockRelease((RuntimeSpinLockValue *)&g_InGameStateTickSpinLock);
  UiFrame_FlushInputAndResetPendingTicks();
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_BUSY);
  g_CursorVisibilityToken++;
  InGamePanel_RebuildPlayerStatusRows(inGameRoot);
  do {
    UiNode_InvalidateRoot(&inGameRoot->playerStatusNode);
    InGamePanel_RebuildPlayerStatusRows(inGameRoot);
    UiFrame_Update(0);
    UiFrame_Draw();
    g_GraphicsFramebufferPresent(g_FramebufferAccess);
    InGameRuntime_UpdateSimulationAndNetworkTick();
  } while ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS) != 0);
  inGameRoot->levelMovieRuntime = NULL;
  inGameRoot->playerStatusLineCount = 0;
  UiPageStack_SetActiveIndex(2,&inGameRoot->primaryPageStack);
  Movie_Close();
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
}


/* Starts the session notification timer and queues the level's five intro notification movies (consecutive ids
   from the first one; none when it is 0). */
static void InGameNewSession_QueueIntroNotifications(void)

{
  InGameNotificationMovieId firstMovieId;
  uint32_t introIndex;

  /* Lost load: the original reads the first of five level intro notification movies from the level image. */
  firstMovieId = g_InGameLevelRuntimeGlobalBlock.conditionStorage->levelImage.worldSettings.introNotificationMovieId;
  g_TimerRegisterPeriodic(10,InGameRuntime_ProcessQueuedSessionNotificationTimer);
  if (firstMovieId == 0) {
    return;
  }
  for (introIndex = 0; introIndex < 5; introIndex++) {
    InGameNotificationQueue_InsertPriorityRecord
              (NOTIFICATION_PAYLOAD_NONE,0,0,0,0,0,1,firstMovieId + introIndex);
  }
}


/* Starts a new game on a level: resets the session counters and the per-player blocks, installs the step timer
   and InGameRuntime_UpdateSimulationAndNetworkTick as the UI synchronization hook, builds the in-game UI root from
   its template, opens the level movie that plays while loading and loads the level (world, terrain, shading,
   technologies, units). It then reports itself ready to the other players and keeps drawing the player-status
   screen while stepping until every player is ready, and finally queues the level's five intro notifications.
   Returns true on success; on failure returns false and stores the error of the failing step in *outError.
*/
Bool8 InGameRuntime_InitializeNewSession(LevelAssetRuntimePrefix *levelAsset,uint16_t *levelMoviePath,
                                        uint32_t *outError)

{
  InGameRuntimeRoot *inGameRoot;
  uint32_t stepError;

  InGameNewSession_ResetSessionState();
  InGameNewSession_BuildSessionName((levelAsset->header).titleTextResourceIndex);
  if (!InGameNewSession_CreateRoot(&inGameRoot,&stepError)) {
    return InGameNewSession_Fail(stepError,outError);
  }
  if (!InGameNewSession_LoadWorld(levelAsset,levelMoviePath,inGameRoot,&stepError)) {
    return InGameNewSession_Fail(stepError,outError);
  }
  if (!InGameNewSession_FinishWorldUnderTickLock(inGameRoot,&stepError)) {
    return InGameNewSession_Fail(stepError,outError);
  }
  InGameNewSession_ReportReadyAndWaitForPlayers(inGameRoot);
  InGameNewSession_QueueIntroNotifications();
  return true;
}


/* Failure exit of InGameRuntime_InitializeLoadedSession: closes the level movie, releases the level entry and
   unmounts the save package (levelAsset is NULL and saveHandle 0 when they were not loaded yet), stores the error
   in *outError and returns false.
*/
static Bool8 InGameLoadedSession_Fail(FrontendLoadedLevelAsset *levelAsset,uint32_t saveHandle,uint32_t error,
                                     uint32_t *outError)

{
  Movie_Close();
  Resource_Release(levelAsset);
  Package_Unmount((EngineFileHandle)saveHandle);
  *outError = error;
  return false;
}


/* Sets the text of the in-game template's save-name edit (saveNameEdit, 32 code units) to the session name of a
   mounted save package: the UTF-16 string at offset 256 of the package header (scanned for at most 36
   characters), without its four-character file extension and cut to 31 characters. Without a terminator the name
   stays empty.
*/
static void InGameLoadedSession_ReadSessionName(uint32_t saveHandle)

{
  uint8_t *headerBuffer;
  uint16_t *nameStart;
  uint16_t *scanEnd;
  uint16_t *sessionNameCursor;
  uint16_t *sourceCursor;
  uint32_t copyCount;
  int remainingCount;
  Bool8 terminatorFound;

  headerBuffer = g_PackageScratchBuffer;
  sessionNameCursor = ((UiRequiredTextEditControl *)&g_InGameRuntimeDefaultImageTemplate.saveNameEdit)->textBuffer;
  for (remainingCount = 32; remainingCount != 0; remainingCount--) {
    *sessionNameCursor = 0;
    sessionNameCursor++;
  }
  g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0,THANDOR_PTR(saveHandle));
  g_FileSystemReadExact(PCK_ENTRY_HEADER_BYTES,headerBuffer,THANDOR_PTR(saveHandle));
  nameStart = (uint16_t *)(headerBuffer + 256);
  terminatorFound = false;
  scanEnd = nameStart;
  for (remainingCount = 36; remainingCount != 0 && !terminatorFound; remainingCount--) {
    terminatorFound = *scanEnd == 0;
    scanEnd++;
  }
  if (!terminatorFound) {
    return;
  }
  /* scanEnd is just past the terminator: the four characters before the terminator (the file extension) are cut
     off */
  scanEnd[-3] = 0;
  scanEnd[-2] = 0;
  scanEnd[-5] = 0;
  scanEnd[-4] = 0;
  copyCount = (uint32_t)(scanEnd - nameStart);
  if (31 < copyCount) {
    copyCount = 31;
  }
  sessionNameCursor = ((UiRequiredTextEditControl *)&g_InGameRuntimeDefaultImageTemplate.saveNameEdit)->textBuffer;
  sourceCursor = nameStart;
  for (; copyCount != 0; copyCount--) {
    *sessionNameCursor = *sourceCursor;
    sourceCursor++;
    sessionNameCursor++;
  }
}


/* Resets the session state for a loaded game: clears all selection blocks and sets up only block 0 (local player 0
   with the saved faction), resets the end-movie, tick and ready state, and starts the periodic step timer and the
   UI synchronisation hooks.
*/
static void InGameLoadedSession_ResetSessionState(uint32_t savedFactionIndex)

{
  uint32_t *clearCursor;
  int remainingCount;

  clearCursor = (uint32_t *)g_SelectionPlayerBlocks;
  for (remainingCount = SELECTION_PLAYER_BLOCK_COUNT * sizeof(SelectionPlayerRuntimeBlock) / sizeof(uint32_t);
       remainingCount != 0; remainingCount--) {
    *clearCursor = 0;
    clearCursor++;
  }
  g_EndMovieSelectionIndex = UINT32_MAX;
  g_EndMovieVariantIndex = 0;
  g_EndMoviePath = NULL;
  g_LocalPlayerRuntimeId = 0;
  g_SelectionPlayerRuntimeBlockPointers[0] = g_SelectionPlayerBlocks;
  g_SelectionPlayerBlocks->factionIndex = savedFactionIndex;
  g_SelectionPlayerBlocks->simulationStepTicks = 1;
  g_UiCommandRuntimeFlags = UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS | UI_COMMAND_RUNTIME_FLAG_PAUSED;
  g_SessionNetworkTickCounter = 1;
  g_HostCommandBatchSyncSentThisInterval = 0;
  g_GameFactionRuntimeImage.tail.simulationTick = 1;
  g_GameFactionRuntimeImage.tail.presentationTick = 0;
  g_InGameSessionNotificationTimeoutTicks = 0;
  g_InGameReadyStateToggleFlags = 0;
  g_InGameNetworkTickCountdown = INGAME_TIMER_TICKS_PER_SIMULATION_STEP;
  g_InGameStateTickSpinLock = 0;
  g_EndGameResultsCurrentMusicTrackId = 0;
  g_InGameSelectedTechnologyId = TEC_000_BASIC_TECHNOLOGY;
  g_TimerRegisterPeriodic(INGAME_PERIODIC_TIMER_HZ,InGameRuntime_PeriodicCountdownAndClockTick);
  UiRuntime_SetSynchronizationHooks
            (InGameRuntime_UpdateSimulationAndNetworkTick,(RuntimeSpinLockValue *)&g_InGameStateTickSpinLock);
}


/* Allocates the zeroed world object pool, the selection info panel resources and the in-game root (a copy of
   g_InGameRuntimeDefaultImageTemplate with its control tree and callbacks), pushes the root onto the UI root stack
   and builds the level's scenario path. Returns true with the root in *outRoot; on failure returns false with the
   error in *outError.
*/
static Bool8 InGameLoadedSession_CreateRoot(FrontendLoadedLevelAsset *levelImage,InGameRuntimeRoot **outRoot,
                                           uint32_t *outError)

{
  void *objectPool;
  InGameRuntimeRoot *inGameRoot;
  SelectionPlayerRuntimeBlock *localPlayerBlock;
  uint32_t *clearCursor;
  uint32_t *copyCursor;
  uint32_t *templateCursor;
  int remainingCount;
  uint32_t allocationError;
  uint32_t stepError;

  /* 4 MB pool for the world objects (0x4000 records of 0x100 bytes), zeroed */
  allocationError = g_MemoryApi.alloc(INGAME_WORLD_OBJECT_RECORD_COUNT * sizeof(WorldObjectRecord),&objectPool);
  if (allocationError != 0) {
    *outError = allocationError;
    return false;
  }
  g_RuntimeObjectRebaseBaseMinusOne = (uint8_t *)objectPool - 1;
  g_InGameWorldObjectRecords = (WorldObjectRecord *)objectPool;
  clearCursor = (uint32_t *)objectPool;
  for (remainingCount = INGAME_WORLD_OBJECT_RECORD_COUNT * sizeof(WorldObjectRecord) / 4; remainingCount != 0;
       remainingCount--) {
    *clearCursor = 0;
    clearCursor++;
  }
  if (!SelectionInfoPanel_InitResources((SelectionInfoEntitySlots *)g_SelectionPlayerBlocks,&stepError)) {
    *outError = stepError;
    return false;
  }
  allocationError = g_MemoryApi.alloc(sizeof(InGameRuntimeRoot),(void **)&inGameRoot);
  if (allocationError != 0) {
    *outError = allocationError;
    return false;
  }
  templateCursor = (uint32_t *)&g_InGameRuntimeDefaultImageTemplate;
  g_InGameRuntimeRoot = inGameRoot;
  /* copy the in-game root template (0x30F9 dwords = 0xC3E4 bytes) */
  copyCursor = (uint32_t *)inGameRoot;
  for (remainingCount = sizeof(InGameRuntimeRoot) / 4; remainingCount != 0; remainingCount--) {
    *copyCursor = *templateCursor;
    templateCursor++;
    copyCursor++;
  }
  if (!InGameUiRuntime_InitializeControlTreeResources((UiRootNode *)inGameRoot,&stepError)) {
    *outError = stepError;
    return false;
  }
  /* signature differs: the overlay callback takes GraphicsBooleanState (int), the slot uint32_t */
  inGameRoot->worldOverlayCallback =
       (void (*)(uint32_t, WorldRuntimeContext *))InGameWorldOverlay_RebuildOrReleaseTransientMarkers;
  (inGameRoot->worldRuntime).selection.dispatchCommandCallback =
       InGameUiRuntime_DispatchCommandByCodeAndModifierFlags;
  (inGameRoot->worldRuntime).selection.resolveContextActionPrimaryCallback =
       InGameWorldInput_ResolveContextActionAndCursor;
  (inGameRoot->worldRuntime).selection.resolveContextActionSecondaryCallback =
       InGameWorldInput_ResolveContextActionAndCursor;
  (inGameRoot->worldRuntime).selection.beginPointerCaptureCallback =
       InGameWorldInput_BeginPointerCapture;
  (inGameRoot->worldRuntime).selection.updateDragSelectionCallback =
       InGameWorldInput_UpdateDragSelectionAndCamera;
  (inGameRoot->worldRuntime).selection.commitPointerActionCallback =
       InGameWorldInput_CommitPointerAction;
  /* signature differs: the callback takes void *, the slot WorldRuntimeContext * */
  (inGameRoot->worldRuntime).fieldRegion.clearTransientStateCallback =
       (void (*)(WorldRuntimeContext *))InGameUiRuntime_ResetNotificationButtonCursor;
  (inGameRoot->worldRuntime).selection.dispatchWorldContextActionCallback =
       InGameUiRuntime_DispatchWorldContextActionCallback;
  (inGameRoot->worldRuntime).minimumCameraDistanceQ12 = 8 * Q12_ONE;
  (inGameRoot->worldRuntime).maximumCameraDistanceQ12 = 19 * Q12_ONE;
  (inGameRoot->worldRuntime).motion.minimumPitchAngle = INGAME_CAMERA_MINIMUM_PITCH_ANGLE16;
  (inGameRoot->worldRuntime).motion.maximumPitchAngle = INGAME_CAMERA_MAXIMUM_PITCH_ANGLE16;
  (inGameRoot->worldRuntime).tickSpinLock = &g_InGameStateTickSpinLock;
  (inGameRoot->worldRuntime).simulationAndNetworkTickCallback =
       InGameRuntime_UpdateSimulationAndNetworkTick;
  localPlayerBlock = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId];
  inGameRoot->localPlayerMarkedCellCount = 0;
  inGameRoot->localPlayerMarkedCells = localPlayerBlock->markedCells;
  UiRootStack_Push(&g_InGameUiRootCallbacks,(UiRootNode *)inGameRoot);
  WidePath_CombineDirectoryAndLeaf
            (g_FrontendScenarioPathScratchUtf16,
             (levelImage->header).levelFileNameUtf16,(uint16_t *)g_ScenarioLevelDirectoryUtf16);
  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_LEV,g_FrontendScenarioPathScratchUtf16);
  *outRoot = inGameRoot;
  return true;
}


/* Opens the level movie and shows its first frames, attaches the world arrays, loads the saved external tables and
   field grid with the level resources, clears the notification queue and creates the terrain texture. Returns
   true on success; on failure returns false with the error in *outError.
*/
static Bool8 InGameLoadedSession_LoadWorld(uint16_t *savePackagePath,FrontendLoadedLevelAsset *levelImage,
                                          InGameRuntimeRoot *inGameRoot,uint32_t *outError)

{
  WorldRuntimeContext *world;
  uint16_t *loadingMoviePath;
  MovieRuntime *firstFrameMovie;
  uint32_t movieEndCode;
  uint32_t localFactionIndex;
  void *fieldGrid;
  uint32_t packageLoadErrorCode;
  uint32_t *clearCursor;
  int remainingCount;
  uint32_t stepError;

  world = &inGameRoot->worldRuntime;
  if (!LevelAsset_PrepareEndingMoviePath
         (savePackagePath,(LevelAssetHeader *)levelImage,&loadingMoviePath,&stepError)) {
    *outError = stepError;
    return false;
  }
  if (!Movie_Open(MOVIE_OPEN_PACKAGE_ONLY,loadingMoviePath,NULL,&stepError)) {
    *outError = stepError;
    return false;
  }
  if (!Movie_AdvanceFrame(&firstFrameMovie,&movieEndCode)) {
    *outError = movieEndCode;
    return false;
  }
  inGameRoot->levelMovieRuntime = firstFrameMovie;
  g_MoviePlaybackBaseFrameGroup = 0;
  g_MoviePlaybackScheduleCounter = 0;
  g_MoviePlaybackScheduleSpan = 0;
  g_MoviePlaybackCurrentFrame = 0;
  MoviePlayback_AdvanceToFrameAndPresent(0);
  MoviePlayback_AdvanceToFrameAndPresent(1);
  RecentTextHistory_SortAndBuildPointerList(8,&inGameRoot->recentTextHistory);
  WorldRuntime_AttachObjectArray(INGAME_WORLD_OBJECT_RECORD_COUNT,g_InGameWorldObjectRecords,world);
  localFactionIndex = g_SelectionPlayerBlocks->factionIndex;
  world->activeFactionRuntimeIndex = (FactionRuntimeIndex)localFactionIndex;
  world->selection.activePlayerRuntimeId = 0;
  WorldRuntime_AttachAndClearDwordArray
            (INGAME_WORLD_DWORD_ARRAY_COUNT,g_InGameWorldRuntimeDwordArray256,world);
  if (GameData_LoadExternalTables()) {
    /* Original quirk: this failure reports the local player's faction index as its error code (a left-over
       intermediate value). */
    *outError = localFactionIndex;
    return false;
  }
  fieldGrid = Package_LoadEntry((uint16_t *)g_FieldHexPathUtf16,&packageLoadErrorCode);
  if (fieldGrid == NULL) {
    *outError = packageLoadErrorCode;
    return false;
  }
  (levelImage->header).pathState.levelPathOffsetOrLoadedFieldGrid = (uint32_t)fieldGrid;
  if (!InGameLevelRuntime_LoadResourcesAfterExternalTables(levelImage,world,&stepError)) {
    *outError = stepError;
    return false;
  }
  /* clear the four notification queue records (0x80 bytes) */
  clearCursor = (uint32_t *)inGameRoot->notificationQueue;
  for (remainingCount = 32; remainingCount != 0; remainingCount--) {
    *clearCursor = 0;
    clearCursor++;
  }
  if (!TerrainCompositeTexture_Create(&stepError)) {
    *outError = stepError;
    return false;
  }
  return true;
}


/* Takes the step spin lock and finishes the world while the step is held off: rebuilds the build/army/command
   grids, sets up the shading texture, mirrors the shading and mouse/panel options into the world runtime flags,
   allocates the grid scratch and rebuilds the derived terrain, influence, technology and lighting data. Returns
   true on success with the lock still held; on failure returns false with the error in *outError.
   Original quirk: the spin lock is not released on failure.
*/
static Bool8 InGameLoadedSession_FinishWorldUnderTickLock(InGameRuntimeRoot *inGameRoot,uint32_t *outError)

{
  WorldRuntimeContext *world;
  uint32_t textureDimension;
  uint32_t gridHalfSize;
  uint32_t subresourceCount;
  uint32_t linkOptionFlags;
  uint32_t gridScratchError;

  world = &inGameRoot->worldRuntime;
  g_SpinLockAcquire((RuntimeSpinLockValue *)&g_InGameStateTickSpinLock);
  InGameBuildCatalog_RebuildGrid((UiNodeBase *)inGameRoot);
  InGameSpecialBuildCatalog_RebuildGrid((UiNodeBase *)inGameRoot);
  InGameArmyStock_RebuildGrid((UiNodeBase *)inGameRoot);
  InGameOtherPlayerCommand_RebuildTargetEntries((UiNodeBase *)inGameRoot);
  g_InGameSimulationStepTicks = 1;
  textureDimension =
       PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_TEXTURE_DIMENSION,PERSISTENT_SETTING_SHADING_TEXTURE_DIMENSION);
  gridHalfSize =
       PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_GRID_HALF_SIZE,PERSISTENT_SETTING_SHADING_GRID_HALF_SIZE);
  subresourceCount =
       PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_SUBRESOURCE_COUNT,PERSISTENT_SETTING_SHADING_SUBRESOURCE_COUNT);
  GraphicsShadingRuntime_InitializeGeneratedTexture(subresourceCount,gridHalfSize,textureDimension);
  /* mirror the shading and mouse/panel options into the world runtime flags */
  InGameSession_SetWorldRuntimeFlag
            (world,WORLD_RUNTIME_FLAG_SHADING_ENABLED,PersistentSettings_Read(1,PERSISTENT_SETTING_SHADING_ENABLED) != 0);
  linkOptionFlags = PersistentSettings_Read(0,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS);
  InGameSession_SetWorldRuntimeFlag
            (world,WORLD_RUNTIME_FLAG_LINK_ROTATION_ZOOM,(linkOptionFlags & PERSISTENT_LINK_OPTION_ROTATION_ZOOM) != 0);
  InGameSession_SetWorldRuntimeFlag
            (world,WORLD_RUNTIME_FLAG_LINK_ROTATION_TILT,(linkOptionFlags & PERSISTENT_LINK_OPTION_ROTATION_TILT) != 0);
  InGameSession_SetWorldRuntimeFlag
            (world,WORLD_RUNTIME_FLAG_HIDE_PANEL,(linkOptionFlags & PERSISTENT_LINK_OPTION_HIDE_PANEL) != 0);
  if (InGameRuntime_InitializeOptionalSubsystemAlwaysSuccess((uint32_t)world->fieldGrid) != 0) {
    /* Original quirk: this (unreachable) failure reports the mouse/panel option flags as its error code. */
    *outError = linkOptionFlags;
    return false;
  }
  if (!GridScratch_AllocateForFieldGrid(world->fieldGrid,&gridScratchError)) {
    *outError = gridScratchError;
    return false;
  }
  GridScratch_RebuildTerrainAndRuntimeClassificationMasks(world);
  GridInfluence_ClearDistanceBandsAndRefreshEntities(world->ownerListHead);
  TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks();
  WorldLightingRuntime_UpdateInterpolatedTerrainLighting();
  return true;
}


/* Continues a saved game: mounts the save package, takes the session name from its header, loads the campaign
   and level entries, and then follows the same steps as InGameRuntime_InitializeNewSession, except that the local
   player is always player 0 of a single block, the world comes from the saved external tables and field grid
   (InGameLevelRuntime_LoadResourcesAfterExternalTables) instead of a fresh level, and no intro notifications are
   queued. The package and the level entry are released again at the end. Returns true on success; on failure
   returns false and stores the error of the failing step in *outError.
*/
Bool8 InGameRuntime_InitializeLoadedSession(uint16_t *savePackagePath,uint32_t *outError)

{
  uint32_t mountResult; /* the save package's handle, or the mount error code */
  uint32_t saveHandle;
  void *campaignAsset;
  FrontendLoadedLevelAsset *levelImage;
  uint32_t packageLoadErrorCode;
  InGameRuntimeRoot *inGameRoot;
  uint32_t stepError;

  g_TextureDownsampleShift = PersistentSettings_Read(0,PERSISTENT_SETTING_TEXTURE_QUALITY);
  if (!Package_Mount(savePackagePath,&mountResult)) {
    return InGameLoadedSession_Fail(NULL,0,mountResult,outError);
  }
  saveHandle = mountResult;
  InGameLoadedSession_ReadSessionName(saveHandle);
  campaignAsset = Package_LoadEntry((uint16_t *)g_CampagneHexPathUtf16,NULL);
  if (campaignAsset != NULL) {
    g_FrontendLoadedCampaignAsset = (uint32_t)campaignAsset;
  }
  levelImage = (FrontendLoadedLevelAsset *)Package_LoadEntry((uint16_t *)g_LevelHexPathUtf16,&packageLoadErrorCode);
  if (levelImage == NULL) {
    return InGameLoadedSession_Fail(NULL,saveHandle,packageLoadErrorCode,outError);
  }
  InGameLoadedSession_ResetSessionState(levelImage->playerSlots[6].aiClassOrMode);
  if (!InGameLoadedSession_CreateRoot(levelImage,&inGameRoot,&stepError) ||
      !InGameLoadedSession_LoadWorld(savePackagePath,levelImage,inGameRoot,&stepError) ||
      !InGameLoadedSession_FinishWorldUnderTickLock(inGameRoot,&stepError)) {
    return InGameLoadedSession_Fail(levelImage,saveHandle,stepError,outError);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
    inGameRoot->worldViewWrappedTextNodeFlags = inGameRoot->worldViewWrappedTextNodeFlags | 8;
  }
  else {
    inGameRoot->worldViewWrappedTextNodeFlags = inGameRoot->worldViewWrappedTextNodeFlags & ~8u;
  }
  /* report this player as loaded */
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_PLAYER_READY,0,0,0);
  }
  g_SpinLockRelease((RuntimeSpinLockValue *)&g_InGameStateTickSpinLock);
  UiFrame_FlushInputAndResetPendingTicks();
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_BUSY);
  g_CursorVisibilityToken++;
  /* show the player-status screen and keep the lockstep running until all are ready */
  InGamePanel_RebuildPlayerStatusRows(inGameRoot);
  do {
    UiNode_InvalidateRoot(&inGameRoot->playerStatusNode);
    InGamePanel_RebuildPlayerStatusRows(inGameRoot);
    UiFrame_Update(0);
    UiFrame_Draw();
    g_GraphicsFramebufferPresent(g_FramebufferAccess);
    InGameRuntime_UpdateSimulationAndNetworkTick();
  } while ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS) != 0);
  inGameRoot->levelMovieRuntime = NULL;
  inGameRoot->playerStatusLineCount = 0;
  UiPageStack_SetActiveIndex(2,&inGameRoot->primaryPageStack);
  Movie_Close();
  Resource_Release(levelImage);
  Package_Unmount((EngineFileHandle)saveHandle);
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
  g_TimerRegisterPeriodic(10,InGameRuntime_ProcessQueuedSessionNotificationTimer);
  return true;
}


/* Ends an in-game session (counterpart of InGameRuntime_InitializeNewSession/InitializeLoadedSession): stops the
   step timer, shows a black screen with the busy cursor, then releases the world (every entity's bindings, the
   level assets), the in-game UI root, the faction scratch buffers, the object pool, the level movie, the terrain
   texture and the four panel texture packages, and resets the sprite registry and pending input so the frontend
   starts clean.
*/
void InGameRuntime_ShutdownAndReleaseResources(void)

{
  WorldRuntimeContext *world;
  InGameRuntimeRoot *inGameRoot;
  Bool8 beginAccessFailed;
  
  g_TimerUnregisterPeriodic(InGameRuntime_PeriodicCountdownAndClockTick);
  inGameRoot = g_InGameRuntimeRoot;
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_BUSY);
  beginAccessFailed = g_GraphicsFramebufferBeginAccess();
  if (!beginAccessFailed) {
    /* opaque black over the whole framebuffer */
    g_GraphicsFramebufferFillRectArgb
              (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0,0
               ,0xff000000,g_FramebufferAccess);
    g_GraphicsFramebufferEndAccess();
  }
  g_GraphicsFramebufferPresent(g_FramebufferAccess);
  GraphicsShadingRuntime_Shutdown();
  if (inGameRoot != NULL) {
    InGameRuntime_SaveWorldViewInfoTextChoice(&inGameRoot->rootUi);
    world = &inGameRoot->worldRuntime;
    /* signature differs: the callback's context is WorldRuntimeContext *, the slot's void * */
    WorldRuntime_ForEachOwnerListNode
              (world,(WorldRuntimeNodeTraversalCallback *)WorldRuntimeNode_ReleaseShutdownBindingsCallback,world);
    InGameLevelRuntime_ShutdownLoadedAssetResources(world);
    if ((inGameRoot->rootUi).previousRoot != NULL) {
      UiRootStack_Pop(&inGameRoot->rootUi);
    }
    g_MemoryApi.free(inGameRoot);
    g_InGameRuntimeRoot = NULL;
  }
  InGameRuntime_ReleaseFactionScratchBuffers();
  g_MemoryApi.free(g_InGameWorldObjectRecords);
  g_InGameWorldObjectRecords = NULL;
  Movie_Close();
  TerrainCompositeTexture_Destroy();
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage((GraphicsTextureSourceAsset *)g_InGameDiagramTextureSource);
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage(g_InGamePanelTextureSource);
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage((GraphicsTextureSourceAsset *)g_InGameTechnologyTextureSource);
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage((GraphicsTextureSourceAsset *)g_InGameWindowTextureSource);
  g_InGameDiagramTextureSource = NULL;
  g_InGamePanelTextureSource = NULL;
  g_InGameTechnologyTextureSource = NULL;
  g_InGameWindowTextureSource = NULL;
  GraphicsShadingRuntime_ClearRecordTable();
  SelectionInfoPanel_ShutdownResources();
  SpriteAssetRegistry_Reset();
  UiFrame_FlushInputAndResetPendingTicks();
  g_CursorVisibilityToken--;
  return;
}


/* Frees the two scratch buffers of each of the eight factions (sets A and B) at session shutdown and clears the
   pointers.
*/
void InGameRuntime_ReleaseFactionScratchBuffers(void)

{
  int remainingFactions;
  void **scratchBufferSetBCursor;
  void **scratchBufferSetACursor;

  remainingFactions = 8;
  scratchBufferSetACursor = g_InGameFactionScratchBufferSetA8;
  scratchBufferSetBCursor = g_InGameFactionScratchBufferSetB8;
  do {
    g_MemoryApi.free(*scratchBufferSetACursor);
    g_MemoryApi.free(*scratchBufferSetBCursor);
    *scratchBufferSetACursor = NULL;
    *scratchBufferSetBCursor = NULL;
    scratchBufferSetACursor++;
    scratchBufferSetBCursor++;
    remainingFactions--;
  } while (remainingFactions != 0);
  return;
}


/* Evaluates a BOOLEAN_POSTFIX_EXPRESSION condition: the tokens after its kind byte run on a bit stack.
   0xFC end, 0xFD NOT, 0xFE AND, 0xFF OR, anything else pushes the satisfied bit of the condition with that index.
   Returns the bit stack; bit 0 is the result. */
static uint32_t InGameScheduledCondition_EvaluatePostfixExpression(InGameLevelConditionStorage *levelConditionStorage,
                                                                   const uint8_t *expression)
{
  uint32_t bitStack;
  uint8_t token;

  bitStack = INGAME_SCHEDULED_CONDITION_NONE_OR_UNUSED;
  for (; *expression != INGAME_CONDITION_TOKEN_END; expression++) {
    token = *expression;
    if (token == INGAME_CONDITION_TOKEN_OR) {
      bitStack = bitStack >> 1 | bitStack & 1;
    }
    else if (token == INGAME_CONDITION_TOKEN_AND) {
      bitStack = bitStack >> 1 & (bitStack | ~1u);
    }
    else if (token == INGAME_CONDITION_TOKEN_NOT) {
      bitStack = bitStack ^ 1;
    }
    else {
      bitStack = ((levelConditionStorage->schedule).conditions[token].statusAndKind.raw &
                  INGAME_SCHEDULED_CONDITION_SATISFIED) + bitStack * 2;
    }
  }
  return bitStack;
}


/* Evaluates one scheduled condition of the level script (its satisfied bit is already cleared in the record).
   COUNTDOWN_ELAPSED also counts its operand 1 down by the step ticks and clamps it at 0 once elapsed.
   Unknown kinds (and unused records) never hold. */
static Bool8 InGameScheduledCondition_Holds(InGameLevelConditionStorage *levelConditionStorage,
                                           InGameScheduledConditionRecord10 *condition,
                                           InGameScheduledConditionKind kind)
{
  uint32_t *operands;
  WorldOwnerListNode *worldNode;
  ArmyRuntimeSlot *army;
  uint32_t firstFactionIndex;
  uint32_t secondFactionIndex;
  uint32_t armiesStillNeeded;
  uint32_t remainingTicks;
  uint32_t runtimeClassId;
  FieldGridAsset *fieldGrid;
  uint32_t cellCount;
  uint32_t cellsLeft;
  uint32_t occupiedCellCount;
  uint8_t *cellBytes;
  ResourceExtractionDescriptor32 *cellOccupancyMask;

  operands = condition->payload.operands;
  switch(kind & INGAME_SCHEDULED_CONDITION_KIND_MASK) {
  case INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY:
    for (worldNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead;
        worldNode != NULL; worldNode = worldNode->nextNode) {
      if ((worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
         (operands[0] ==
          ((ModelRuntimeSlot *)worldNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex)) {
        return false;
      }
    }
    return true;
  case INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_COMMAND_GROUP_A_ARMY:
    for (worldNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead;
        worldNode != NULL; worldNode = worldNode->nextNode) {
      if (((worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
          (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand
           [((ModelRuntimeSlot *)worldNode->runtimePayload)->definitionOrSavedId.runtimeDefinition->runtimeClassId] ==
           ArmyRuntime_ClassCommandHandlerGroupA)) &&
         (((ModelRuntimeSlot *)worldNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex ==
          operands[0])) {
        return false;
      }
    }
    return true;
  case INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY_OF_ASSET:
    for (worldNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead;
        worldNode != NULL; worldNode = worldNode->nextNode) {
      if (worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        army = ((ModelRuntimeSlot *)worldNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
        if ((operands[0] == army->factionIndex) && (army->armyAssetId == operands[2])) {
          return false;
        }
      }
    }
    return true;
  case INGAME_SCHEDULED_CONDITION_FACTION_INACTIVE_OR_RELATION_AT_LEAST_8:
    firstFactionIndex = operands[1];
    secondFactionIndex = operands[0];
    return (g_GameFactionRuntimeImage.tail.factionLifecycleStates[firstFactionIndex] !=
            FACTION_RUNTIME_LIFECYCLE_ACTIVE) ||
           (g_GameFactionRuntimeImage.tail.factionLifecycleStates[secondFactionIndex] !=
            FACTION_RUNTIME_LIFECYCLE_ACTIVE) ||
           (FACTION_RELATION_STATE_ALLIED - 1 < (g_GameFactionRuntimeImage.records[firstFactionIndex].packedRelationStates >>
                 ((char)secondFactionIndex * 4 & 31U) & 0xf));
  case INGAME_SCHEDULED_CONDITION_XENITE_AT_LEAST:
    return (int)operands[1] <= (int)g_GameFactionRuntimeImage.records[operands[0]].xeniteCurrentQ4;
  case INGAME_SCHEDULED_CONDITION_TRITIUM_AT_LEAST:
    return (int)operands[1] <= (int)g_GameFactionRuntimeImage.records[operands[0]].tritiumCurrentQ4;
  case INGAME_SCHEDULED_CONDITION_TRITIUM_EXTRACTION_RATE_AT_LEAST:
    return (int)operands[1] <= (int)g_GameFactionRuntimeImage.records[operands[0]].tritiumExtractionRateQ4PerTick;
  case INGAME_SCHEDULED_CONDITION_ARMY_OF_ASSET_COUNT_AT_LEAST:
    armiesStillNeeded = operands[1];
    for (worldNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead;
        worldNode != NULL; worldNode = worldNode->nextNode) {
      if ((worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
         (((ModelRuntimeSlot *)worldNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex ==
          operands[0]) &&
         (((ModelRuntimeSlot *)worldNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime->armyAssetId ==
          operands[2])) {
        armiesStillNeeded--;
        if (armiesStillNeeded == 0) {
          return true;
        }
      }
    }
    return false;
  case INGAME_SCHEDULED_CONDITION_FACTION_TERRAIN_OCCUPANCY_MASK_F9_PERCENT_AT_LEAST:
    /* operand 0 is a byte offset into the cell records; operand 1 the percentage */
    fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
    cellCount = fieldGrid->gridWidth * fieldGrid->gridHeight;
    cellBytes = (uint8_t *)fieldGrid->cells + operands[0];
    occupiedCellCount = 0;
    cellsLeft = cellCount;
    do {
      cellOccupancyMask = (ResourceExtractionDescriptor32 *)(cellBytes + offsetof(FieldGridCell, occupancyMask));
      cellBytes = cellBytes + sizeof(FieldGridCell);
      occupiedCellCount = occupiedCellCount + ((*cellOccupancyMask & FIELD_CELL_OCCUPANCY_PRESENCE_BITS) != 0);
      cellsLeft--;
    } while (cellsLeft != 0);
    return (int)operands[1] <= (int)(((uint64_t)occupiedCellCount * 100) / (uint64_t)cellCount);
  case INGAME_SCHEDULED_CONDITION_COUNTDOWN_ELAPSED:
    remainingTicks = operands[1] - g_InGameSimulationStepTicks;
    operands[1] = remainingTicks;
    if ((int)remainingTicks < 1) {
      operands[1] = 0;
      return true;
    }
    return false;
  case INGAME_SCHEDULED_CONDITION_XENITE_STORAGE_LIMIT_AT_MOST_0FA0:
    return (int)g_GameFactionRuntimeImage.records[operands[0]].xeniteStorageLimitQ4 < (250 << Q4_SHIFT) + 1;
  case INGAME_SCHEDULED_CONDITION_NO_ARMY_OF_CLASS_OUTSIDE_COMMAND_GROUP_A:
    for (worldNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead;
        worldNode != NULL; worldNode = worldNode->nextNode) {
      if (worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        runtimeClassId =
             ((ModelRuntimeSlot *)worldNode->runtimePayload)->definitionOrSavedId.runtimeDefinition->runtimeClassId;
        if ((g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand[runtimeClassId] !=
             ArmyRuntime_ClassCommandHandlerGroupA) && (runtimeClassId == operands[0])) {
          return false;
        }
      }
    }
    return true;
  case INGAME_SCHEDULED_CONDITION_BOOLEAN_POSTFIX_EXPRESSION:
    return (InGameScheduledCondition_EvaluatePostfixExpression(levelConditionStorage,
                                                               &condition->statusAndKind.kindAndExpression[1]) &
            1) != 0;
  default:
    return false;
  }
}


/* True while two active factions (1..7) are still not allied (relation state below 8): the game goes on. */
static Bool8 InGameConditionRuntime_HasUnalliedActiveFactionPair(void)
{
  uint32_t factionIndex;
  uint32_t otherFactionIndex;

  for (factionIndex = 1; factionIndex < 7; factionIndex++) {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      continue;
    }
    for (otherFactionIndex = factionIndex + 1; otherFactionIndex < 8; otherFactionIndex++) {
      if ((g_GameFactionRuntimeImage.tail.factionLifecycleStates[otherFactionIndex] ==
           FACTION_RUNTIME_LIFECYCLE_ACTIVE) &&
         ((g_GameFactionRuntimeImage.records[otherFactionIndex].packedRelationStates >> (factionIndex * 4 & 31) &
           0xf) < FACTION_RELATION_STATE_ALLIED)) {
        return true;
      }
    }
  }
  return false;
}


/* Chooses the end movie from the local faction's view and requests it: the trigger's variant when the ended
   faction is the local one or one it rates above 3, the other variant when the local faction has not ended and
   rates it 3 or below, variant 0 when the local faction has ended too (or is unused). */
static void InGameConditionRuntime_RequestEndMovie(const InGameEndConditionTriggerRecord8 *endTrigger,
                                                   const WorldRuntimeContext *worldRuntime,
                                                   uint32_t endedFactionIndex)
{
  uint32_t localFactionIndex;

  localFactionIndex = worldRuntime->activeFactionRuntimeIndex;
  g_EndMovieVariantIndex = (uint32_t)endTrigger->movieVariantSelector;
  if (localFactionIndex != endedFactionIndex) {
    g_EndMovieVariantIndex = 0;
    if ((g_GameFactionRuntimeImage.tail.factionLifecycleStates[localFactionIndex] <
         FACTION_RUNTIME_LIFECYCLE_ENDED_OR_TRANSITIONED) &&
       (g_GameFactionRuntimeImage.tail.factionLifecycleStates[localFactionIndex] !=
        FACTION_RUNTIME_LIFECYCLE_INACTIVE)) {
      g_EndMovieVariantIndex = endTrigger->movieVariantSelector ^ 1;
      if (FACTION_RELATION_STATE_FRIENDLY - 1 <
          (g_GameFactionRuntimeImage.records[localFactionIndex].packedRelationStates >>
               ((char)endedFactionIndex * 4 & 31U) & 0xf)) {
        g_EndMovieVariantIndex = (uint32_t)endTrigger->movieVariantSelector;
      }
    }
  }
  g_EndMovieSelectionIndex = (uint32_t)endTrigger->endMovieSelectionIndex;
  g_EndMoviePath = (uint16_t *)g_SessionEndMoviePathUtf16;
  if (g_EndMovieVariantIndex == 0) {
    g_EndMoviePath = (uint16_t *)g_FlmEnde0001FlmPathUtf16;
  }
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING;
}


/* Ends the (active) faction of a fired end trigger: marks it ENDING_PENDING and, unless skipArmyDisableWhenOne
   == 1, destroys its units, takes map input from the local player when it is theirs and stops while two active
   factions are still not allied (then the local player's build/stock/diplomacy panels are closed when the ended
   faction is theirs). Otherwise the end movie is requested. */
static void InGameConditionRuntime_EndTriggerFaction(const InGameEndConditionTriggerRecord8 *endTrigger)
{
  InGameRuntimeRoot *triggerRoot;
  InGameRuntimeRoot *relationRoot;
  WorldRuntimeContext *worldRuntime;
  WorldOwnerListNode *worldNode;
  ArmyRuntimeSlot *army;
  uint32_t endedFactionIndex;

  triggerRoot = g_InGameRuntimeRoot;
  endedFactionIndex = (uint32_t)endTrigger->factionRuntimeIndex;
  worldRuntime = &g_InGameRuntimeRoot->worldRuntime;
  g_GameFactionRuntimeImage.tail.factionLifecycleStates[endedFactionIndex] = FACTION_RUNTIME_LIFECYCLE_ENDING_PENDING;
  if (endTrigger->skipArmyDisableWhenOne != 1) {
    worldNode = (triggerRoot->worldRuntime).ownerListHead;
    if (worldNode != NULL) {
      for (; worldNode != NULL; worldNode = worldNode->nextNode) {
        if (worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
          army = ((ModelRuntimeSlot *)worldNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
          if (endedFactionIndex == army->factionIndex) {
            ModelRuntimeHierarchy_MarkDestroyedRecursive(worldRuntime,army);
          }
        }
      }
      g_GameFactionRuntimeImage.records[endedFactionIndex].secondaryArmyAssetCount = 0;
      g_GameFactionRuntimeImage.records[endedFactionIndex].primaryArmyAssetCount = 0;
    }
    relationRoot = g_InGameRuntimeRoot;
    if (endedFactionIndex == (triggerRoot->worldRuntime).activeFactionRuntimeIndex) {
      g_UiCommandRuntimeFlags =
           g_UiCommandRuntimeFlags |
           (UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED | UI_COMMAND_RUNTIME_FLAG_LOCAL_FACTION_ENDED);
    }
    if (InGameConditionRuntime_HasUnalliedActiveFactionPair()) {
      if ((uint32_t)endTrigger->factionRuntimeIndex == (g_InGameRuntimeRoot->worldRuntime).activeFactionRuntimeIndex) {
        g_InGameRuntimeRoot->diplomacyPanelNodeFlags = g_InGameRuntimeRoot->diplomacyPanelNodeFlags | 8;
        INGAME_UI(relationRoot,buildCatalogPanel)->nodeFlags = INGAME_UI(relationRoot,buildCatalogPanel)->nodeFlags | 8;
        INGAME_UI(relationRoot,specialBuildCatalogPanel)->nodeFlags =
             INGAME_UI(relationRoot,specialBuildCatalogPanel)->nodeFlags | 8;
        INGAME_UI(relationRoot,armyStockPanel)->nodeFlags = INGAME_UI(relationRoot,armyStockPanel)->nodeFlags | 8;
      }
      return;
    }
    worldRuntime = &g_InGameRuntimeRoot->worldRuntime;
  }
  InGameConditionRuntime_RequestEndMovie(endTrigger,worldRuntime,endedFactionIndex);
}


/* The level script, evaluated every 20 simulation steps: first promotes factions that were marked as ending to
   ended, then re-evaluates the level's 64 scheduled conditions (bit 0 of each record's kind = satisfied: unit
   counts, resource amounts, map share, countdowns, boolean expressions over other conditions), and finally checks
   the 16 end triggers. The first active trigger whose condition holds ends its faction: its units are disabled,
   the local player loses map input when it is theirs, and unless two remaining active factions are still not
   allied (relation state below 8) the end movie is chosen and UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING ends the
   session. A trigger with skipArmyDisableWhenOne == 1 goes to the end movie directly.
   Expression tokens: 0xFC end, 0xFD NOT, 0xFE AND, 0xFF OR, anything else pushes that condition's result bit
   (see InGameScheduledCondition_EvaluatePostfixExpression).
*/
void InGameConditionRuntime_UpdateScheduledRecords(void)

{
  InGameLevelConditionStorage *levelConditionStorage;
  uint32_t factionIndex;
  int conditionIndex;
  int triggerIndex;
  InGameScheduledConditionRecord10 *condition;
  InGameScheduledConditionKind kind;
  InGameEndConditionTriggerRecord8 *endTrigger;

  DebugHook_LevelScriptBeforeEvaluation();
  for (factionIndex = 1; factionIndex < 8; factionIndex++) {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] ==
        FACTION_RUNTIME_LIFECYCLE_ENDING_PENDING) {
      g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] =
           FACTION_RUNTIME_LIFECYCLE_ENDED_OR_TRANSITIONED;
    }
  }
  levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
  condition = (levelConditionStorage->schedule).conditions;
  for (conditionIndex = 0; conditionIndex < INGAME_SCHEDULED_CONDITION_COUNT; conditionIndex++) {
    /* clear the satisfied bit, then set it again when the condition holds */
    kind = condition->statusAndKind.kind;
    condition->statusAndKind.raw = condition->statusAndKind.raw & ~(uint32_t)INGAME_SCHEDULED_CONDITION_SATISFIED;
    if (InGameScheduledCondition_Holds(levelConditionStorage,condition,kind)) {
      condition->statusAndKind.raw = condition->statusAndKind.raw | INGAME_SCHEDULED_CONDITION_SATISFIED;
    }
    condition++;
  }
  DebugHook_LevelScriptAfterEvaluation(levelConditionStorage);
  endTrigger = (InGameEndConditionTriggerRecord8 *)(levelConditionStorage->schedule).triggers;
  for (triggerIndex = 0; triggerIndex < INGAME_END_CONDITION_TRIGGER_COUNT; triggerIndex++, endTrigger++) {
    if ((endTrigger->stateFlags == INGAME_END_CONDITION_TRIGGER_ACTIVE) &&
       (((levelConditionStorage->schedule).conditions[endTrigger->conditionIndex].statusAndKind.raw &
         INGAME_SCHEDULED_CONDITION_SATISFIED) != 0)) {
      endTrigger->stateFlags = endTrigger->stateFlags | INGAME_END_CONDITION_TRIGGER_PROCESSED;
      DebugHook_LevelScriptEndTrigger(levelConditionStorage,triggerIndex,endTrigger);
      if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[endTrigger->factionRuntimeIndex] ==
          FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
        InGameConditionRuntime_EndTriggerFaction(endTrigger);
        return;
      }
    }
  }
}


/* Energy consumer of the faction economy, collected into the region scratch buffer
   (g_TerrainRegionCollectionEntries) as 16-byte entries, at most 256. */
typedef struct FactionEnergyConsumerEntry {
  uint32_t modelRuntime; /* the consumer's model runtime (pointer value) */
  uint32_t factionIndex;
  EnergyDemandQ4 demandQ4; /* model runtime classState.energyLoadQ4 */
  uint32_t priority; /* g_FactionEnergyAllocationPriorityByModelClass[runtime class] */
} FactionEnergyConsumerEntry;

/* Economy step 1, per faction: reset the step's energy demand and extraction rates, decay the faction's row of
   the pair-pressure matrix by 7/8, count the notification/anchor cooldowns down (anchorCooldown1/2 are the
   energy notification cooldowns) and advance the relation transition tick. */
static void InGameFactionEconomy_ResetAndDecayFactionState(void)
{
  GameFactionRuntimeRecord *factionRecord;
  uint32_t *pairPressureRow;
  int factionIndex;
  int column;

  pairPressureRow = g_GameDataAuxState.pairPressureMatrix8x8;
  for (factionIndex = 0; factionIndex < 8; factionIndex++) {
    factionRecord = &g_GameFactionRuntimeImage.records[factionIndex];
    factionRecord->suppliedEnergyDemandQ4 = 0;
    factionRecord->unpoweredEnergyDemandQ4 = 0;
    factionRecord->xeniteExtractionRateQ4PerTick = 0;
    factionRecord->tritiumExtractionRateQ4PerTick = 0;
    for (column = 0; column < 8; column++) {
      pairPressureRow[column] = pairPressureRow[column] * 7 >> 3;
    }
    if (factionRecord->anchorCooldown1 != 0) {
      factionRecord->anchorCooldown1--;
    }
    if (factionRecord->anchorCooldown2 != 0) {
      factionRecord->anchorCooldown2--;
    }
    if (factionRecord->primaryAnchorCooldown != 0) {
      factionRecord->primaryAnchorCooldown--;
    }
    if (factionRecord->anchorCooldown0 != 0) {
      factionRecord->anchorCooldown0--;
    }
    factionRecord->relationTransitionTick++;
    pairPressureRow = pairPressureRow + 8;
  }
}

/* Pays one collected region (g_TerrainRegionCollectionStoredCount != 0): every entry {extraction descriptor,
   model offset} gives its faction (descriptor bits 13..23) a rate of cells-per-entry * 2 * share (bits 24..31) *
   terrainContributionScaleQ8 >> 15, added to the rate, the stock and the extracted total (Xenite or Tritium
   fields); the extracting model shows its current yield. */
static void InGameFactionEconomy_PayCollectedRegion(Bool8 payTritium)
{
  int cellsPerEntry;
  const uint32_t *entry;
  TerrainRegionCollectionCount remainingEntries;
  uint32_t factionIndex;
  GameFactionRuntimeRecord *factionRecord;
  uint32_t extractionRate;
  uint32_t modelOffset;
  int tickContribution;
  ModelRuntimeSlot *extractingModel;

  cellsPerEntry = (int)g_TerrainRegionCollectionVisitedCount / (int)g_TerrainRegionCollectionStoredCount;
  entry = (const uint32_t *)(uintptr_t)g_TerrainRegionCollectionEntries;
  for (remainingEntries = g_TerrainRegionCollectionStoredCount; remainingEntries != 0; remainingEntries--) {
    factionIndex = entry[0] >> RESOURCE_EXTRACTION_FACTION_SHIFT & RESOURCE_EXTRACTION_FACTION_MASK;
    factionRecord = &g_GameFactionRuntimeImage.records[factionIndex];
    extractionRate = cellsPerEntry * 2 * (entry[0] >> RESOURCE_EXTRACTION_SHARE_SHIFT) *
                     factionRecord->terrainContributionScaleQ8 >> 15;
    modelOffset = entry[1];
    tickContribution = extractionRate * g_InGameSimulationStepTicks;
    if (payTritium) {
      factionRecord->tritiumExtractionRateQ4PerTick = factionRecord->tritiumExtractionRateQ4PerTick + extractionRate;
      factionRecord->tritiumCurrentQ4 = factionRecord->tritiumCurrentQ4 + tickContribution;
      factionRecord->tritiumExtractedTotalQ4 = factionRecord->tritiumExtractedTotalQ4 + tickContribution;
    }
    else {
      factionRecord->xeniteExtractionRateQ4PerTick = factionRecord->xeniteExtractionRateQ4PerTick + extractionRate;
      factionRecord->xeniteCurrentQ4 = factionRecord->xeniteCurrentQ4 + tickContribution;
      factionRecord->xeniteExtractedTotalQ4 = factionRecord->xeniteExtractedTotalQ4 + tickContribution;
    }
    if (modelOffset != 0) {
      extractingModel = (ModelRuntimeSlot *)((int)modelOffset + g_ModelRuntimeRebaseDelta);
      if (extractingModel->rootModelNodeOrSavedOffset.raw != 0) {
        extractingModel->classLinkState.modelLinkOrState.signedScalarState = tickContribution;
      }
    }
    entry = entry + 2;
  }
}

/* One mining pass: clears the connected-region marks of all cells, then collects every not yet visited region
   of cells with requiredCellFlags (Xenite or Tritium support) and pays it out. */
static void InGameFactionEconomy_PayResourceRegions
          (FieldGridAsset *fieldGrid,FieldGridRegionMask requiredCellFlags,Bool8 payTritium)
{
  FieldGridDimension fieldGridWidth;
  int cellCount;
  int cellIndex;
  FieldGridCell *firstCell;
  FieldGridCell *cell;

  fieldGridWidth = fieldGrid->gridWidth;
  cellCount = fieldGridWidth * fieldGrid->gridHeight;
  firstCell = fieldGrid->cells;
  for (cellIndex = 0; cellIndex < cellCount; cellIndex++) {
    firstCell[cellIndex].flagsAndMaterial =
         firstCell[cellIndex].flagsAndMaterial & ~FIELD_CELL_CONNECTED_REGION_VISITED;
  }
  cell = firstCell;
  for (cellIndex = 0; cellIndex < cellCount; cellIndex++) {
    if (((cell->flagsAndMaterial & (FIELD_CELL_GRID_EDGE_MASK | FIELD_CELL_CONNECTED_REGION_VISITED)) == 0) &&
       ((cell->flagsAndMaterial & requiredCellFlags) != 0)) {
      g_TerrainRegionCollectionStoredCount = 0;
      g_TerrainRegionCollectionVisitedCount = 0;
      TerrainRegionCollection_CollectConnectedCellsRecursive(requiredCellFlags,fieldGridWidth << 7,cell);
      if (g_TerrainRegionCollectionStoredCount != 0) {
        InGameFactionEconomy_PayCollectedRegion(payTritium);
      }
    }
    cell++;
  }
}

/* Caps the Xenite and Tritium stocks of all factions at their storage limits. */
static void InGameFactionEconomy_CapStocksAtStorageLimits(void)
{
  GameFactionRuntimeRecord *factionRecord;
  int factionIndex;

  for (factionIndex = 0; factionIndex < 8; factionIndex++) {
    factionRecord = &g_GameFactionRuntimeImage.records[factionIndex];
    if (factionRecord->xeniteStorageLimitQ4 < factionRecord->xeniteCurrentQ4) {
      factionRecord->xeniteCurrentQ4 = factionRecord->xeniteStorageLimitQ4;
    }
    if (factionRecord->tritiumStorageLimitQ4 < factionRecord->tritiumCurrentQ4) {
      factionRecord->tritiumCurrentQ4 = factionRecord->tritiumStorageLimitQ4;
    }
  }
}

/* Fills one consumer entry from a model runtime (ownerArmyRuntimeOrSavedOffset army slot, classState.energyLoadQ4
   energy demand). */
static void InGameFactionEconomy_FillEnergyConsumer(FactionEnergyConsumerEntry *consumer,int *modelRuntime)
{
  consumer->modelRuntime = (uint32_t)(uintptr_t)modelRuntime;
  consumer->factionIndex = ((ArmyRuntimeSlot *)modelRuntime[2])->factionIndex;
  consumer->demandQ4 = modelRuntime[61];
  consumer->priority =
       g_FactionEnergyAllocationPriorityByModelClass[((ModelDefinition *)*modelRuntime)->runtimeClassId];
}

/* Collects the powered models (energy demand classState.energyLoadQ4 != 0, not dismantling) and, for models
   whose definition has MODEL_DEFINITION_FLAG_COUNT_ATTACHED_ENERGY, their powered attached parts (attachmentCount,
   part runtimes in attachments[], 32-byte slots) into consumers, at most 256. Returns the number collected. */
static uint32_t InGameFactionEconomy_CollectEnergyConsumers(FactionEnergyConsumerEntry *consumers)
{
  uint32_t consumerCount;
  WorldOwnerListNode *worldNode;
  int *modelRuntime;
  int *attachmentSlot;
  int *attachedRuntime;
  int remainingAttachments;

  consumerCount = 0;
  for (worldNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead;
      worldNode != NULL; worldNode = worldNode->nextNode) {
    if (worldNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) continue;
    modelRuntime = (int *)worldNode->runtimePayload;
    if ((modelRuntime[59] & ARMY_MODEL_STATE_DISMANTLING) != 0) continue;
    if (modelRuntime[61] != 0) {
      /* buffer full: the attached parts are skipped as well */
      if (255 < consumerCount) continue;
      InGameFactionEconomy_FillEnergyConsumer(&consumers[consumerCount],modelRuntime);
      consumerCount++;
    }
    if ((consumerCount < 256) &&
       ((((ModelDefinition *)*modelRuntime)->modelFlags & MODEL_DEFINITION_FLAG_COUNT_ATTACHED_ENERGY) != 0)) {
      attachmentSlot = modelRuntime;
      for (remainingAttachments = modelRuntime[3]; remainingAttachments != 0; remainingAttachments--) {
        attachedRuntime = (int *)attachmentSlot[80];
        if (((attachedRuntime != NULL) && (attachedRuntime[61] != 0)) && (consumerCount < 256)) {
          InGameFactionEconomy_FillEnergyConsumer(&consumers[consumerCount],attachedRuntime);
          consumerCount++;
        }
        attachmentSlot = attachmentSlot + 8;
      }
    }
  }
  return consumerCount;
}

/* Selection sort of the consumers by priority, highest first: each position is swapped with every later entry
   of higher priority. */
static void InGameFactionEconomy_SortEnergyConsumersByPriority
          (FactionEnergyConsumerEntry *consumers,uint32_t consumerCount)
{
  uint32_t first;
  uint32_t other;
  FactionEnergyConsumerEntry swapped;

  for (first = 0; first + 1 < consumerCount; first++) {
    for (other = first + 1; other < consumerCount; other++) {
      if (consumers[first].priority < consumers[other].priority) {
        swapped = consumers[first];
        consumers[first] = consumers[other];
        consumers[other] = swapped;
      }
    }
  }
}

/* Energy shortage of the local player's faction: notification 400 (generation capacity too low) or 401 (supply
   too low), each at most every 150 economy runs (anchorCooldown1 / anchorCooldown2). */
static void InGameFactionEconomy_NotifyLocalEnergyShortage
          (GameFactionRuntimeRecord *factionRecord,EnergyDemandQ4 suppliedDemandQ4)
{
  InGameLevelConditionStorage *levelConditionStorage;
  InGameNotificationMovieId notificationMovieId;

  if (factionRecord->energyGenerationCapacityQ4 < suppliedDemandQ4 + factionRecord->unpoweredEnergyDemandQ4) {
    if (factionRecord->anchorCooldown1 != 0) return;
    notificationMovieId = 400;
    factionRecord->anchorCooldown1 = 150;
  }
  else {
    if (factionRecord->anchorCooldown2 != 0) return;
    notificationMovieId = 401;
    factionRecord->anchorCooldown2 = 150;
  }
  /* Level header text starting with UTF-16 "t00_tu": a fixed notification, and both cooldowns never
     expire. */
  levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
  if (((*(int *)&(levelConditionStorage->levelImage).header.levelFileNameUtf16[0] == UTF16_CHAR_PAIR('t','0')) &&
      (*(int *)&(levelConditionStorage->levelImage).header.levelFileNameUtf16[2] == UTF16_CHAR_PAIR('0','_'))) &&
     (*(int *)&(levelConditionStorage->levelImage).header.levelFileNameUtf16[4] == UTF16_CHAR_PAIR('t','u'))) {
    notificationMovieId = 402;
    factionRecord->anchorCooldown1 = INT32_MAX;
    factionRecord->anchorCooldown2 = INT32_MAX;
  }
  InGameNotificationQueue_InsertPriorityRecord(NOTIFICATION_PAYLOAD_NONE,0,0,0,0,0,3,notificationMovieId);
}

/* Energy allocation for one faction: the supply (baselineEnergySupplyQ4 + Tritium stock * 16, capped by
   energyGenerationCapacityQ4, signed comparison) first covers the fixed demand of its army assets, then its
   consumers in priority order; consumers left over get model runtime classState.stateFlags bit 0 (unpowered).
   The energy used above the baseline burns Tritium. */
static void InGameFactionEconomy_AllocateFactionEnergy
          (GameFactionRuntimeRecord *factionRecord,uint32_t factionIndex,
          const FactionEnergyConsumerEntry *consumers,uint32_t consumerCount)
{
  EnergyDemandQ4 armyAssetDemand;
  FactionArmyAssetCount assetIndex;
  EnergyAmountQ4 supply;
  EnergyAmountQ4 remainingEnergy;
  uint32_t consumerIndex;
  const FactionEnergyConsumerEntry *consumer;
  ModelRuntimeSlot *consumerRuntime;
  EnergyDemandQ4 suppliedDemand;
  EnergyAmountQ4 tritiumBurnEnergy;

  /* fixed demand: 1 energy (0x10 Q4) per army asset, 5 (0x50) when its definitionClassValue74 is set */
  armyAssetDemand = 0;
  for (assetIndex = 0; assetIndex < factionRecord->primaryArmyAssetCount; assetIndex++) {
    if (((ArmyAssetRecord *)factionRecord->primaryArmyAssetPointersOrIds[assetIndex])->definitionClassValue74 == 0) {
      armyAssetDemand = armyAssetDemand + 16;
    }
    else {
      armyAssetDemand = armyAssetDemand + 80;
    }
  }
  supply = factionRecord->tritiumCurrentQ4 * 16 + factionRecord->baselineEnergySupplyQ4;
  if ((int)factionRecord->energyGenerationCapacityQ4 < (int)supply) {
    supply = factionRecord->energyGenerationCapacityQ4;
  }
  factionRecord->suppliedEnergyDemandQ4 = factionRecord->suppliedEnergyDemandQ4 + armyAssetDemand;
  remainingEnergy = supply - armyAssetDemand;
  if (supply < armyAssetDemand) {
    remainingEnergy = 0;
  }
  for (consumerIndex = 0; consumerIndex < consumerCount; consumerIndex++) {
    consumer = &consumers[consumerIndex];
    if (factionIndex != consumer->factionIndex) continue;
    consumerRuntime = (ModelRuntimeSlot *)(uintptr_t)consumer->modelRuntime;
    if (remainingEnergy < consumer->demandQ4) {
      consumerRuntime->classState.stateFlags = consumerRuntime->classState.stateFlags | 1;
      factionRecord->unpoweredEnergyDemandQ4 = factionRecord->unpoweredEnergyDemandQ4 + consumer->demandQ4;
    }
    else {
      remainingEnergy = remainingEnergy - consumer->demandQ4;
      factionRecord->suppliedEnergyDemandQ4 = factionRecord->suppliedEnergyDemandQ4 + consumer->demandQ4;
      consumerRuntime->classState.stateFlags = consumerRuntime->classState.stateFlags & ~1u;
    }
  }
  /* energy above the baseline supply is Tritium burnt */
  suppliedDemand = factionRecord->suppliedEnergyDemandQ4;
  tritiumBurnEnergy = suppliedDemand - factionRecord->baselineEnergySupplyQ4;
  if (suppliedDemand < factionRecord->baselineEnergySupplyQ4) {
    tritiumBurnEnergy = 0;
  }
  if (factionRecord->unpoweredEnergyDemandQ4 == 0) {
    factionRecord->anchorCooldown1 = 0;
  }
  else if ((g_InGameRuntimeRoot->worldRuntime).activeFactionRuntimeIndex == factionIndex) {
    InGameFactionEconomy_NotifyLocalEnergyShortage(factionRecord,suppliedDemand);
  }
  factionRecord->tritiumCurrentQ4 =
       factionRecord->tritiumCurrentQ4 - (tritiumBurnEnergy >> 4) * g_InGameSimulationStepTicks;
}

/* Stat table row simulationTick / 128 (0x1000 rows of 7 factions x 2 dwords): the metrics
   combinedProgressScore/activeArmyContribution of factions 1..7, clamped at zero. */
static void InGameFactionEconomy_StoreStatTableSample(void)
{
  GameFactionRuntimeRecord *statFactionRecord;
  WorldRuntimeContext *worldRuntime;
  int *statSample;
  FactionRuntimeIndex factionIndex;
  FactionProgressScore progressScore;
  FactionProgressScore armyContribution;

  statFactionRecord = &g_GameFactionRuntimeImage.records[1];
  worldRuntime = &g_InGameRuntimeRoot->worldRuntime;
  if (g_GameFactionRuntimeImage.tail.simulationTick >> 7 >= 4096) return;
  statSample = (int *)((uint8_t *)g_GameStatTableImage +
                       (g_GameFactionRuntimeImage.tail.simulationTick >> 7) * RESULTS_STAT_SAMPLE_BYTES);
  for (factionIndex = 1; factionIndex < 8; factionIndex++) {
    GameFactionRuntime_RecomputeProgressAndScoreMetrics(factionIndex,worldRuntime);
    progressScore = statFactionRecord->combinedProgressScore;
    armyContribution = statFactionRecord->activeArmyContribution;
    if (progressScore < 0) {
      progressScore = 0;
    }
    if (armyContribution < 0) {
      armyContribution = 0;
    }
    statSample[0] = progressScore;
    statSample[1] = armyContribution;
    statFactionRecord++;
    statSample = statSample + 2;
  }
}

/* The faction economy, run every 8th simulation step (job 0 of InGameRuntime_UpdateSimulationAndNetworkTick):
   1. per faction: reset the step's energy demand and extraction rates, decay the pair-pressure matrix by 7/8,
      count the notification/anchor cooldowns down;
   2. mining: every connected region of Xenite cells (FIELD_CELL_XENITE_SUPPORT), then of Tritium cells
      (FIELD_CELL_TRITIUM_SUPPORT), pays its owning factions (rate, stock and total), then stocks are capped at
      the storage limits;
   3. energy: all powered models (and the attached parts of models whose class has flag 0x80) are sorted by the
      priority of their class (g_FactionEnergyAllocationPriorityByModelClass); per faction 7..1 the supply
      (baselineEnergySupplyQ4 + Tritium stock * 16, capped by energyGenerationCapacityQ4) first covers the fixed
      demand of its army assets, then the consumers in priority order; consumers left over are flagged unpowered
      and the local player gets notification 400 (generation capacity too low) or 401 (supply too low), at
      most every 150 economy runs. The energy used above the baseline burns Tritium;
   4. every 128 steps the progress/score metrics of factions 1..7 are stored in the stat table
      (g_GameStatTableImage, shown by the results screen).
   All amounts are Q4 fixed point.
*/
void InGameRuntime_UpdateFactionResourceExtractionAndEnergyAllocationState(void)

{
  FieldGridAsset *fieldGrid;
  FactionEnergyConsumerEntry *consumers;
  uint32_t consumerCount;
  uint32_t factionIndex;

  InGameFactionEconomy_ResetAndDecayFactionState();
  /* 2. mining: first pass Xenite cells, second pass Tritium cells */
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  InGameFactionEconomy_PayResourceRegions(fieldGrid,FIELD_CELL_XENITE_SUPPORT,false);
  InGameFactionEconomy_PayResourceRegions(fieldGrid,FIELD_CELL_TRITIUM_SUPPORT,true);
  InGameFactionEconomy_CapStocksAtStorageLimits();
  /* 3. energy */
  consumers = (FactionEnergyConsumerEntry *)(uintptr_t)g_TerrainRegionCollectionEntries;
  consumerCount = InGameFactionEconomy_CollectEnergyConsumers(consumers);
  /* without any consumer the whole allocation is skipped (no army-asset demand, no Tritium burn) */
  if (consumerCount != 0) {
    InGameFactionEconomy_SortEnergyConsumersByPriority(consumers,consumerCount);
    /* allocate per faction 7..1 (faction 0 gets nothing) */
    for (factionIndex = 7; factionIndex != 0; factionIndex--) {
      InGameFactionEconomy_AllocateFactionEnergy
                (&g_GameFactionRuntimeImage.records[factionIndex],factionIndex,consumers,consumerCount);
    }
  }
  /* 4. every 128 steps: stat table sample */
  if ((g_GameFactionRuntimeImage.tail.simulationTick & INGAME_STAT_SAMPLE_TICK_MASK) == 0) {
    InGameFactionEconomy_StoreStatTableSample();
  }
}


/* Stores the field-grid cell under the target position of the in-game world motion (the cursor/view target)
   and, unless automatic rotation or zoom is switched off in the map settings,
        copies its heading and a zoom value derived from the committed distance
   (distance * 3/128) into the in-game root's view cache.
*/
void InGameRuntime_UpdateCursorGridAndViewScaleCache(void)

{
  UQ12 committedDistance;
  uint32_t viewSettings;
  FieldGridCoordinates cursorGridPosition;
  InGameRuntimeRoot *inGameRoot;
  
  inGameRoot = g_InGameRuntimeRoot;
  cursorGridPosition = FieldGrid_WorldToGridQ12
                    ((g_InGameRuntimeRoot->worldRuntime).motion.targetPositionYQ12,
                     (g_InGameRuntimeRoot->worldRuntime).motion.targetPositionXQ12);
  inGameRoot->minimapOriginGridPosition = cursorGridPosition;
  viewSettings = PersistentSettings_Read(0,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
  committedDistance = (inGameRoot->worldRuntime).motion.committedDistanceQ12;
  if ((viewSettings & PERSISTENT_MAP_OPTION_AUTOMATIC_ROTATION_OFF) == 0) {
    inGameRoot->minimapRotationAngle =
         (inGameRoot->worldRuntime).motion.headingAngle;
  }
  if ((viewSettings & PERSISTENT_MAP_OPTION_AUTOMATIC_ZOOM_OFF) == 0) {
    /* high dword of distance * 0x6000000 (FIXED_MUL_HIGH) */
    inGameRoot->minimapSampleScaleQ12 =
         FIXED_MUL_HIGH((int)committedDistance,INGAME_MINIMAP_DISTANCE_SCALE_Q32);
  }
  return;
}


/* Called before the session shutdown: remembers which info text the world view shows (text resource
   0x112..0x117, cycled by the player) in the in-game template and in the frontend template's status text, so the
   choice survives the next copy of the templates.
*/
void InGameRuntime_SaveWorldViewInfoTextChoice(UiRootNode *inGameRoot)

{
  INGAME_UI_FIELD(&g_InGameRuntimeDefaultImageTemplate,worldViewCyclingInfoText,0x54,TextResourceId) =
       (TextResourceId)((UiSingleLineTextControl *)INGAME_UI(inGameRoot,worldViewCyclingInfoText))->text;
  FRONTEND_UI_FIELD(&g_FrontendRootInitializationTemplate,bottomBarStatusText,0x54,TextResourceId) =
       INGAME_UI_FIELD(&g_InGameRuntimeDefaultImageTemplate,worldViewCyclingInfoText,0x54,TextResourceId);
  return;
}


/* Network lockstep of a simulation step on the host or in single player. Returns false when the step has to wait:
   the periodic timer has not counted down yet, or (host, interval boundary) the collected command batch could not
   be broadcast because a peer has not submitted yet. */
static Bool8 InGameTick_RunHostOrLocalLockstep(void)

{
  void *packet;
  void *packetEndpoint;

  if (g_InGameNetworkTickCountdown != 0) {
    return false;
  }
  g_InGameNetworkTickCountdown = INGAME_TIMER_TICKS_PER_SIMULATION_STEP;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) == SESSION_NETWORK_ROLE_LOCAL) {
    return true;
  }
  if (g_SessionNetworkTickCounter % g_SessionNetworkTickInterval == 0) {
    /* interval boundary: the batch must be out before it is executed, else wait for the peers */
    while (g_HostCommandBatchSyncSentThisInterval == 0) {
      if (!UiRuntimeRecordRing_TakeOldest(&packet,&packetEndpoint)) {
        if (FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(1)) {
          return false;
        }
        break;
      }
      FrontendTransfer_HostHandleCommandSubmitOrWaitAck
                ((NetworkSessionContext *)packetEndpoint,(FrontendTransferPacketUnion *)packet);
    }
    FrontendTransfer_DispatchStagedCommandRecords();
    g_HostCommandBatchSyncSentThisInterval = 0;
  }
  else {
    /* within the interval: handle sync requests and broadcast the batch as soon as every peer is ready */
    while (UiRuntimeRecordRing_TakeOldest(&packet,&packetEndpoint)) {
      FrontendTransfer_HostHandleCommandSubmitOrWaitAck
                ((NetworkSessionContext *)packetEndpoint,(FrontendTransferPacketUnion *)packet);
    }
    if ((g_HostCommandBatchSyncSentThisInterval == 0) &&
        !FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(0)) {
      g_HostCommandBatchSyncSentThisInterval = 1;
    }
  }
  return true;
}


/* Network lockstep of a simulation step on a client. Returns false when the step has to wait: at an interval
   boundary until the host's command batch has arrived and was executed, otherwise until the periodic timer has
   counted down. */
static Bool8 InGameTick_RunClientLockstep(void)

{
  void *packet;
  void *packetEndpoint;

  if (g_SessionNetworkTickCounter % g_SessionNetworkTickInterval == 0) {
    /* interval boundary: wait for the host's command batch, then execute it */
    if (!UiRuntimeRecordRing_ContainsId(g_FrontendSessionToken)) {
      return false;
    }
    while (UiRuntimeRecordRing_TakeOldest(&packet,&packetEndpoint)) {
      if (FrontendNetwork_HandleCommandBatchAndPlayerTimeout
                    ((NetworkSessionContext *)packetEndpoint,(FrontendTransferPacketUnion *)packet)) {
        break;
      }
    }
    if (FrontendTransfer_ConsumeProcessedFlag()) {
      return false;
    }
  }
  else if (g_InGameNetworkTickCountdown != 0) {
    return false;
  }
  g_InGameNetworkTickCountdown = INGAME_TIMER_TICKS_PER_SIMULATION_STEP;
  return true;
}


/* Runs the terrainStateRefresh callback of every node of the world owner list, starting at firstNode. */
static void InGameTick_RefreshEntityTerrainStates(WorldRuntimeContext *worldRuntime,ModelRuntimeNode *firstNode)

{
  ModelRuntimeNode *modelNode;

  for (modelNode = firstNode; modelNode != NULL;
      modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode) {
    (*(&g_RuntimeMaintenanceCallbackPhases.terrainStateRefresh.army)
      [modelNode->ownerClassId])(worldRuntime,modelNode);
  }
}


/* The world job of this step, chosen by tickPhase (simulationTick & 7), so each job runs every 8th step. */
static void InGameTick_RunWorldJob(InGameRuntimeRoot *inGameRoot,uint32_t tickPhase)

{
  WorldRuntimeContext *worldRuntime;
  WorldOwnerListNode *worldNode;
  ModelRuntimeNode *firstModelNode;

  worldRuntime = &inGameRoot->worldRuntime;
  switch(tickPhase) {
  case 0:
    InGameRuntime_UpdateFactionResourceExtractionAndEnergyAllocationState();
    FrontendRuntime_UpdateCurrentFactionMetricCache();
    GameFactionRuntime_SynchronizeTechnologiesForRelationStates8To10();
    WorldLightingRuntime_UpdateInterpolatedTerrainLighting();
    break;
  case 1:
    TerrainGrid_RelaxNeighborHeightsForwardWithSignGate(worldRuntime->fieldGrid);
    break;
  case 2:
    AiFactionRuntime_RebuildPlanningCapacityState();
    break;
  case 3:
    /* field-grid clamp; with entities present: their terrain state, then the clamp once more */
    firstModelNode = (ModelRuntimeNode *)worldRuntime->ownerListHead;
    FieldGrid_ApplyByteClampLookupToCells(worldRuntime->activeFactionRuntimeIndex,worldRuntime->fieldGrid);
    if (firstModelNode != NULL) {
      InGameTick_RefreshEntityTerrainStates(worldRuntime,firstModelNode);
      FieldGrid_ApplyByteClampLookupToCells(worldRuntime->activeFactionRuntimeIndex,worldRuntime->fieldGrid);
    }
    break;
  case 4:
    GridInfluence_ClearDistanceBandsAndRefreshEntities(worldRuntime->ownerListHead);
    break;
  case 5:
    TerrainGrid_RelaxNeighborHeightsReverseWithSignGate(worldRuntime->fieldGrid);
    break;
  case 6:
    AiFactionRuntime_RebuildPlanningCapacityState();
    break;
  case 7:
    /* occupancy rebuild: clear the mask bits, let every entity mark its cells, then refresh their terrain state */
    worldNode = worldRuntime->ownerListHead;
    FieldGrid_ClearOccupancyMaskBits0To6AllCells(worldRuntime->fieldGrid);
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_LOCAL_FACTION_ENDED) != 0) {
      FieldGrid_SetOccupancyMaskByteBit0AllCells(worldRuntime->activeFactionRuntimeIndex,worldRuntime->fieldGrid);
    }
    if (worldNode != NULL) {
      for (; worldNode != NULL; worldNode = worldNode->nextNode) {
        (*(&g_RuntimeMaintenanceCallbackPhases.occupancyRebuild.army)[worldNode->ownerClassId])
                  (worldRuntime,worldNode);
      }
      InGameTick_RefreshEntityTerrainStates(worldRuntime,(ModelRuntimeNode *)worldRuntime->ownerListHead);
      FieldGrid_ApplyByteClampLookupToCells(worldRuntime->activeFactionRuntimeIndex,worldRuntime->fieldGrid);
      GridScratch_PropagateFieldOccupancyMaskNeighborhood(worldRuntime->fieldGrid);
    }
    break;
  }
}


/* Full simulation step (steps 3-5 of InGameRuntime_UpdateSimulationAndNetworkTick): advances simulationTick, updates
   every entity, every 20 steps the scripted conditions, then the world job of this step. */
static void InGameTick_RunSimulationStep(InGameRuntimeRoot *inGameRoot)

{
  uint32_t tickPhase;
  WorldRuntimeContext *worldRuntime;
  WorldOwnerListNode *worldNode;

  DebugHook_SimulationStepBegin();
  g_GameFactionRuntimeImage.tail.simulationTick++;
  worldRuntime = &inGameRoot->worldRuntime;
  tickPhase = g_GameFactionRuntimeImage.tail.simulationTick & 7;
  /* per-entity update of every model, shot and effect, dispatched by owner class */
  for (worldNode = worldRuntime->ownerListHead; worldNode != NULL; worldNode = worldNode->nextNode) {
    (*(&g_RuntimeMaintenanceCallbackPhases.primaryUpdate.army)[worldNode->ownerClassId])(worldRuntime,worldNode);
  }
  if (g_GameFactionRuntimeImage.tail.simulationTick % 20 == 0) {
    InGameConditionRuntime_UpdateScheduledRecords();
  }
  InGameTick_RunWorldJob(inGameRoot,tickPhase);
  DebugHook_SimulationStepEnd();
}


/* Reduced update while UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE (0x04) is set; it also runs while
   paused. Every second step it re-seats every model on the terrain, and every 16th step it refreshes either the
   influence distance bands or the terrain/runtime classification masks. */
static void InGameTick_RunReducedUpdate(InGameRuntimeRoot *inGameRoot)

{
  ModelRuntimeNode *modelNode;
  ModelDefinition *modelDefinition;

  modelNode = (ModelRuntimeNode *)(inGameRoot->worldRuntime).ownerListHead;
  g_GameFactionRuntimeImage.tail.simulationTick++;
  if ((g_GameFactionRuntimeImage.tail.simulationTick & 1) != 0) {
    return;
  }
  for (; modelNode != NULL; modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode) {
    if (modelNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
      modelDefinition = (((modelNode->runtimePayload).modelRuntime)->definitionOrSavedId).runtimeDefinition;
      g_ArmyPlacementContactKindDispatchTable.callbacks[modelDefinition->placementContactKindIndex]
                (modelDefinition->placementHeightOffsetQ12,
                 (modelNode->worldTransform).translation.y,
                 (modelNode->worldTransform).translation.x,modelNode,
                 &inGameRoot->worldRuntime);
      ModelNodeRuntime_RebuildTransformsFromRoot(modelNode);
    }
  }
  if ((g_GameFactionRuntimeImage.tail.simulationTick & INGAME_REDUCED_GRID_REFRESH_TICK_MASK) == 0) {
    if ((g_GameFactionRuntimeImage.tail.simulationTick & 2) == 0) {
      GridInfluence_ClearDistanceBandsAndRefreshEntities((inGameRoot->worldRuntime).ownerListHead);
    }
    else {
      GridScratch_RebuildTerrainAndRuntimeClassificationMasks(&inGameRoot->worldRuntime);
    }
  }
}


/* One simulation step of the running game: the heart of the game loop. It is not called from a fixed place in
   the frame; the UI runtime calls it as its synchronization hook (UiRuntime_SetSynchronizationHooks) every time
   it releases the in-game tick lock, and the loading loops and movie playback call it directly. Pacing therefore
   happens here: the call is ignored while the lock is held, while the simulation is more than two steps ahead of
   the renderer (g_InGamePendingSimulationTicks, two are consumed per drawn frame) and until the 80 Hz periodic
   timer has counted g_InGameNetworkTickCountdown down to zero (at most 20 steps per second).

   Order of one step:
   1. Network lockstep (every g_SessionNetworkTickInterval steps is an interval boundary):
      - host: takes in the peers' command submissions (FrontendTransfer_HostHandleCommandSubmitOrWaitAck),
        broadcasts the collected command batch once every peer has submitted (at the boundary it waits for
        the peers until then) and at the boundary executes the staged batch
        (FrontendTransfer_DispatchStagedCommandRecords: player commands take effect here);
      - client: at the boundary waits for the host's batch and executes it
        (FrontendNetwork_HandleCommandBatchAndPlayerTimeout);
      - single player: only the timer pacing. A step that has to wait returns before anything below.
   2. Nothing more while the session waits for players or is paused (UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS,
      UI_COMMAND_RUNTIME_FLAG_PAUSED); otherwise simulationTick is advanced.
   3. Per-entity update: every node of the world owner list (models = units and buildings, shots, effects)
      is dispatched through g_RuntimeMaintenanceCallbackPhases.primaryUpdate[ownerClassId]
      (ArmyRuntimeMaintenance_UpdateHierarchyAiAndTimers, ShotModelRuntimeMaintenance_UpdateProjectile...,
      EffectModelRuntimeMaintenance_UpdateLifecycle...): unit hierarchy, AI and timers, projectile motion and
      hits, effect lifetimes.
   4. Every 20 steps: the level's scripted conditions and end triggers
      (InGameConditionRuntime_UpdateScheduledRecords).
   5. One of eight world jobs by simulationTick & 7, so each runs every 8th step:
      0 faction economy (resources, energy), faction metric cache, technology sync, terrain lighting;
      1 and 5 terrain height relaxation (forward / reverse); 2 and 6 faction AI planning;
      3 field-grid clamp and per-entity terrain state (terrainStateRefresh callbacks);
      4 influence distance bands; 7 occupancy rebuild (occupancyRebuild and terrainStateRefresh callbacks).
   While g_UiCommandRuntimeFlags bit 2 (0x04, origin not identified) is set, steps 2-5 are replaced by a
   reduced update that ignores pause and only
   re-seats every model on the terrain every second step (g_ArmyPlacementContactKindDispatchTable) and refreshes
   the influence / classification grids every 16th step.
*/
void InGameRuntime_UpdateSimulationAndNetworkTick(void)

{
  Bool8 lockAlreadyHeld;
  Bool8 stepDue;
  InGameRuntimeRoot *inGameRoot;

  lockAlreadyHeld = g_SpinLockTryAcquire((RuntimeSpinLockValue *)&g_InGameStateTickSpinLock);
  inGameRoot = g_InGameRuntimeRoot;
  if (lockAlreadyHeld) {
    return;
  }
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS) == 0) {
    /* do not run ahead of the renderer by more than a few steps */
    if (2 < (int)g_InGamePendingSimulationTicks) {
      g_SpinLockRelease((RuntimeSpinLockValue *)&g_InGameStateTickSpinLock);
      return;
    }
    g_InGamePendingSimulationTicks++;
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    stepDue = InGameTick_RunHostOrLocalLockstep();
  }
  else {
    stepDue = InGameTick_RunClientLockstep();
  }
  if (!stepDue) {
    g_SpinLockRelease((RuntimeSpinLockValue *)&g_InGameStateTickSpinLock);
    return;
  }
  g_SessionNetworkTickCounter++;
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE) != 0) {
    InGameTick_RunReducedUpdate(inGameRoot);
  }
  else if ((g_UiCommandRuntimeFlags &
           (UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS | UI_COMMAND_RUNTIME_FLAG_PAUSED)) == 0) {
    InGameTick_RunSimulationStep(inGameRoot);
  }
  g_SpinLockRelease((RuntimeSpinLockValue *)&g_InGameStateTickSpinLock);
  return;
}


/* Optional initialisation step of new and loaded sessions; it always succeeds (returns 0), so the callers' failure
   branches never run.
*/
uint8_t InGameRuntime_InitializeOptionalSubsystemAlwaysSuccess(uint32_t unusedArgument)

{
  return 0;
}

