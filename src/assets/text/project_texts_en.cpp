/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/text/project_texts_en.cpp
 * Project code (not in the original game)
 */

/* The English texts of open-thandor's own controls (TEXT_ID_PROJECT_BASE..), shown for every language but German
   (TextResource_TryResolve; the German texts are in resources.cpp). Same order and form as the German ones: the
   choices start with the opcode "colour from palette entry 3", the titles are plain. The texts filled in at run time
   (UI scale note, campaign map lines, campaign button labels) have no entry here; their buffers serve every
   language. */

#include <thandor/assets/text/resources.h>
#include <thandor/assets/text/richtext.h>

#include <array>
#include <cstddef>

namespace {

constexpr uint16_t PROJECT_TEXT_EN_BRIGHT = 0x8000 | RICHTEXT_OP_COLOR_PALETTE_3;

/* a UTF-16 stream from a literal: plain, or behind the bright colour opcode */
template <size_t N>
constexpr std::array<uint16_t, N> Plain(const char16_t (&text)[N])
{
  std::array<uint16_t, N> units{};
  for (size_t index = 0; index < N; index++) {
    units[index] = static_cast<uint16_t>(text[index]);
  }
  return units;
}

template <size_t N>
constexpr std::array<uint16_t, N + 1> Bright(const char16_t (&text)[N])
{
  std::array<uint16_t, N + 1> units{};
  units[0] = PROJECT_TEXT_EN_BRIGHT;
  for (size_t index = 0; index < N; index++) {
    units[index + 1] = static_cast<uint16_t>(text[index]);
  }
  return units;
}

/* not const: TextResource_Resolve hands out uint16_t * (the streams have no records to patch) */
auto s_DisplayModeTitle = Plain(u"Display mode:");
auto s_Window = Bright(u"Window");
auto s_Borderless = Bright(u"Borderless");
auto s_Fullscreen = Bright(u"Fullscreen");
auto s_Display = Plain(u"Display");
auto s_DisplaySettings = Plain(u"Display settings");
auto s_Advanced = Plain(u"Advanced");
auto s_AdvancedSettings = Plain(u"Advanced settings");
auto s_EdgesTitle = Plain(u"3D edges:");
auto s_Smooth = Bright(u"Smooth");
auto s_Original = Bright(u"Original");
auto s_UiScaleTitle = Plain(u"UI scale:");
auto s_Auto = Bright(u"Auto");
auto s_Scale1 = Bright(u"1x");
auto s_Scale2 = Bright(u"2x");
auto s_Scale3 = Bright(u"3x");
auto s_FrameLimitTitle = Plain(u"Frame rate limit:");
auto s_Off = Bright(u"Off");
auto s_Fps60 = Bright(u"60 fps");
auto s_Fps120 = Bright(u"120 fps");
auto s_Fps144 = Bright(u"144 fps");
auto s_Vsync = Bright(u"VSync");
auto s_NoteSoftware = Plain(u"3D edges and UI scale only work with Vulkan or DirectX 12");
auto s_CampaignDone = Plain(u"Completed - can be played again.");
auto s_CampaignDoneCarry = Plain(u"Completed - can be played again. Takes over the units from mission %u (%s).");
auto s_CampaignUnlockedFirst = Plain(u"Unlocked");
auto s_CampaignUnlocked = Plain(u"Unlocked - follows mission %u (%s)");
auto s_CampaignUnlockedBranch = Plain(u"Unlocked - alternative branch after mission %u (%s)");
auto s_CampaignLocked = Plain(u"Locked - follows mission %u (%s)");
auto s_CampaignNeedsCarry = Plain(u"Needs the units from mission %u (%s) - win mission %u.");
auto s_CampaignNetworkCarry = Plain(u"In a network game only reachable via mission %u.");
auto s_CampaignMissionHeading = Plain(u"Mission %u: %s");
auto s_CampaignLegendCarry = Plain(u"Unit carry-over");
auto s_CampaignTutorial = Plain(u"Tutorial");
auto s_CampaignInvasion = Plain(u"The Invasion");
auto s_CampaignKyrrdon = Plain(u"Kyrrdon");
auto s_CampaignPirates = Plain(u"Pirates");

/* index = resource id - TEXT_ID_PROJECT_BASE; nullptr: no English text (the German text or buffer is used) */
uint16_t *const s_ProjectTextsEnglish[] = {
    s_DisplayModeTitle.data(), s_Window.data(), s_Borderless.data(), s_Fullscreen.data(),
    s_Display.data(), s_DisplaySettings.data(), s_Advanced.data(), s_AdvancedSettings.data(),
    s_EdgesTitle.data(), s_Smooth.data(), s_Original.data(), s_UiScaleTitle.data(),
    s_Auto.data(), s_Scale1.data(), s_Scale2.data(), s_Scale3.data(),
    s_FrameLimitTitle.data(), s_Off.data(), s_Fps60.data(), s_Fps120.data(),
    s_Fps144.data(), s_Vsync.data(), s_NoteSoftware.data(), nullptr /* UI scale note, TextResource_SetUiScaleNote */,
    s_CampaignDone.data(), s_CampaignDoneCarry.data(), s_CampaignUnlockedFirst.data(), s_CampaignUnlocked.data(),
    s_CampaignUnlockedBranch.data(), s_CampaignLocked.data(), s_CampaignNeedsCarry.data(),
    s_CampaignNetworkCarry.data(), s_CampaignMissionHeading.data(), s_CampaignLegendCarry.data(),
    s_CampaignTutorial.data(), s_CampaignInvasion.data(), s_CampaignKyrrdon.data(), s_CampaignPirates.data()
    /* 38..45: the filled-in campaign map lines and button labels */};

} // namespace

uint16_t *TextResource_ProjectTextEnglish(uint32_t index)
{
  if (index >= sizeof(s_ProjectTextsEnglish) / sizeof(s_ProjectTextsEnglish[0])) {
    return nullptr;
  }
  return s_ProjectTextsEnglish[index];
}
