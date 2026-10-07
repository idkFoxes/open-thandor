/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/selftest/uitemplate_selftest.cpp
 * Project code (not in the original game)
 */

/* OPEN_THANDOR_SELFTEST=uitemplate: the safety net for retyping the UI template images (step 13, W3). It hashes
   the bytes of the five template images as initialised (InGameUiImage g_InGameRuntimeDefaultImageTemplate,
   FrontendUiImage g_FrontendRootInitializationTemplate, DisplaySettingsUiImage g_UiDisplaySettingsRootTemplate,
   FourValueDialogUiImage g_UiFourValueDialogTemplateImage, FatalErrorUiImage g_FatalErrorUiRootTemplateImage)
   and of an in-game root built from its template the way the game builds it: the dword copy of
   InGameSession_CreateRoot and the tree relocation of UiRootStack_Push (nextSibling cleared, then
   UiSerializedTree_Relocate with the root address as delta, which runs every node's relocate method). Not
   included: the texture binding and layout between the copy and the relocation (they need the game files and a
   display mode) and the anchor rectangle UiRootStack_Push computes from the framebuffer size. The gauges'
   tooltip texts resolve to an empty stream while relocating (no text resources are loaded in a self-test).

   What is hashed (FNV-1a over a token stream, one token per dword; every image size is a multiple of 4):
   - a dword that points into a known module object (the node vtables and the text buffers / scratch objects
     the templates reference, table g_UiTemplateKnownObjects) is hashed as that object's name and the byte
     offset into it, so the address itself (which differs between builds and compilers) does not count;
   - any other dword is hashed as its value. A value inside the executable image that resolves to no known
     object is counted as "unresolved" (its raw, build dependent value is hashed, so a new kind of template
     pointer shows up as a different hash between builds and in the unresolved count);
   - relocated copy: two copies are built at different addresses with the same random seed (sprite buttons
     draw a random start frame while relocating). A dword equal in both is hashed as above; a dword that
     differs by exactly the distance of the copies is a link into the image and is hashed as its offset from
     the image start; anything else counts as a mismatch.
   One line per image: name, size, pointers resolved to known objects, unresolved image values, hash; the
   relocated line also gives the relocated links and the mismatches. The output is independent of the run, the
   build and the compiler as long as the template bytes and the relocate methods stay the same. */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/selftest/selftest.h>
#include <thandor/assets/text/resources.h>
#include <thandor/core/math/random.h>
#include <thandor/ui/controls/container.h>
#include <thandor/ui/controls/gauge.h>
#include <thandor/ui/controls/panels.h>
#include <thandor/ui/controls/text_buttons.h>
#include <thandor/ui/dialogs/display_settings.h>
#include <thandor/ui/dialogs/fatal_error.h>
#include <thandor/ui/frontend/display_settings.h>
#include <thandor/ui/frontend/menu_room.h>
#include <thandor/ui/frontend/network.h>
#include <thandor/ui/frontend/player.h>
#include <thandor/ui/frontend/results.h>
#include <thandor/ui/frontend/ui_template.h>
#include <thandor/ui/ingame/hud.h>
#include <thandor/ui/ingame/root_frame.h>
#include <thandor/ui/ingame/ui_template.h>

/* The linker-defined start of the executable image (MinGW ld and MSVC link both define it); <windows.h> clashes
   with the project's own Win32 declarations, so the PE header fields are read by their fixed offsets. */
extern "C" uint8_t __ImageBase[];
constexpr auto UITEMPLATE_PE_HEADER_OFFSET_FIELD = 0x3C; /* IMAGE_DOS_HEADER.e_lfanew */
constexpr auto UITEMPLATE_PE_SIZE_OF_IMAGE_FIELD = 0x50; /* IMAGE_NT_HEADERS(32|64).OptionalHeader.SizeOfImage */
constexpr auto UITEMPLATE_PE_SIZE_OF_HEADERS_FIELD = 0x54; /* IMAGE_NT_HEADERS(32|64).OptionalHeader.SizeOfHeaders */

struct UiTemplateKnownObject {
    const char *name;
    const void *address;
    uint32_t size;
};

#define UITEMPLATE_OBJECT(object) {#object, &(object), (uint32_t)sizeof(object)}
/* a function a template dword points to (only its exact address resolves) */
#define UITEMPLATE_FUNCTION(function) {#function, reinterpret_cast<const void *>(&(function)), 0}

/* Every module object a template dword points to (vtables of the template nodes, text buffers and scratch
   objects bound to template controls, the results chart column callbacks). A pointer to anything else counts as
   unresolved. */
static const UiTemplateKnownObject g_UiTemplateKnownObjects[] = {
    UITEMPLATE_OBJECT(g_FrontendModelPointerContextVtable),
    UITEMPLATE_OBJECT(g_FrontendResultsTableVtable),
    UITEMPLATE_OBJECT(g_UiArmyMetricsPanelVtable),
    UITEMPLATE_OBJECT(g_UiCatalogEntryControlVtable),
    UITEMPLATE_OBJECT(g_UiCommandSpriteButtonControlVtable),
    UITEMPLATE_OBJECT(g_UiCommandSpriteButtonWithDetailsVtable),
    UITEMPLATE_OBJECT(g_UiCommandVisibilitySingleLineTextVtable),
    UITEMPLATE_OBJECT(g_UiCommandVisibilityWrappedTextVtable),
    UITEMPLATE_OBJECT(g_UiConditionalActionControlVtable),
    UITEMPLATE_OBJECT(g_UiFillPanelControlVtable),
    UITEMPLATE_OBJECT(g_UiFocusProxyControlVtable),
    UITEMPLATE_OBJECT(g_UiFormattedContainerVtable),
    UITEMPLATE_OBJECT(g_UiFramedTextButtonControlVtable),
    UITEMPLATE_OBJECT(g_UiGraphicsAdapterTextButtonVtable),
    UITEMPLATE_OBJECT(g_UiImageActionControlVtable),
    UITEMPLATE_OBJECT(g_UiImageControlVtable),
    UITEMPLATE_OBJECT(g_UiImagePanelControlVtable),
    UITEMPLATE_OBJECT(g_UiLayoutContainerControlVtable),
    UITEMPLATE_OBJECT(g_UiListControlVtable),
    UITEMPLATE_OBJECT(g_UiListOffsetControlVtable),
    UITEMPLATE_OBJECT(g_UiNineSlicePanelControlVtable),
    UITEMPLATE_OBJECT(g_UiNumericPairTextButtonVtable),
    UITEMPLATE_OBJECT(g_UiPanelControlVtable),
    UITEMPLATE_OBJECT(g_UiPayloadPairTextButtonVtable),
    UITEMPLATE_OBJECT(g_UiRangeSliderControlVtable),
    UITEMPLATE_OBJECT(g_UiRequiredTextEditControlVtable),
    UITEMPLATE_OBJECT(g_UiResizableWindowControlVtable),
    UITEMPLATE_OBJECT(g_UiScrollableControlVtable),
    UITEMPLATE_OBJECT(g_UiSelectionGeometryControlVtable),
    UITEMPLATE_OBJECT(g_UiSoftwareTexturePreviewControlVtable),
    UITEMPLATE_OBJECT(g_UiSpriteButtonControlVtable),
    UITEMPLATE_OBJECT(g_UiTextButtonControlVtable),
    UITEMPLATE_OBJECT(g_UiTextListControlVtable),
    UITEMPLATE_OBJECT(g_UiTitledWindowControlVtable),
    UITEMPLATE_OBJECT(g_UiTransferProgressGaugeVtable),
    UITEMPLATE_OBJECT(g_EmptyFrontendPlayerNameUtf16),
    UITEMPLATE_OBJECT(g_FrontendCurrentFactionPrimaryResourceTextUtf16),
    UITEMPLATE_OBJECT(g_FrontendNetworkPlayerCountTextUtf16),
    UITEMPLATE_OBJECT(g_FrontendNetworkSpeedLabelUtf16),
    UITEMPLATE_OBJECT(g_FrontendUiDisplayModeAndTaskAssignmentScratch),
    UITEMPLATE_OBJECT(g_InGameCountdownTextUtf16),
    UITEMPLATE_OBJECT(g_InGamePlayerStatusTextSlots),
    UITEMPLATE_FUNCTION(FrontendResultsGraph_DrawFactionWeightSumColumn),
    UITEMPLATE_FUNCTION(FrontendResultsGraph_DrawFactionWeightLane0Column),
    UITEMPLATE_FUNCTION(FrontendResultsGraph_DrawFactionWeightLane1Column),
};

/* Token tags, hashed in front of each dword's token. */
enum UiTemplateToken : uint32_t {
    UITEMPLATE_TOKEN_VALUE = 0,      /* plain value */
    UITEMPLATE_TOKEN_OBJECT = 1,     /* pointer into a known object: name, offset */
    UITEMPLATE_TOKEN_UNRESOLVED = 2, /* value inside the executable image, no known object */
    UITEMPLATE_TOKEN_LINK = 3,       /* relocated copy: pointer into the copy itself, offset */
    UITEMPLATE_TOKEN_MISMATCH = 4,   /* relocated copy: the two copies differ by something else */
};

struct UiTemplateHash {
    uint32_t hash;
    unsigned objectPointers;
    unsigned unresolved;
    unsigned links;
    unsigned mismatches;
};

static void UiTemplateHash_Byte(UiTemplateHash *state, uint8_t value)
{
    state->hash = (state->hash ^ value) * 16777619u;
}

static void UiTemplateHash_Dword(UiTemplateHash *state, uint32_t value)
{
    int i;
    for (i = 0; i < 4; i++) {
        UiTemplateHash_Byte(state, (uint8_t)(value >> (i * 8)));
    }
}

static uintptr_t g_UiTemplateImageStart;
static uintptr_t g_UiTemplateImageEnd;

static void UiTemplate_FindExecutableImage()
{
    const uint8_t *image = __ImageBase;
    int32_t ntHeadersOffset;
    uint32_t sizeOfImage;
    uint32_t sizeOfHeaders;
    memcpy(&ntHeadersOffset, image + UITEMPLATE_PE_HEADER_OFFSET_FIELD, 4);
    memcpy(&sizeOfImage, image + ntHeadersOffset + UITEMPLATE_PE_SIZE_OF_IMAGE_FIELD, 4);
    memcpy(&sizeOfHeaders, image + ntHeadersOffset + UITEMPLATE_PE_SIZE_OF_HEADERS_FIELD, 4);
    /* from the first section on: the headers are no pointer target, and the image base itself (0x10000000) is
       also a common anchor value (1/8 in Q31) */
    g_UiTemplateImageStart = reinterpret_cast<uintptr_t>(image) + sizeOfHeaders;
    g_UiTemplateImageEnd = reinterpret_cast<uintptr_t>(image) + sizeOfImage;
}

/* Hashes one dword that is not a link of a relocated copy (see the file comment). */
static void UiTemplateHash_Value(UiTemplateHash *state, uint32_t value)
{
    size_t index;
    for (index = 0; index < sizeof g_UiTemplateKnownObjects / sizeof g_UiTemplateKnownObjects[0]; index++) {
        const UiTemplateKnownObject *object = &g_UiTemplateKnownObjects[index];
        uintptr_t start = reinterpret_cast<uintptr_t>(object->address);
        /* the end address counts too (a pointer one past an object) */
        if ((uintptr_t)value >= start && (uintptr_t)value <= start + object->size) {
            const char *name;
            UiTemplateHash_Dword(state, UITEMPLATE_TOKEN_OBJECT);
            for (name = object->name; *name != 0; name++) {
                UiTemplateHash_Byte(state, (uint8_t)*name);
            }
            UiTemplateHash_Byte(state, 0);
            UiTemplateHash_Dword(state, (uint32_t)((uintptr_t)value - start));
            state->objectPointers++;
            return;
        }
    }
    if ((uintptr_t)value >= g_UiTemplateImageStart && (uintptr_t)value < g_UiTemplateImageEnd) {
        UiTemplateHash_Dword(state, UITEMPLATE_TOKEN_UNRESOLVED);
        UiTemplateHash_Dword(state, value);
        state->unresolved++;
        return;
    }
    UiTemplateHash_Dword(state, UITEMPLATE_TOKEN_VALUE);
    UiTemplateHash_Dword(state, value);
}

static void UiTemplateHash_Init(UiTemplateHash *state)
{
    memset(state, 0, sizeof *state);
    state->hash = 2166136261u;
}

static void UiTemplate_LogTemplate(const char *name, const void *image, uint32_t size)
{
    UiTemplateHash state;
    uint32_t offset;
    UiTemplateHash_Init(&state);
    for (offset = 0; offset < size; offset += 4) {
        uint32_t value;
        memcpy(&value, static_cast<const uint8_t *>(image) + offset, 4);
        UiTemplateHash_Value(&state, value);
    }
    Thandor_Log("uitemplate %s template: size=0x%X pointers=%u unresolved=%u hash=%08X", name, size,
                state.objectPointers, state.unresolved, state.hash);
}

/* The text resources are not loaded when a self-test runs, but the gauges (g_UiFormattedContainerVtable) resolve
   their tooltip text while relocating (UiFormattedContainer_RelocateWithPatchedTextPayloads; its id is the dword
   in front of the node). These ids resolve to an empty rich text stream through a temporary override table. */
static TextResourceOverrideTable g_UiTemplateTextOverrides;
static uint16_t g_UiTemplateEmptyText[1] = {0};

static void UiTemplate_InstallGaugeTextOverrides(const void *image, uint32_t size)
{
    const uint8_t *bytes = static_cast<const uint8_t *>(image);
    uint32_t gaugeVtable = Thandor_PointerToU32(&g_UiFormattedContainerVtable);
    unsigned count = 0;
    unsigned index;
    uint32_t offset;
    for (index = 0; index < TEXT_RESOURCE_OVERRIDE_CAPACITY; index++) {
        g_UiTemplateTextOverrides.resourceIds[index] = TEXT_RESOURCE_ID_NONE; /* never looked up here */
        g_UiTemplateTextOverrides.textPointers[index] = g_UiTemplateEmptyText;
    }
    for (offset = offsetof(UiNodeBase, vtable); offset + 4 <= size; offset += 4) {
        uint32_t value;
        uint32_t nodeOffset = offset - (uint32_t)offsetof(UiNodeBase, vtable);
        memcpy(&value, bytes + offset, 4);
        if (value == gaugeVtable && nodeOffset >= 4 && count < TEXT_RESOURCE_OVERRIDE_CAPACITY) {
            memcpy(&g_UiTemplateTextOverrides.resourceIds[count], bytes + nodeOffset - 4, 4);
            count++;
        }
    }
    g_TextResourceOverrides = &g_UiTemplateTextOverrides;
}

/* The in-game root as InGameSession_CreateRoot copies it and UiRootStack_Push relocates it. */
static void UiTemplate_BuildInGameRoot(InGameRuntimeRoot *root)
{
    const uint32_t *templateCursor = reinterpret_cast<const uint32_t *>(&g_InGameRuntimeDefaultImageTemplate);
    uint32_t *copyCursor = reinterpret_cast<uint32_t *>(root);
    unsigned remainingCount;
    for (remainingCount = sizeof(InGameRuntimeRoot) / 4; remainingCount != 0; remainingCount--) {
        *copyCursor = *templateCursor;
        templateCursor++;
        copyCursor++;
    }
    Random_SetBothSeeds(0x5EED);
    root->rootUi.base.nextSibling = UI_NODE_NONE;
    UiSerializedTree_Relocate(Thandor_PointerToI32(root), &root->rootUi.base);
}

static void UiTemplate_LogRelocatedInGameRoot()
{
    RandomGeneratorState savedRandom = g_RandomGeneratorState;
    TextResourceOverrideTable *savedOverrides = g_TextResourceOverrides;
    InGameRuntimeRoot *first = static_cast<InGameRuntimeRoot *>(malloc(sizeof(InGameRuntimeRoot)));
    InGameRuntimeRoot *second = static_cast<InGameRuntimeRoot *>(malloc(sizeof(InGameRuntimeRoot)));
    UiTemplateHash state;
    uint32_t distance;
    uint32_t offset;
    if (first == nullptr || second == nullptr) {
        Thandor_Log("uitemplate InGameUiImage relocated: allocation failed");
        free(first);
        free(second);
        return;
    }
    UiTemplate_InstallGaugeTextOverrides(&g_InGameRuntimeDefaultImageTemplate, sizeof(InGameUiImage));
    UiTemplate_BuildInGameRoot(first);
    UiTemplate_BuildInGameRoot(second);
    g_RandomGeneratorState = savedRandom;
    g_TextResourceOverrides = savedOverrides;
    distance = Thandor_PointerToU32(second) - Thandor_PointerToU32(first);
    UiTemplateHash_Init(&state);
    for (offset = 0; offset < sizeof(InGameRuntimeRoot); offset += 4) {
        uint32_t firstValue;
        uint32_t secondValue;
        memcpy(&firstValue, reinterpret_cast<const uint8_t *>(first) + offset, 4);
        memcpy(&secondValue, reinterpret_cast<const uint8_t *>(second) + offset, 4);
        if (firstValue == secondValue) {
            UiTemplateHash_Value(&state, firstValue);
        }
        else if (secondValue - firstValue == distance &&
                 firstValue - Thandor_PointerToU32(first) <= sizeof(InGameRuntimeRoot)) {
            UiTemplateHash_Dword(&state, UITEMPLATE_TOKEN_LINK);
            UiTemplateHash_Dword(&state, firstValue - Thandor_PointerToU32(first));
            state.links++;
        }
        else {
            UiTemplateHash_Dword(&state, UITEMPLATE_TOKEN_MISMATCH);
            UiTemplateHash_Dword(&state, offset);
            state.mismatches++;
        }
    }
    Thandor_Log("uitemplate InGameUiImage relocated: size=0x%X links=%u pointers=%u unresolved=%u mismatches=%u "
                "hash=%08X",
                (unsigned)sizeof(InGameRuntimeRoot), state.links, state.objectPointers, state.unresolved,
                state.mismatches, state.hash);
    free(first);
    free(second);
}

void Thandor_SelfTestUiTemplate()
{
    UiTemplate_FindExecutableImage();
    UiTemplate_LogTemplate("InGameUiImage", &g_InGameRuntimeDefaultImageTemplate, sizeof(InGameUiImage));
    UiTemplate_LogTemplate("FrontendUiImage", &g_FrontendRootInitializationTemplate, sizeof(FrontendUiImage));
    UiTemplate_LogTemplate("DisplaySettingsUiImage", &g_UiDisplaySettingsRootTemplate,
                           sizeof(DisplaySettingsUiImage));
    UiTemplate_LogTemplate("FourValueDialogUiImage", &g_UiFourValueDialogTemplateImage,
                           sizeof(FourValueDialogUiImage));
    UiTemplate_LogTemplate("FatalErrorUiImage", &g_FatalErrorUiRootTemplateImage, sizeof(FatalErrorUiImage));
    UiTemplate_LogRelocatedInGameRoot();
}
