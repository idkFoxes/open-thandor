/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_TYPES_H
#define THANDOR_UI_FRONTEND_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/core/memory/types.h>
#include <thandor/core/types.h>
#include <thandor/gameplay/session/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/network/protocol/types.h>
#include <thandor/ui/controls/types.h>

typedef union UiCommandPayloadTextBatch48 UiCommandPayloadTextBatch48, *PUiCommandPayloadTextBatch48;
typedef struct UiCommandPayloadTriple UiCommandPayloadTriple, *PUiCommandPayloadTriple;
typedef union FrontendUiScratch FrontendUiScratch, *PFrontendUiScratch;
typedef struct FrontendDisplayModeCandidateValues FrontendDisplayModeCandidateValues, *PFrontendDisplayModeCandidateValues;
typedef struct FrontendTaskAssignmentFactionTexts FrontendTaskAssignmentFactionTexts, *PFrontendTaskAssignmentFactionTexts;
typedef struct FrontendDisplayModeEnumerationState FrontendDisplayModeEnumerationState, *PFrontendDisplayModeEnumerationState;
typedef struct FrontendTaskAssignmentFactionTextRow FrontendTaskAssignmentFactionTextRow, *PFrontendTaskAssignmentFactionTextRow;
typedef union FrontendDisplayModeCandidates FrontendDisplayModeCandidates, *PFrontendDisplayModeCandidates;
typedef struct FrontendDisplayModeSelection FrontendDisplayModeSelection, *PFrontendDisplayModeSelection;
typedef struct FrontendPersistentSettingsPage FrontendPersistentSettingsPage, *PFrontendPersistentSettingsPage;
typedef struct FrontendNetworkSetupPageState FrontendNetworkSetupPageState, *PFrontendNetworkSetupPageState;
typedef struct FrontendRootPageState FrontendRootPageState, *PFrontendRootPageState;
typedef struct FrontendSessionDiscoveryRecord FrontendSessionDiscoveryRecord, *PFrontendSessionDiscoveryRecord;
typedef struct RuntimeModelClassPriorityTable24 RuntimeModelClassPriorityTable24, *PRuntimeModelClassPriorityTable24;
typedef struct FrontendModelPointerHitContext FrontendModelPointerHitContext, *PFrontendModelPointerHitContext;
typedef struct FrontendResultsRowMetrics FrontendResultsRowMetrics, *PFrontendResultsRowMetrics;
typedef struct UiSettingsValueControl UiSettingsValueControl, *PUiSettingsValueControl;
typedef struct SoftwareMaskRuntimeView SoftwareMaskRuntimeView, *PSoftwareMaskRuntimeView;
typedef struct FrontendDisplaySettingsPageOptionState FrontendDisplaySettingsPageOptionState, *PFrontendDisplaySettingsPageOptionState;
typedef struct FrontendDisplayAdapterRows FrontendDisplayAdapterRows, *PFrontendDisplayAdapterRows;
typedef struct FrontendDisplayResolutionRows FrontendDisplayResolutionRows, *PFrontendDisplayResolutionRows;
typedef struct FrontendDisplayColorDepthRows FrontendDisplayColorDepthRows, *PFrontendDisplayColorDepthRows;
typedef struct FrontendDisplayAdapterOptionRow FrontendDisplayAdapterOptionRow, *PFrontendDisplayAdapterOptionRow;
typedef struct FrontendDisplayResolutionOptionRow FrontendDisplayResolutionOptionRow, *PFrontendDisplayResolutionOptionRow;
typedef struct FrontendDisplayColorDepthOptionRow FrontendDisplayColorDepthOptionRow, *PFrontendDisplayColorDepthOptionRow;
typedef struct UiSelectableOptionRow60 UiSelectableOptionRow60, *PUiSelectableOptionRow60;
typedef struct FrontendTextureResolutionRows FrontendTextureResolutionRows, *PFrontendTextureResolutionRows;
typedef struct UiSelectableOptionRow68 UiSelectableOptionRow68, *PUiSelectableOptionRow68;
typedef struct FrontendShadingResolutionRows FrontendShadingResolutionRows, *PFrontendShadingResolutionRows;
typedef struct FrontendGraphicsRuntimeSettingsPageState FrontendGraphicsRuntimeSettingsPageState, *PFrontendGraphicsRuntimeSettingsPageState;
typedef struct FrontendUiActionHandlerPage20Prefix FrontendUiActionHandlerPage20Prefix, *PFrontendUiActionHandlerPage20Prefix;
typedef struct FrontendTaskAssignmentControlOffsetRow FrontendTaskAssignmentControlOffsetRow, *PFrontendTaskAssignmentControlOffsetRow;
typedef struct FrontendTaskAssignmentControlOffsetTables FrontendTaskAssignmentControlOffsetTables, *PFrontendTaskAssignmentControlOffsetTables;
typedef struct FrontendPlayerFactionAssignmentState FrontendPlayerFactionAssignmentState, *PFrontendPlayerFactionAssignmentState;
typedef struct FrontendPlayerNameUtf16 FrontendPlayerNameUtf16, *PFrontendPlayerNameUtf16;
typedef struct FrontendNetworkBackendCommonPrefix FrontendNetworkBackendCommonPrefix, *PFrontendNetworkBackendCommonPrefix;
typedef struct FrontendNetworkSettingsPageCommonPrefix FrontendNetworkSettingsPageCommonPrefix, *PFrontendNetworkSettingsPageCommonPrefix;
typedef union FrontendNetworkSettingsControlView FrontendNetworkSettingsControlView, *PFrontendNetworkSettingsControlView;
typedef struct FrontendNetworkSettingsPageCommonState FrontendNetworkSettingsPageCommonState, *PFrontendNetworkSettingsPageCommonState;
typedef struct FrontendNetworkSettingsUiNodeView FrontendNetworkSettingsUiNodeView, *PFrontendNetworkSettingsUiNodeView;
typedef struct FrontendNetworkSettingsTextEditView FrontendNetworkSettingsTextEditView, *PFrontendNetworkSettingsTextEditView;
typedef struct FrontendNetworkSettingsPointerListView FrontendNetworkSettingsPointerListView, *PFrontendNetworkSettingsPointerListView;
typedef struct FrontendNetworkSettingsPrimaryPageStackView FrontendNetworkSettingsPrimaryPageStackView, *PFrontendNetworkSettingsPrimaryPageStackView;
typedef struct FrontendNetworkSettingsSecondaryPageStackView FrontendNetworkSettingsSecondaryPageStackView, *PFrontendNetworkSettingsSecondaryPageStackView;
typedef struct FrontendNetworkSettingsGeneratedNameBufferView FrontendNetworkSettingsGeneratedNameBufferView, *PFrontendNetworkSettingsGeneratedNameBufferView;
typedef struct FrontendNetworkBackendModePageState FrontendNetworkBackendModePageState, *PFrontendNetworkBackendModePageState;
typedef union FrontendNetworkBackendModeOverlap FrontendNetworkBackendModeOverlap, *PFrontendNetworkBackendModeOverlap;
typedef struct FrontendNetworkGeneratedNamePrefix FrontendNetworkGeneratedNamePrefix, *PFrontendNetworkGeneratedNamePrefix;
typedef struct FrontendPlayerRuntimeRecord FrontendPlayerRuntimeRecord, *PFrontendPlayerRuntimeRecord;
typedef struct FrontendCreditsUiStateView FrontendCreditsUiStateView, *PFrontendCreditsUiStateView;
typedef struct FrontendPointerHintControl FrontendPointerHintControl, *PFrontendPointerHintControl;
typedef struct FrontendPointerSceneRuntimeView FrontendPointerSceneRuntimeView, *PFrontendPointerSceneRuntimeView;
typedef struct FrontendScenarioSelectionPageView FrontendScenarioSelectionPageView, *PFrontendScenarioSelectionPageView;
typedef struct FrontendRootResourceSlots FrontendRootResourceSlots, *PFrontendRootResourceSlots;
typedef struct FrontendTaskAssignmentPageInitView FrontendTaskAssignmentPageInitView, *PFrontendTaskAssignmentPageInitView;
typedef struct FrontendLoadedLevelPathOffsets FrontendLoadedLevelPathOffsets, *PFrontendLoadedLevelPathOffsets;
typedef struct FrontendLoadedLevelHeader FrontendLoadedLevelHeader, *PFrontendLoadedLevelHeader;
typedef struct FrontendLoadedLevelAsset FrontendLoadedLevelAsset, *PFrontendLoadedLevelAsset;
typedef struct FrontendNetworkListsRuntimeView FrontendNetworkListsRuntimeView, *PFrontendNetworkListsRuntimeView;
typedef struct FrontendResultsFactionWeightPair FrontendResultsFactionWeightPair, *PFrontendResultsFactionWeightPair;
typedef struct FrontendResultsColumnSequenceControl FrontendResultsColumnSequenceControl, *PFrontendResultsColumnSequenceControl;
typedef struct FrontendResultsEightColumnTemplate FrontendResultsEightColumnTemplate, *PFrontendResultsEightColumnTemplate;
typedef struct ScenarioCatalogDisplayRecord ScenarioCatalogDisplayRecord, *PScenarioCatalogDisplayRecord;
typedef struct FieldGridAsset FieldGridAsset;
typedef struct GameEntityRuntime GameEntityRuntime;
typedef struct ModelRuntimeNode ModelRuntimeNode;
typedef struct WorldRuntimeContext WorldRuntimeContext;

using TerrainGridMaskIndex = int;

/* The three payload dwords of a queued command, in memory order (see UiCommandQueueRecord). */
struct UiCommandPayloadTriple {
    uint32_t payload3;
    uint32_t payload2;
    uint32_t payload1;
};

union UiCommandPayloadTextBatch48 {
    uint8_t textBytes[48]; 
    struct UiCommandPayloadTriple triples[4];
};

using FrontendDisplayAdapterIndex = int;

using FrontendDisplayDimensionPixels = uint32_t;

using FrontendColorDepthBits = uint32_t;

struct FrontendTaskAssignmentFactionTextRow {
    uint16_t textUtf16[40]; 
};

struct FrontendDisplayModeSelection {
    FrontendDisplayAdapterIndex adapterIndex; 
    FrontendDisplayDimensionPixels width; 
    FrontendDisplayDimensionPixels height; 
    FrontendColorDepthBits bitsPerPixel; 
};

union FrontendDisplayModeCandidates {
    uint32_t bitsPerPixelCandidates[4];
    uint32_t packedResolutionCandidates[10];
    uint32_t rawDwords[10]; 
};

struct FrontendDisplayModeEnumerationState {
    union FrontendDisplayModeCandidates candidates; 
    uint8_t reserved0028_003F[24]; 
    struct FrontendDisplayModeSelection persistentSelection; 
    uint8_t reserved0050_027F[560]; 
};

struct FrontendDisplayModeCandidateValues {
    uint32_t candidateValues[10]; 
};

struct FrontendTaskAssignmentFactionTexts {
    struct FrontendTaskAssignmentFactionTextRow rows[8]; 
};

union FrontendUiScratch {
    struct FrontendDisplayModeCandidateValues displayModeScratch; 
    struct FrontendTaskAssignmentFactionTexts taskAssignmentText; 
    struct FrontendDisplayModeEnumerationState displayEnumeration; 
    uint8_t raw[640]; 
};

enum {
    FRONTEND_COMMAND_SYNC_CLEAR=0,
    FRONTEND_COMMAND_SYNC_PENDING=1
};
using FrontendCommandSyncPendingState = int;

enum {
    RUNTIME_MODEL_CLASS_PRIORITY_LOW=0,
    RUNTIME_MODEL_CLASS_PRIORITY_MEDIUM=1,
    RUNTIME_MODEL_CLASS_PRIORITY_HIGH=2
};
using RuntimeModelClassPriority = int;

using FrontendSelectionTransferModeFlags = uint32_t;

using UiListRowIndex = uint32_t;

using FrontendMessageValueA = uint32_t;

using FrontendMessageValueB = uint32_t;

using FrontendMessageValueC = uint32_t;

using RomVisibilityFrontendValue = uint32_t;

using FrontendReadyFlagMask = uint32_t;

using FrontendRoleStateFlags = uint32_t;

using SoftwareMaskRadiusStep = int;

using FrontendResultsFactionFieldByteOffset = uint32_t;

using FrontendReturnCallbackContext32 = uint32_t;

using FrontendScenarioAvailabilityMask2 = uint32_t;

using FrontendPackedTextCommandState = uint32_t;

typedef struct UiNodeBase *FrontendPersistentSettingsPageSourceNodePtr; /* interior pointer: points at FrontendPersistentSettingsPage.sourceNode; the containing FrontendPersistentSettingsPage is found by subtracting the field offset */

struct FrontendPersistentSettingsPage {
    struct UiNodeBase pageRoot; 
    uint8_t reserved004C_019B[336]; 
    struct UiPageStackControl settingsPageStack; 
    uint8_t reserved01F0_2487[8856]; 
    struct UiNodeBase sourceNode; 
    uint8_t reserved24D4_3BBB[5864]; 
    struct UiSelectableControl musicEnabledControl; 
    uint8_t reserved3C10_3C1B[12]; 
    struct UiSelectableControl soundEffectsEnabledControl; 
    uint8_t reserved3C70_3C7B[12]; 
    struct UiSelectableControl reverseStereoControl; 
    uint8_t reserved3CD0_3DEF[288]; 
    struct UiNumericTextControl soundEffectsGainControl; 
    uint8_t reserved3E84_3F6B[232]; 
    struct UiNumericTextControl movieDefaultAudioGainControl; 
    uint8_t reserved4000_40E7[232]; 
    struct UiNumericTextControl musicGainControl; 
};

using SoftwareMaskThresholdStep = int;

using FrontendTextCommandValue0 = uint32_t;

using FrontendTextCommandValue2 = uint32_t;

using FrontendTextCommandValue1 = uint32_t;

using UiBooleanState32 = uint32_t;

using FrontendScenarioAvailabilityMask0 = uint32_t;

using FrontendScenarioAvailabilityMask1 = uint32_t;

using FrontendCallbackArgument5 = uint32_t;

typedef struct UiPointerListControl *FrontendNetworkSetupPageBackendListPtr; /* interior pointer: points at FrontendNetworkSetupPageState.backendList; the containing FrontendNetworkSetupPageState is found by subtracting the field offset */

struct FrontendNetworkSetupPageState {
    struct UiNodeBase rootNode; 
    uint8_t reserved004C_036B[800]; 
    struct UiNodeBase compactLayoutControl; 
    uint8_t reserved03B8_0507[336]; 
    struct UiPageStackControl primaryPageStack; 
    uint8_t reserved055C_4A6F[17684]; 
    struct UiPointerListControl backendList; 
    uint8_t reserved4AD4_4B67[148]; 
    struct UiPointerListControl sessionList; 
};

using UiPixelMetric = int;

using FrontendScenarioSelectionControlAddress32 = intptr_t; /* address of the gameSelectStartButton node (5f) */

using TechnologyIndexOrRestoreCode = uint32_t;

using FrontendIndexedSelectionArgument = uint32_t;

using FrontendBooleanState32 = int;

using FrontendPlayerRuntimeBlockCount = uint32_t;

struct FrontendRootPageState {
    struct UiNodeBase rootNode; 
    uint8_t reserved004C_036B[800]; 
    struct UiNodeBase compactLayoutControl; 
    uint8_t reserved03B8_0507[336]; 
    struct UiPageStackControl primaryPageStack; 
    uint8_t reserved055C_2677[8476]; 
    struct UiNodeBase returnToMainActionControl; 
};

struct FrontendSessionDiscoveryRecord {
    struct FrontendPacket50001SessionAdvertisement advertisement; 
    struct UiTransferEndpointDescriptor senderEndpoint; 
};

struct RuntimeModelClassPriorityTable24 {
    RuntimeModelClassPriority modelClass00Priority; 
    RuntimeModelClassPriority modelClass01Priority; 
    RuntimeModelClassPriority modelClass02Priority; 
    RuntimeModelClassPriority modelClass03Priority; 
    RuntimeModelClassPriority modelClass04Priority; 
    RuntimeModelClassPriority modelClass05Priority; 
    RuntimeModelClassPriority modelClass06Priority; 
    RuntimeModelClassPriority modelClass07Priority; 
    RuntimeModelClassPriority modelClass08Priority; 
    RuntimeModelClassPriority modelClass09Priority; 
    RuntimeModelClassPriority modelClass10Priority; 
    RuntimeModelClassPriority modelClass11Priority; 
    RuntimeModelClassPriority modelClass12Priority; 
    RuntimeModelClassPriority modelClass13Priority; 
    RuntimeModelClassPriority modelClass14Priority; 
    RuntimeModelClassPriority modelClass15Priority; 
    RuntimeModelClassPriority modelClass16Priority; 
    RuntimeModelClassPriority modelClass17Priority; 
    RuntimeModelClassPriority modelClass18Priority; 
    RuntimeModelClassPriority modelClass19Priority; 
    RuntimeModelClassPriority modelClass20Priority; 
    RuntimeModelClassPriority modelClass21Priority; 
    RuntimeModelClassPriority modelClass22Priority; 
    RuntimeModelClassPriority modelClass23Priority; 
};

struct FrontendModelPointerHitContext {
    struct UiNodeBase base; // Accepted frontend UI-node prefix.
    FrontendModelPointerContextFlags contextFlags; // Directly observed model-pointer selection and action-routing flags.
    uint8_t reserved50_5F[16]; // Unresolved.
    Q12 hitReferenceWorldXQ12; // Subtracted from model world-transform X when computing the hit metric.
    Q12 hitReferenceWorldYQ12; // Subtracted from model world-transform Y when computing the hit metric.
    Q12 hitReferenceWorldZQ12; // Subtracted from model world-transform Z when computing the hit metric.
    uint8_t reserved6C_D7[108]; // Unresolved.
    Ptr32<struct ModelRuntimeNode> candidateModelListHead; // Head traversed through ModelRuntimeNode.common.nextNode.
    uint32_t activePlayerRuntimeId; // WorldRuntimeContext.selection.activePlayerRuntimeId of the in-game world view; not consumed by the frontend selection paths.
    Ptr32<struct ModelRuntimeNode> selectedModelNode; // Model-node half of the model selector's result.
    int selectedHitMetric; // Hit-metric half of the model selector's result.
    uint32_t surfaceHitWorldX; // Terrain point under the cursor: world X interpolated by the terrain triangle pick; passed to the pointer callbacks.
    uint32_t surfaceHitWorldY; // Terrain point under the cursor: world Y interpolated by the terrain triangle pick; passed to the pointer callbacks.
    uint32_t surfaceHitDepth; // View depth of the terrain hit (WORLD_POINTER_NO_HIT when none); passed to the pointer callbacks.
    uint8_t reservedF4_FF[12]; // reserved bytes before keyboard fallback callback
    Ptr32<Bool8 (UiKeyboardStateMask, UiActionId, struct UiRootNode *)> keyboardFallback; // root keyboard fallback callback; the bool result is the status
    Ptr32<uint32_t (uint32_t, uint32_t, uint32_t, int, struct ModelRuntimeNode *, struct FrontendModelPointerHitContext *)> hoverCursorCallback; // Pointer move with no button held: returns the cursor frame (surface hit depth/Y/X, hit metric, hit model, context).
    Ptr32<uint32_t (uint32_t, uint32_t, uint32_t, int, struct ModelRuntimeNode *, struct FrontendModelPointerHitContext *)> heldButtonCursorCallback; // Pointer move while a non-right button is held (ROUTE_TO_SECONDARY_CALLBACK): returns the cursor frame.
    Ptr32<uint32_t (uint32_t, uint32_t, uint32_t, int, struct ModelRuntimeNode *, struct FrontendModelPointerHitContext *)> buttonPressCallback; // Non-right button press (FrontendModelPointerContext_NonRightPress).
    Ptr32<uint32_t (uint32_t, uint32_t, uint32_t, int, struct ModelRuntimeNode *, struct FrontendModelPointerHitContext *)> buttonDragCallback; // Non-right button drag (FrontendModelPointerContext_NonRightDrag).
    Ptr32<uint32_t (uint32_t, uint32_t, uint32_t, int, struct ModelRuntimeNode *, struct FrontendModelPointerHitContext *)> buttonReleaseCallback; // Non-right button release (FrontendModelPointerContext_NonRightRelease).
};

struct FrontendResultsRowMetrics {
    uint8_t reserved00_5B[92]; 
    UiPixelMetric headerBaselineOffsetPixels; 
    UiPixelMetric rowAdvancePixels; 
};

struct UiSettingsValueControl {
    struct UiNodeBase base; 
    uint8_t reserved4C_57[12]; 
    uint32_t boundValue; 
    uint8_t reserved5C_67[12]; 
};

struct SoftwareMaskRuntimeView {
    uint8_t unresolved00_4F[80]; 
    Ptr32<struct GraphicsTextureSourceAsset> textureSource; 
    uint32_t outgoingSubresource; /* UiSoftwareTexturePreviewControl.outgoingSubresource */
    uint32_t incomingSubresource; /* UiSoftwareTexturePreviewControl.incomingSubresource */
    uint32_t actionId; /* UiSoftwareTexturePreviewControl.actionId */
    Ptr32<uint8_t> maskPixels;
    UPtr32 blendedSourcePixels; /* second work buffer (UiSoftwareTexturePreviewControl.blendedSourcePixels) */
    int tickCounter; 
};

struct FrontendDisplayResolutionOptionRow {
    FrontendDisplayDimensionPixels width; 
    FrontendDisplayDimensionPixels height; 
    uint8_t reserved0008_0067[96]; 
};

struct FrontendDisplayResolutionRows {
    struct FrontendDisplayResolutionOptionRow rows[10]; 
};

struct FrontendDisplayAdapterOptionRow {
    Ptr32<uint16_t> adapterDescriptionUtf16; 
    Ptr32<uint16_t> deviceNameUtf16; 
    uint8_t reserved0008_0067[96]; 
};

struct FrontendDisplayAdapterRows {
    struct FrontendDisplayAdapterOptionRow rows[5]; 
};

struct FrontendDisplayColorDepthOptionRow {
    uint32_t bitsPerPixel; 
    uint8_t reserved0004_0067[100]; 
};

/* the colour depth choices (displayColorDepthOption1..4), no longer filled: 32-bit colour only */
struct FrontendDisplayColorDepthRows {
    struct FrontendDisplayColorDepthOptionRow rows[4]; 
};

struct FrontendDisplaySettingsPageOptionState {
    uint8_t reserved0000_07AF[1968]; 
    struct FrontendDisplayAdapterRows adapterRows; 
    uint8_t reserved09B8_0A0B[84]; 
    struct FrontendDisplayResolutionRows resolutionRows; 
    uint8_t reserved0E1C_0E6F[84]; 
    struct FrontendDisplayColorDepthRows colorDepthRows; 
};

struct UiSelectableOptionRow60 {
    struct UiSelectableControl control; 
    uint8_t reserved0054_005F[12]; 
};

struct FrontendTextureResolutionRows {
    struct UiSelectableOptionRow60 rows[3]; 
};

struct UiSelectableOptionRow68 {
    struct UiSelectableControl control; 
    uint8_t reserved0054_0067[20]; 
};

struct FrontendShadingResolutionRows {
    struct UiSelectableOptionRow68 rows[6]; 
};

struct FrontendGraphicsRuntimeSettingsPageState {
    struct UiNodeBase base; 
    uint8_t reserved4C_1067[4124]; 
    struct UiSelectableControl shadingEnabledControl; 
    uint8_t reserved10BC_111B[96]; 
    struct FrontendShadingResolutionRows shadingResolutionRows; 
    uint8_t reserved138C_14F7[364]; 
    uint32_t polygonResolutionLodThresholdQ8; 
    uint8_t reserved14FC_155B[96]; 
    struct FrontendTextureResolutionRows textureResolutionRows; 
};

struct FrontendUiActionHandlerPage20Prefix {
    Ptr32<void (void *)> handlers00_54[85]; // Generic queued action handlers for action IDs 0x2000-0x2054.
    Ptr32<void (uint32_t, uint32_t, uint32_t, uint32_t)> scenarioCatalogRebuildCallbacks[3]; // Indexed 3-way save/level/campaign record-list rebuild callbacks, picked by a selector 0..2.
    Ptr32<void (void *)> handlers58_5A[3]; // Not in the original: action IDs 0x2058-0x205A (the display mode kind choices).
    Ptr32<void (void *)> handlers5B_5F[5]; // Not in the original: action IDs 0x205B-0x205F (the advanced settings page).
};

struct FrontendTaskAssignmentControlOffsetRow {
    uint32_t offsets[7]; // Node offsets (from the frontend root) of the controls of faction rows 1..7: offsets[row - 1].
};

/* Stored right after the 32 entries of g_FrontendPlayerRuntimeRecordPointers32. The original code indexes
   each row table with the row number (1..7) from a base 4 bytes lower, which once made these tables look like
   they started there with an unused entry 0. */
struct FrontendTaskAssignmentControlOffsetTables {
    struct FrontendTaskAssignmentControlOffsetRow assignmentControls;
    struct FrontendTaskAssignmentControlOffsetRow playerControls;
    struct FrontendTaskAssignmentControlOffsetRow factionControls;
    struct FrontendTaskAssignmentControlOffsetRow selectionRows;
    struct FrontendTaskAssignmentControlOffsetRow statusRows;
};

struct FrontendPlayerFactionAssignmentState {
    FrontendReadyOrWaitState readyOrWaitState; 
    FrontendFactionAssignmentIndex factionAssignmentIndex; 
    FrontendConsensusValue consensusValue; 
    FrontendRoleStateFlags roleStateFlags; 
};

struct FrontendPlayerNameUtf16 {
    uint16_t textUtf16[20]; 
};

struct FrontendNetworkSettingsPageCommonPrefix {
    Ptr32<struct UiNodeBase> nextSibling; 
    Ptr32<struct UiNodeBase> firstChild; 
    Ptr32<struct UiNodeBase> parent; 
    Ptr32<struct UiNodeVtable> vtable; 
    int32_t left; 
    int32_t top; 
    int32_t right; 
    int32_t bottom; 
    int32_t leftOffset; 
    int32_t topOffset; 
    int32_t rightOffset; 
    int32_t bottomOffset; 
    UiAnchorFractionQ31 leftAnchorQ31; 
    UiAnchorFractionQ31 topAnchorQ31; 
    UiAnchorFractionQ31 rightAnchorQ31; 
    UiAnchorFractionQ31 bottomAnchorQ31; 
    int32_t layoutWidth; 
};

struct FrontendNetworkBackendCommonPrefix {
    struct FrontendNetworkSettingsPageCommonPrefix commonPrefix; 
};

struct FrontendNetworkSettingsPrimaryPageStackView {
    uint8_t reserved0000_0067[104]; 
    struct UiPageStackControl pageStack; 
    uint8_t reserved00BC_024F[404]; 
};

struct FrontendNetworkSettingsGeneratedNameBufferView {
    uint8_t reserved0000_0177[376]; 
    uint16_t generatedNameUtf16[40]; 
    uint8_t reserved01C8_024F[136]; 
};

struct FrontendNetworkSettingsUiNodeView {
    struct UiNodeBase base; 
    uint8_t reserved004C_024F[516]; 
};

struct FrontendNetworkGeneratedNamePrefix {
    uint8_t backendModeMarker; 
    uint8_t reserved01_05[5]; 
    char generatedName[40]; 
};

union FrontendNetworkBackendModeOverlap {
    struct FrontendNetworkBackendCommonPrefix commonPrefix; 
    struct FrontendNetworkGeneratedNamePrefix generatedNamePrefix; 
    uint8_t raw[68]; 
};

struct FrontendNetworkBackendModePageState {
    union FrontendNetworkBackendModeOverlap backendModeOverlap; 
    uint8_t reserved44_24F[524]; 
};

struct FrontendNetworkSettingsTextEditView {
    uint8_t reserved0000_0043[68]; 
    struct UiTextEditControl textEdit; 
    uint8_t reserved00C4_024F[396]; 
};

struct FrontendNetworkSettingsPointerListView {
    uint8_t reserved0000_005B[92]; 
    struct UiPointerListControl pointerList; 
    uint8_t reserved00C0_024F[400]; 
};

struct FrontendNetworkSettingsPageCommonState {
    struct FrontendNetworkSettingsPageCommonPrefix commonPrefix; 
    uint8_t reserved0044_024F[524]; 
};

struct FrontendNetworkSettingsSecondaryPageStackView {
    uint8_t reserved0000_0147[328]; 
    struct UiPageStackControl pageStack; 
    uint8_t reserved019C_024F[180]; 
};

union FrontendNetworkSettingsControlView {
    uint8_t raw[592]; 
    struct FrontendNetworkSettingsPageCommonState commonState; 
    struct FrontendNetworkSettingsUiNodeView nodeView; 
    struct FrontendNetworkSettingsTextEditView textEditView; 
    struct FrontendNetworkSettingsPointerListView pointerListView; 
    struct FrontendNetworkSettingsPrimaryPageStackView primaryPageStackView; 
    struct FrontendNetworkSettingsSecondaryPageStackView secondaryPageStackView; 
    struct FrontendNetworkSettingsGeneratedNameBufferView generatedNameBufferView; 
    struct FrontendNetworkBackendModePageState backendModeState; 
};

struct FrontendPlayerRuntimeRecord {
    uint32_t reserved00; // Never accessed by name; only the start of whole-record dword copies.
    UiTransferSequenceToken peerSequenceToken; // Peer/session sequence token.
    uint32_t reserved08; // Never accessed.
    uint32_t reserved0C; // Never accessed.
    FrontendHeartbeatTickCount heartbeatExpiryTicks; // Heartbeat expiry countdown.
    FrontendPlayerRuntimeId playerRuntimeId; // Stable player runtime identifier.
    struct FrontendPlayerNameUtf16 playerName; // Typed UTF-16 player name/descriptor text.
    struct UiTransferEndpointDescriptor endpoint; // Remote IPv4 endpoint; address is compared at record offset +0x44.
    FrontendCommandSyncPendingState commandSyncPending; // Command synchronization pending state.
    struct FrontendPlayerFactionAssignmentState factionAssignment; // Exact ready/wait, faction assignment, consensus, and role-state subrecord. commandSyncPending at +0x50 remains separate.
    uint32_t colourCycleFlags; /* +0x64 toggled by FRONTEND_COMMAND_XOR_PLAYER_STATE; bit 0 adds an eighth faction colour */
    FrontendSnapshotTransferFlags snapshotTransferFlags; // Snapshot transfer-state flags.
    FrontendSnapshotChunkByteOffset snapshotChunkOffset; // Current snapshot chunk offset.
    uint32_t transferProgressBytes; /* +0x70 mailbox transfer: end offset of the chunk last requested, 0x7FFFFFFF = done */
    FrontendCapabilityFlags capabilityFlags; // Player capability/selection flags.
    uint8_t capabilityLabelUtf16[8]; /* +0x78 UTF-16 label, L"CD" with FRONTEND_CAPABILITY_CD (written byte by byte) */
    uint32_t reserved80; // Never accessed.
    uint32_t scenarioAvailabilityMask0; // Scenario-availability bitmask for catalog group 0; selected level index maps to one bit within this dword.
    uint32_t scenarioAvailabilityMask1; // Scenario-availability bitmask for catalog group 1.
    uint32_t scenarioAvailabilityMask2; // Scenario-availability bitmask for catalog group 2.
    int32_t pingRoundTripTicks; /* +0x90 round trip of the last 0x10032/0x10033 ping in 8 ms timer ticks */
    uint16_t pingTextUtf16[14]; /* +0x94 "<round trip * 4>ms" */
    uint8_t snapshotPayload[4864]; // Fixed snapshot image transferred in 0xE0/0xE8-byte chunks.
};

struct FrontendCreditsUiStateView {
    uint8_t opaqueGap0000_0057[88]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    struct UiPageStackControl pageStack; // frontend page stack used by credits activation
    uint8_t opaqueGap00AC_01D3[296]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    struct SoftwareMaskRuntimeView creditsMaskRuntime; // credits texture/mask work state
};

struct FrontendPointerHintControl {
    struct UiNodeBase base;
    uint32_t reserved4C; // Never accessed.
    uint32_t hintActive;
    Ptr32<uint16_t> commandStream;
};

struct FrontendPointerSceneRuntimeView {
    struct UiNodeBase base;
    FrontendModelPointerContextFlags contextFlags;
    uint32_t activeFactionRuntimeIndex;
    Ptr32<struct FieldGridAsset> fieldGrid;
    uint32_t worldObjectArray;
    uint32_t renderedPrimitiveCount;
    Q12 hitReferenceWorldXQ12;
    Q12 hitReferenceWorldYQ12;
    Q12 hitReferenceWorldZQ12;
    GraphicsProjectionScale projectionScale;
    GraphicsViewAngle16 viewAngle0;
    GraphicsViewAngle16 viewAngle1;
    GraphicsProjectionShift projectionShift;
    UQ12 committedDistanceOrSoundZOffset;
    GraphicsWorldCoordinateQ12 targetPositionXQ12;
    GraphicsWorldCoordinateQ12 targetPositionYQ12;
    GraphicsWorldCoordinateQ12 targetPositionZQ12;
    UQ12 targetDistanceQ12;
    AngleTurn32 minimumPitchAngle;
    AngleTurn32 maximumPitchAngle;
    uint32_t minimumDistanceQ12;
    uint32_t maximumDistanceOrSurfaceLimitQ12;
    UiPixelCoordinate capturedPointerX;
    UiPixelCoordinate capturedPointerY;
    UiPointerWheelDelta capturedWheelDelta;
    uint32_t worldObjectCount;
    uint32_t clearTransientStateCallback;
    uint32_t selectedResourceMarkerIndex;
    AngleTurn32 auxiliaryOrientationAngle0;
    AngleTurn32 auxiliaryOrientationAngle1;
    uint32_t workspaceDwordArray;
    uint32_t workspaceDwordCount;
    Ptr32<struct GraphicsPrimitiveQueue> activePrimitiveQueue;
    uint32_t runtimeControlFlags;
    Ptr32<RuntimeSpinLockValue> renderSpinLock;
    Ptr32<void ()> renderSpinLockReleaseCallback;
    Ptr32<struct ModelRuntimeNode> candidateModelListHead;
    uint32_t activePlayerRuntimeId;
    Ptr32<struct ModelRuntimeNode> selectedModelNode;
    int selectedHitMetric;
    uint32_t surfaceHitWorldX;
    uint32_t surfaceHitWorldY;
    uint32_t surfaceHitDepth;
    Q12 cursorWorldXQ12;
    Q12 cursorWorldYQ12;
    Ptr32<struct GameEntityRuntime> selectedOverlayEntity;
    Ptr32<Bool8 (UiKeyboardStateMask, UiActionId, struct UiRootNode *)> keyboardFallback;
    Ptr32<uint32_t (uint32_t, uint32_t, uint32_t, int, struct ModelRuntimeNode *, struct FrontendModelPointerHitContext *)> hoverCursorCallback;
    Ptr32<uint32_t (uint32_t, uint32_t, uint32_t, int, struct ModelRuntimeNode *, struct FrontendModelPointerHitContext *)> heldButtonCursorCallback;
    Ptr32<uint32_t (uint32_t, uint32_t, uint32_t, int, struct ModelRuntimeNode *, struct FrontendModelPointerHitContext *)> buttonPressCallback;
    Ptr32<uint32_t (uint32_t, uint32_t, uint32_t, int, struct ModelRuntimeNode *, struct FrontendModelPointerHitContext *)> buttonDragCallback;
    Ptr32<uint32_t (uint32_t, uint32_t, uint32_t, int, struct ModelRuntimeNode *, struct FrontendModelPointerHitContext *)> buttonReleaseCallback;
    uint32_t rightClickCallback;
    uint32_t rightButtonHeldTicks;
    GraphicsSceneExtentFixed sceneBound0;
    GraphicsSceneExtentFixed sceneBound1;
    GraphicsSceneExtentFixed sceneBound2;
    GraphicsSceneExtentFixed sceneBound3;
    GraphicsSceneExtentFixed sceneBound4;
    GraphicsSceneExtentFixed sceneBound5;
    GraphicsSceneExtentFixed sceneBound6;
    GraphicsSceneExtentFixed sceneBound7;
    uint8_t reserved140_15B[28];
    uint32_t renderPhaseCallback;
    UiPixelCoordinate dragFrameStartX;
    UiPixelCoordinate dragFrameStartY;
    UiPixelCoordinate dragFrameEndX;
    UiPixelCoordinate dragFrameEndY;
    Ptr32<int> terrainMarkerCoordinatePairs;
    int terrainMarkerPointCount;
    uint32_t reserved178;
    uint8_t opaqueGap017C_019F[36]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    struct UiPageStackControl activePageStack;
    uint8_t opaqueGap01F4_438F[16796]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    struct FrontendPointerHintControl hintBox;
};

struct FrontendScenarioSelectionPageView {
    uint8_t opaqueGap0000_036B[876]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    struct UiNodeBase compactLayoutControl;
    uint8_t opaqueGap03B8_0507[336]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    struct UiPageStackControl primaryPageStack;
    uint8_t opaqueGap055C_1C93[5944]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    struct UiSelectableOptionRow60 scenarioOptionRow0;
    struct UiSelectableOptionRow60 scenarioOptionRow1;
    struct UiSelectableOptionRow60 scenarioOptionRow2;
    struct UiSelectableOptionRow60 scenarioOptionRow3;
    struct UiSelectableOptionRow60 scenarioOptionRow4;
    uint8_t opaqueGap1E74_26C3[2128]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
};

struct FrontendRootResourceSlots {
    uint8_t opaqueGap0000_05DF[1504]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct GraphicsTextureSourceAsset> menuTextureSource_05E0;
    uint8_t opaqueGap05E4_0643[96]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_0644;
    uint8_t opaqueGap0648_06A3[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_06A4;
    uint8_t opaqueGap06A8_0703[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_0704;
    uint8_t opaqueGap0708_0763[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_0764;
    uint8_t opaqueGap0768_0A8B[804]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet5_0A8C;
    uint8_t opaqueGap0A90_0AE3[84]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct GraphicsTextureSourceAsset> menuTextureSource_0AE4;
    uint8_t opaqueGap0AE8_0B47[96]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_0B48;
    uint8_t opaqueGap0B4C_0BA7[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_0BA8;
    uint8_t opaqueGap0BAC_0C67[188]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_0C68;
    uint8_t opaqueGap0C6C_1C8B[4128]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct GraphicsTextureSourceAsset> menuTextureSource_1C8C;
    uint8_t opaqueGap1C90_1CEF[96]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_1CF0;
    uint8_t opaqueGap1CF4_1D4F[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_1D50;
    uint8_t opaqueGap1D54_1DAF[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_1DB0;
    uint8_t opaqueGap1DB4_1E0F[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_1E10;
    uint8_t opaqueGap1E14_1E6F[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_1E70;
    uint8_t opaqueGap1E74_2023[432]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet6_2024;
    uint8_t opaqueGap2028_21EB[452]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet6_21EC;
    uint8_t opaqueGap21F0_23CB[476]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet6_23CC;
    uint8_t opaqueGap23D0_24F7[296]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct GraphicsTextureSourceAsset> menuTextureSource_24F8;
    uint8_t opaqueGap24FC_255B[96]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_255C;
    uint8_t opaqueGap2560_25BB[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_25BC;
    uint8_t opaqueGap25C0_266F[176]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct GraphicsTextureSourceAsset> menuTextureSource_2670;
    uint8_t opaqueGap2674_26D3[96]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_26D4;
    uint8_t opaqueGap26D8_278F[184]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_2790;
    uint8_t opaqueGap2794_27EF[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_27F0;
    uint8_t opaqueGap27F4_284F[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_2850;
    uint8_t opaqueGap2854_28AF[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_28B0;
    uint8_t opaqueGap28B4_2ADF[556]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_2AE0;
    uint8_t opaqueGap2AE4_2B3F[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_2B40;
    uint8_t opaqueGap2B44_2BF3[176]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_2BF4;
    uint8_t opaqueGap2BF8_2C53[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_2C54;
    uint8_t opaqueGap2C58_2CB3[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_2CB4;
    uint8_t opaqueGap2CB8_2D0B[84]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct GraphicsTextureSourceAsset> menuTextureSource_2D0C;
    uint8_t opaqueGap2D10_2D6F[96]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_2D70;
    uint8_t opaqueGap2D74_2DCF[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_2DD0;
    uint8_t opaqueGap2DD4_2EDF[268]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_2EE0;
    uint8_t opaqueGap2EE4_2F47[100]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_2F48;
    uint8_t opaqueGap2F4C_2FAF[100]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_2FB0;
    uint8_t opaqueGap2FB4_3017[100]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_3018;
    uint8_t opaqueGap301C_307F[100]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_3080;
    uint8_t opaqueGap3084_313B[184]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_313C;
    uint8_t opaqueGap3140_31A3[100]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_31A4;
    uint8_t opaqueGap31A8_320B[100]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_320C;
    uint8_t opaqueGap3210_3273[100]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_3274;
    uint8_t opaqueGap3278_32DB[100]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_32DC;
    uint8_t opaqueGap32E0_3343[100]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_3344;
    uint8_t opaqueGap3348_33AB[100]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_33AC;
    uint8_t opaqueGap33B0_3413[100]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_3414;
    uint8_t opaqueGap3418_347B[100]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_347C;
    uint8_t opaqueGap3480_34E3[100]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_34E4;
    uint8_t opaqueGap34E8_359F[184]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_35A0;
    uint8_t opaqueGap35A4_3607[100]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_3608;
    uint8_t opaqueGap360C_366F[100]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_3670;
    uint8_t opaqueGap3674_36D7[100]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_36D8;
    uint8_t opaqueGap36DC_3737[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct GraphicsTextureSourceAsset> menuTextureSource_3738;
    uint8_t opaqueGap373C_379B[96]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_379C;
    uint8_t opaqueGap37A0_3857[184]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_3858;
    uint8_t opaqueGap385C_390B[176]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_390C;
    uint8_t opaqueGap3910_3973[100]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_3974;
    uint8_t opaqueGap3978_39DB[100]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_39DC;
    uint8_t opaqueGap39E0_3A43[100]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_3A44;
    uint8_t opaqueGap3A48_3AAB[100]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_3AAC;
    uint8_t opaqueGap3AB0_3B13[100]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_3B14;
    uint8_t opaqueGap3B18_3C97[384]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet5_3C98;
    uint8_t opaqueGap3C9C_3D4B[176]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_3D4C;
    uint8_t opaqueGap3D50_3DAB[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_3DAC;
    uint8_t opaqueGap3DB0_3E0B[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_3E0C;
    uint8_t opaqueGap3E10_3E63[84]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct GraphicsTextureSourceAsset> menuTextureSource_3E64;
    uint8_t opaqueGap3E68_3EC7[96]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_3EC8;
    uint8_t opaqueGap3ECC_3F83[184]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_3F84;
    uint8_t opaqueGap3F88_3FE3[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_3FE4;
    uint8_t opaqueGap3FE8_4043[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet4_4044;
    uint8_t opaqueGap4048_41BF[376]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet5_41C0;
    uint8_t opaqueGap41C4_433B[376]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet5_433C;
    uint8_t opaqueGap4340_44B7[376]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet5_44B8;
    uint8_t opaqueGap44BC_4633[376]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet5_4634;
    uint8_t opaqueGap4638_485B[548]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct GraphicsTextureSourceAsset> menuTextureSource_485C;
    uint8_t opaqueGap4860_491B[188]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_491C;
    uint8_t opaqueGap4920_497B[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_497C;
    uint8_t opaqueGap4980_49DB[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_49DC;
    uint8_t opaqueGap49E0_4AD3[244]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet6_4AD4;
    uint8_t opaqueGap4AD8_4BCF[248]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet6_4BD0;
    uint8_t opaqueGap4BD4_4DC3[496]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet6_4DC4;
    uint8_t opaqueGap4DC8_4EAF[232]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet6_4EB0;
    uint8_t opaqueGap4EB4_4F2F[124]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct GraphicsTextureSourceAsset> menuTextureSource_4F30;
    uint8_t opaqueGap4F34_4FEF[188]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_4FF0;
    uint8_t opaqueGap4FF4_504F[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_5050;
    uint8_t opaqueGap5054_50BB[104]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet6_50BC;
    uint8_t opaqueGap50C0_514B[140]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet5_514C;
    uint8_t opaqueGap5150_520F[192]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet5_5210;
    uint8_t opaqueGap5214_53D7[452]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct GraphicsTextureSourceAsset> menuTextureSource_53D8;
    uint8_t opaqueGap53DC_5497[188]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_5498;
    uint8_t opaqueGap549C_54F7[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_54F8;
    uint8_t opaqueGap54FC_5557[92]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_5558;
    uint8_t opaqueGap555C_5653[248]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet6_5654;
    uint8_t opaqueGap5658_571F[200]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct GraphicsTextureSourceAsset> menuTextureSource_5720;
    uint8_t opaqueGap5724_57DF[188]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    Ptr32<struct SoundVoiceSet> buttonVoiceSet3_57E0;
    uint8_t opaqueGap57E4_5953[368]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
};

struct FrontendTaskAssignmentPageInitView { // Function-specific init view. Dynamic control-offset regions remain byte arrays; fixed-offset 0x055C..0x2677 is neutral dword[] because FrontendTaskAssignmentPage_Initialize accesses it with 32-bit stores. Canonical FrontendRootPageState remains unchanged.
    struct UiNodeBase rootNode;
    uint8_t taskRowControls[800]; // 800-byte backing region for the seven task-assignment row controls. Accessed through player/faction/selection/status/assignment offset tables; individual offsets remain dynamic.
    struct UiNodeBase compactLayoutControl;
    uint8_t reserved03B8_0507[336];
    struct UiPageStackControl primaryPageStack;
    uint32_t taskPageControlState[2119]; // Dword backing region for fixed task-assignment page controls and network-role visibility/enable/value state. Kept as neutral dwords where individual control identities are not yet proven.
    struct UiNodeBase returnToMainActionControl;
};

struct FrontendLoadedLevelPathOffsets {
    uint32_t levelPathOffsetOrLoadedFieldGrid;
    AssetRelativeOffset groundTextureBasePathOffset;
    AssetRelativeOffset surfaceTextureBasePathOffset;
    AssetRelativeOffset skyTextureBasePathOffset;
    AssetRelativeOffset armyTextureBasePathOffset;
    AssetRelativeOffset shotTextureBasePathOffset;
    AssetRelativeOffset effectTextureBasePathOffset;
    AssetRelativeOffset endingMovieBasePathOffset;
    AssetRelativeOffset soundBasePathOffset;
    AssetRelativeOffset technologyPathOffset;
};

struct FrontendLoadedLevelHeader {
    struct GeneratedAssetCommonPrefix common;
    struct FrontendLoadedLevelPathOffsets pathState;
    LevelAssetRecordCount initialArmyPlacementRecordCount; // Same slot as LevelAssetHeader.initialArmyPlacementRecordCount; unused by the frontend.
    struct LevelAssetResourceTables resourceTables;
    uint16_t levelFileNameUtf16[56]; // Same slot as LevelAssetHeader.levelFileNameUtf16.
    UiTextResourceId titleTextResourceIndex;
    uint8_t opaque174_18F[28];
    LevelCampaignAssociationIndex campaignAssociationIndex;
    uint8_t opaque194_1FF[108];
};

struct FrontendLoadedLevelAsset {
    struct FrontendLoadedLevelHeader header;
    struct LevelPlayerSlotRecord playerSlots[7];
    struct LevelWorldSettings worldSettings; // Terrain lighting, field region, faction counts and relations, intro movie and sample selectors at image offset 0x2E0.
};

struct FrontendNetworkListsRuntimeView {
    uint8_t opaqueGap0000_4B67[19304]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    struct UiPointerListControl sessionDiscoveryList; // frontend session-discovery pointer list
    uint8_t opaqueGap4BCC_55EB[2592]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    struct UiPointerListControl playerRuntimeList; // frontend player-runtime pointer list
};

struct FrontendResultsFactionWeightPair {
    uint32_t lane0;
    uint32_t lane1;
};

struct FrontendResultsColumnSequenceControl {
    struct UiNodeBase base; // runtime UiNode prefix
    uint32_t modeFlags; // bit 0 selects column sequence vs faction-weight raster path
    Ptr32<void (UiPixelCoordinate, UiPixelCoordinate, UiPixelCoordinate, struct FrontendResultsFactionWeightPair *)> factionWeightRaster; // four-argument raster column callback
    uint32_t columnTypeCount; // number of trailing column type dwords
    uint32_t rowCount; /* +0x58 rows (active factions) of the table, set by the end-of-game results screen */
    UiPixelMetric headerBaselineOffsetPixels; // read through the FrontendResultsRowMetrics view by the column painters
    UiPixelMetric rowAdvancePixels; // read through the FrontendResultsRowMetrics view by the column painters
    uint32_t columnTypes0; // first element of variable-length trailing column type list
};

struct FrontendResultsEightColumnTemplate {
    struct UiNodeBase base; // serialized/runtime UiNode prefix
    uint32_t modeFlags;
    Ptr32<void (UiPixelCoordinate, UiPixelCoordinate, UiPixelCoordinate, struct FrontendResultsFactionWeightPair *)> factionWeightRaster;
    uint32_t columnTypeCount;
    uint32_t rowCount; /* +0x58 */
    UiPixelMetric headerBaselineOffsetPixels; // FrontendResultsRowMetrics view of +0x5C
    UiPixelMetric rowAdvancePixels; // FrontendResultsRowMetrics view of +0x60
    uint32_t columnTypes[8];
};
#pragma pack(push, 1) /* packed layout: no alignment padding */
struct ScenarioCatalogDisplayRecord {
    uint16_t identifier[32]; // UTF-16 scenario identifier
    uint8_t opaqueGap0040_004F[16]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    uint32_t titleTextResourceId; // resource id used with family-specific base
    uint16_t titleDisplayTag; // runtime display tag
    Ptr32<uint16_t> titleResolvedText; // resolved UTF-16 text pointer
    uint8_t opaqueGap005A_005F[6]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    uint32_t subtitleTextResourceId; // resource id used with family-specific base
    uint16_t subtitleDisplayTag; // runtime display tag
    Ptr32<uint16_t> subtitleResolvedText; // resolved UTF-16 text pointer
    uint8_t opaqueGap006A_006F[6]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    uint32_t scenarioTextResourceId; // resource id used with family-specific base
    uint16_t scenarioDisplayTag; // runtime display tag
    Ptr32<uint16_t> scenarioResolvedText; // resolved UTF-16 text pointer
    uint8_t opaqueGap007A_007F[6]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
    uint32_t modeTextResourceId; // resource id used with family-specific base
    uint16_t modeDisplayTag; // runtime display tag
    Ptr32<uint16_t> modeResolvedText; // resolved UTF-16 text pointer
    uint8_t opaqueGap008A_00FF[118]; // Opaque byte span compacted from autogenerated undefined1 components; offsets and all known semantic fields preserved.
};
#pragma pack(pop)
using InGameWorldTransientStateClearCallbackProc = void (WorldRuntimeContext * arg0);
using ScenarioCatalogRefreshSelectedRecordCallback = void (uint32_t arg0, uint32_t arg1, uint32_t arg2, UiListRowIndex selectionIndex);
#pragma pack(push, 1)

/* Not in the original: the resolution rows of the display settings page: displayResolutionOption1..10 and the
   extra rows (FrontendUiImage displayResolutionExtraOptions), 24 pixels apart in displayResolutionRowPanel */
#define FRONTEND_DISPLAY_RESOLUTION_TEMPLATE_OPTIONS 10
#define FRONTEND_DISPLAY_RESOLUTION_EXTRA_OPTIONS 54
#define FRONTEND_DISPLAY_RESOLUTION_OPTIONS \
          (FRONTEND_DISPLAY_RESOLUTION_TEMPLATE_OPTIONS + FRONTEND_DISPLAY_RESOLUTION_EXTRA_OPTIONS)
#define FRONTEND_DISPLAY_RESOLUTION_ROW_HEIGHT 24
#define FRONTEND_DISPLAY_RESOLUTION_ROW_INSET 3 /* the rows' left/top/right offsets in the panel */

/* g_FrontendRootInitializationTemplate: 226 UI nodes (open-thandor: 232 plus the extra resolution rows, plus the 20
   nodes of the advanced settings page). FRONTEND_UI(root, node) is the node in a copy of it (or a node's <node>_prefix),
   FRONTEND_UI_FIELD(root, node, offset, type) a class field behind the UiNodeBase of the node. */
typedef struct FrontendUiImage {
    UiPanelControl frontendRoot; /* +0000 g_UiPanelControlVtable: Root panel of the frontend template. */
    UiLayoutContainerControl<2> frontendViewModeStack; /* +0058 g_UiLayoutContainerControlVtable: Two-page stack: page 0 = menu room (3D room view, dialog page stack, top/bottom bars), page 1 = full-screen movie view. */
    UiLayoutContainerControl<2> chatInputSlot; /* +00B0 g_UiLayoutContainerControlVtable: Page stack inside the bottom bar (0x4694): empty page or the chat input line. */
    UiRequiredTextEditControl chatInputEdit; /* +0108 g_UiRequiredTextEditControlVtable: Text edit for typing a network chat message; action 0x204C publishes it via the player message buffer. */
    uint32_t chatInputEdit_trailing[19]; /* +0188: template dwords behind the control */
    UiSoftwareTexturePreviewControl moviePlaybackView; /* +01D4 g_UiSoftwareTexturePreviewControlVtable: Software texture view that plays a movie; clicking (action 0x2048) closes the movie and returns to the main page. */
    UiFillPanelControl movieLetterboxTopBar; /* +0240 g_UiFillPanelControlVtable: Black fill bar above the movie view (top 1/8 of the screen). */
    UiFillPanelControl movieLetterboxBottomBar; /* +029C g_UiFillPanelControlVtable: Black fill bar below the movie view (bottom 1/8 of the screen). */
    UiConditionalActionControl chatMessageHistory; /* +02F8 g_UiConditionalActionControlVtable: Top-left strip showing the five most recent chat messages (action 0x200E trims/sorts the recent-text history). */
    uint32_t chatMessageHistory_trailing[4]; /* +0358: template dwords behind the control */
    FrontendModelPointerContext menuRoomModelView; /* +0368 g_FrontendModelPointerContextVtable: 3D model/pointer context rendering the main menu room between the top and bottom bars. */
    uint32_t menuRoomModelView_trailing[9]; /* +04E4: template dwords behind the control */
    UiLayoutContainerControl<13> frontendPageStack; /* +0508 g_UiLayoutContainerControlVtable: Stack of 13 frontend dialog pages (0 none, 5 options, 9 quit, 10 game selection, 11 faction setup, 12 mission briefing; 1-4 and 6-8 are in part 2). */
    UiImagePanelControl missionBriefingPage; /* +058C g_UiImagePanelControlVtable: Mission briefing page (page 12): description text, image, opponent setting, Back/Begin or Exit/Save. */
    UiFramedTextButtonControl briefingBackButton; /* +05E8 g_UiFramedTextButtonControlVtable: "Back" button (action 0x2043); shown when the briefing is opened from the frontend. */
    UiFramedTextButtonControl briefingExitButton; /* +0648 g_UiFramedTextButtonControlVtable: "Exit" button (action 0x204F); replaces Back when the briefing is opened in-game. */
    UiFramedTextButtonControl briefingSaveButton; /* +06A8 g_UiFramedTextButtonControlVtable: "Save" button (action 0x2050); shown only in the in-game variant of the briefing. */
    UiFramedTextButtonControl briefingBeginButton; /* +0708 g_UiFramedTextButtonControlVtable: "Begin" button (action 0x2047) that starts the mission. */
    UiFocusProxyControl briefingTitleLabel; /* +0768 g_UiFocusProxyControlVtable: Title "Mission description (%s)". */
    UiScrollableControl briefingTextScroller; /* +07C4 g_UiScrollableControlVtable: Scrollable frame holding the mission briefing text. */
    UiListOffsetControl briefingText; /* +0854 g_UiListOffsetControlVtable: Mission description text; its text id is set from the level and it is resized to the text extent. */
    UiNodeBase briefingImage; /* +08B0 g_UiImageActionControlVtable: Animated mission image next to the text; its first frame is set from the level. */
    uint32_t briefingImage_fields[6];
    UiFocusProxyControl opponentSettingsGroup; /* +0914 g_UiFocusProxyControlVtable: Group "Settings for computer opponent" with weak/strong labels and slider. */
    UiFocusProxyControl opponentWeakLabel; /* +0970 g_UiFocusProxyControlVtable: Label "weak" at the left end of the opponent slider. */
    UiFocusProxyControl opponentStrongLabel; /* +09CC g_UiFocusProxyControlVtable: Label "strong" at the right end of the opponent slider. */
    UiRangeSliderControl gameSpeedSlider; /* +0A28 g_UiRangeSliderControlVtable: Slider 80..120 (default 100); action 0x204A applies it as game-speed percent and persists it. */
    UiImagePanelControl factionSetupPage; /* +0A90 g_UiImagePanelControlVtable: Page 11 "Choose faction": faction roster (colour, mode, play checkbox, participants) and task description. */
    UiFramedTextButtonControl factionSetupBackButton; /* +0AEC g_UiFramedTextButtonControlVtable: "Back" button (action 0x2040). */
    UiFramedTextButtonControl factionSetupNextButton; /* +0B4C g_UiFramedTextButtonControlVtable: "Next" button (action 0x2041). */
    UiFocusProxyControl factionSetupTitleLabel; /* +0BAC g_UiFocusProxyControlVtable: Title "Choose faction"; hint text says a network game waits until all players are ready. */
    uint32_t factionSetupTitleLabel_trailing[1]; /* +0C08: template dwords behind the control */
    UiFramedTextButtonControl factionSetupFinishButton; /* +0C0C g_UiFramedTextButtonControlVtable: "Finish" button (action 0x2042); its flag bit 2 gates the roster row checks. */
    UiLayoutContainerControl<1> factionRosterTable; /* +0C6C g_UiLayoutContainerControlVtable: Single-page container holding the 7-row faction table and its column headers. */
    UiFocusProxyControl factionRow1NumberLabel; /* +0CC0 g_UiFocusProxyControlVtable: Row number label "1." in the Faction column. */
    UiFocusProxyControl factionRow2NumberLabel; /* +0D1C g_UiFocusProxyControlVtable: Row number label "2." in the Faction column. */
    UiFocusProxyControl factionRow3NumberLabel; /* +0D78 g_UiFocusProxyControlVtable: Row number label "3." in the Faction column. */
    UiFocusProxyControl factionRow4NumberLabel; /* +0DD4 g_UiFocusProxyControlVtable: Row number label "4." in the Faction column. */
    UiFocusProxyControl factionRow5NumberLabel; /* +0E30 g_UiFocusProxyControlVtable: Row number label "5." in the Faction column. */
    UiFocusProxyControl factionRow6NumberLabel; /* +0E8C g_UiFocusProxyControlVtable: Row number label "6." in the Faction column. */
    UiFocusProxyControl factionRow7NumberLabel; /* +0EE8 g_UiFocusProxyControlVtable: Row number label "7." in the Faction column. */
    UiFramedTextButtonControl factionRow1ColourButton; /* +0F44 g_UiFramedTextButtonControlVtable: Faction 1 name with colour swatch in the Colour column; action 0x2044 (factionControls table) cycles the colour. */
    UiFramedTextButtonControl factionRow2ColourButton; /* +0FA4 g_UiFramedTextButtonControlVtable: Faction 2 name with colour swatch in the Colour column; action 0x2044 (factionControls table) cycles the colour. */
    UiFramedTextButtonControl factionRow3ColourButton; /* +1004 g_UiFramedTextButtonControlVtable: Faction 3 name with colour swatch in the Colour column; action 0x2044 (factionControls table) cycles the colour. */
    UiFramedTextButtonControl factionRow4ColourButton; /* +1064 g_UiFramedTextButtonControlVtable: Faction 4 name with colour swatch in the Colour column; action 0x2044 (factionControls table) cycles the colour. */
    UiFramedTextButtonControl factionRow5ColourButton; /* +10C4 g_UiFramedTextButtonControlVtable: Faction 5 name with colour swatch in the Colour column; action 0x2044 (factionControls table) cycles the colour. */
    UiFramedTextButtonControl factionRow6ColourButton; /* +1124 g_UiFramedTextButtonControlVtable: Faction 6 name with colour swatch in the Colour column; action 0x2044 (factionControls table) cycles the colour. */
    UiFramedTextButtonControl factionRow7ColourButton; /* +1184 g_UiFramedTextButtonControlVtable: Faction 7 name with colour swatch in the Colour column; action 0x2044 (factionControls table) cycles the colour. */
    UiFramedTextButtonControl factionRow1ModeButton; /* +11E4 g_UiFramedTextButtonControlVtable: Mode button of faction 1 (Player/Computer/No-one), action 0x2045 (playerControls table). */
    UiFramedTextButtonControl factionRow2ModeButton; /* +1244 g_UiFramedTextButtonControlVtable: Mode button of faction 2 (Player/Computer/No-one), action 0x2045 (playerControls table). */
    UiFramedTextButtonControl factionRow3ModeButton; /* +12A4 g_UiFramedTextButtonControlVtable: Mode button of faction 3 (Player/Computer/No-one), action 0x2045 (playerControls table). */
    UiFramedTextButtonControl factionRow4ModeButton; /* +1304 g_UiFramedTextButtonControlVtable: Mode button of faction 4 (Player/Computer/No-one), action 0x2045 (playerControls table). */
    UiFramedTextButtonControl factionRow5ModeButton; /* +1364 g_UiFramedTextButtonControlVtable: Mode button of faction 5 (Player/Computer/No-one), action 0x2045 (playerControls table). */
    UiFramedTextButtonControl factionRow6ModeButton; /* +13C4 g_UiFramedTextButtonControlVtable: Mode button of faction 6 (Player/Computer/No-one), action 0x2045 (playerControls table). */
    UiFramedTextButtonControl factionRow7ModeButton; /* +1424 g_UiFramedTextButtonControlVtable: Mode button of faction 7 (Player/Computer/No-one), action 0x2045 (playerControls table). */
    UiTextButtonControl factionRow1PlayCheckbox; /* +1484 g_UiTextButtonControlVtable: Checkbox under "Accept" choosing faction 1 as the one to play, action 0x2046 (selectionRows table). */
    UiTextButtonControl factionRow2PlayCheckbox; /* +14E4 g_UiTextButtonControlVtable: Checkbox under "Accept" choosing faction 2 as the one to play, action 0x2046 (selectionRows table). */
    UiTextButtonControl factionRow3PlayCheckbox; /* +1544 g_UiTextButtonControlVtable: Checkbox under "Accept" choosing faction 3 as the one to play, action 0x2046 (selectionRows table). */
    UiTextButtonControl factionRow4PlayCheckbox; /* +15A4 g_UiTextButtonControlVtable: Checkbox under "Accept" choosing faction 4 as the one to play, action 0x2046 (selectionRows table). */
    UiTextButtonControl factionRow5PlayCheckbox; /* +1604 g_UiTextButtonControlVtable: Checkbox under "Accept" choosing faction 5 as the one to play, action 0x2046 (selectionRows table). */
    UiTextButtonControl factionRow6PlayCheckbox; /* +1664 g_UiTextButtonControlVtable: Checkbox under "Accept" choosing faction 6 as the one to play, action 0x2046 (selectionRows table). */
    UiTextButtonControl factionRow7PlayCheckbox; /* +16C4 g_UiTextButtonControlVtable: Checkbox under "Accept" choosing faction 7 as the one to play, action 0x2046 (selectionRows table). */
    UiFocusProxyControl factionRow1ParticipantsLabel; /* +1724 g_UiFocusProxyControlVtable: Participant column text for faction 1: names of the network players assigned to it. */
    UiFocusProxyControl factionRow2ParticipantsLabel; /* +1780 g_UiFocusProxyControlVtable: Participant column text for faction 2: names of the network players assigned to it. */
    UiFocusProxyControl factionRow3ParticipantsLabel; /* +17DC g_UiFocusProxyControlVtable: Participant column text for faction 3: names of the network players assigned to it. */
    UiFocusProxyControl factionRow4ParticipantsLabel; /* +1838 g_UiFocusProxyControlVtable: Participant column text for faction 4: names of the network players assigned to it. */
    UiFocusProxyControl factionRow5ParticipantsLabel; /* +1894 g_UiFocusProxyControlVtable: Participant column text for faction 5: names of the network players assigned to it. */
    UiFocusProxyControl factionRow6ParticipantsLabel; /* +18F0 g_UiFocusProxyControlVtable: Participant column text for faction 6: names of the network players assigned to it. */
    UiFocusProxyControl factionRow7ParticipantsLabel; /* +194C g_UiFocusProxyControlVtable: Participant column text for faction 7: names of the network players assigned to it. */
    UiFocusProxyControl rosterFactionHeader; /* +19A8 g_UiFocusProxyControlVtable: Column header "Faction" (hint: click to make the faction appear in the game). */
    uint32_t rosterFactionHeader_trailing[1]; /* +1A04: template dwords behind the control */
    UiFocusProxyControl rosterModeHeader; /* +1A08 g_UiFocusProxyControlVtable: Column header "Mode" (hint: click several times to cycle). */
    uint32_t rosterModeHeader_trailing[1]; /* +1A64: template dwords behind the control */
    UiFocusProxyControl rosterColourHeader; /* +1A68 g_UiFocusProxyControlVtable: Column header "Colour" (hint: choose the faction to play). */
    uint32_t rosterColourHeader_trailing[1]; /* +1AC4: template dwords behind the control */
    UiFocusProxyControl rosterAcceptHeader; /* +1AC8 g_UiFocusProxyControlVtable: Column header "Accept" above the play checkboxes. */
    UiFocusProxyControl rosterParticipantHeader; /* +1B24 g_UiFocusProxyControlVtable: Column header "Participant"; hidden in local (non-network) games. */
    UiFocusProxyControl taskDescriptionLabel; /* +1B80 g_UiFocusProxyControlVtable: Label "Task description (%s):" for the selected faction. */
    UiListOffsetControl taskDescriptionText; /* +1BDC g_UiListOffsetControlVtable: Faction task text from the level (text id 0x230010 + faction + level*0x10). */
    UiImagePanelControl gameSelectPage; /* +1C38 g_UiImagePanelControlVtable: Page 10 "Choose game": tabs Load game / Single game / Campaigns with lists and descriptions. */
    UiFramedTextButtonControl gameSelectCancelButton; /* +1C94 g_UiFramedTextButtonControlVtable: "Cancel" button (action 0x2034). */
    UiFramedTextButtonControl gameSelectStartButton; /* +1CF4 g_UiFramedTextButtonControlVtable: "Start" button (action 0x2038) starting the selected entry. */
    UiFramedTextButtonControl loadGameTabButton; /* +1D54 g_UiFramedTextButtonControlVtable: Tab button "Load game" (action 0x2035). */
    UiFramedTextButtonControl singleGameTabButton; /* +1DB4 g_UiFramedTextButtonControlVtable: Tab button "Single game" (action 0x2036). */
    UiFramedTextButtonControl campaignsTabButton; /* +1E14 g_UiFramedTextButtonControlVtable: Tab button "Campaigns" (action 0x2037). */
    UiFocusProxyControl gameSelectTitleLabel; /* +1E74 g_UiFocusProxyControlVtable: Title "Choose game". */
    UiLayoutContainerControl<3> gameSelectTabStack; /* +1ED0 g_UiLayoutContainerControlVtable: Three-page stack: saved games, individual missions, campaigns. */
    UiScrollableControl savedGamesScroller; /* +1F2C g_UiScrollableControlVtable: Scroll frame of the saved-games list (Load game tab). */
    UiListControl savedGamesList; /* +1FBC g_UiListControlVtable: List of saved games (name, time, date), action 0x2039. */
    uint32_t savedGamesList_trailing[3]; /* +2030: template dwords behind the control */
    UiFocusProxyControl savedGamesLabel; /* +203C g_UiFocusProxyControlVtable: Label "Games saved:" above the saved-games list. */
    UiListOffsetControl savedGameDescriptionText; /* +2098 g_UiListOffsetControlVtable: Description text of the selected saved game. */
    UiScrollableControl missionsScroller; /* +20F4 g_UiScrollableControlVtable: Scroll frame of the individual-missions list (Single game tab). */
    UiListControl missionsList; /* +2184 g_UiListControlVtable: List of single missions (map, planet, size, human/total factions), action 0x203A. */
    uint32_t missionsList_trailing[9]; /* +21F8: template dwords behind the control */
    UiFocusProxyControl missionsLabel; /* +221C g_UiFocusProxyControlVtable: Label "Individual missions:". */
    UiListOffsetControl missionDescriptionText; /* +2278 g_UiListOffsetControlVtable: Description text of the selected single mission. */
    UiScrollableControl campaignsScroller; /* +22D4 g_UiScrollableControlVtable: Scroll frame of the campaigns list (Campaigns tab). */
    UiListControl campaignsList; /* +2364 g_UiListControlVtable: List of campaigns (name, human/total factions), action 0x203B. */
    uint32_t campaignsList_trailing[5]; /* +23D8: template dwords behind the control */
    UiFocusProxyControl campaignsLabel; /* +23EC g_UiFocusProxyControlVtable: Label "Campaigns:". */
    UiListOffsetControl campaignDescriptionText; /* +2448 g_UiListOffsetControlVtable: Description text of the selected campaign. */
    UiImagePanelControl quitConfirmPage; /* +24A4 g_UiImagePanelControlVtable: Page 9 "Exit programme" confirmation. */
    UiFramedTextButtonControl quitNoButton; /* +2500 g_UiFramedTextButtonControlVtable: "no" button (action 0x2033) closing the quit dialog. */
    UiFramedTextButtonControl quitYesButton; /* +2560 g_UiFramedTextButtonControlVtable: "yes" button (no action id in the template) that exits the programme. */
    UiFocusProxyControl quitTitleLabel; /* +25C0 g_UiFocusProxyControlVtable: Title "Exit programme". */
    UiImagePanelControl optionsPage; /* +261C g_UiImagePanelControlVtable: Page 5 "Options": graphics/3D/sound sub-pages, panel, scroll speed, map and mouse settings. */
    UiFramedTextButtonControl optionsOkButton; /* +2678 g_UiFramedTextButtonControlVtable: "Ok" button (action 0x2010) closing the options. */
    UiFocusProxyControl optionsTitleLabel; /* +26D8 g_UiFocusProxyControlVtable: Title "Options". */
    UiFramedTextButtonControl graphicsSettingsButton; /* +2734 g_UiFramedTextButtonControlVtable: "Graphics" button (action 0x2011) opening the graphics settings page. */
    UiFramedTextButtonControl settings3DButton; /* +2794 g_UiFramedTextButtonControlVtable: "3D" button (action 0x2012) opening the 3D settings page. */
    UiFramedTextButtonControl soundSettingsButton; /* +27F4 g_UiFramedTextButtonControlVtable: "Sound" button (action 0x2013) opening the sound settings page. */
    UiTextButtonControl hidePanelCheckbox; /* +2854 g_UiTextButtonControlVtable: Checkbox "Hide panel" (action 0x2049, persisted bit 4). */
    UiFocusProxyControl scrollSpeedGroup; /* +28B4 g_UiFocusProxyControlVtable: Group "Scroll speed:" with slow/fast labels and slider. */
    UiFocusProxyControl scrollSpeedSlowLabel; /* +2910 g_UiFocusProxyControlVtable: Label "slow" of the scroll-speed slider. */
    UiFocusProxyControl scrollSpeedFastLabel; /* +296C g_UiFocusProxyControlVtable: Label "fast" of the scroll-speed slider. */
    UiRangeSliderControl scrollSpeedSlider; /* +29C8 g_UiRangeSliderControlVtable: Scroll-speed slider 8..128 (action 0x204B, persisted). */
    UiTitledWindowControl generalMapGroup; /* +2A30 g_UiTitledWindowControlVtable: Titled box "General map:" with auto-zoom and auto-rotation options. */
    UiTextButtonControl autoZoomOffCheckbox; /* +2A84 g_UiTextButtonControlVtable: Checkbox "Automatic zoom off" (action 0x203C, bit 1). */
    UiTextButtonControl autoRotationOffCheckbox; /* +2AE4 g_UiTextButtonControlVtable: Checkbox "Automatic rotation off" (action 0x203D, bit 2). */
    UiTitledWindowControl mouseCommandsGroup; /* +2B44 g_UiTitledWindowControlVtable: Titled box "Mouse commands:" with the mouse option checkboxes. */
    UiTextButtonControl linkRotationZoomCheckbox; /* +2B98 g_UiTextButtonControlVtable: Checkbox "Link rotation/zoom" (action 0x203E, bit 1). */
    UiTextButtonControl linkRotationTiltCheckbox; /* +2BF8 g_UiTextButtonControlVtable: Checkbox "Link rotation/tilt" (action 0x203F, bit 2). */
    UiTextButtonControl rightButtonNoScrollCheckbox; /* +2C58 g_UiTextButtonControlVtable: Checkbox "Right button does not scroll" (action 0x2051). */
    UiImagePanelControl displaySettingsPage; /* +2CB8 g_UiImagePanelControlVtable: Page-stack page 6 (opened by action 0x2011; open-thandor: page 0 of displayPageStack): display adapter, resolution and colour-depth selection. */
    UiFramedTextButtonControl displaySettingsBackButton; /* +2D14 g_UiFramedTextButtonControlVtable: Action 0x2010: returns to the options menu page (page 5). */
    UiFramedTextButtonControl displaySettingsApplyButton; /* +2D74 g_UiFramedTextButtonControlVtable: Action 0x2031 FrontendDisplaySettings_ApplyMode: applies the pending display mode. */
    UiFocusProxyControl displaySettingsTitle; /* +2DD4 g_UiFocusProxyControlVtable: Page title caption (text 0x2124) of the display settings page. */
    UiTitledWindowControl displayAdapterGroup; /* +2E30 g_UiTitledWindowControlVtable: Titled box (text 0x2125) holding the five graphics-adapter choices. */
    UiPayloadPairTextButton displayAdapterOption1; /* +2E84 g_UiPayloadPairTextButtonVtable: Adapter choice 0 (action 0x202C); label filled from g_GraphicsAdapters[0]. */
    UiPayloadPairTextButton displayAdapterOption2; /* +2EEC g_UiPayloadPairTextButtonVtable: Adapter choice 1 (action 0x202D). */
    UiPayloadPairTextButton displayAdapterOption3; /* +2F54 g_UiPayloadPairTextButtonVtable: Adapter choice 2 (action 0x202E). */
    UiPayloadPairTextButton displayAdapterOption4; /* +2FBC g_UiPayloadPairTextButtonVtable: Adapter choice 3 (action 0x202F). */
    UiPayloadPairTextButton displayAdapterOption5; /* +3024 g_UiPayloadPairTextButtonVtable: Adapter choice 4 (action 0x2030). */
    UiTitledWindowControl displayResolutionGroup; /* +308C g_UiTitledWindowControlVtable: Titled box (text 0x2126) holding the ten resolution choices. */
    UiNumericPairTextButton displayResolutionOption1; /* +30E0 g_UiNumericPairTextButtonVtable: Resolution choice 1 (action 0x2022, width/height pair set at runtime). */
    UiNumericPairTextButton displayResolutionOption2; /* +3148 g_UiNumericPairTextButtonVtable: Resolution choice 2 (action 0x2023). */
    UiNumericPairTextButton displayResolutionOption3; /* +31B0 g_UiNumericPairTextButtonVtable: Resolution choice 3 (action 0x2024). */
    UiNumericPairTextButton displayResolutionOption4; /* +3218 g_UiNumericPairTextButtonVtable: Resolution choice 4 (action 0x2025). */
    UiNumericPairTextButton displayResolutionOption5; /* +3280 g_UiNumericPairTextButtonVtable: Resolution choice 5 (action 0x2026). */
    UiNumericPairTextButton displayResolutionOption6; /* +32E8 g_UiNumericPairTextButtonVtable: Resolution choice 6 (action 0x2027). */
    UiNumericPairTextButton displayResolutionOption7; /* +3350 g_UiNumericPairTextButtonVtable: Resolution choice 7 (action 0x2028). */
    UiNumericPairTextButton displayResolutionOption8; /* +33B8 g_UiNumericPairTextButtonVtable: Resolution choice 8 (action 0x2029). */
    UiNumericPairTextButton displayResolutionOption9; /* +3420 g_UiNumericPairTextButtonVtable: Resolution choice 9 (action 0x202A). */
    UiNumericPairTextButton displayResolutionOption10; /* +3488 g_UiNumericPairTextButtonVtable: Resolution choice 10 (action 0x202B). */
    /* The colour depth group and its four choices: unused since open-thandor runs in 32-bit colour only (not linked
       into the display settings page; kept so the image keeps its layout). */
    UiTitledWindowControl displayColorDepthGroup; /* +34F0 g_UiTitledWindowControlVtable: Titled box (text 0x2127) holding the four colour-depth choices. */
    UiNumericPairTextButton displayColorDepthOption1; /* +3544 g_UiNumericPairTextButtonVtable: Lowest available bits-per-pixel choice (action 0x201E, value filled by action 0x2011). */
    UiNumericPairTextButton displayColorDepthOption2; /* +35AC g_UiNumericPairTextButtonVtable: Second colour-depth choice (action 0x201F). */
    UiNumericPairTextButton displayColorDepthOption3; /* +3614 g_UiNumericPairTextButtonVtable: Third colour-depth choice (action 0x2020). */
    UiNumericPairTextButton displayColorDepthOption4; /* +367C g_UiNumericPairTextButtonVtable: Fourth colour-depth choice (action 0x2021). */
    UiImagePanelControl graphicsSettingsPage; /* +36E4 g_UiImagePanelControlVtable: Page-stack page 7 (action 0x2012): shading, polygon detail and texture quality. */
    UiFramedTextButtonControl graphicsSettingsBackButton; /* +3740 g_UiFramedTextButtonControlVtable: Action 0x2010: returns to the options menu page. */
    UiFocusProxyControl graphicsSettingsTitle; /* +37A0 g_UiFocusProxyControlVtable: Page title caption (text 0x212E) of the graphics settings page. */
    UiTextButtonControl shadingEnabledCheckbox; /* +37FC g_UiTextButtonControlVtable: Toggle (action 0x2014 FrontendShadingSettings_SetEnabled) for shading on/off. */
    UiTitledWindowControl shadingLevelGroup; /* +385C g_UiTitledWindowControlVtable: Titled box (text 0x2130) with the six shading grid/depth levels. */
    UiNumericPairTextButton shadingLevelGrid32Depth32; /* +38B0 g_UiNumericPairTextButtonVtable: Shading level choice (action 0x2015) with grid 0x20, depth 0x20. */
    UiNumericPairTextButton shadingLevelGrid32Depth64; /* +3918 g_UiNumericPairTextButtonVtable: Shading level choice (action 0x2015) with grid 0x20, depth 0x40. */
    UiNumericPairTextButton shadingLevelGrid32Depth128; /* +3980 g_UiNumericPairTextButtonVtable: Shading level choice (action 0x2015) with grid 0x20, depth 0x80. */
    UiNumericPairTextButton shadingLevelGrid64Depth64; /* +39E8 g_UiNumericPairTextButtonVtable: Shading level choice (action 0x2015) with grid 0x40, depth 0x40. */
    UiNumericPairTextButton shadingLevelGrid64Depth128; /* +3A50 g_UiNumericPairTextButtonVtable: Shading level choice (action 0x2015) with grid 0x40, depth 0x80. */
    UiNumericPairTextButton shadingLevelGrid128Depth128; /* +3AB8 g_UiNumericPairTextButtonVtable: Shading level choice (action 0x2015) with grid 0x80, depth 0x80. */
    UiFocusProxyControl polygonDetailLabel; /* +3B20 g_UiFocusProxyControlVtable: Caption (text 0x2131) bound to the polygon-detail slider 0x3C34. */
    UiFocusProxyControl polygonDetailMinCaption; /* +3B7C g_UiFocusProxyControlVtable: Low-end caption (text 0x2134) under the polygon-detail slider. */
    UiFocusProxyControl polygonDetailMaxCaption; /* +3BD8 g_UiFocusProxyControlVtable: High-end caption (text 0x2135) under the polygon-detail slider. */
    UiRangeSliderControl polygonDetailSlider; /* +3C34 g_UiRangeSliderControlVtable: Slider 0x4000..0x40000 (action 0x2016) setting the model LOD depth threshold. */
    UiTitledWindowControl textureQualityGroup; /* +3C9C g_UiTitledWindowControlVtable: Titled box (text 0x2132) with the three texture-quality choices. */
    UiTextButtonControl textureQualityLow; /* +3CF0 g_UiTextButtonControlVtable: Texture quality choice 1 (action 0x2017, text 0x2136). */
    UiTextButtonControl textureQualityMedium; /* +3D50 g_UiTextButtonControlVtable: Texture quality choice 2 (action 0x2017, text 0x2137). */
    UiTextButtonControl textureQualityHigh; /* +3DB0 g_UiTextButtonControlVtable: Texture quality choice 3 (action 0x2017, text 0x2138). */
    UiImagePanelControl audioSettingsPage; /* +3E10 g_UiImagePanelControlVtable: Page-stack page 8 (action 0x2013): sound toggles and volume sliders. */
    UiFramedTextButtonControl audioSettingsBackButton; /* +3E6C g_UiFramedTextButtonControlVtable: Action 0x2010: returns to the options menu page. */
    UiFocusProxyControl audioSettingsTitle; /* +3ECC g_UiFocusProxyControlVtable: Page title caption (text 0x213A) of the audio settings page. */
    UiTextButtonControl musicEnabledCheckbox; /* +3F28 g_UiTextButtonControlVtable: Toggle (action 0x2019) that starts/stops frontend music. */
    UiTextButtonControl soundEffectsEnabledCheckbox; /* +3F88 g_UiTextButtonControlVtable: Toggle (action 0x2018) for sound effects on/off. */
    UiTextButtonControl reverseStereoCheckbox; /* +3FE8 g_UiTextButtonControlVtable: Toggle (action 0x201A) that swaps left/right stereo channels. */
    UiFocusProxyControl effectsVolumeLabel; /* +4048 g_UiFocusProxyControlVtable: Caption (text 0x213E) bound to the effects-volume slider 0x415C. */
    UiFocusProxyControl effectsVolumeMinCaption; /* +40A4 g_UiFocusProxyControlVtable: Low-end caption (text 0x2141) of the effects-volume slider. */
    UiFocusProxyControl effectsVolumeMaxCaption; /* +4100 g_UiFocusProxyControlVtable: High-end caption (text 0x2142) of the effects-volume slider. */
    UiRangeSliderControl effectsVolumeSlider; /* +415C g_UiRangeSliderControlVtable: Slider 0..0x8000 (action 0x201B) setting the effects gain. */
    UiFocusProxyControl movieVolumeLabel; /* +41C4 g_UiFocusProxyControlVtable: Caption (text 0x213F) bound to the movie-volume slider 0x42D8. */
    UiFocusProxyControl movieVolumeMinCaption; /* +4220 g_UiFocusProxyControlVtable: Low-end caption (text 0x2141) of the movie-volume slider. */
    UiFocusProxyControl movieVolumeMaxCaption; /* +427C g_UiFocusProxyControlVtable: High-end caption (text 0x2142) of the movie-volume slider. */
    UiRangeSliderControl movieVolumeSlider; /* +42D8 g_UiRangeSliderControlVtable: Slider 0..0x8000 (action 0x201C) setting the default movie audio gain. */
    UiFocusProxyControl musicVolumeLabel; /* +4340 g_UiFocusProxyControlVtable: Caption (text 0x2140) bound to the music-volume slider 0x4454. */
    UiFocusProxyControl musicVolumeMinCaption; /* +439C g_UiFocusProxyControlVtable: Low-end caption (text 0x2141) of the music-volume slider. */
    UiFocusProxyControl musicVolumeMaxCaption; /* +43F8 g_UiFocusProxyControlVtable: High-end caption (text 0x2142) of the music-volume slider. */
    UiRangeSliderControl musicVolumeSlider; /* +4454 g_UiRangeSliderControlVtable: Slider 0..0x8000 (action 0x201D) setting the music gain. */
    UiFocusProxyControl movieEventVolumeLabel; /* +44BC g_UiFocusProxyControlVtable: Caption (text 0x2143) bound to the alternate movie-volume slider 0x45D0. */
    UiFocusProxyControl movieEventVolumeMinCaption; /* +4518 g_UiFocusProxyControlVtable: Low-end caption (text 0x2141) of the alternate movie-volume slider. */
    UiFocusProxyControl movieEventVolumeMaxCaption; /* +4574 g_UiFocusProxyControlVtable: High-end caption (text 0x2142) of the alternate movie-volume slider. */
    UiRangeSliderControl movieEventVolumeSlider; /* +45D0 g_UiRangeSliderControlVtable: Slider 0..0x8000 (action 0x204E) setting the alternate movie gain used by timed movie events. */
    UiFillPanelControl topBlackBar; /* +4638 g_UiFillPanelControlVtable: Black fill panel over the top eighth of the screen, sibling after the page stack. */
    UiFillPanelControl bottomBar; /* +4694 g_UiFillPanelControlVtable: Black fill panel over the bottom eighth of the screen; holds the status text and the chat input container 0xB0. */
    UiConditionalActionControl bottomBarConditionalAction; /* +46F0 g_UiConditionalActionControlVtable: Zero-size ConditionalAction control centred in the bottom bar (action -1); exact role unknown. */
    UiFocusProxyControl bottomBarStatusText; /* +4750 g_UiFocusProxyControlVtable: Full-size caption (style 0xA, text 0x112) in the bottom bar; probably the status/help line (role inferred). */
    UiHorizontalGaugeControl transferProgressGauge; /* +47AC g_UiTransferProgressGaugeVtable: Horizontal gauge (UiHorizontalGaugeControl subclass) in the bottom-right corner of the bottom bar; reloads its range from the transfer mailbox before drawing: the file-transfer progress. */
    UiImagePanelControl networkGamePage; /* +4808 g_UiImagePanelControlVtable: Page-stack page 1 (action 0x2003): network protocol, player name, host address and session list. */
    UiFocusProxyControl networkGameTitle; /* +4864 g_UiFocusProxyControlVtable: Page title caption (text 0x2107) of the network game page. */
    UiFramedTextButtonControl networkGameBackButton; /* +48C0 g_UiFramedTextButtonControlVtable: Action 0x2000: resets networking and returns to the main menu. */
    UiFramedTextButtonControl networkGameHostButton; /* +4920 g_UiFramedTextButtonControlVtable: Action 0x2001: initialises the host (create game) setup page. */
    UiFramedTextButtonControl networkGameJoinButton; /* +4980 g_UiFramedTextButtonControlVtable: Action 0x2002: sends the join request (player descriptor) to the selected session. */
    UiScrollableControl networkProtocolScrollBox; /* +49E0 g_UiScrollableControlVtable: Scroll frame around the network protocol/backend list. */
    UiTextListControl networkProtocolList; /* +4A70 g_UiTextListControlVtable: Text list of network backends (action 0x200F selects/opens the backend). */
    UiScrollableControl sessionListScrollBox; /* +4AD8 g_UiScrollableControlVtable: Scroll frame around the list of discovered sessions. */
    UiListControl sessionList; /* +4B68 g_UiListControlVtable: List of discovered network sessions (g_FrontendSessionListRows, action 0x2009 updates Join availability). */
    uint32_t sessionList_trailing[4]; /* +4BDC: template dwords behind the control */
    UiFocusProxyControl hostAddressLabel; /* +4BEC g_UiFocusProxyControlVtable: Caption (text 0x2103) above the host address edit 0x4D5C. */
    UiFocusProxyControl playerNameLabel; /* +4C48 g_UiFocusProxyControlVtable: Caption (text 0x2106) above the player name edit 0x4E48. */
    UiFocusProxyControl networkProtocolLabel; /* +4CA4 g_UiFocusProxyControlVtable: Caption (text 0x2105) above the network protocol list. */
    UiFocusProxyControl sessionListLabel; /* +4D00 g_UiFocusProxyControlVtable: Caption (text 0x2104) above the session list. */
    UiRequiredTextEditControl hostAddressEdit; /* +4D5C g_UiRequiredTextEditControlVtable: 64-char endpoint/address edit (action 0x200D validates and requests the session mailbox). */
    uint32_t hostAddressEdit_trailing[27]; /* +4DDC: template dwords behind the control */
    UiRequiredTextEditControl playerNameEdit; /* +4E48 g_UiRequiredTextEditControlVtable: 20-char player name edit (action 0x2032 persists the player name). */
    uint32_t playerNameEdit_trailing[5]; /* +4EC8: template dwords behind the control */
    UiImagePanelControl hostGameSetupPage; /* +4EDC g_UiImagePanelControlVtable: Page-stack page 2: game name, player count and network speed for hosting. */
    UiFocusProxyControl hostGameSetupTitle; /* +4F38 g_UiFocusProxyControlVtable: Page title caption (text 0x210C) of the host game setup page. */
    UiFramedTextButtonControl hostGameSetupBackButton; /* +4F94 g_UiFramedTextButtonControlVtable: Action 0x2003: returns to the network game page. */
    UiFramedTextButtonControl hostGameCreateButton; /* +4FF4 g_UiFramedTextButtonControlVtable: Action 0x2004: creates the session with one local player and opens the host lobby. */
    UiRequiredTextEditControl gameNameEdit; /* +5054 g_UiRequiredTextEditControlVtable: 20-char game name edit (action 0x2008 FrontendNetworkSettings_SetGameName). */
    uint32_t gameNameEdit_trailing[5]; /* +50D4: template dwords behind the control */
    UiRangeSliderControl maxPlayersSlider; /* +50E8 g_UiRangeSliderControlVtable: Slider 2..8 (action 0x2007) setting the session player count. */
    UiFocusProxyControl maxPlayersValueText; /* +5150 g_UiFocusProxyControlVtable: Text display bound to g_FrontendNetworkPlayerCountTextUtf16 showing the player count. */
    UiRangeSliderControl networkSpeedSlider; /* +51AC g_UiRangeSliderControlVtable: Slider 1..7 (action 0x204D) setting g_SessionNetworkTickInterval and its label; 'speed' reading inferred. */
    UiFocusProxyControl networkSpeedValueText; /* +5214 g_UiFocusProxyControlVtable: Text display bound to g_FrontendNetworkPlayerCountLabelUtf16 (label built by action 0x204D). */
    UiFocusProxyControl maxPlayersLabel; /* +5270 g_UiFocusProxyControlVtable: Caption (text 0x210A) left of the player count slider. */
    UiFocusProxyControl networkSpeedLabel; /* +52CC g_UiFocusProxyControlVtable: Caption (text 0x210D) left of the network tick-interval slider. */
    UiFocusProxyControl gameNameLabel; /* +5328 g_UiFocusProxyControlVtable: Caption (text 0x210B) above the game name edit. */
    UiImagePanelControl hostLobbyPage; /* +5384 g_UiImagePanelControlVtable: Page-stack page 3: host waits for joining players, can kick them and start the game. */
    UiFocusProxyControl hostLobbyTitle; /* +53E0 g_UiFocusProxyControlVtable: Page title caption (text 0x2119) of the host lobby page. */
    UiFramedTextButtonControl hostLobbyBackButton; /* +543C g_UiFramedTextButtonControlVtable: Action 0x2005: drops to local mode, resets the roster and returns to the host setup page. */
    UiFramedTextButtonControl hostLobbyKickPlayerButton; /* +549C g_UiFramedTextButtonControlVtable: Action 0x200B: expires/removes the selected joined player. */
    UiFramedTextButtonControl hostLobbyStartButton; /* +54FC g_UiFramedTextButtonControlVtable: Action 0x2006: seeds the random streams and starts the network game. */
    UiScrollableControl hostLobbyPlayerScrollBox; /* +555C g_UiScrollableControlVtable: Scroll frame around the host lobby player list. */
    UiListControl hostLobbyPlayerList; /* +55EC g_UiListControlVtable: List of joined players (g_FrontendPlayerRuntimeRecordPointers32, action 0x200C toggles Kick). */
    uint32_t hostLobbyPlayerList_trailing[4]; /* +5660: template dwords behind the control */
    UiFocusProxyControl hostLobbyPlayerListLabel; /* +5670 g_UiFocusProxyControlVtable: Caption (text 0x2117) above the host lobby player list. */
    UiImagePanelControl clientLobbyPage; /* +56CC g_UiImagePanelControlVtable: Page-stack page 4 (join ack): client waits in the session and sees the player list. */
    UiFocusProxyControl clientLobbyTitle; /* +5728 g_UiFocusProxyControlVtable: Page title caption (text 0x211E) of the client lobby page. */
    UiFramedTextButtonControl clientLobbyLeaveButton; /* +5784 g_UiFramedTextButtonControlVtable: Action 0x200A: leaves the session and reopens the network game page (also used on timeout). */
    UiScrollableControl clientLobbyPlayerScrollBox; /* +57E4 g_UiScrollableControlVtable: Scroll frame around the client lobby player list. */
    UiListControl clientLobbyPlayerList; /* +5874 g_UiListControlVtable: List of session players (g_FrontendPlayerListRows) filled from host packets. */
    uint32_t clientLobbyPlayerList_trailing[4]; /* +58E8: template dwords behind the control */
    UiFocusProxyControl clientLobbyPlayerListLabel; /* +58F8 g_UiFocusProxyControlVtable: Caption (text 0x2117) above the client lobby player list. */
    /* Not in the original (open-thandor): the display mode kind choice of the display settings page, appended
       after the original nodes and linked in after displayResolutionGroup. */
    UiTitledWindowControl displayModeKindGroup; /* +5954 g_UiTitledWindowControlVtable: Titled box "Anzeigemodus:" (TEXT_ID_DISPLAY_MODE_KIND_TITLE). */
    UiTextButtonControl displayModeKindWindow; /* +59A8 g_UiTextButtonControlVtable: "Fenster" (action FRONTEND_ACTION_DISPLAY_MODE_KIND_WINDOW). */
    UiTextButtonControl displayModeKindBorderless; /* +5A08 g_UiTextButtonControlVtable: "Vollbildfenster" (action FRONTEND_ACTION_DISPLAY_MODE_KIND_BORDERLESS). */
    UiTextButtonControl displayModeKindFullscreen; /* +5A68 g_UiTextButtonControlVtable: "Vollbild" (action FRONTEND_ACTION_DISPLAY_MODE_KIND_FULLSCREEN). */
    /* Not in the original (open-thandor): the resolution choices as a scrollable list. displayResolutionGroup
       holds the scroll frame, the frame scrolls the row panel, the panel holds displayResolutionOption1..10
       followed by the extra rows (built at run time from displayResolutionOption1 when the page opens; zero
       in the template). */
    UiScrollableControl displayResolutionScrollBox; /* +5AC8 g_UiScrollableControlVtable: Scroll frame inside displayResolutionGroup (vertical bar only). */
    UiPanelControl displayResolutionRowPanel; /* +5B58 g_UiPanelControlVtable: Scrolled content: one radio row per resolution, size set at run time. */
    UiNumericPairTextButton displayResolutionExtraOptions[FRONTEND_DISPLAY_RESOLUTION_EXTRA_OPTIONS]; /* +5BB0 g_UiNumericPairTextButtonVtable: Resolution choices 11.. (action 0x2022). */
    /* Not in the original (open-thandor): the "Erweitert" settings page. frontendPageStack page 6 is
       displayPageStack, whose page 0 is displaySettingsPage ("Anzeige") and page 1 advancedSettingsPage; the
       options page gets a fourth button (advancedSettingsButton) below "Sound". */
    UiLayoutContainerControl<2> displayPageStack; /* +71A0 g_UiLayoutContainerControlVtable: Two-page stack in frontendPageStack page 6: display settings, advanced settings. */
    UiFramedTextButtonControl advancedSettingsButton; /* +71F8 g_UiFramedTextButtonControlVtable: "Erweitert" button of the options page (action FRONTEND_ACTION_OPEN_ADVANCED_SETTINGS). */
    UiImagePanelControl advancedSettingsPage; /* +7258 g_UiImagePanelControlVtable: displayPageStack page 1: GPU edges, UI scale, frame limit, VSync. */
    UiFramedTextButtonControl advancedSettingsBackButton; /* +72B4 g_UiFramedTextButtonControlVtable: Action 0x2010: returns to the options page. */
    UiFocusProxyControl advancedSettingsTitle; /* +7314 g_UiFocusProxyControlVtable: Page title "Erweiterte Einstellungen". */
    UiTitledWindowControl advancedEdgesGroup; /* +7370 g_UiTitledWindowControlVtable: Titled box "3D-Kanten:" ([graphics] gpu_rasterization). */
    UiTextButtonControl advancedEdgesSmooth; /* +73C4 g_UiTextButtonControlVtable: "Glatt" (action FRONTEND_ACTION_ADVANCED_EDGES). */
    UiTextButtonControl advancedEdgesExact; /* +7424 g_UiTextButtonControlVtable: "Original" (action FRONTEND_ACTION_ADVANCED_EDGES). */
    UiTitledWindowControl advancedUiScaleGroup; /* +7484 g_UiTitledWindowControlVtable: Titled box "UI-Skalierung:" ([graphics] ui_scale). */
    UiTextButtonControl advancedUiScaleAuto; /* +74D8 g_UiTextButtonControlVtable: "Auto" (action FRONTEND_ACTION_ADVANCED_UI_SCALE). */
    UiTextButtonControl advancedUiScale1; /* +7538 g_UiTextButtonControlVtable: "1x" (action FRONTEND_ACTION_ADVANCED_UI_SCALE). */
    UiTextButtonControl advancedUiScale2; /* +7598 g_UiTextButtonControlVtable: "2x" (action FRONTEND_ACTION_ADVANCED_UI_SCALE). */
    UiTextButtonControl advancedUiScale3; /* +75F8 g_UiTextButtonControlVtable: "3x" (action FRONTEND_ACTION_ADVANCED_UI_SCALE). */
    UiTitledWindowControl advancedFrameLimitGroup; /* +7658 g_UiTitledWindowControlVtable: Titled box "Bildratenbegrenzung:". */
    UiTextButtonControl advancedFrameLimitOff; /* +76AC g_UiTextButtonControlVtable: "Aus" (action FRONTEND_ACTION_ADVANCED_FRAME_LIMIT). */
    UiTextButtonControl advancedFrameLimit60; /* +770C g_UiTextButtonControlVtable: "60 Bilder/s" (action FRONTEND_ACTION_ADVANCED_FRAME_LIMIT). */
    UiTextButtonControl advancedFrameLimit120; /* +776C g_UiTextButtonControlVtable: "120 Bilder/s" (action FRONTEND_ACTION_ADVANCED_FRAME_LIMIT). */
    UiTextButtonControl advancedFrameLimit144; /* +77CC g_UiTextButtonControlVtable: "144 Bilder/s" (action FRONTEND_ACTION_ADVANCED_FRAME_LIMIT). */
    UiTextButtonControl advancedVsyncCheckbox; /* +782C g_UiTextButtonControlVtable: Checkbox "VSync" (action FRONTEND_ACTION_ADVANCED_VSYNC). */
    UiFocusProxyControl advancedNoteLabel; /* +788C g_UiFocusProxyControlVtable: Note under the boxes (text set when the page opens: software renderer / UI scale). */
} FrontendUiImage;
#define FRONTEND_UI(root, node) (&((FrontendUiImage *)(uintptr_t)(root))->node)
#define FRONTEND_UI_FIELD(root, node, offset, type) (*(type *)((uint8_t *)FRONTEND_UI(root, node) + (offset)))
#pragma pack(pop)

#endif /* THANDOR_UI_FRONTEND_TYPES_H */
