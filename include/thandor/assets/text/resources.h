/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/text/resources.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_TEXT_RESOURCES_H
#define THANDOR_ASSETS_TEXT_RESOURCES_H

#include <thandor/assets/text/types.h>
#include <thandor/core/types.h>
#include <thandor/core/contracts.h>

/* Text resource ids: a compact id (bits 16-23 zero) is page << 8 | index, an extended id page << 16 | index
   with a 16-bit index. TEXT_RESOURCE_ID_NONE resolves to the shared empty string; FontRuntime_Init
   fills the whole override table with it. */
inline constexpr uint32_t TEXT_RESOURCE_ID_NONE = 0xFFFFFFFF;
inline constexpr int TEXT_RESOURCE_OVERRIDE_CAPACITY = 0x1000; /* entries of TextResourceOverrideTable */
/* Page 0x30 holds the text of the loaded level (its .str entry, see TextResourcePage_LoadCompatibilityAliases):
   index 0 the title, 1 the description, 2..15 fourteen further description lines. */
inline constexpr int TEXT_RESOURCE_PAGE_LEVEL = 0x30;
inline constexpr int TEXT_ID_LEVEL_PAGE_TITLE = 0x3000;
inline constexpr int TEXT_ID_LEVEL_PAGE_DESCRIPTION = 0x3001;
inline constexpr int TEXT_ID_LEVEL_PAGE_EXTRA_LINES = 0x3002;
inline constexpr int TEXT_LEVEL_EXTRA_LINE_COUNT = 14;
/* Not in the original: texts of open-thandor's own controls (literal German, and English for every other language:
   project_texts_en.cpp, TextResource_ProjectTextsAreGerman), resolved by TextResource_TryResolve before the override
   table. Page 0x7F (extended ids) is not used by the game data. */
inline constexpr int TEXT_ID_PROJECT_BASE = 0x7F0000;
#define TEXT_ID_DISPLAY_MODE_KIND_TITLE (TEXT_ID_PROJECT_BASE + 0) /* "Anzeigemodus:" */
#define TEXT_ID_DISPLAY_MODE_KIND_WINDOW (TEXT_ID_PROJECT_BASE + 1) /* "Fenster" */
#define TEXT_ID_DISPLAY_MODE_KIND_BORDERLESS (TEXT_ID_PROJECT_BASE + 2) /* "Vollbildfenster" */
#define TEXT_ID_DISPLAY_MODE_KIND_FULLSCREEN (TEXT_ID_PROJECT_BASE + 3) /* "Vollbild" */
/* the options page's renamed "Graphik" button and display settings page title, and the advanced settings page */
#define TEXT_ID_OPTIONS_DISPLAY_BUTTON (TEXT_ID_PROJECT_BASE + 4) /* "Anzeige" */
#define TEXT_ID_FRONTEND_DISPLAY_TITLE (TEXT_ID_PROJECT_BASE + 5) /* "Anzeigeeinstellungen" */
#define TEXT_ID_OPTIONS_ADVANCED_BUTTON (TEXT_ID_PROJECT_BASE + 6) /* "Erweitert" */
#define TEXT_ID_ADVANCED_SETTINGS_TITLE (TEXT_ID_PROJECT_BASE + 7) /* "Erweiterte Einstellungen" */
#define TEXT_ID_ADVANCED_EDGES_TITLE (TEXT_ID_PROJECT_BASE + 8) /* "3D-Kanten:" */
#define TEXT_ID_ADVANCED_EDGES_SMOOTH (TEXT_ID_PROJECT_BASE + 9) /* "Glatt" */
#define TEXT_ID_ADVANCED_EDGES_EXACT (TEXT_ID_PROJECT_BASE + 10) /* "Original" */
#define TEXT_ID_ADVANCED_UI_SCALE_TITLE (TEXT_ID_PROJECT_BASE + 11) /* "UI-Skalierung:" */
#define TEXT_ID_ADVANCED_UI_SCALE_AUTO (TEXT_ID_PROJECT_BASE + 12) /* "Auto" */
#define TEXT_ID_ADVANCED_UI_SCALE_1 (TEXT_ID_PROJECT_BASE + 13) /* "1x" (2x, 3x follow) */
#define TEXT_ID_ADVANCED_FRAME_LIMIT_TITLE (TEXT_ID_PROJECT_BASE + 16) /* "Bildratenbegrenzung:" */
#define TEXT_ID_ADVANCED_FRAME_LIMIT_OFF (TEXT_ID_PROJECT_BASE + 17) /* "Aus" (60, 120, 144 Bilder/s follow) */
#define TEXT_ID_ADVANCED_VSYNC (TEXT_ID_PROJECT_BASE + 21) /* "VSync" */
#define TEXT_ID_ADVANCED_NOTE_SOFTWARE (TEXT_ID_PROJECT_BASE + 22) /* edges and UI scale need a GPU renderer */
/* the effective UI scale when it differs from the chosen one, or the one auto chose; written by
   TextResource_SetUiScaleNote */
#define TEXT_ID_ADVANCED_NOTE_UI_SCALE (TEXT_ID_PROJECT_BASE + 23)
/* the campaign map (docs/plans/campaign_map.md 5.5): status line templates of the selected mission. "%u" stands
   for a mission number, "%s" for a mission title; TextResource_SetCampaignMapLine fills them in. */
#define TEXT_ID_CAMPAIGN_STATUS_DONE (TEXT_ID_PROJECT_BASE + 24) /* "Abgeschlossen - erneut spielbar." */
#define TEXT_ID_CAMPAIGN_STATUS_DONE_CARRY (TEXT_ID_PROJECT_BASE + 25) /* the same + " Uebernimmt Einheiten aus Mission %u (%s)." */
#define TEXT_ID_CAMPAIGN_STATUS_UNLOCKED_FIRST (TEXT_ID_PROJECT_BASE + 26) /* "Freigeschaltet" (the first mission) */
#define TEXT_ID_CAMPAIGN_STATUS_UNLOCKED (TEXT_ID_PROJECT_BASE + 27) /* "Freigeschaltet - folgt auf Mission %u (%s)" */
#define TEXT_ID_CAMPAIGN_STATUS_UNLOCKED_BRANCH (TEXT_ID_PROJECT_BASE + 28) /* "Freigeschaltet - alternativer Zweig nach Mission %u (%s)" */
#define TEXT_ID_CAMPAIGN_STATUS_LOCKED (TEXT_ID_PROJECT_BASE + 29) /* "Gesperrt - folgt auf Mission %u (%s)" */
/* a carry target without a valid snapshot (Start disabled); both "%u" are the carry source */
#define TEXT_ID_CAMPAIGN_STATUS_NEEDS_CARRY (TEXT_ID_PROJECT_BASE + 30) /* "Benoetigt die Einheiten aus Mission %u (%s) - Mission %u gewinnen." */
#define TEXT_ID_CAMPAIGN_STATUS_NETWORK_CARRY (TEXT_ID_PROJECT_BASE + 31) /* "Im Netzwerkspiel nur ueber Mission %u erreichbar." */
#define TEXT_ID_CAMPAIGN_MISSION_HEADING (TEXT_ID_PROJECT_BASE + 32) /* "Mission %u: %s" (the selected mission's line) */
/* the map's one legend item, shown only when the campaign has a carry-over edge (plan O2) */
#define TEXT_ID_CAMPAIGN_LEGEND_CARRY (TEXT_ID_PROJECT_BASE + 33) /* "Einheitenuebernahme" */
/* short names of the stock campaigns for the button row, see TextResource_CampaignShortName */
#define TEXT_ID_CAMPAIGN_SHORT_NAME_TUTORIAL (TEXT_ID_PROJECT_BASE + 34) /* "Tutorial" (tutorial.cgn) */
#define TEXT_ID_CAMPAIGN_SHORT_NAME_HANSOLO (TEXT_ID_PROJECT_BASE + 35) /* "Die Invasion" (hansolo.cgn) */
#define TEXT_ID_CAMPAIGN_SHORT_NAME_NIMM2 (TEXT_ID_PROJECT_BASE + 36) /* "Kyrrdon" (nimm2.cgn) */
#define TEXT_ID_CAMPAIGN_SHORT_NAME_LUKE (TEXT_ID_PROJECT_BASE + 37) /* "Piraten" (luke.cgn) */
/* the two filled-in lines of the campaign map, written by TextResource_SetCampaignMapLine */
#define TEXT_ID_CAMPAIGN_MAP_HEADING_LINE (TEXT_ID_PROJECT_BASE + 38)
#define TEXT_ID_CAMPAIGN_MAP_STATUS_LINE (TEXT_ID_PROJECT_BASE + 39)
/* the labels of the campaign tab's six campaign buttons ("<short name> <done>/<visible>"), written by
   TextResource_SetCampaignButtonLabel; button i shows TEXT_ID_CAMPAIGN_BUTTON_LABEL_0 + i */
#define TEXT_ID_CAMPAIGN_BUTTON_LABEL_0 (TEXT_ID_PROJECT_BASE + 40)
inline constexpr int TEXT_ID_CAMPAIGN_BUTTON_LABEL_COUNT = 6;
inline constexpr int TEXT_ID_CAMPAIGN_BUTTON_LABEL_UNITS = 64; /* code units of each label buffer, terminator included */
inline constexpr int TEXT_ID_PROJECT_COUNT = 46;
/* TEXT_ID_PROJECT_BASE + 46..49 are reserved for the language choice of the advanced settings page. */

/* Not in the original: sets the text of TEXT_ID_ADVANCED_NOTE_UI_SCALE (Latin-1, cut to its buffer). */
void TextResource_SetUiScaleNote(const char *text);

/* Not in the original: fills in the campaign map template templateId (a TEXT_ID_CAMPAIGN_STATUS_* or
   TEXT_ID_CAMPAIGN_MISSION_HEADING text) and stores the result as the text of lineId
   (TEXT_ID_CAMPAIGN_MAP_HEADING_LINE or TEXT_ID_CAMPAIGN_MAP_STATUS_LINE; any other id does nothing). Every "%u"
   becomes missionNumber in decimal, every "%s" missionTitle as plain text (rich-text commands and code units above
   0xFF dropped, leading blanks trimmed; nullptr gives an empty title). The result is cut to the line's buffer.
   Returns lineId. */
TextResourceId TextResource_SetCampaignMapLine(TextResourceId lineId,TextResourceId templateId,uint32_t missionNumber,
                                               uint16_t *missionTitle);

/* Not in the original: the short name of a stock campaign for the campaign map's button row by the base name of its
   .cgn file (no directory, no extension; ASCII case ignored), e.g. "hansolo" -> TEXT_ID_CAMPAIGN_SHORT_NAME_HANSOLO.
   TEXT_RESOURCE_ID_NONE for any other campaign and when the game texts are not German (the bound campaign title
   page has no German locale block; the short names exist in German only): the caller then shows the full campaign
   title (TEXT_ID_CAMPAIGN_TITLE_BASE + index) cut to the button width (plan 5.5, Q8). */
TextResourceId TextResource_CampaignShortName(uint16_t *cgnBaseName);

/* Not in the original: stores text (plain UTF-16, cut to TEXT_ID_CAMPAIGN_BUTTON_LABEL_UNITS - 1 code units) as the
   label of campaign button buttonIndex (0..TEXT_ID_CAMPAIGN_BUTTON_LABEL_COUNT - 1) and returns its text id
   TEXT_ID_CAMPAIGN_BUTTON_LABEL_0 + buttonIndex; any other index gives TEXT_RESOURCE_ID_NONE. */
TextResourceId TextResource_SetCampaignButtonLabel(int buttonIndex,const uint16_t *text);

/* Not in the original: the key binding list (ui/core/key_bindings.h): the page title, the four section titles and
   one action text per binding row (German; TextResource_SetKeyBindingTextsEnglish switches them to English). Ids
   TEXT_ID_PROJECT_BASE + 46..49 belong to another project text block. */
#define TEXT_ID_KEY_BINDING_BASE (TEXT_ID_PROJECT_BASE + 50)
#define TEXT_ID_KEYS_TITLE (TEXT_ID_KEY_BINDING_BASE + 0)
#define TEXT_ID_KEYS_SECTION_GENERAL (TEXT_ID_KEY_BINDING_BASE + 1)
#define TEXT_ID_KEYS_SECTION_UNITS (TEXT_ID_KEY_BINDING_BASE + 2)
#define TEXT_ID_KEYS_SECTION_CAMERA (TEXT_ID_KEY_BINDING_BASE + 3)
#define TEXT_ID_KEYS_SECTION_MENUS (TEXT_ID_KEY_BINDING_BASE + 4)
#define TEXT_ID_KEYS_GAME_MENU (TEXT_ID_KEY_BINDING_BASE + 5)
#define TEXT_ID_KEYS_OBJECTIVES (TEXT_ID_KEY_BINDING_BASE + 6)
#define TEXT_ID_KEYS_SAVE (TEXT_ID_KEY_BINDING_BASE + 7)
#define TEXT_ID_KEYS_QUIT_MENU (TEXT_ID_KEY_BINDING_BASE + 8)
#define TEXT_ID_KEYS_LEAVE_GAME (TEXT_ID_KEY_BINDING_BASE + 9)
#define TEXT_ID_KEYS_PAUSE (TEXT_ID_KEY_BINDING_BASE + 10)
#define TEXT_ID_KEYS_FASTER (TEXT_ID_KEY_BINDING_BASE + 11)
#define TEXT_ID_KEYS_SLOWER (TEXT_ID_KEY_BINDING_BASE + 12)
#define TEXT_ID_KEYS_SIDE_PANEL (TEXT_ID_KEY_BINDING_BASE + 13)
#define TEXT_ID_KEYS_CHAT_LINE (TEXT_ID_KEY_BINDING_BASE + 14)
#define TEXT_ID_KEYS_MESSAGE_WINDOW (TEXT_ID_KEY_BINDING_BASE + 15)
#define TEXT_ID_KEYS_SCORE_OVERVIEW (TEXT_ID_KEY_BINDING_BASE + 16)
#define TEXT_ID_KEYS_INFO_LINE (TEXT_ID_KEY_BINDING_BASE + 17)
#define TEXT_ID_KEYS_SCREENSHOT (TEXT_ID_KEY_BINDING_BASE + 18)
#define TEXT_ID_KEYS_NOTIFICATION_GO (TEXT_ID_KEY_BINDING_BASE + 19)
#define TEXT_ID_KEYS_NOTIFICATION_BACK (TEXT_ID_KEY_BINDING_BASE + 20)
#define TEXT_ID_KEYS_GROUP_RECALL (TEXT_ID_KEY_BINDING_BASE + 21)
#define TEXT_ID_KEYS_GROUP_ADD_TO_SELECTION (TEXT_ID_KEY_BINDING_BASE + 22)
#define TEXT_ID_KEYS_GROUP_STORE (TEXT_ID_KEY_BINDING_BASE + 23)
#define TEXT_ID_KEYS_GROUP_ADD_SELECTION (TEXT_ID_KEY_BINDING_BASE + 24)
#define TEXT_ID_KEYS_STOP (TEXT_ID_KEY_BINDING_BASE + 25)
#define TEXT_ID_KEYS_STOP_MOVEMENT (TEXT_ID_KEY_BINDING_BASE + 26)
#define TEXT_ID_KEYS_CANCEL_ATTACK (TEXT_ID_KEY_BINDING_BASE + 27)
#define TEXT_ID_KEYS_SELF_DESTRUCT (TEXT_ID_KEY_BINDING_BASE + 28)
#define TEXT_ID_KEYS_SELECT_AIRPORTS (TEXT_ID_KEY_BINDING_BASE + 29)
#define TEXT_ID_KEYS_UPGRADE_PAGE (TEXT_ID_KEY_BINDING_BASE + 30)
#define TEXT_ID_KEYS_CAMERA_SCROLL (TEXT_ID_KEY_BINDING_BASE + 31)
#define TEXT_ID_KEYS_CAMERA_TILT (TEXT_ID_KEY_BINDING_BASE + 32)
#define TEXT_ID_KEYS_CAMERA_ROTATE (TEXT_ID_KEY_BINDING_BASE + 33)
#define TEXT_ID_KEYS_CAMERA_ZOOM (TEXT_ID_KEY_BINDING_BASE + 34)
#define TEXT_ID_KEYS_CAMERA_SELECTION (TEXT_ID_KEY_BINDING_BASE + 35)
#define TEXT_ID_KEYS_CAMERA_HQ (TEXT_ID_KEY_BINDING_BASE + 36)
#define TEXT_ID_KEYS_CAMERA_NOTIFICATION (TEXT_ID_KEY_BINDING_BASE + 37)
#define TEXT_ID_KEYS_CAMERA_FREE (TEXT_ID_KEY_BINDING_BASE + 38)
#define TEXT_ID_KEYS_MENU_LEAVE (TEXT_ID_KEY_BINDING_BASE + 39)
#define TEXT_ID_KEYS_UI_NEXT_CONTROL (TEXT_ID_KEY_BINDING_BASE + 40)
#define TEXT_ID_KEYS_UI_PRESS_BUTTON (TEXT_ID_KEY_BINDING_BASE + 41)
#define TEXT_ID_KEYS_UI_CONFIRM_CANCEL (TEXT_ID_KEY_BINDING_BASE + 42)
#define TEXT_ID_KEYS_UI_LIST (TEXT_ID_KEY_BINDING_BASE + 43)
#define TEXT_ID_KEYS_UI_SLIDER (TEXT_ID_KEY_BINDING_BASE + 44)
#define TEXT_ID_KEYS_INTRO_SKIP_ALL (TEXT_ID_KEY_BINDING_BASE + 45)
#define TEXT_ID_KEYS_INTRO_SKIP (TEXT_ID_KEY_BINDING_BASE + 46)
#define TEXT_ID_KEYS_END_MOVIE_SKIP (TEXT_ID_KEY_BINDING_BASE + 47)
#define TEXT_ID_KEYS_END_MOVIE_SCREENSHOT (TEXT_ID_KEY_BINDING_BASE + 48)
inline constexpr int TEXT_ID_KEY_BINDING_COUNT = 49;

/* Not in the original: false (the default) resolves the TEXT_ID_KEYS_* texts in German, true in English. */
void TextResource_SetKeyBindingTextsEnglish(bool english);

bool TextResourcePage_LoadCompatibilityAliases(uint32_t levelTitleIndex,uint16_t *path);

bool TextResourcePage_Load(TextResourcePageIndex pageIndex,uint16_t *path,uintptr_t *outLocaleBlockOrError);

void TextResourceOverride_Register(TextResourceId resourceId,uint16_t *text);

bool TextResource_TryResolve(TextResourceId resourceId,uint16_t **outText);

/* TextResource_TryResolve without the found flag (a missing text gives an empty string, not the sentinel) */
uint16_t *TextResource_Resolve(TextResourceId resourceId);

/* Not in the original: the text language (docs: options, language switch). Every loaded 'str' page keeps all its
   usable locale blocks prepared (TextResourcePage_Load), so the language can change at run time without loading or
   freeing anything: TextResource_SelectLanguage rebinds every loaded page.
   Country codes are telephone country codes (LocaleTelephoneCountryCode); 0 means "from Windows". */
inline constexpr int TEXT_RESOURCE_LANGUAGE_AUTO = 0;
inline constexpr int TEXT_RESOURCE_LANGUAGE_MAX = 5; /* de, en, fr, it, es */

/* Rebinds every loaded text page to the locale block of countryCode (0: the Windows default country), else the
   Great Britain block, else the page's first usable block, and uses countryCode for pages loaded later. Allocates
   nothing. Bindings that point into a text page (nested-stream patches, the mouse-help icons) must be applied
   again afterwards (CoreAssets_BindLanguageTexts; Game_ApplyLanguage does all of it). */
void TextResource_SelectLanguage(uint32_t countryCode);

/* The country code last passed to TextResource_SelectLanguage, else the thandor.ini setting
   (g_LocaleCountryCodeOverride); 0 = auto. */
uint32_t TextResource_SelectedLanguage();

/* The country code of the block bound for the menu texts (menue.str); before that page is loaded the country code
   a page would get (the selected one, or the Windows default for auto). */
uint32_t TextResource_ActiveLanguage();

/* Fills outCodes (capacity entries) with the languages menue.str has a usable block for, in the order German,
   English, French, Italian, Spanish (49, 44, 33, 39, 34); other country codes are not listed. Returns the count
   (0 before menue.str is loaded). */
int TextResource_AvailableLanguages(uint32_t *outCodes,int capacity);

/* True when the project texts (TEXT_ID_PROJECT_BASE..) are shown in German: the active language is German.
   Every other language shows them in English. */
bool TextResource_ProjectTextsAreGerman();

/* The English text of project text index (0..TEXT_ID_PROJECT_COUNT - 1), or nullptr where there is none (the
   texts filled in at run time; the German text is used). project_texts_en.cpp. */
uint16_t *TextResource_ProjectTextEnglish(uint32_t index);

/* Not in the original: binds an already loaded 'str' asset (allocated through g_MemoryApi, byteCount bytes) as page
   pageIndex like TextResourcePage_Load (which loads it from the packages and then calls this; path is only used in
   log lines). On failure the asset is released and the binding stays unchanged. */
bool TextResourcePage_BindAsset(TextResourcePageIndex pageIndex,uint16_t *path,TextResourceAssetHeader *allocation,
                                uint32_t byteCount,uintptr_t *outLocaleBlockOrError);

extern TextResourceOverrideTable *g_TextResourceOverrides;

#endif /* THANDOR_ASSETS_TEXT_RESOURCES_H */
