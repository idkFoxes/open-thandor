/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/ui_template.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/ui_template.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

FrontendUiImage g_FrontendRootInitializationTemplate = {
        { /* +0000 frontendRoot g_UiPanelControlVtable */
            .root = {
                .base = {
                    .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x58), .parent = UI_TEMPLATE_NO_LINK,
                    .vtable = THANDOR_PTR(&g_UiPanelControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                    .layoutWidth = -1, .layoutHeight = -1}}},
        { /* +0058 frontendViewModeStack g_UiLayoutContainerControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x2F8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
                .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .pageCount = 0x00000002, .pages = {UI_TEMPLATE_LINK_BITS(0x368), UI_TEMPLATE_LINK_BITS(0x1D4)}},
        { /* +00B0 chatInputSlot g_UiLayoutContainerControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4694),
                .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .pageCount = 0x00000002, .pages = {UI_TEMPLATE_NO_LINK_BITS, UI_TEMPLATE_LINK_BITS(0x108)}},
        { /* +0108 chatInputEdit g_UiRequiredTextEditControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xB0),
                .vtable = THANDOR_PTR(&g_UiRequiredTextEditControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topOffset = -24,
                .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
            .editStateFlags = 0x00000E00, .actionId = 0x0000204C, .bufferCapacityCodeUnits = 0x00000030},
        {},
        { /* +01D4 moviePlaybackView g_UiSoftwareTexturePreviewControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x240), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x58),
                .vtable = THANDOR_PTR(&g_UiSoftwareTexturePreviewControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topAnchorQ31 = 0x10000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x70000000,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
            .actionId = 0x00002048},
        { /* +0240 movieLetterboxTopBar g_UiFillPanelControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x29C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x58),
                .vtable = THANDOR_PTR(&g_UiFillPanelControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x10000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .subresourceOrFillArgb = 0xFF000000},
        { /* +029C movieLetterboxBottomBar g_UiFillPanelControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x58),
                .vtable = THANDOR_PTR(&g_UiFillPanelControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topAnchorQ31 = 0x70000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .subresourceOrFillArgb = 0xFF000000},
        { /* +02F8 chatMessageHistory g_UiConditionalActionControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
                .vtable = THANDOR_PTR(&g_UiConditionalActionControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 16, .topOffset = -29, .rightOffset = 432, .bottomOffset = 29,
                .topAnchorQ31 = 0x8000000, .bottomAnchorQ31 = 0x8000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .actionId = 0x0000200E},
        {},
        { /* +0368 menuRoomModelView g_FrontendModelPointerContextVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x508), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x58),
                .vtable = THANDOR_PTR(&g_FrontendModelPointerContextVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topAnchorQ31 = 0x10000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x70000000,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
            .contextFlags = 0x00091000},
        {},
        /* not in the original: page 6 is displayPageStack (+71A0) instead of displaySettingsPage (+2CB8) */
        { /* +0508 frontendPageStack g_UiLayoutContainerControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x4638), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x58),
                .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topAnchorQ31 = 0x10000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x70000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .pageCount = 0x0000000D,
            .pages = {
                UI_TEMPLATE_NO_LINK_BITS, UI_TEMPLATE_LINK_BITS(0x4808), UI_TEMPLATE_LINK_BITS(0x4EDC),
                UI_TEMPLATE_LINK_BITS(0x5384), UI_TEMPLATE_LINK_BITS(0x56CC), UI_TEMPLATE_LINK_BITS(0x261C),
                UI_TEMPLATE_LINK_BITS(0x71A0), UI_TEMPLATE_LINK_BITS(0x36E4), UI_TEMPLATE_LINK_BITS(0x3E10),
                UI_TEMPLATE_LINK_BITS(0x24A4), UI_TEMPLATE_LINK_BITS(0x1C38), UI_TEMPLATE_LINK_BITS(0xA90),
                UI_TEMPLATE_LINK_BITS(0x58C)}},
        { /* +058C missionBriefingPage g_UiImagePanelControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x5E8), .parent = UI_TEMPLATE_LINK(0x508),
                .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .panelFlags = 0x00000045, .subresource = 0x00000008},
        { /* +05E8 briefingBackButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x648), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x58C),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -256, .topOffset = 128, .rightOffset = -144, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000088, .actionId = 0x00002043},
            .textResourceId = 0x0000219C},
        { /* +0648 briefingExitButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x6A8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x58C),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -256, .topOffset = 128, .rightOffset = -144, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000080, .actionId = 0x0000204F},
            .textResourceId = 0x000021A1},
        { /* +06A8 briefingSaveButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x708), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x58C),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -128, .topOffset = 128, .rightOffset = -16, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000080, .actionId = 0x00002050},
            .textResourceId = 0x000021A2},
        { /* +0708 briefingBeginButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x768), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x58C),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 144, .topOffset = 128, .rightOffset = 256, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000084, .actionId = 0x00002047},
            .textResourceId = 0x0000219D},
        { /* +0768 briefingTitleLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x7C4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x58C),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = -164, .rightOffset = 288, .bottomOffset = -140,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000001, .text = THANDOR_PTR32_BITS(0x0000219B)},
        { /* +07C4 briefingTextScroller g_UiScrollableControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x8B0), .firstChild = UI_TEMPLATE_LINK(0x854), .parent = UI_TEMPLATE_LINK(0x58C),
                .vtable = THANDOR_PTR(&g_UiScrollableControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 4, .topOffset = -128, .rightOffset = 288, .bottomOffset = 56,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .scrollStateFlags = 0x000004A0, .autoScrollStepX = 0x0000000F, .autoScrollStepY = 0x0000000F},
        { /* +0854 briefingText g_UiListOffsetControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x7C4),
                .vtable = THANDOR_PTR(&g_UiListOffsetControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 3, .topOffset = 3, .rightOffset = 266, .bottomOffset = 6,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000040, .wrapWidth = 0x00000105, .text = THANDOR_PTR32_BITS(0x0000219B)},
        { /* +08B0 briefingImage g_UiImageActionControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x914), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x58C),
            .vtable = THANDOR_PTR(&g_UiImageActionControlVtable),
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -288, .topOffset = -128, .rightOffset = -4, .bottomOffset = 112,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000001, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
        { /* +0914 opponentSettingsGroup g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x970), .parent = UI_TEMPLATE_LINK(0x58C),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 4, .topOffset = 64, .rightOffset = 288, .bottomOffset = 112,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000041, .focusChild = UI_TEMPLATE_LINK_BITS(0xA28),
            .text = THANDOR_PTR32_BITS(0x0000219E)},
        { /* +0970 opponentWeakLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x9CC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x914),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topOffset = 24, .rightOffset = 64,
                .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000002, .text = THANDOR_PTR32_BITS(0x0000219F)},
        { /* +09CC opponentStrongLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0xA28), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x914),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -64, .topOffset = 24,
                .leftAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .text = THANDOR_PTR32_BITS(0x000021A0)},
        { /* +0A28 gameSpeedSlider g_UiRangeSliderControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x914),
                .vtable = THANDOR_PTR(&g_UiRangeSliderControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 69, .topOffset = 24, .rightOffset = -69,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
            .sliderFlags = 0x00000004, .minimumValue = 0x00000050, .maximumValue = 0x00000078, .value = 0x00000064,
            .stepValue = 0x00000001, .actionId = 0x0000204A},
        { /* +0A90 factionSetupPage g_UiImagePanelControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0xAEC), .parent = UI_TEMPLATE_LINK(0x508),
                .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .panelFlags = 0x00000045, .subresource = 0x00000008},
        { /* +0AEC factionSetupBackButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0xB4C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xA90),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -256, .topOffset = 128, .rightOffset = -144, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000088, .actionId = 0x00002040},
            .textResourceId = 0x00002183},
        { /* +0B4C factionSetupNextButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0xBAC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xA90),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 144, .topOffset = 128, .rightOffset = 256, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000084, .actionId = 0x00002041},
            .textResourceId = 0x00002184},
        { /* +0BAC factionSetupTitleLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0xC0C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xA90),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = -164, .rightOffset = 288, .bottomOffset = -140,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000001, .text = THANDOR_PTR32_BITS(0x00002182)},
        {0x00002190},
        { /* +0C0C factionSetupFinishButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0xC6C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xA90),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 16, .topOffset = 128, .rightOffset = 128, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x120},
                .stateFlags = 0x00000491, .actionId = 0x00002042},
            .textResourceId = 0x00002185},
        { /* +0C6C factionRosterTable g_UiLayoutContainerControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x1B80), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xA90),
                .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .pageCount = 0x00000001, .pages = {UI_TEMPLATE_LINK_BITS(0xCC0)}},
        { /* +0CC0 factionRow1NumberLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0xD1C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = -100, .rightOffset = -192, .bottomOffset = -76,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000005, .text = THANDOR_PTR32_BITS(0x00002191)},
        { /* +0D1C factionRow2NumberLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0xD78), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = -80, .rightOffset = -192, .bottomOffset = -56,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000005, .text = THANDOR_PTR32_BITS(0x00002192)},
        { /* +0D78 factionRow3NumberLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0xDD4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = -60, .rightOffset = -192, .bottomOffset = -36,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000005, .text = THANDOR_PTR32_BITS(0x00002193)},
        { /* +0DD4 factionRow4NumberLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0xE30), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = -40, .rightOffset = -192, .bottomOffset = -16,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000005, .text = THANDOR_PTR32_BITS(0x00002194)},
        { /* +0E30 factionRow5NumberLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0xE8C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = -20, .rightOffset = -192, .bottomOffset = 4,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000005, .text = THANDOR_PTR32_BITS(0x00002195)},
        { /* +0E8C factionRow6NumberLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0xEE8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .rightOffset = -192, .bottomOffset = 24,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000005, .text = THANDOR_PTR32_BITS(0x00002196)},
        { /* +0EE8 factionRow7NumberLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0xF44), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = 20, .rightOffset = -192, .bottomOffset = 44,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000005, .text = THANDOR_PTR32_BITS(0x00002197)},
        { /* +0F44 factionRow1ColourButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0xFA4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -96, .topOffset = -98, .bottomOffset = -78,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000C80, .actionId = 0x00002044},
            .textResourceId = 0x00002174},
        { /* +0FA4 factionRow2ColourButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x1004), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -96, .topOffset = -78, .bottomOffset = -58,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000C80, .actionId = 0x00002044},
            .textResourceId = 0x00002175},
        { /* +1004 factionRow3ColourButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x1064), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -96, .topOffset = -58, .bottomOffset = -38,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000C80, .actionId = 0x00002044},
            .textResourceId = 0x00002176},
        { /* +1064 factionRow4ColourButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x10C4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -96, .topOffset = -38, .bottomOffset = -18,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000C80, .actionId = 0x00002044},
            .textResourceId = 0x00002177},
        { /* +10C4 factionRow5ColourButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x1124), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -96, .topOffset = -18, .bottomOffset = 2,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000C80, .actionId = 0x00002044},
            .textResourceId = 0x00002178},
        { /* +1124 factionRow6ColourButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x1184), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -96, .topOffset = 2, .bottomOffset = 22,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000C80, .actionId = 0x00002044},
            .textResourceId = 0x00002179},
        { /* +1184 factionRow7ColourButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x11E4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -96, .topOffset = 22, .bottomOffset = 42,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000C80, .actionId = 0x00002044},
            .textResourceId = 0x0000217A},
        { /* +11E4 factionRow1ModeButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x1244), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -192, .topOffset = -98, .rightOffset = -96, .bottomOffset = -78,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000C80, .actionId = 0x00002045},
            .textResourceId = 0x00002198},
        { /* +1244 factionRow2ModeButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x12A4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -192, .topOffset = -78, .rightOffset = -96, .bottomOffset = -58,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000C80, .actionId = 0x00002045},
            .textResourceId = 0x00002198},
        { /* +12A4 factionRow3ModeButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x1304), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -192, .topOffset = -58, .rightOffset = -96, .bottomOffset = -38,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000C80, .actionId = 0x00002045},
            .textResourceId = 0x00002198},
        { /* +1304 factionRow4ModeButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x1364), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -192, .topOffset = -38, .rightOffset = -96, .bottomOffset = -18,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000C80, .actionId = 0x00002045},
            .textResourceId = 0x00002198},
        { /* +1364 factionRow5ModeButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x13C4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -192, .topOffset = -18, .rightOffset = -96, .bottomOffset = 2,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000C80, .actionId = 0x00002045},
            .textResourceId = 0x00002198},
        { /* +13C4 factionRow6ModeButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x1424), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -192, .topOffset = 2, .rightOffset = -96, .bottomOffset = 22,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000C80, .actionId = 0x00002045},
            .textResourceId = 0x00002198},
        { /* +1424 factionRow7ModeButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x1484), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -192, .topOffset = 22, .rightOffset = -96, .bottomOffset = 42,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000C80, .actionId = 0x00002045},
            .textResourceId = 0x00002198},
        { /* +1484 factionRow1PlayCheckbox g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x14E4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 40, .topOffset = -97, .rightOffset = 56, .bottomOffset = -73,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00001480, .actionId = 0x00002046},
            .textResourceId = 0x00002186},
        { /* +14E4 factionRow2PlayCheckbox g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x1544), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 40, .topOffset = -77, .rightOffset = 56, .bottomOffset = -53,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00001480, .actionId = 0x00002046},
            .textResourceId = 0x00002186},
        { /* +1544 factionRow3PlayCheckbox g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x15A4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 40, .topOffset = -57, .rightOffset = 56, .bottomOffset = -33,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00001480, .actionId = 0x00002046},
            .textResourceId = 0x00002186},
        { /* +15A4 factionRow4PlayCheckbox g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x1604), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 40, .topOffset = -37, .rightOffset = 56, .bottomOffset = -13,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00001480, .actionId = 0x00002046},
            .textResourceId = 0x00002186},
        { /* +1604 factionRow5PlayCheckbox g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x1664), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 40, .topOffset = -17, .rightOffset = 56, .bottomOffset = 7,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00001480, .actionId = 0x00002046},
            .textResourceId = 0x00002186},
        { /* +1664 factionRow6PlayCheckbox g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x16C4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 40, .topOffset = 3, .rightOffset = 56, .bottomOffset = 27,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00001480, .actionId = 0x00002046},
            .textResourceId = 0x00002186},
        { /* +16C4 factionRow7PlayCheckbox g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x1724), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 40, .topOffset = 23, .rightOffset = 56, .bottomOffset = 47,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00001480, .actionId = 0x00002046},
            .textResourceId = 0x00002186},
        { /* +1724 factionRow1ParticipantsLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x1780), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 96, .topOffset = -100, .rightOffset = 288, .bottomOffset = -76,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000015,
            .text = THANDOR_PTR((uint8_t *)&g_FrontendUiDisplayModeAndTaskAssignmentScratch + 0x50)}, /* 5f-format: FrontendUiImage.factionRow1ParticipantsLabel_fields (UI template text pointer) */
        { /* +1780 factionRow2ParticipantsLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x17DC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 96, .topOffset = -80, .rightOffset = 288, .bottomOffset = -56,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000015,
            .text = THANDOR_PTR((uint8_t *)&g_FrontendUiDisplayModeAndTaskAssignmentScratch + 0xA0)}, /* 5f-format: FrontendUiImage.factionRow2ParticipantsLabel_fields (UI template text pointer) */
        { /* +17DC factionRow3ParticipantsLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x1838), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 96, .topOffset = -60, .rightOffset = 288, .bottomOffset = -36,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000015,
            .text = THANDOR_PTR((uint8_t *)&g_FrontendUiDisplayModeAndTaskAssignmentScratch + 0xF0)}, /* 5f-format: FrontendUiImage.factionRow3ParticipantsLabel_fields (UI template text pointer) */
        { /* +1838 factionRow4ParticipantsLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x1894), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 96, .topOffset = -40, .rightOffset = 288, .bottomOffset = -16,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000015,
            .text = THANDOR_PTR((uint8_t *)&g_FrontendUiDisplayModeAndTaskAssignmentScratch + 0x140)}, /* 5f-format: FrontendUiImage.factionRow4ParticipantsLabel_fields (UI template text pointer) */
        { /* +1894 factionRow5ParticipantsLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x18F0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 96, .topOffset = -20, .rightOffset = 288, .bottomOffset = 4,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000015,
            .text = THANDOR_PTR((uint8_t *)&g_FrontendUiDisplayModeAndTaskAssignmentScratch + 0x190)}, /* 5f-format: FrontendUiImage.factionRow5ParticipantsLabel_fields (UI template text pointer) */
        { /* +18F0 factionRow6ParticipantsLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x194C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 96, .rightOffset = 288, .bottomOffset = 24,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000015,
            .text = THANDOR_PTR((uint8_t *)&g_FrontendUiDisplayModeAndTaskAssignmentScratch + 0x1E0)}, /* 5f-format: FrontendUiImage.factionRow6ParticipantsLabel_fields (UI template text pointer) */
        { /* +194C factionRow7ParticipantsLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x19A8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 96, .topOffset = 20, .rightOffset = 288, .bottomOffset = 44,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000015,
            .text = THANDOR_PTR((uint8_t *)&g_FrontendUiDisplayModeAndTaskAssignmentScratch + 0x230)}, /* 5f-format: FrontendUiImage.factionRow7ParticipantsLabel_fields (UI template text pointer) */
        { /* +19A8 rosterFactionHeader g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x1A08), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = -124, .rightOffset = -192, .bottomOffset = -100,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000005, .text = THANDOR_PTR32_BITS(0x00002187)},
        {0x0000218D},
        { /* +1A08 rosterModeHeader g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x1A68), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -192, .topOffset = -124, .rightOffset = -96, .bottomOffset = -100,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x100},
            .labelFlags = 0x00000005, .text = THANDOR_PTR32_BITS(0x00002188)},
        {0x0000218E},
        { /* +1A68 rosterColourHeader g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x1AC8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -96, .topOffset = -124, .bottomOffset = -100,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x100},
            .labelFlags = 0x00000005, .text = THANDOR_PTR32_BITS(0x00002189)},
        {0x0000218F},
        { /* +1AC8 rosterAcceptHeader g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x1B24), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topOffset = -124, .rightOffset = 96, .bottomOffset = -100,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x100},
            .labelFlags = 0x00000005, .text = THANDOR_PTR32_BITS(0x0000218A)},
        { /* +1B24 rosterParticipantHeader g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xC6C),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 96, .topOffset = -124, .rightOffset = 288, .bottomOffset = -100,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000045, .text = THANDOR_PTR32_BITS(0x0000218B)},
        { /* +1B80 taskDescriptionLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x1BDC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xA90),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -256, .topOffset = 56, .rightOffset = 288, .bottomOffset = 72,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .text = THANDOR_PTR32_BITS(0x0000218C)},
        { /* +1BDC taskDescriptionText g_UiListOffsetControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xA90),
                .vtable = THANDOR_PTR(&g_UiListOffsetControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -256, .topOffset = 76, .rightOffset = 288, .bottomOffset = 104,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .text = THANDOR_PTR32_BITS(0x00230010)},
        { /* +1C38 gameSelectPage g_UiImagePanelControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x1C94), .parent = UI_TEMPLATE_LINK(0x508),
                .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .panelFlags = 0x00000045, .subresource = 0x00000008},
        { /* +1C94 gameSelectCancelButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x1CF4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1C38),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -256, .topOffset = 128, .rightOffset = -144, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000088, .actionId = 0x00002034},
            .textResourceId = 0x00002155},
        { /* +1CF4 gameSelectStartButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x1D54), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1C38),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 144, .topOffset = 128, .rightOffset = 256, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000084, .actionId = 0x00002038},
            .textResourceId = 0x00002159},
        { /* +1D54 loadGameTabButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x1DB4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1C38),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -304, .topOffset = -112, .rightOffset = -192, .bottomOffset = -88,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000081, .actionId = 0x00002035},
            .textResourceId = 0x00002156},
        { /* +1DB4 singleGameTabButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x1E14), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1C38),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -304, .topOffset = -80, .rightOffset = -192, .bottomOffset = -56,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000081, .actionId = 0x00002036},
            .textResourceId = 0x00002157},
        { /* +1E14 campaignsTabButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x1E74), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1C38),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -304, .topOffset = -48, .rightOffset = -192, .bottomOffset = -24,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000083, .actionId = 0x00002037},
            .textResourceId = 0x00002158},
        { /* +1E74 gameSelectTitleLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x1ED0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1C38),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = -164, .rightOffset = 288, .bottomOffset = -140,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000001, .text = THANDOR_PTR32_BITS(0x00002154)},
        { /* +1ED0 gameSelectTabStack g_UiLayoutContainerControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1C38),
                .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .pageCount = 0x00000003,
            .pages = {
                UI_TEMPLATE_LINK_BITS(0x1F2C), UI_TEMPLATE_LINK_BITS(0x20F4), UI_TEMPLATE_LINK_BITS(0x22D4)}},
        { /* +1F2C savedGamesScroller g_UiScrollableControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x203C), .firstChild = UI_TEMPLATE_LINK(0x1FBC), .parent = UI_TEMPLATE_LINK(0x1ED0),
                .vtable = THANDOR_PTR(&g_UiScrollableControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -176, .topOffset = -112, .rightOffset = 304, .bottomOffset = 64,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .scrollStateFlags = 0x00000480, .autoScrollStepX = 0x0000000F, .autoScrollStepY = 0x0000000F},
        { /* +1FBC savedGamesList g_UiListControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1F2C),
                .vtable = THANDOR_PTR(&g_UiListControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
            .listStateFlags = 0x0000000A, .actionId = 0x00002039, .columnCount = 0x00000002,
            .columns = {{.width = 0x00000100}}},
        {0x000000C9, 0x000000C0, 0x000021DD},
        { /* +203C savedGamesLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x2098), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1ED0),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -160, .topOffset = -128, .rightOffset = 288, .bottomOffset = -112,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x100},
            .text = THANDOR_PTR32_BITS(0x0000215A)},
        { /* +2098 savedGameDescriptionText g_UiListOffsetControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1ED0),
                .vtable = THANDOR_PTR(&g_UiListOffsetControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -160, .topOffset = 72, .rightOffset = 288, .bottomOffset = 112,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .text = THANDOR_PTR32_BITS(0x00230010)},
        { /* +20F4 missionsScroller g_UiScrollableControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x221C), .firstChild = UI_TEMPLATE_LINK(0x2184), .parent = UI_TEMPLATE_LINK(0x1ED0),
                .vtable = THANDOR_PTR(&g_UiScrollableControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -176, .topOffset = -112, .rightOffset = 304, .bottomOffset = 64,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .scrollStateFlags = 0x00000480, .autoScrollStepX = 0x0000000F, .autoScrollStepY = 0x0000000F},
        { /* +2184 missionsList g_UiListControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x20F4),
                .vtable = THANDOR_PTR(&g_UiListControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
            .listStateFlags = 0x0000000A, .actionId = 0x0000203A, .columnCount = 0x00000005,
            .columns = {{.width = 0x000000C0, .rowTextOffset = 0x00000074}}},
        {0x0000005C, 0x00000054, 0x0000005C, 0x00000084, 0x00000030, 0x00000040, 0x00000021, 0x00000048, 0x000021DB},
        { /* +221C missionsLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x2278), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1ED0),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -160, .topOffset = -128, .rightOffset = 288, .bottomOffset = -112,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x100},
            .text = THANDOR_PTR32_BITS(0x0000215B)},
        { /* +2278 missionDescriptionText g_UiListOffsetControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1ED0),
                .vtable = THANDOR_PTR(&g_UiListOffsetControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -160, .topOffset = 72, .rightOffset = 288, .bottomOffset = 112,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .text = THANDOR_PTR32_BITS(0x00230010)},
        { /* +22D4 campaignsScroller g_UiScrollableControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x23EC), .firstChild = UI_TEMPLATE_LINK(0x2364), .parent = UI_TEMPLATE_LINK(0x1ED0),
                .vtable = THANDOR_PTR(&g_UiScrollableControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -176, .topOffset = -112, .rightOffset = 304, .bottomOffset = 64,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .scrollStateFlags = 0x00000480, .autoScrollStepX = 0x0000000F, .autoScrollStepY = 0x0000000F},
        { /* +2364 campaignsList g_UiListControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x22D4),
                .vtable = THANDOR_PTR(&g_UiListControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
            .listStateFlags = 0x0000000A, .actionId = 0x0000203B, .columnCount = 0x00000003,
            .columns = {{.width = 0x00000179, .rowTextOffset = 0x00000054}}},
        {0x00000030, 0x00000040, 0x00000020, 0x00000048, 0x000021DC},
        { /* +23EC campaignsLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x2448), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1ED0),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -160, .topOffset = -128, .rightOffset = 288, .bottomOffset = -112,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x100},
            .text = THANDOR_PTR32_BITS(0x0000215C)},
        { /* +2448 campaignDescriptionText g_UiListOffsetControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x1ED0),
                .vtable = THANDOR_PTR(&g_UiListOffsetControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -160, .topOffset = 72, .rightOffset = 288, .bottomOffset = 112,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .text = THANDOR_PTR32_BITS(0x00230000)},
        { /* +24A4 quitConfirmPage g_UiImagePanelControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x2500), .parent = UI_TEMPLATE_LINK(0x508),
                .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .panelFlags = 0x00000045, .subresource = 0x00000007},
        { /* +2500 quitNoButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x2560), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x24A4),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -256, .topOffset = 128, .rightOffset = -144, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000088, .actionId = 0x00002033},
            .textResourceId = 0x00002147},
        { /* +2560 quitYesButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x25C0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x24A4),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 144, .topOffset = 128, .rightOffset = 256, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000084},
            .textResourceId = 0x00002146},
        { /* +25C0 quitTitleLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x24A4),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = -164, .rightOffset = 288, .bottomOffset = -140,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000001, .text = THANDOR_PTR32_BITS(0x00002145)},
        { /* +261C optionsPage g_UiImagePanelControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x2678), .parent = UI_TEMPLATE_LINK(0x508),
                .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .panelFlags = 0x00000045, .subresource = 0x00000003},
        { /* +2678 optionsOkButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x26D8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x261C),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -256, .topOffset = 128, .rightOffset = -144, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x0000008C, .actionId = 0x00002010},
            .textResourceId = 0x0000211F},
        { /* +26D8 optionsTitleLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x2734), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x261C),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = -164, .rightOffset = 288, .bottomOffset = -140,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000001, .text = THANDOR_PTR32_BITS(0x00002123)},
        /* not in the original: "Anzeige" (text 0x2120 "Graphik") */
        { /* +2734 graphicsSettingsButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x2794), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x261C),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -256, .topOffset = -48, .rightOffset = -144, .bottomOffset = -24,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000080, .actionId = 0x00002011},
            .textResourceId = TEXT_ID_OPTIONS_DISPLAY_BUTTON},
        { /* +2794 settings3DButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x27F4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x261C),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -256, .topOffset = -16, .rightOffset = -144, .bottomOffset = 8,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000080, .actionId = 0x00002012},
            .textResourceId = 0x00002121},
        { /* +27F4 soundSettingsButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    /* not in the original: followed by advancedSettingsButton (then hidePanelCheckbox) */
                    .nextSibling = UI_TEMPLATE_LINK(0x71F8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x261C),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -256, .topOffset = 16, .rightOffset = -144, .bottomOffset = 40,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000080, .actionId = 0x00002013},
            .textResourceId = 0x00002122},
        { /* +2854 hidePanelCheckbox g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x28B4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x261C),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 13, .topOffset = 60, .rightOffset = 240, .bottomOffset = 84,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000091, .actionId = 0x00002049},
            .textResourceId = 0x000021C8},
        { /* +28B4 scrollSpeedGroup g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x2A30), .firstChild = UI_TEMPLATE_LINK(0x2910), .parent = UI_TEMPLATE_LINK(0x261C),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 8, .topOffset = 88, .rightOffset = 240, .bottomOffset = 160,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .focusChild = UI_TEMPLATE_LINK_BITS(0x29C8), .text = THANDOR_PTR32_BITS(0x000021C9)},
        { /* +2910 scrollSpeedSlowLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x296C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x28B4),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topOffset = -28,
                .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .text = THANDOR_PTR32_BITS(0x000021CA)},
        { /* +296C scrollSpeedFastLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x29C8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x28B4),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topOffset = -28,
                .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000002, .text = THANDOR_PTR32_BITS(0x000021CB)},
        { /* +29C8 scrollSpeedSlider g_UiRangeSliderControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x28B4),
                .vtable = THANDOR_PTR(&g_UiRangeSliderControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topOffset = 24, .bottomOffset = -24,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
            .sliderFlags = 0x00000004, .minimumValue = 0x00000008, .maximumValue = 0x00000080, .value = 0x00000020,
            .stepValue = 0x00000001, .actionId = 0x0000204B},
        { /* +2A30 generalMapGroup g_UiTitledWindowControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x2B44), .firstChild = UI_TEMPLATE_LINK(0x2A84), .parent = UI_TEMPLATE_LINK(0x261C),
                .vtable = THANDOR_PTR(&g_UiTitledWindowControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 8, .topOffset = -120, .rightOffset = 240, .bottomOffset = -58,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .titleTextResourceId = 0x0000215F},
        { /* +2A84 autoZoomOffCheckbox g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x2AE4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x2A30),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 3, .topOffset = 3, .rightOffset = -3, .bottomOffset = 27,
                    .rightAnchorQ31 = 0x80000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000091, .actionId = 0x0000203C},
            .textResourceId = 0x00002160},
        { /* +2AE4 autoRotationOffCheckbox g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x2A30),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 3, .topOffset = 27, .rightOffset = -3, .bottomOffset = 51,
                    .rightAnchorQ31 = 0x80000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000091, .actionId = 0x0000203D},
            .textResourceId = 0x00002161},
        { /* +2B44 mouseCommandsGroup g_UiTitledWindowControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x2B98), .parent = UI_TEMPLATE_LINK(0x261C),
                .vtable = THANDOR_PTR(&g_UiTitledWindowControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 8, .topOffset = -42, .rightOffset = 240, .bottomOffset = 44,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .titleTextResourceId = 0x00002162},
        { /* +2B98 linkRotationZoomCheckbox g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x2BF8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x2B44),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 3, .topOffset = 3, .rightOffset = -3, .bottomOffset = 27,
                    .rightAnchorQ31 = 0x80000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000091, .actionId = 0x0000203E},
            .textResourceId = 0x00002163},
        { /* +2BF8 linkRotationTiltCheckbox g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x2C58), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x2B44),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 3, .topOffset = 27, .rightOffset = -3, .bottomOffset = 51,
                    .rightAnchorQ31 = 0x80000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000091, .actionId = 0x0000203F},
            .textResourceId = 0x00002164},
        { /* +2C58 rightButtonNoScrollCheckbox g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x2B44),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 3, .topOffset = 51, .rightOffset = -3, .bottomOffset = 75,
                    .rightAnchorQ31 = 0x80000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000091, .actionId = 0x00002051},
            .textResourceId = 0x00002165},
        { /* +2CB8 displaySettingsPage g_UiImagePanelControlVtable */
            .base = {
                /* not in the original: page 0 of displayPageStack (+71A0) instead of a page of frontendPageStack */
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x2D14), .parent = UI_TEMPLATE_LINK(0x71A0),
                .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .panelFlags = 0x00000045, .subresource = 0x00000004},
        { /* +2D14 displaySettingsBackButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x2D74), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x2CB8),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -256, .topOffset = 128, .rightOffset = -144, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x0000008C, .actionId = 0x00002010},
            .textResourceId = 0x00002129},
        { /* +2D74 displaySettingsApplyButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x2DD4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x2CB8),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 144, .topOffset = 128, .rightOffset = 256, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
                .stateFlags = 0x00000080, .actionId = 0x00002031},
            .textResourceId = 0x00002128},
        /* not in the original: "Anzeigeeinstellungen" (text 0x2124 "Graphikeinstellungen") */
        { /* +2DD4 displaySettingsTitle g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x2E30), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x2CB8),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = -164, .rightOffset = 288, .bottomOffset = -140,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000001, .text = THANDOR_PTR32_BITS(TEXT_ID_FRONTEND_DISPLAY_TITLE)},
        { /* +2E30 displayAdapterGroup g_UiTitledWindowControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x308C), .firstChild = UI_TEMPLATE_LINK(0x2E84), .parent = UI_TEMPLATE_LINK(0x2CB8),
                .vtable = THANDOR_PTR(&g_UiTitledWindowControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                /* not in the original (bottom -10): the upper half of the left column, displayModeKindGroup the lower */
                .leftOffset = -288, .topOffset = -142, .rightOffset = 32, .bottomOffset = -22,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .titleTextResourceId = 0x00002125},
        { /* +2E84 displayAdapterOption1 g_UiPayloadPairTextButtonVtable */
            .base = {
                .selectable = {
                    .base = {
                        .nextSibling = UI_TEMPLATE_LINK(0x2EEC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x2E30),
                        .vtable = THANDOR_PTR(&g_UiPayloadPairTextButtonVtable),
                        .left = -1, .top = -1, .right = -1, .bottom = -1,
                        .leftOffset = 3, .topOffset = 3, .rightOffset = -3, .bottomOffset = 27,
                        .rightAnchorQ31 = 0x80000000,
                        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
                    .stateFlags = 0x00000481, .actionId = 0x0000202C},
                .textResourceId = 0x0000212A}},
        { /* +2EEC displayAdapterOption2 g_UiPayloadPairTextButtonVtable */
            .base = {
                .selectable = {
                    .base = {
                        .nextSibling = UI_TEMPLATE_LINK(0x2F54), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x2E30),
                        .vtable = THANDOR_PTR(&g_UiPayloadPairTextButtonVtable),
                        .left = -1, .top = -1, .right = -1, .bottom = -1,
                        .leftOffset = 3, .topOffset = 27, .rightOffset = -3, .bottomOffset = 51,
                        .rightAnchorQ31 = 0x80000000,
                        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
                    .stateFlags = 0x00000481, .actionId = 0x0000202D},
                .textResourceId = 0x0000212A}},
        { /* +2F54 displayAdapterOption3 g_UiPayloadPairTextButtonVtable */
            .base = {
                .selectable = {
                    .base = {
                        .nextSibling = UI_TEMPLATE_LINK(0x2FBC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x2E30),
                        .vtable = THANDOR_PTR(&g_UiPayloadPairTextButtonVtable),
                        .left = -1, .top = -1, .right = -1, .bottom = -1,
                        .leftOffset = 3, .topOffset = 51, .rightOffset = -3, .bottomOffset = 75,
                        .rightAnchorQ31 = 0x80000000,
                        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
                    .stateFlags = 0x00000481, .actionId = 0x0000202E},
                .textResourceId = 0x0000212A}},
        { /* +2FBC displayAdapterOption4 g_UiPayloadPairTextButtonVtable */
            .base = {
                .selectable = {
                    .base = {
                        .nextSibling = UI_TEMPLATE_LINK(0x3024), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x2E30),
                        .vtable = THANDOR_PTR(&g_UiPayloadPairTextButtonVtable),
                        .left = -1, .top = -1, .right = -1, .bottom = -1,
                        .leftOffset = 3, .topOffset = 75, .rightOffset = -3, .bottomOffset = 99,
                        .rightAnchorQ31 = 0x80000000,
                        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
                    .stateFlags = 0x00000481, .actionId = 0x0000202F},
                .textResourceId = 0x0000212A}},
        { /* +3024 displayAdapterOption5 g_UiPayloadPairTextButtonVtable */
            .base = {
                .selectable = {
                    .base = {
                        .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x2E30),
                        .vtable = THANDOR_PTR(&g_UiPayloadPairTextButtonVtable),
                        .left = -1, .top = -1, .right = -1, .bottom = -1,
                        .leftOffset = 3, .topOffset = 99, .rightOffset = -3, .bottomOffset = 123,
                        .rightAnchorQ31 = 0x80000000,
                        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
                    .stateFlags = 0x00000481, .actionId = 0x00002030},
                .textResourceId = 0x0000212A}},
        { /* +308C displayResolutionGroup g_UiTitledWindowControlVtable */
            .base = {
                /* not in the original: followed by displayModeKindGroup (displayColorDepthGroup is no longer linked);
                holds displayResolutionScrollBox, which scrolls the resolution rows */
                .nextSibling = UI_TEMPLATE_LINK(0x5954), .firstChild = UI_TEMPLATE_LINK(0x5AC8), .parent = UI_TEMPLATE_LINK(0x2CB8),
                .vtable = THANDOR_PTR(&g_UiTitledWindowControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 48, .topOffset = -142, .rightOffset = 288, .bottomOffset = 110,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .titleTextResourceId = 0x00002126},
        { /* +30E0 displayResolutionOption1 g_UiNumericPairTextButtonVtable */
            .base = {
                .selectable = {
                    .base = {
                        /* not in the original: the rows are children of displayResolutionRowPanel (the scrolled content);
                        displayResolutionOption10 is followed by the extra rows at run time */
                        .nextSibling = UI_TEMPLATE_LINK(0x3148), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5B58),
                        .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
                        .left = -1, .top = -1, .right = -1, .bottom = -1,
                        .leftOffset = 3, .topOffset = 3, .rightOffset = -3, .bottomOffset = 27,
                        .rightAnchorQ31 = 0x80000000,
                        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
                    .stateFlags = 0x00000481, .actionId = 0x00002022},
                .textResourceId = 0x0000212B}},
        { /* +3148 displayResolutionOption2 g_UiNumericPairTextButtonVtable */
            .base = {
                .selectable = {
                    .base = {
                        .nextSibling = UI_TEMPLATE_LINK(0x31B0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5B58),
                        .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
                        .left = -1, .top = -1, .right = -1, .bottom = -1,
                        .leftOffset = 3, .topOffset = 27, .rightOffset = -3, .bottomOffset = 51,
                        .rightAnchorQ31 = 0x80000000,
                        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
                    .stateFlags = 0x00000481, .actionId = 0x00002023},
                .textResourceId = 0x0000212B}},
        { /* +31B0 displayResolutionOption3 g_UiNumericPairTextButtonVtable */
            .base = {
                .selectable = {
                    .base = {
                        .nextSibling = UI_TEMPLATE_LINK(0x3218), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5B58),
                        .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
                        .left = -1, .top = -1, .right = -1, .bottom = -1,
                        .leftOffset = 3, .topOffset = 51, .rightOffset = -3, .bottomOffset = 75,
                        .rightAnchorQ31 = 0x80000000,
                        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
                    .stateFlags = 0x00000481, .actionId = 0x00002024},
                .textResourceId = 0x0000212B}},
        { /* +3218 displayResolutionOption4 g_UiNumericPairTextButtonVtable */
            .base = {
                .selectable = {
                    .base = {
                        .nextSibling = UI_TEMPLATE_LINK(0x3280), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5B58),
                        .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
                        .left = -1, .top = -1, .right = -1, .bottom = -1,
                        .leftOffset = 3, .topOffset = 75, .rightOffset = -3, .bottomOffset = 99,
                        .rightAnchorQ31 = 0x80000000,
                        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
                    .stateFlags = 0x00000481, .actionId = 0x00002025},
                .textResourceId = 0x0000212B}},
        { /* +3280 displayResolutionOption5 g_UiNumericPairTextButtonVtable */
            .base = {
                .selectable = {
                    .base = {
                        .nextSibling = UI_TEMPLATE_LINK(0x32E8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5B58),
                        .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
                        .left = -1, .top = -1, .right = -1, .bottom = -1,
                        .leftOffset = 3, .topOffset = 99, .rightOffset = -3, .bottomOffset = 123,
                        .rightAnchorQ31 = 0x80000000,
                        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
                    .stateFlags = 0x00000481, .actionId = 0x00002026},
                .textResourceId = 0x0000212B}},
        { /* +32E8 displayResolutionOption6 g_UiNumericPairTextButtonVtable */
            .base = {
                .selectable = {
                    .base = {
                        .nextSibling = UI_TEMPLATE_LINK(0x3350), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5B58),
                        .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
                        .left = -1, .top = -1, .right = -1, .bottom = -1,
                        .leftOffset = 3, .topOffset = 123, .rightOffset = -3, .bottomOffset = 147,
                        .rightAnchorQ31 = 0x80000000,
                        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
                    .stateFlags = 0x00000481, .actionId = 0x00002027},
                .textResourceId = 0x0000212B}},
        { /* +3350 displayResolutionOption7 g_UiNumericPairTextButtonVtable */
            .base = {
                .selectable = {
                    .base = {
                        .nextSibling = UI_TEMPLATE_LINK(0x33B8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5B58),
                        .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
                        .left = -1, .top = -1, .right = -1, .bottom = -1,
                        .leftOffset = 3, .topOffset = 147, .rightOffset = -3, .bottomOffset = 171,
                        .rightAnchorQ31 = 0x80000000,
                        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
                    .stateFlags = 0x00000481, .actionId = 0x00002028},
                .textResourceId = 0x0000212B}},
        { /* +33B8 displayResolutionOption8 g_UiNumericPairTextButtonVtable */
            .base = {
                .selectable = {
                    .base = {
                        .nextSibling = UI_TEMPLATE_LINK(0x3420), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5B58),
                        .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
                        .left = -1, .top = -1, .right = -1, .bottom = -1,
                        .leftOffset = 3, .topOffset = 171, .rightOffset = -3, .bottomOffset = 195,
                        .rightAnchorQ31 = 0x80000000,
                        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
                    .stateFlags = 0x00000481, .actionId = 0x00002029},
                .textResourceId = 0x0000212B}},
        { /* +3420 displayResolutionOption9 g_UiNumericPairTextButtonVtable */
            .base = {
                .selectable = {
                    .base = {
                        .nextSibling = UI_TEMPLATE_LINK(0x3488), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5B58),
                        .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
                        .left = -1, .top = -1, .right = -1, .bottom = -1,
                        .leftOffset = 3, .topOffset = 195, .rightOffset = -3, .bottomOffset = 219,
                        .rightAnchorQ31 = 0x80000000,
                        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
                    .stateFlags = 0x00000481, .actionId = 0x0000202A},
                .textResourceId = 0x0000212B}},
        { /* +3488 displayResolutionOption10 g_UiNumericPairTextButtonVtable */
            .base = {
                .selectable = {
                    .base = {
                        .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5B58),
                        .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
                        .left = -1, .top = -1, .right = -1, .bottom = -1,
                        .leftOffset = 3, .topOffset = 219, .rightOffset = -3, .bottomOffset = 243,
                        .rightAnchorQ31 = 0x80000000,
                        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
                    .stateFlags = 0x00000481, .actionId = 0x0000202B},
                .textResourceId = 0x0000212B}},
        { /* +34F0 displayColorDepthGroup g_UiTitledWindowControlVtable */
            .base = {
                /* not in the original: no longer linked into the page (32-bit colour only); the node and its four
                choices stay as unused template data, so the image keeps its layout */
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x3544), .parent = UI_TEMPLATE_LINK(0x2CB8),
                .vtable = THANDOR_PTR(&g_UiTitledWindowControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = 2, .rightOffset = 32, .bottomOffset = 110,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .titleTextResourceId = 0x00002127},
        { /* +3544 displayColorDepthOption1 g_UiNumericPairTextButtonVtable */
            .base = {
                .selectable = {
                    .base = {
                        .nextSibling = UI_TEMPLATE_LINK(0x35AC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x34F0),
                        .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
                        .left = -1, .top = -1, .right = -1, .bottom = -1,
                        .leftOffset = 3, .topOffset = 3, .rightOffset = -3, .bottomOffset = 27,
                        .rightAnchorQ31 = 0x80000000,
                        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
                    .stateFlags = 0x00000481, .actionId = 0x0000201E},
                .textResourceId = 0x0000212C}},
        { /* +35AC displayColorDepthOption2 g_UiNumericPairTextButtonVtable */
            .base = {
                .selectable = {
                    .base = {
                        .nextSibling = UI_TEMPLATE_LINK(0x3614), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x34F0),
                        .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
                        .left = -1, .top = -1, .right = -1, .bottom = -1,
                        .leftOffset = 3, .topOffset = 27, .rightOffset = -3, .bottomOffset = 51,
                        .rightAnchorQ31 = 0x80000000,
                        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
                    .stateFlags = 0x00000481, .actionId = 0x0000201F},
                .textResourceId = 0x0000212C}},
        { /* +3614 displayColorDepthOption3 g_UiNumericPairTextButtonVtable */
            .base = {
                .selectable = {
                    .base = {
                        .nextSibling = UI_TEMPLATE_LINK(0x367C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x34F0),
                        .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
                        .left = -1, .top = -1, .right = -1, .bottom = -1,
                        .leftOffset = 3, .topOffset = 51, .rightOffset = -3, .bottomOffset = 75,
                        .rightAnchorQ31 = 0x80000000,
                        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
                    .stateFlags = 0x00000481, .actionId = 0x00002020},
                .textResourceId = 0x0000212C}},
        { /* +367C displayColorDepthOption4 g_UiNumericPairTextButtonVtable */
            .base = {
                .selectable = {
                    .base = {
                        .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x34F0),
                        .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
                        .left = -1, .top = -1, .right = -1, .bottom = -1,
                        .leftOffset = 3, .topOffset = 75, .rightOffset = -3, .bottomOffset = 99,
                        .rightAnchorQ31 = 0x80000000,
                        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
                    .stateFlags = 0x00000481, .actionId = 0x00002021},
                .textResourceId = 0x0000212C}},
        { /* +36E4 graphicsSettingsPage g_UiImagePanelControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x3740), .parent = UI_TEMPLATE_LINK(0x508),
                .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .panelFlags = 0x00000045, .subresource = 0x00000005},
        { /* +3740 graphicsSettingsBackButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x37A0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x36E4),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -256, .topOffset = 128, .rightOffset = -144, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x0000008C, .actionId = 0x00002010},
            .textResourceId = 0x0000211F},
        { /* +37A0 graphicsSettingsTitle g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x37FC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x36E4),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = -164, .rightOffset = 288, .bottomOffset = -140,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000001, .text = THANDOR_PTR32_BITS(0x0000212E)},
        { /* +37FC shadingEnabledCheckbox g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x385C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x36E4),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -280, .topOffset = -112, .rightOffset = -16, .bottomOffset = -88,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000091, .actionId = 0x00002014},
            .textResourceId = 0x0000212F},
        { /* +385C shadingLevelGroup g_UiTitledWindowControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x3B20), .firstChild = UI_TEMPLATE_LINK(0x38B0), .parent = UI_TEMPLATE_LINK(0x36E4),
                .vtable = THANDOR_PTR(&g_UiTitledWindowControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = -80, .rightOffset = -8, .bottomOffset = 78,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .titleTextResourceId = 0x00002130},
        { /* +38B0 shadingLevelGrid32Depth32 g_UiNumericPairTextButtonVtable */
            .base = {
                .selectable = {
                    .base = {
                        .nextSibling = UI_TEMPLATE_LINK(0x3918), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x385C),
                        .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
                        .left = -1, .top = -1, .right = -1, .bottom = -1,
                        .leftOffset = 3, .topOffset = 3, .rightOffset = -3, .bottomOffset = 27,
                        .rightAnchorQ31 = 0x80000000,
                        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                    .stateFlags = 0x00000481, .actionId = 0x00002015},
                .textResourceId = 0x00002133},
            .firstValue = 0x00000020, .secondValue = 0x00000020},
        { /* +3918 shadingLevelGrid32Depth64 g_UiNumericPairTextButtonVtable */
            .base = {
                .selectable = {
                    .base = {
                        .nextSibling = UI_TEMPLATE_LINK(0x3980), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x385C),
                        .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
                        .left = -1, .top = -1, .right = -1, .bottom = -1,
                        .leftOffset = 3, .topOffset = 27, .rightOffset = -3, .bottomOffset = 51,
                        .rightAnchorQ31 = 0x80000000,
                        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                    .stateFlags = 0x00000481, .actionId = 0x00002015},
                .textResourceId = 0x00002133},
            .firstValue = 0x00000020, .secondValue = 0x00000040},
        { /* +3980 shadingLevelGrid32Depth128 g_UiNumericPairTextButtonVtable */
            .base = {
                .selectable = {
                    .base = {
                        .nextSibling = UI_TEMPLATE_LINK(0x39E8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x385C),
                        .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
                        .left = -1, .top = -1, .right = -1, .bottom = -1,
                        .leftOffset = 3, .topOffset = 51, .rightOffset = -3, .bottomOffset = 75,
                        .rightAnchorQ31 = 0x80000000,
                        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                    .stateFlags = 0x00000481, .actionId = 0x00002015},
                .textResourceId = 0x00002133},
            .firstValue = 0x00000020, .secondValue = 0x00000080},
        { /* +39E8 shadingLevelGrid64Depth64 g_UiNumericPairTextButtonVtable */
            .base = {
                .selectable = {
                    .base = {
                        .nextSibling = UI_TEMPLATE_LINK(0x3A50), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x385C),
                        .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
                        .left = -1, .top = -1, .right = -1, .bottom = -1,
                        .leftOffset = 3, .topOffset = 75, .rightOffset = -3, .bottomOffset = 99,
                        .rightAnchorQ31 = 0x80000000,
                        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                    .stateFlags = 0x00000481, .actionId = 0x00002015},
                .textResourceId = 0x00002133},
            .firstValue = 0x00000040, .secondValue = 0x00000040},
        { /* +3A50 shadingLevelGrid64Depth128 g_UiNumericPairTextButtonVtable */
            .base = {
                .selectable = {
                    .base = {
                        .nextSibling = UI_TEMPLATE_LINK(0x3AB8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x385C),
                        .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
                        .left = -1, .top = -1, .right = -1, .bottom = -1,
                        .leftOffset = 3, .topOffset = 99, .rightOffset = -3, .bottomOffset = 123,
                        .rightAnchorQ31 = 0x80000000,
                        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                    .stateFlags = 0x00000481, .actionId = 0x00002015},
                .textResourceId = 0x00002133},
            .firstValue = 0x00000040, .secondValue = 0x00000080},
        { /* +3AB8 shadingLevelGrid128Depth128 g_UiNumericPairTextButtonVtable */
            .base = {
                .selectable = {
                    .base = {
                        .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x385C),
                        .vtable = THANDOR_PTR(&g_UiNumericPairTextButtonVtable),
                        .left = -1, .top = -1, .right = -1, .bottom = -1,
                        .leftOffset = 3, .topOffset = 123, .rightOffset = -3, .bottomOffset = 147,
                        .rightAnchorQ31 = 0x80000000,
                        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                    .stateFlags = 0x00000481, .actionId = 0x00002015},
                .textResourceId = 0x00002133},
            .firstValue = 0x00000080, .secondValue = 0x00000080},
        { /* +3B20 polygonDetailLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x3C9C), .firstChild = UI_TEMPLATE_LINK(0x3B7C), .parent = UI_TEMPLATE_LINK(0x36E4),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 8, .topOffset = -112, .rightOffset = 288, .bottomOffset = -40,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .focusChild = UI_TEMPLATE_LINK_BITS(0x3C34), .text = THANDOR_PTR32_BITS(0x00002131)},
        { /* +3B7C polygonDetailMinCaption g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x3BD8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3B20),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topOffset = -28,
                .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .text = THANDOR_PTR32_BITS(0x00002134)},
        { /* +3BD8 polygonDetailMaxCaption g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x3C34), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3B20),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topOffset = -28,
                .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000002, .text = THANDOR_PTR32_BITS(0x00002135)},
        { /* +3C34 polygonDetailSlider g_UiRangeSliderControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3B20),
                .vtable = THANDOR_PTR(&g_UiRangeSliderControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topOffset = 24, .bottomOffset = -24,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
            .sliderFlags = 0x00000004, .minimumValue = 0x00004000, .maximumValue = 0x00040000, .value = 0x00010000,
            .stepValue = 0x00001000, .actionId = 0x00002016},
        { /* +3C9C textureQualityGroup g_UiTitledWindowControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x3CF0), .parent = UI_TEMPLATE_LINK(0x36E4),
                .vtable = THANDOR_PTR(&g_UiTitledWindowControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 8, .topOffset = -8, .rightOffset = 288, .bottomOffset = 78,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .titleTextResourceId = 0x00002132},
        { /* +3CF0 textureQualityLow g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x3D50), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3C9C),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 3, .topOffset = 3, .rightOffset = -3, .bottomOffset = 27,
                    .rightAnchorQ31 = 0x80000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000481, .actionId = 0x00002017},
            .textResourceId = 0x00002136},
        { /* +3D50 textureQualityMedium g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x3DB0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3C9C),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 3, .topOffset = 27, .rightOffset = -3, .bottomOffset = 51,
                    .rightAnchorQ31 = 0x80000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000481, .actionId = 0x00002017},
            .textResourceId = 0x00002137},
        { /* +3DB0 textureQualityHigh g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3C9C),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 3, .topOffset = 51, .rightOffset = -3, .bottomOffset = 75,
                    .rightAnchorQ31 = 0x80000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000481, .actionId = 0x00002017},
            .textResourceId = 0x00002138},
        { /* +3E10 audioSettingsPage g_UiImagePanelControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x3E6C), .parent = UI_TEMPLATE_LINK(0x508),
                .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .panelFlags = 0x00000045, .subresource = 0x00000006},
        { /* +3E6C audioSettingsBackButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x3ECC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3E10),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -256, .topOffset = 128, .rightOffset = -144, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x0000008C, .actionId = 0x00002010},
            .textResourceId = 0x0000211F},
        { /* +3ECC audioSettingsTitle g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x3F28), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3E10),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = -164, .rightOffset = 288, .bottomOffset = -140,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000001, .text = THANDOR_PTR32_BITS(0x0000213A)},
        { /* +3F28 musicEnabledCheckbox g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x3F88), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3E10),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -280, .topOffset = -112, .rightOffset = -16, .bottomOffset = -88,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000091, .actionId = 0x00002019},
            .textResourceId = 0x0000213B},
        { /* +3F88 soundEffectsEnabledCheckbox g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x3FE8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3E10),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -280, .topOffset = -80, .rightOffset = -16, .bottomOffset = -56,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000091, .actionId = 0x00002018},
            .textResourceId = 0x0000213C},
        { /* +3FE8 reverseStereoCheckbox g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x4048), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x3E10),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -280, .topOffset = -16, .rightOffset = -16, .bottomOffset = 8,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000091, .actionId = 0x0000201A},
            .textResourceId = 0x0000213D},
        { /* +4048 effectsVolumeLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x41C4), .firstChild = UI_TEMPLATE_LINK(0x40A4), .parent = UI_TEMPLATE_LINK(0x3E10),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 8, .topOffset = -120, .rightOffset = 288, .bottomOffset = -52,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .focusChild = UI_TEMPLATE_LINK_BITS(0x415C), .text = THANDOR_PTR32_BITS(0x0000213E)},
        { /* +40A4 effectsVolumeMinCaption g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x4100), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4048),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topOffset = -28,
                .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .text = THANDOR_PTR32_BITS(0x00002141)},
        { /* +4100 effectsVolumeMaxCaption g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x415C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4048),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topOffset = -28,
                .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000002, .text = THANDOR_PTR32_BITS(0x00002142)},
        { /* +415C effectsVolumeSlider g_UiRangeSliderControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4048),
                .vtable = THANDOR_PTR(&g_UiRangeSliderControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topOffset = 24, .bottomOffset = -24,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
            .sliderFlags = 0x00000004, .maximumValue = 0x00008000, .value = 0x00008000, .stepValue = 0x00000800,
            .actionId = 0x0000201B},
        { /* +41C4 movieVolumeLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x4340), .firstChild = UI_TEMPLATE_LINK(0x4220), .parent = UI_TEMPLATE_LINK(0x3E10),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 8, .topOffset = -52, .rightOffset = 288, .bottomOffset = 16,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .focusChild = UI_TEMPLATE_LINK_BITS(0x42D8), .text = THANDOR_PTR32_BITS(0x0000213F)},
        { /* +4220 movieVolumeMinCaption g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x427C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x41C4),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topOffset = -28,
                .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .text = THANDOR_PTR32_BITS(0x00002141)},
        { /* +427C movieVolumeMaxCaption g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x42D8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x41C4),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topOffset = -28,
                .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000002, .text = THANDOR_PTR32_BITS(0x00002142)},
        { /* +42D8 movieVolumeSlider g_UiRangeSliderControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x41C4),
                .vtable = THANDOR_PTR(&g_UiRangeSliderControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topOffset = 24, .bottomOffset = -24,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
            .sliderFlags = 0x00000004, .maximumValue = 0x00008000, .value = 0x00008000, .stepValue = 0x00000800,
            .actionId = 0x0000201C},
        { /* +4340 musicVolumeLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x44BC), .firstChild = UI_TEMPLATE_LINK(0x439C), .parent = UI_TEMPLATE_LINK(0x3E10),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 8, .topOffset = 84, .rightOffset = 288, .bottomOffset = 152,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .focusChild = UI_TEMPLATE_LINK_BITS(0x4454), .text = THANDOR_PTR32_BITS(0x00002140)},
        { /* +439C musicVolumeMinCaption g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x43F8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4340),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topOffset = -28,
                .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .text = THANDOR_PTR32_BITS(0x00002141)},
        { /* +43F8 musicVolumeMaxCaption g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x4454), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4340),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topOffset = -28,
                .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000002, .text = THANDOR_PTR32_BITS(0x00002142)},
        { /* +4454 musicVolumeSlider g_UiRangeSliderControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4340),
                .vtable = THANDOR_PTR(&g_UiRangeSliderControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topOffset = 24, .bottomOffset = -24,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
            .sliderFlags = 0x00000004, .maximumValue = 0x00008000, .value = 0x00008000, .stepValue = 0x00000800,
            .actionId = 0x0000201D},
        { /* +44BC movieEventVolumeLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x4518), .parent = UI_TEMPLATE_LINK(0x3E10),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 8, .topOffset = 16, .rightOffset = 288, .bottomOffset = 84,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .focusChild = UI_TEMPLATE_LINK_BITS(0x45D0), .text = THANDOR_PTR32_BITS(0x00002143)},
        { /* +4518 movieEventVolumeMinCaption g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x4574), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x44BC),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topOffset = -28,
                .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .text = THANDOR_PTR32_BITS(0x00002141)},
        { /* +4574 movieEventVolumeMaxCaption g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x45D0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x44BC),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topOffset = -28,
                .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000002, .text = THANDOR_PTR32_BITS(0x00002142)},
        { /* +45D0 movieEventVolumeSlider g_UiRangeSliderControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x44BC),
                .vtable = THANDOR_PTR(&g_UiRangeSliderControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topOffset = 24, .bottomOffset = -24,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
            .sliderFlags = 0x00000004, .maximumValue = 0x00008000, .value = 0x00008000, .stepValue = 0x00000800,
            .actionId = 0x0000204E},
        { /* +4638 topBlackBar g_UiFillPanelControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x4694), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x58),
                .vtable = THANDOR_PTR(&g_UiFillPanelControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x10000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .subresourceOrFillArgb = 0xFF000000},
        { /* +4694 bottomBar g_UiFillPanelControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x46F0), .parent = UI_TEMPLATE_LINK(0x58),
                .vtable = THANDOR_PTR(&g_UiFillPanelControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .topAnchorQ31 = 0x70000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .subresourceOrFillArgb = 0xFF000000},
        { /* +46F0 bottomBarConditionalAction g_UiConditionalActionControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x4750), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4694),
                .vtable = THANDOR_PTR(&g_UiConditionalActionControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .actionId = -1},
        { /* +4750 bottomBarStatusText g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x47AC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4694),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x0000000A, .text = THANDOR_PTR32_BITS(0x00000112)},
        { /* +47AC transferProgressGauge g_UiTransferProgressGaugeVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0xB0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4694),
                .vtable = THANDOR_PTR(&g_UiTransferProgressGaugeVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -160, .topOffset = -32, .rightOffset = -16,
                .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1}},
        { /* +4808 networkGamePage g_UiImagePanelControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x4864), .parent = UI_TEMPLATE_LINK(0x508),
                .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .panelFlags = 0x00000045},
        { /* +4864 networkGameTitle g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x48C0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4808),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = -164, .rightOffset = 288, .bottomOffset = -140,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000001, .text = THANDOR_PTR32_BITS(0x00002107)},
        { /* +48C0 networkGameBackButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x4920), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4808),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -256, .topOffset = 128, .rightOffset = -144, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000088, .actionId = 0x00002000},
            .textResourceId = 0x00002100},
        { /* +4920 networkGameHostButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x4980), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4808),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 144, .topOffset = 128, .rightOffset = 256, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
                .stateFlags = 0x00000080, .actionId = 0x00002001},
            .textResourceId = 0x00002101},
        { /* +4980 networkGameJoinButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x49E0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4808),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 16, .topOffset = 128, .rightOffset = 128, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
                .stateFlags = 0x00000084, .actionId = 0x00002002},
            .textResourceId = 0x00002102},
        { /* +49E0 networkProtocolScrollBox g_UiScrollableControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x4AD8), .firstChild = UI_TEMPLATE_LINK(0x4A70), .parent = UI_TEMPLATE_LINK(0x4808),
                .vtable = THANDOR_PTR(&g_UiScrollableControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 16, .topOffset = -116, .rightOffset = 256, .bottomOffset = -48,
                .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .scrollStateFlags = 0x000004A0, .autoScrollStepX = 0x0000000F, .autoScrollStepY = 0x0000000F},
        { /* +4A70 networkProtocolList g_UiTextListControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x49E0),
                .vtable = THANDOR_PTR(&g_UiTextListControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
            .listStateFlags = 0x0000000A, .actionId = 0x0000200F},
        { /* +4AD8 sessionListScrollBox g_UiScrollableControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x4BEC), .firstChild = UI_TEMPLATE_LINK(0x4B68), .parent = UI_TEMPLATE_LINK(0x4808),
                .vtable = THANDOR_PTR(&g_UiScrollableControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -256, .topOffset = -16, .rightOffset = 256, .bottomOffset = 112,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .scrollStateFlags = 0x000004A0, .autoScrollStepX = 0x0000000F, .autoScrollStepY = 0x0000000F},
        { /* +4B68 sessionList g_UiListControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4AD8),
                .vtable = THANDOR_PTR(&g_UiListControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
            .listStateFlags = 0x0000000A, .actionId = 0x00002009, .columnCount = 0x00000003,
            .columns = {{.width = -128, .rowTextOffset = 0x00000018}}},
        {0x00000148, 0x00000040, 0x00000030, 0x00000098},
        { /* +4BEC hostAddressLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x4C48), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4808),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -240, .topOffset = -81, .rightOffset = -32, .bottomOffset = -65,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000004, .text = THANDOR_PTR32_BITS(0x00002103)},
        { /* +4C48 playerNameLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x4CA4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4808),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -240, .topOffset = -132, .rightOffset = -32, .bottomOffset = -116,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000004, .text = THANDOR_PTR32_BITS(0x00002106)},
        { /* +4CA4 networkProtocolLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x4D00), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4808),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 32, .topOffset = -132, .rightOffset = 240, .bottomOffset = -116,
                .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000004, .text = THANDOR_PTR32_BITS(0x00002105)},
        { /* +4D00 sessionListLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x4D5C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4808),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -240, .topOffset = -32, .rightOffset = 240, .bottomOffset = -16,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000004, .text = THANDOR_PTR32_BITS(0x00002104)},
        { /* +4D5C hostAddressEdit g_UiRequiredTextEditControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x4E48), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4808),
                .vtable = THANDOR_PTR(&g_UiRequiredTextEditControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -256, .topOffset = -65, .rightOffset = -16, .bottomOffset = -48,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
            .editStateFlags = 0x00000608, .actionId = 0x0000200D, .bufferCapacityCodeUnits = 0x00000040},
        {},
        { /* +4E48 playerNameEdit g_UiRequiredTextEditControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4808),
                .vtable = THANDOR_PTR(&g_UiRequiredTextEditControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -256, .topOffset = -116, .rightOffset = -16, .bottomOffset = -99,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
            .editStateFlags = 0x00000408, .actionId = 0x00002032, .bufferCapacityCodeUnits = 0x00000014},
        {},
        { /* +4EDC hostGameSetupPage g_UiImagePanelControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x4F38), .parent = UI_TEMPLATE_LINK(0x508),
                .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .panelFlags = 0x00000045, .subresource = 0x00000001},
        { /* +4F38 hostGameSetupTitle g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x4F94), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4EDC),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = -164, .rightOffset = 288, .bottomOffset = -140,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000001, .text = THANDOR_PTR32_BITS(0x0000210C)},
        { /* +4F94 hostGameSetupBackButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x4FF4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4EDC),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -256, .topOffset = 128, .rightOffset = -144, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000088, .actionId = 0x00002003},
            .textResourceId = 0x00002108},
        { /* +4FF4 hostGameCreateButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x5054), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4EDC),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 144, .topOffset = 128, .rightOffset = 256, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000084, .actionId = 0x00002004},
            .textResourceId = 0x00002109},
        { /* +5054 gameNameEdit g_UiRequiredTextEditControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x50E8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4EDC),
                .vtable = THANDOR_PTR(&g_UiRequiredTextEditControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -192, .topOffset = -89, .rightOffset = 192, .bottomOffset = -72,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
            .editStateFlags = 0x00000408, .actionId = 0x00002008, .bufferCapacityCodeUnits = 0x00000014},
        {},
        { /* +50E8 maxPlayersSlider g_UiRangeSliderControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x5150), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4EDC),
                .vtable = THANDOR_PTR(&g_UiRangeSliderControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -48, .topOffset = 72, .rightOffset = 80, .bottomOffset = 89,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
            .sliderFlags = 0x00000004, .minimumValue = 0x00000002, .maximumValue = 0x00000008, .value = 0x00000008,
            .stepValue = 0x00000001, .actionId = 0x00002007},
        { /* +5150 maxPlayersValueText g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x51AC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4EDC),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 88, .topOffset = 72, .rightOffset = 144, .bottomOffset = 89,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000014, .text = THANDOR_PTR(&g_FrontendNetworkPlayerCountTextUtf16)}, /* 5f-format: FrontendUiImage.maxPlayersValueText_fields (UI template text pointer) */
        { /* +51AC networkSpeedSlider g_UiRangeSliderControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x5214), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4EDC),
                .vtable = THANDOR_PTR(&g_UiRangeSliderControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -48, .topOffset = 48, .rightOffset = 80, .bottomOffset = 65,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
            .sliderFlags = 0x0000000C, .minimumValue = 0x00000001, .maximumValue = 0x00000007, .value = 0x00000001,
            .stepValue = 0x00000001, .actionId = 0x0000204D},
        { /* +5214 networkSpeedValueText g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x5270), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4EDC),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 88, .topOffset = 48, .rightOffset = 144, .bottomOffset = 65,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000014, .text = THANDOR_PTR(&g_FrontendNetworkSpeedLabelUtf16)}, /* 5f-format: FrontendUiImage.networkSpeedValueText_fields (UI template text pointer) */
        { /* +5270 maxPlayersLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x52CC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4EDC),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -256, .topOffset = 72, .rightOffset = -56, .bottomOffset = 89,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000006, .text = THANDOR_PTR32_BITS(0x0000210A)},
        { /* +52CC networkSpeedLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x5328), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4EDC),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -256, .topOffset = 48, .rightOffset = -56, .bottomOffset = 65,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000006, .text = THANDOR_PTR32_BITS(0x0000210D)},
        { /* +5328 gameNameLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x4EDC),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -176, .topOffset = -105, .rightOffset = 176, .bottomOffset = -89,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000008, .text = THANDOR_PTR32_BITS(0x0000210B)},
        { /* +5384 hostLobbyPage g_UiImagePanelControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x53E0), .parent = UI_TEMPLATE_LINK(0x508),
                .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .panelFlags = 0x00000045, .subresource = 0x00000002},
        { /* +53E0 hostLobbyTitle g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x543C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5384),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = -164, .rightOffset = 288, .bottomOffset = -140,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000001, .text = THANDOR_PTR32_BITS(0x00002119)},
        { /* +543C hostLobbyBackButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x549C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5384),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -256, .topOffset = 128, .rightOffset = -144, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000088, .actionId = 0x00002005},
            .textResourceId = 0x00002115},
        { /* +549C hostLobbyKickPlayerButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x54FC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5384),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -128, .topOffset = 128, .rightOffset = -16, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000088, .actionId = 0x0000200B},
            .textResourceId = 0x00002118},
        { /* +54FC hostLobbyStartButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x555C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5384),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 144, .topOffset = 128, .rightOffset = 256, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000084, .actionId = 0x00002006},
            .textResourceId = 0x00002116},
        { /* +555C hostLobbyPlayerScrollBox g_UiScrollableControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x5670), .firstChild = UI_TEMPLATE_LINK(0x55EC), .parent = UI_TEMPLATE_LINK(0x5384),
                .vtable = THANDOR_PTR(&g_UiScrollableControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -256, .topOffset = -112, .rightOffset = 256, .bottomOffset = 112,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .scrollStateFlags = 0x000004A0, .autoScrollStepX = 0x0000000F, .autoScrollStepY = 0x0000000F},
        { /* +55EC hostLobbyPlayerList g_UiListControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x555C),
                .vtable = THANDOR_PTR(&g_UiListControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
            .listStateFlags = 0x0000000A, .actionId = 0x0000200C, .columnCount = 0x00000003,
            .columns = {{.width = 0x00000180, .rowTextOffset = 0x00000018}}},
        {0x00000038, 0x00000078, 0x00000040, 0x00000094},
        { /* +5670 hostLobbyPlayerListLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5384),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -240, .topOffset = -128, .rightOffset = 240, .bottomOffset = -112,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000008, .text = THANDOR_PTR32_BITS(0x00002117)},
        { /* +56CC clientLobbyPage g_UiImagePanelControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x5728), .parent = UI_TEMPLATE_LINK(0x508),
                .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .panelFlags = 0x00000045, .subresource = 0x00000002},
        { /* +5728 clientLobbyTitle g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x5784), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x56CC),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = -164, .rightOffset = 288, .bottomOffset = -140,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000001, .text = THANDOR_PTR32_BITS(0x0000211E)},
        { /* +5784 clientLobbyLeaveButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x57E4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x56CC),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -256, .topOffset = 128, .rightOffset = -144, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000088, .actionId = 0x0000200A},
            .textResourceId = 0x0000211D},
        { /* +57E4 clientLobbyPlayerScrollBox g_UiScrollableControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x58F8), .firstChild = UI_TEMPLATE_LINK(0x5874), .parent = UI_TEMPLATE_LINK(0x56CC),
                .vtable = THANDOR_PTR(&g_UiScrollableControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -256, .topOffset = -112, .rightOffset = 256, .bottomOffset = 112,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .scrollStateFlags = 0x000004A0, .autoScrollStepX = 0x0000000F, .autoScrollStepY = 0x0000000F},
        { /* +5874 clientLobbyPlayerList g_UiListControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x57E4),
                .vtable = THANDOR_PTR(&g_UiListControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x8},
            .actionId = -1, .columnCount = 0x00000003, .columns = {{.width = 0x00000180, .rowTextOffset = 0x00000038}}},
        {0x00000038, 0x00000014, 0x00000040, 0x00000060},
        { /* +58F8 clientLobbyPlayerListLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x56CC),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -240, .topOffset = -128, .rightOffset = 240, .bottomOffset = -112,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000008, .text = THANDOR_PTR32_BITS(0x00002117)},
        /* not in the original (open-thandor): the display mode kind choice of the display settings page, the lower
           half of the left column below the adapter (renderer) group; same look as the adapter choices */
        { /* +5954 displayModeKindGroup g_UiTitledWindowControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x59A8), .parent = UI_TEMPLATE_LINK(0x2CB8),
                .vtable = THANDOR_PTR(&g_UiTitledWindowControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = -10, .rightOffset = 32, .bottomOffset = 110,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .titleTextResourceId = TEXT_ID_DISPLAY_MODE_KIND_TITLE},
        { /* +59A8 displayModeKindWindow g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x5A08), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5954),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 3, .topOffset = 3, .rightOffset = -3, .bottomOffset = 27,
                    .rightAnchorQ31 = 0x80000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
                .stateFlags = 0x00000481, .actionId = FRONTEND_ACTION_DISPLAY_MODE_KIND_WINDOW},
            .textResourceId = TEXT_ID_DISPLAY_MODE_KIND_WINDOW},
        { /* +5A08 displayModeKindBorderless g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x5A68), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5954),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 3, .topOffset = 27, .rightOffset = -3, .bottomOffset = 51,
                    .rightAnchorQ31 = 0x80000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
                .stateFlags = 0x00000481, .actionId = FRONTEND_ACTION_DISPLAY_MODE_KIND_BORDERLESS},
            .textResourceId = TEXT_ID_DISPLAY_MODE_KIND_BORDERLESS},
        { /* +5A68 displayModeKindFullscreen g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x5954),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 3, .topOffset = 51, .rightOffset = -3, .bottomOffset = 75,
                    .rightAnchorQ31 = 0x80000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
                .stateFlags = 0x00000481, .actionId = FRONTEND_ACTION_DISPLAY_MODE_KIND_FULLSCREEN},
            .textResourceId = TEXT_ID_DISPLAY_MODE_KIND_FULLSCREEN},
        /* not in the original (open-thandor): the scrollable resolution list inside displayResolutionGroup. The
           scroll frame fills the group and allows only a vertical bar at the right (0x80), auto-scroll steps as
           the lobby player lists; the panel is the scrolled content (no background, rootFlags 0), its size
           (rightOffset/bottomOffset) is set when the page opens. displayResolutionExtraOptions follow (zero
           here, copied from displayResolutionOption1 at run time). */
        { /* +5AC8 displayResolutionScrollBox g_UiScrollableControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x5B58), .parent = UI_TEMPLATE_LINK(0x308C),
                .vtable = THANDOR_PTR(&g_UiScrollableControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .scrollStateFlags = 0x00000080, .autoScrollStepX = 0x0000000F, .autoScrollStepY = 0x0000000F},
        { /* +5B58 displayResolutionRowPanel g_UiPanelControlVtable */
            .root = {
                .base = {
                    .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x30E0), .parent = UI_TEMPLATE_LINK(0x5AC8),
                    .vtable = THANDOR_PTR(&g_UiPanelControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .layoutWidth = -1, .layoutHeight = -1}}},
        {}, /* displayResolutionExtraOptions: made at run time */
        /* not in the original (open-thandor): the advanced settings page. frontendPageStack page 6 is
           displayPageStack, a two-page stack (as frontendViewModeStack) holding displaySettingsPage ("Anzeige")
           and advancedSettingsPage ("Erweitert"); the options page's fourth button opens the latter. The page
           has the options page's background (subresource 3); its boxes, radio rows and checkbox are those of the
           display settings and options pages */
        { /* +71A0 displayPageStack g_UiLayoutContainerControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x508),
                .vtable = THANDOR_PTR(&g_UiLayoutContainerControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .pageCount = 0x00000002, .pages = {UI_TEMPLATE_LINK_BITS(0x2CB8), UI_TEMPLATE_LINK_BITS(0x7258)}},
        { /* +71F8 advancedSettingsButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x2854), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x261C),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -256, .topOffset = 48, .rightOffset = -144, .bottomOffset = 72,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000080, .actionId = FRONTEND_ACTION_OPEN_ADVANCED_SETTINGS},
            .textResourceId = TEXT_ID_OPTIONS_ADVANCED_BUTTON},
        { /* +7258 advancedSettingsPage g_UiImagePanelControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x72B4), .parent = UI_TEMPLATE_LINK(0x71A0),
                .vtable = THANDOR_PTR(&g_UiImagePanelControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .panelFlags = 0x00000045, .subresource = 0x00000003},
        { /* +72B4 advancedSettingsBackButton g_UiFramedTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x7314), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x7258),
                    .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = -256, .topOffset = 128, .rightOffset = -144, .bottomOffset = 152,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x0000008C, .actionId = 0x00002010},
            .textResourceId = 0x00002129},
        { /* +7314 advancedSettingsTitle g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x7370), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x7258),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = -164, .rightOffset = 288, .bottomOffset = -140,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .labelFlags = 0x00000001, .text = THANDOR_PTR32_BITS(TEXT_ID_ADVANCED_SETTINGS_TITLE)},
        { /* +7370 advancedEdgesGroup g_UiTitledWindowControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x7484), .firstChild = UI_TEMPLATE_LINK(0x73C4), .parent = UI_TEMPLATE_LINK(0x7258),
                .vtable = THANDOR_PTR(&g_UiTitledWindowControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = -142, .rightOffset = 32, .bottomOffset = -80,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .titleTextResourceId = TEXT_ID_ADVANCED_EDGES_TITLE},
        { /* +73C4 advancedEdgesSmooth g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x7424), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x7370),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 3, .topOffset = 3, .rightOffset = -3, .bottomOffset = 27,
                    .rightAnchorQ31 = 0x80000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000481, .actionId = FRONTEND_ACTION_ADVANCED_EDGES},
            .textResourceId = TEXT_ID_ADVANCED_EDGES_SMOOTH},
        { /* +7424 advancedEdgesExact g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x7370),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 3, .topOffset = 27, .rightOffset = -3, .bottomOffset = 51,
                    .rightAnchorQ31 = 0x80000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000481, .actionId = FRONTEND_ACTION_ADVANCED_EDGES},
            .textResourceId = TEXT_ID_ADVANCED_EDGES_EXACT},
        { /* +7484 advancedUiScaleGroup g_UiTitledWindowControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x7658), .firstChild = UI_TEMPLATE_LINK(0x74D8), .parent = UI_TEMPLATE_LINK(0x7258),
                .vtable = THANDOR_PTR(&g_UiTitledWindowControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = -58, .rightOffset = 32, .bottomOffset = 52,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .titleTextResourceId = TEXT_ID_ADVANCED_UI_SCALE_TITLE},
        { /* +74D8 advancedUiScaleAuto g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x7538), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x7484),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 3, .topOffset = 3, .rightOffset = -3, .bottomOffset = 27,
                    .rightAnchorQ31 = 0x80000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000481, .actionId = FRONTEND_ACTION_ADVANCED_UI_SCALE},
            .textResourceId = TEXT_ID_ADVANCED_UI_SCALE_AUTO},
        { /* +7538 advancedUiScale1 g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x7598), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x7484),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 3, .topOffset = 27, .rightOffset = -3, .bottomOffset = 51,
                    .rightAnchorQ31 = 0x80000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000481, .actionId = FRONTEND_ACTION_ADVANCED_UI_SCALE},
            .textResourceId = TEXT_ID_ADVANCED_UI_SCALE_1},
        { /* +7598 advancedUiScale2 g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x75F8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x7484),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 3, .topOffset = 51, .rightOffset = -3, .bottomOffset = 75,
                    .rightAnchorQ31 = 0x80000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000481, .actionId = FRONTEND_ACTION_ADVANCED_UI_SCALE},
            .textResourceId = TEXT_ID_ADVANCED_UI_SCALE_1 + 1},
        { /* +75F8 advancedUiScale3 g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x7484),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 3, .topOffset = 75, .rightOffset = -3, .bottomOffset = 99,
                    .rightAnchorQ31 = 0x80000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000481, .actionId = FRONTEND_ACTION_ADVANCED_UI_SCALE},
            .textResourceId = TEXT_ID_ADVANCED_UI_SCALE_1 + 2},
        { /* +7658 advancedFrameLimitGroup g_UiTitledWindowControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_LINK(0x782C), .firstChild = UI_TEMPLATE_LINK(0x76AC), .parent = UI_TEMPLATE_LINK(0x7258),
                .vtable = THANDOR_PTR(&g_UiTitledWindowControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = 48, .topOffset = -142, .rightOffset = 288, .bottomOffset = -32,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .titleTextResourceId = TEXT_ID_ADVANCED_FRAME_LIMIT_TITLE},
        { /* +76AC advancedFrameLimitOff g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x770C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x7658),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 3, .topOffset = 3, .rightOffset = -3, .bottomOffset = 27,
                    .rightAnchorQ31 = 0x80000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000481, .actionId = FRONTEND_ACTION_ADVANCED_FRAME_LIMIT},
            .textResourceId = TEXT_ID_ADVANCED_FRAME_LIMIT_OFF},
        { /* +770C advancedFrameLimit60 g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x776C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x7658),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 3, .topOffset = 27, .rightOffset = -3, .bottomOffset = 51,
                    .rightAnchorQ31 = 0x80000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000481, .actionId = FRONTEND_ACTION_ADVANCED_FRAME_LIMIT},
            .textResourceId = TEXT_ID_ADVANCED_FRAME_LIMIT_OFF + 1},
        { /* +776C advancedFrameLimit120 g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x77CC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x7658),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 3, .topOffset = 51, .rightOffset = -3, .bottomOffset = 75,
                    .rightAnchorQ31 = 0x80000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000481, .actionId = FRONTEND_ACTION_ADVANCED_FRAME_LIMIT},
            .textResourceId = TEXT_ID_ADVANCED_FRAME_LIMIT_OFF + 2},
        { /* +77CC advancedFrameLimit144 g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x7658),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 3, .topOffset = 75, .rightOffset = -3, .bottomOffset = 99,
                    .rightAnchorQ31 = 0x80000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000481, .actionId = FRONTEND_ACTION_ADVANCED_FRAME_LIMIT},
            .textResourceId = TEXT_ID_ADVANCED_FRAME_LIMIT_OFF + 3},
        { /* +782C advancedVsyncCheckbox g_UiTextButtonControlVtable */
            .selectable = {
                .base = {
                    .nextSibling = UI_TEMPLATE_LINK(0x788C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x7258),
                    .vtable = THANDOR_PTR(&g_UiTextButtonControlVtable),
                    .left = -1, .top = -1, .right = -1, .bottom = -1,
                    .leftOffset = 53, .topOffset = -18, .rightOffset = 288, .bottomOffset = 6,
                    .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                    .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
                .stateFlags = 0x00000091, .actionId = FRONTEND_ACTION_ADVANCED_VSYNC},
            .textResourceId = TEXT_ID_ADVANCED_VSYNC},
        { /* +788C advancedNoteLabel g_UiFocusProxyControlVtable */
            .base = {
                .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x7258),
                .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
                .left = -1, .top = -1, .right = -1, .bottom = -1,
                .leftOffset = -288, .topOffset = 68, .rightOffset = 288, .bottomOffset = 92,
                .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
                .layoutWidth = -1, .layoutHeight = -1},
            .text = THANDOR_PTR32_BITS(TEXT_ID_ADVANCED_NOTE_SOFTWARE)},
};
