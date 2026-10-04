/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/ui_template.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/ui_template.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

THANDOR_ALIGN(16) InGameUiImage g_InGameRuntimeDefaultImageTemplate = {
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
            0x00000000, (uint32_t)FrontendResultsGraph_DrawFactionWeightSumColumn, /* 5f-format: InGameUiImage.resultsChart1_fields (UI template pointer dword) */
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
            0x00000000, (uint32_t)FrontendResultsGraph_DrawFactionWeightLane0Column, /* 5f-format: InGameUiImage.resultsChart2_fields (UI template pointer dword) */
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
            0x00000000, (uint32_t)FrontendResultsGraph_DrawFactionWeightLane1Column, /* 5f-format: InGameUiImage.resultsChart3_fields (UI template pointer dword) */
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
            0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000, (uint32_t)&g_InGamePlayerStatusTextSlots, /* 5f-format: InGameUiImage.playerStatusBox_fields (UI template pointer dword) */
            (uint32_t)((uint8_t *)&g_InGamePlayerStatusTextSlots + 0x80), /* 5f-format: InGameUiImage.playerStatusBox_fields (UI template pointer dword) */
            (uint32_t)((uint8_t *)&g_InGamePlayerStatusTextSlots + 0x100), /* 5f-format: InGameUiImage.playerStatusBox_fields (UI template pointer dword) */
            (uint32_t)((uint8_t *)&g_InGamePlayerStatusTextSlots + 0x180), /* 5f-format: InGameUiImage.playerStatusBox_fields (UI template pointer dword) */
            (uint32_t)((uint8_t *)&g_InGamePlayerStatusTextSlots + 0x200), /* 5f-format: InGameUiImage.playerStatusBox_fields (UI template pointer dword) */
            (uint32_t)((uint8_t *)&g_InGamePlayerStatusTextSlots + 0x280), /* 5f-format: InGameUiImage.playerStatusBox_fields (UI template pointer dword) */
            (uint32_t)((uint8_t *)&g_InGamePlayerStatusTextSlots + 0x300), /* 5f-format: InGameUiImage.playerStatusBox_fields (UI template pointer dword) */
            (uint32_t)((uint8_t *)&g_InGamePlayerStatusTextSlots + 0x380)}, /* 5f-format: InGameUiImage.playerStatusBox_fields (UI template pointer dword) */
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
            0x00000015, 0x00000000, (uint32_t)&g_InGameCountdownTextUtf16}, /* 5f-format: InGameUiImage.countdownText_fields (UI template pointer dword) */
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
            0x00000312, 0x00000000, (uint32_t)&g_FrontendCurrentFactionPrimaryResourceTextUtf16, /* 5f-format: InGameUiImage.xeniteAmountText_fields (UI template pointer dword) */
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
            0x00000010, 0x00000000, (uint32_t)&g_EmptyFrontendPlayerNameUtf16}, /* 5f-format: InGameUiImage.diplomacyRow1PlayerNameLabel_fields (UI template pointer dword) */
        { /* +5824 diplomacyRow2PlayerNameLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5AC8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4E2C),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 32, .rightOffset = -4, .bottomOffset = 46,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000010, 0x00000000, (uint32_t)&g_EmptyFrontendPlayerNameUtf16}, /* 5f-format: InGameUiImage.diplomacyRow2PlayerNameLabel_fields (UI template pointer dword) */
        { /* +5880 diplomacyRow3PlayerNameLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5B44), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4E84),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 32, .rightOffset = -4, .bottomOffset = 46,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000010, 0x00000000, (uint32_t)&g_EmptyFrontendPlayerNameUtf16}, /* 5f-format: InGameUiImage.diplomacyRow3PlayerNameLabel_fields (UI template pointer dword) */
        { /* +58DC diplomacyRow4PlayerNameLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5BC0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4EDC),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 32, .rightOffset = -4, .bottomOffset = 46,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000010, 0x00000000, (uint32_t)&g_EmptyFrontendPlayerNameUtf16}, /* 5f-format: InGameUiImage.diplomacyRow4PlayerNameLabel_fields (UI template pointer dword) */
        { /* +5938 diplomacyRow5PlayerNameLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5C3C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4F34),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 32, .rightOffset = -4, .bottomOffset = 46,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000010, 0x00000000, (uint32_t)&g_EmptyFrontendPlayerNameUtf16}, /* 5f-format: InGameUiImage.diplomacyRow5PlayerNameLabel_fields (UI template pointer dword) */
        { /* +5994 diplomacyRow6PlayerNameLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5CB8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4F8C),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 32, .rightOffset = -4, .bottomOffset = 46,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000010, 0x00000000, (uint32_t)&g_EmptyFrontendPlayerNameUtf16}, /* 5f-format: InGameUiImage.diplomacyRow6PlayerNameLabel_fields (UI template pointer dword) */
        { /* +59F0 diplomacyRow7PlayerNameLabel g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5D34), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4FE4),
            .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = 4, .topOffset = 32, .rightOffset = -4, .bottomOffset = 46,
            .rightAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000010, 0x00000000, (uint32_t)&g_EmptyFrontendPlayerNameUtf16}, /* 5f-format: InGameUiImage.diplomacyRow7PlayerNameLabel_fields (UI template pointer dword) */
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
