/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/selftest/inifixtures_selftest.cpp
 * Project code (not in the original game)
 */

/* OPEN_THANDOR_SELFTEST=inifixtures: thandor.ini files as real versions wrote them, and broken ones, through the
   real load and save path (PersistentSettings_Load / _Read / _GetRegionOrFallback / _Flush). The fixtures are the
   files of tools/test/fixtures/ini (OPEN_THANDOR_INI_FIXTURES names another directory; by default the directory is
   looked up from the executable directory and the current directory upwards). Every <name>.expect there is one
   case: its fixture (<name>.ini, <name>.dat or the "source:" file) is copied as thandor.ini (a .dat as thandor.dat)
   into a work directory of its own, which becomes the current directory and, for the time of the test, the
   executable directory (g_ExecutableDirectoryUtf16), so the loader's fallback next to thandor.exe never reaches the
   player's settings. Then: Load, check the values; Flush (forced dirty) writes thandor.ini there, check the
   "preserved:" / "absent:" lines in it; Load again, every key must read as after the first load (round trip);
   Flush again, the text must not change (stable). Work directories: OPEN_THANDOR_INI_OUT, else
   %TEMP%\open-thandor-inifixtures-<pid> (removed at the end when nothing failed).

   .expect lines ('#' starts a comment line):
     section.key = value           the key reads as this value, written as FormatIni writes it (enum names,
                                   true/false, 0x.. for option bits, quoted UTF-8 names)
     section.key = <default>       the key reads as its default (not loaded, or loaded with the default value)
     section.key = a | b           either (behaviour that is allowed to differ)
     others = <default>            every key of the table below that the file does not list reads as its default
     preserved: line               after the save, thandor.ini has this line (trimmed); "preserved: [sec] line"
                                   requires it inside section [sec], "preserved: [sec]" the section header
     order: a < b                  after the save, the first line matching a ("preserved:" form) comes before the
                                   first one matching b
     absent: text                  after the save, no line of thandor.ini contains text
     absent-file: name             after the save, there is no file name in the work directory
     check: readonly_retry         a change saved while thandor.ini is read-only fails; once it is writable
                                   again, the next Flush must write it (the settings stayed dirty)
     source: file                  the fixture file (default <name>.ini or <name>.dat)
     setup: stale_tmp              a thandor.ini.tmp with junk lies in the work directory before the load
     include: file.expect          the lines of another .expect
   A <name>.downgrade file next to a fixture holds the same lines for an older build (the key names of 1.0.7 / dev:
   language_country_code, [reserved] dword_50..58); it is not a case here, tools/test/ini_downgrade.py runs it
   as <name>.expect with the older build's self-test.
   A line prefixed "pending A:" or "pending B:" (also "pending A+B:") is behaviour that only holds once the
   robustness branch (A: unknown keys kept, UTF-16 / CR-only files, unclosed headers, range checks, no .dat bytes
   0x50..0x58) or the atomic-save branch (B) is merged; a failing pending check is logged as "expected-fail" and does
   not fail the test, a passing one as "pending ... passes". OPEN_THANDOR_INI_STRICT=1 counts failing pending checks
   as failures (after the merge). OPEN_THANDOR_INI_FIXTURE=name[,name...] runs only those cases.
   Result line: "inifixtures: ok, ..." or "inifixtures: FAILED, ...". */

#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <new>
#include <sstream>
#include <string>
#include <system_error>
#include <vector>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/selftest/selftest.h>
#include <thandor/ui/core/key_bindings_ini.h>

namespace fs = std::filesystem;

namespace {

/* The keys of thandor.ini as this version names them (deliberately a copy of persistent.cpp's key table: a
   renamed key fails here). defaultValue: what the game reads when the key is missing. */
enum class IniFixtureKind { Uint, Int, Hex, Bool, Bit, Gain, Enum, Name, Raw, Language };

struct IniFixtureKey {
    const char *section;
    const char *key;
    uint32_t offset;
    IniFixtureKind kind;
    uint32_t defaultValue; /* Bit: the whole dword */
    uint32_t mask;         /* Bit */
    std::vector<const char *> names; /* Enum */
};

const std::vector<IniFixtureKey> &IniFixture_Keys()
{
    using K = IniFixtureKind;
    const uint32_t soundDefault = ToBits(PERSISTENT_SOUND_OPTION_DEFAULT);
    static const std::vector<IniFixtureKey> keys = {
        {"display", "adapter", PERSISTENT_SETTING_ADAPTER_INDEX, K::Uint, 0, 0, {}},
        {"display", "width", PERSISTENT_SETTING_DISPLAY_WIDTH, K::Uint, 640, 0, {}},
        {"display", "height", PERSISTENT_SETTING_DISPLAY_HEIGHT, K::Uint, 480, 0, {}},
        {"display", "bits_per_pixel", PERSISTENT_SETTING_BITS_PER_PIXEL, K::Uint, PERSISTENT_DEFAULT_BITS_PER_PIXEL, 0, {}},
        {"display", "renderer", PERSISTENT_SETTING_RENDERER, K::Enum, PERSISTENT_RENDERER_VULKAN, 0,
         {"vulkan", "d3d12", "software"}},
        {"display", "display_mode", PERSISTENT_SETTING_DISPLAY_MODE_KIND, K::Enum, PERSISTENT_DISPLAY_MODE_FULLSCREEN, 0,
         {"fullscreen", "borderless", "window"}},
        {"graphics", "shading", PERSISTENT_SETTING_SHADING_ENABLED, K::Bool, 1, 0, {}},
        {"graphics", "shading_grid_half_size", PERSISTENT_SETTING_SHADING_GRID_HALF_SIZE, K::Uint,
         PERSISTENT_DEFAULT_SHADING_GRID_HALF_SIZE, 0, {}},
        {"graphics", "shading_texture_size", PERSISTENT_SETTING_SHADING_TEXTURE_DIMENSION, K::Uint,
         PERSISTENT_DEFAULT_SHADING_TEXTURE_DIMENSION, 0, {}},
        {"graphics", "shading_depth", PERSISTENT_SETTING_SHADING_SUBRESOURCE_COUNT, K::Uint,
         PERSISTENT_DEFAULT_SHADING_SUBRESOURCE_COUNT, 0, {}},
        {"graphics", "texture_quality", PERSISTENT_SETTING_TEXTURE_QUALITY, K::Enum, TEXTURE_QUALITY_HIGH, 0,
         {"high", "medium", "low"}},
        {"graphics", "model_detail", PERSISTENT_SETTING_MODEL_LOD_DEPTH_THRESHOLD, K::Int,
         PERSISTENT_DEFAULT_MODEL_LOD_DEPTH_THRESHOLD, 0, {}},
        {"graphics", "gpu_rasterization", PERSISTENT_SETTING_GPU_RASTERIZATION, K::Enum,
         PERSISTENT_GPU_RASTERIZATION_SMOOTH, 0, {"smooth", "exact"}},
        {"graphics", "ui_scale", PERSISTENT_SETTING_UI_SCALE, K::Enum, PERSISTENT_UI_SCALE_AUTO, 0,
         {"auto", "1", "2", "3"}},
        {"graphics", "vsync", PERSISTENT_SETTING_VSYNC, K::Enum, PERSISTENT_VSYNC_ON, 0, {"on", "off"}},
        {"graphics", "frame_limit", PERSISTENT_SETTING_FRAME_LIMIT, K::Uint, PERSISTENT_FRAME_LIMIT_OFF, 0, {}},
        {"sound", "effects", PERSISTENT_SETTING_SOUND_OPTION_FLAGS, K::Bit, soundDefault,
         ToBits(PERSISTENT_SOUND_OPTION_EFFECTS), {}},
        {"sound", "music", PERSISTENT_SETTING_SOUND_OPTION_FLAGS, K::Bit, soundDefault,
         ToBits(PERSISTENT_SOUND_OPTION_MUSIC), {}},
        {"sound", "reverse_stereo", PERSISTENT_SETTING_SOUND_OPTION_FLAGS, K::Bit, soundDefault,
         ToBits(PERSISTENT_SOUND_OPTION_REVERSE_STEREO), {}},
        {"sound", "effects_volume", PERSISTENT_SETTING_EFFECTS_GAIN, K::Gain, PERSISTENT_DEFAULT_GAIN_Q15, 0, {}},
        {"sound", "music_volume", PERSISTENT_SETTING_MUSIC_GAIN, K::Gain, PERSISTENT_DEFAULT_GAIN_Q15, 0, {}},
        {"sound", "movie_volume", PERSISTENT_SETTING_MOVIE_DEFAULT_GAIN, K::Gain, PERSISTENT_DEFAULT_GAIN_Q15, 0, {}},
        {"sound", "movie_alternate_volume", PERSISTENT_SETTING_MOVIE_ALTERNATE_GAIN, K::Gain,
         PERSISTENT_DEFAULT_GAIN_Q15, 0, {}},
        {"game", "speed_percent", PERSISTENT_SETTING_GAME_SPEED_PERCENT, K::Uint, 100, 0, {}},
        {"game", "scroll_step", PERSISTENT_SETTING_CAMERA_SCROLL_STEP, K::Int, PERSISTENT_DEFAULT_CAMERA_SCROLL_STEP, 0, {}},
        {"game", "map_mouse_options", PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS, K::Hex, 0, 0, {}},
        {"game", "link_panel_options", PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS, K::Hex, 0, 0, {}},
        {"game", "player_name", PERSISTENT_SETTING_PLAYER_NAME, K::Name, 0, 0, {}},
        {"game", "game_name", PERSISTENT_SETTING_GAME_NAME, K::Name, 0, 0, {}},
        /* one dword, two keys: language (the name this version writes) and language_country_code (the number the
           1.0.7 and dev builds read and write, still read and written here) */
        {"game", "language_country_code", PERSISTENT_SETTING_LOCALE_COUNTRY_CODE, K::Uint, 0, 0, {}},
        {"game", "language", PERSISTENT_SETTING_LOCALE_COUNTRY_CODE, K::Language, 0, 0, {}},
        {"game", "pathing", PERSISTENT_SETTING_PATHING_MODE, K::Enum, PERSISTENT_PATHING_IMPROVED, 0,
         {"improved", "original"}},
        {"game", "objective_overlay", PERSISTENT_SETTING_OBJECTIVE_OVERLAY, K::Enum, PERSISTENT_OBJECTIVE_OVERLAY_ON, 0,
         {"on", "off"}},
        {"game", "autosave", PERSISTENT_SETTING_AUTOSAVE, K::Enum, PERSISTENT_AUTOSAVE_ON, 0, {"on", "off"}},
        {"game", "original_thandor", PERSISTENT_SETTING_ORIGINAL_THANDOR, K::Enum, PERSISTENT_ORIGINAL_THANDOR_OFF, 0,
         {"off", "on"}},
        {"network", "players", PERSISTENT_SETTING_NETWORK_PLAYER_COUNT, K::Uint, 4, 0, {}},
    };
    return keys;
}

/* What a key reads as: loaded or not, and the value as FormatIni writes it. */
struct IniFixtureValue {
    bool present;
    uint32_t value;
    std::string text;
};

bool operator==(const IniFixtureValue &a, const IniFixtureValue &b)
{
    return a.present == b.present && (!a.present || a.text == b.text);
}

std::string IniFixture_Utf16ToUtf8(const uint16_t *units, size_t count)
{
    std::string out;
    for (size_t index = 0; index < count && units[index] != 0; index++) {
        uint32_t codePoint = units[index];
        if (codePoint >= 0xD800 && codePoint < 0xDC00 && index + 1 < count && units[index + 1] >= 0xDC00 &&
            units[index + 1] < 0xE000) {
            codePoint = 0x10000 + ((codePoint - 0xD800) << 10) + (units[index + 1] - 0xDC00u);
            index++;
        }
        if (codePoint < 0x80) {
            out += (char)codePoint;
        }
        else if (codePoint < 0x800) {
            out += (char)(0xC0 | (codePoint >> 6));
            out += (char)(0x80 | (codePoint & 0x3F));
        }
        else if (codePoint < 0x10000) {
            out += (char)(0xE0 | (codePoint >> 12));
            out += (char)(0x80 | ((codePoint >> 6) & 0x3F));
            out += (char)(0x80 | (codePoint & 0x3F));
        }
        else {
            out += (char)(0xF0 | (codePoint >> 18));
            out += (char)(0x80 | ((codePoint >> 12) & 0x3F));
            out += (char)(0x80 | ((codePoint >> 6) & 0x3F));
            out += (char)(0x80 | (codePoint & 0x3F));
        }
    }
    return out;
}

std::string IniFixture_Format(const char *format, uint32_t value)
{
    char text[32];
    snprintf(text, sizeof text, format, value);
    return text;
}

/* The key through the game's accessors: two different caller defaults tell whether it was loaded. */
IniFixtureValue IniFixture_ReadKey(const IniFixtureKey &key)
{
    using K = IniFixtureKind;
    IniFixtureValue result = {false, 0, "<absent>"};
    if (key.kind == K::Name) {
        static uint16_t fallback[PERSISTENT_SETTINGS_NAME_BYTES / 2];
        void *region = PersistentSettings_GetRegionOrFallback(PERSISTENT_SETTINGS_NAME_BYTES, fallback, key.offset);
        if (region != fallback) {
            uint16_t units[PERSISTENT_SETTINGS_NAME_BYTES / 2];
            memcpy(units, region, sizeof units);
            result = {true, 0, "\"" + IniFixture_Utf16ToUtf8(units, PERSISTENT_SETTINGS_NAME_BYTES / 2) + "\""};
        }
        return result;
    }
    const uint32_t first = PersistentSettings_Read(0xA5A5A5A5u, key.offset);
    const uint32_t second = PersistentSettings_Read(0x5A5A5A5Au, key.offset);
    if (first == 0xA5A5A5A5u && second == 0x5A5A5A5Au) {
        return result;
    }
    result.present = true;
    result.value = first;
    switch (key.kind) {
    case K::Int:
        result.text = IniFixture_Format("%d", first);
        break;
    case K::Hex:
    case K::Raw:
        result.text = IniFixture_Format("0x%X", first);
        break;
    case K::Bool:
        result.text = first <= 1 ? (first != 0 ? "true" : "false") : IniFixture_Format("%u", first);
        break;
    case K::Bit:
        result.text = (first & key.mask) != 0 ? "true" : "false";
        break;
    case K::Enum:
        result.text = first < key.names.size() ? key.names[first] : IniFixture_Format("%u", first);
        break;
    case K::Language: {
        static const std::pair<uint32_t, const char *> languages[] = {{0, "auto"}, {49, "de"}, {44, "en"},
                                                                       {33, "fr"}, {39, "it"}, {34, "es"}};
        result.text = IniFixture_Format("%u", first);
        for (const auto &[countryCode, languageName] : languages) {
            if (countryCode == first) {
                result.text = languageName;
            }
        }
        break;
    }
    default:
        result.text = IniFixture_Format("%u", first);
        break;
    }
    return result;
}

bool IniFixture_IsDefault(const IniFixtureKey &key, const IniFixtureValue &value)
{
    if (!value.present) {
        return true;
    }
    if (key.kind == IniFixtureKind::Name) {
        return false;
    }
    if (key.kind == IniFixtureKind::Bit) {
        return (value.value & key.mask) == (key.defaultValue & key.mask);
    }
    return value.value == key.defaultValue;
}

std::string IniFixture_Trim(const std::string &text)
{
    size_t begin = 0;
    size_t end = text.size();
    while (begin < end && strchr(" \t\r\n", text[begin]) != nullptr) {
        begin++;
    }
    while (end > begin && strchr(" \t\r\n", text[end - 1]) != nullptr) {
        end--;
    }
    return text.substr(begin, end - begin);
}

std::string IniFixture_Lower(std::string text)
{
    std::transform(text.begin(), text.end(), text.begin(), [](char c) {
        return (c >= 'A' && c <= 'Z') ? (char)(c - 'A' + 'a') : c;
    });
    return text;
}

bool IniFixture_ReadFile(const fs::path &path, std::string *out)
{
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return false;
    }
    out->assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return true;
}

struct IniFixtureLine {
    std::string pending; /* "", "A", "B", "A+B" */
    std::string text;
    std::string origin;  /* file:line, for the log */
};

/* The lines of an .expect, with its includes (a pending tag of the include line goes to the included lines that
   have none). */
bool IniFixture_LoadExpect(const fs::path &path, const std::string &pending, std::vector<IniFixtureLine> *lines,
                           int depth)
{
    std::string text;
    if (depth > 4 || !IniFixture_ReadFile(path, &text)) {
        return false;
    }
    std::istringstream stream(text);
    std::string raw;
    int number = 0;
    while (std::getline(stream, raw)) {
        number++;
        IniFixtureLine line = {pending, IniFixture_Trim(raw), path.filename().string() + ":" + std::to_string(number)};
        if (line.text.empty() || line.text[0] == '#') {
            continue;
        }
        if (line.text.rfind("pending ", 0) == 0) {
            const size_t colon = line.text.find(':');
            if (colon == std::string::npos) {
                return false;
            }
            line.pending = IniFixture_Trim(line.text.substr(8, colon - 8));
            line.text = IniFixture_Trim(line.text.substr(colon + 1));
        }
        if (line.text.rfind("include:", 0) == 0) {
            if (!IniFixture_LoadExpect(path.parent_path() / IniFixture_Trim(line.text.substr(8)), line.pending, lines,
                                       depth + 1)) {
                return false;
            }
            continue;
        }
        lines->push_back(line);
    }
    return true;
}

struct IniFixtureTally {
    unsigned checks = 0;
    unsigned failed = 0;
    unsigned expectedFail = 0;
    unsigned pendingPass = 0;
};

class IniFixtureCase {
public:
    IniFixtureCase(std::string name, IniFixtureTally *tally, bool strict) : name_(std::move(name)), tally_(tally),
                                                                            strict_(strict) {}

    /* One check; pending: the branch it waits for ("" = holds now). */
    void Check(bool ok, const std::string &pending, const std::string &what)
    {
        tally_->checks++;
        checks_++;
        if (ok) {
            if (!pending.empty()) {
                tally_->pendingPass++;
                Thandor_Log("inifixtures %s: pending %s check passes: %s", name_.c_str(), pending.c_str(), what.c_str());
            }
            return;
        }
        if (!pending.empty() && !strict_) {
            tally_->expectedFail++;
            expectedFail_++;
            Thandor_Log("inifixtures %s: expected-fail (pending %s): %s", name_.c_str(), pending.c_str(), what.c_str());
            return;
        }
        tally_->failed++;
        failed_++;
        Thandor_Log("inifixtures %s: FAIL%s: %s", name_.c_str(),
                    pending.empty() ? "" : (" (pending " + pending + ", strict)").c_str(), what.c_str());
    }

    void Summary() const
    {
        Thandor_Log("inifixtures %s: %s, %u checks, %u expected-fail", name_.c_str(), failed_ == 0 ? "ok" : "FAILED",
                    checks_, expectedFail_);
    }

    unsigned Failed() const { return failed_; }

private:
    std::string name_;
    IniFixtureTally *tally_;
    bool strict_;
    unsigned checks_ = 0;
    unsigned failed_ = 0;
    unsigned expectedFail_ = 0;
};

std::vector<IniFixtureValue> IniFixture_Snapshot()
{
    std::vector<IniFixtureValue> values;
    for (const IniFixtureKey &key : IniFixture_Keys()) {
        values.push_back(IniFixture_ReadKey(key));
    }
    return values;
}

/* Load with the language override of a fresh process (the loader only sets it when the file has the key). */
void IniFixture_Load()
{
    g_LocaleCountryCodeOverride = 0;
    PersistentSettings_Load();
}

/* Flush with the settings marked dirty, so it writes even when nothing changed since the load. The block is the
   language dword with the value Flush itself stores there. */
void IniFixture_ForceFlush()
{
    uint32_t language = g_LocaleCountryCodeOverride;
    PersistentSettings_WriteBlock(4, &language, PERSISTENT_SETTING_LOCALE_COUNTRY_CODE);
    PersistentSettings_Flush();
}

/* Trimmed lines of ini text, each with its section (lower case, "" before the first header). */
std::vector<std::pair<std::string, std::string>> IniFixture_Lines(const std::string &text)
{
    std::vector<std::pair<std::string, std::string>> lines;
    std::string section;
    size_t begin = 0;
    while (begin <= text.size()) {
        size_t end = text.find('\n', begin);
        if (end == std::string::npos) {
            end = text.size();
        }
        const std::string line = IniFixture_Trim(text.substr(begin, end - begin));
        if (!line.empty() && line[0] == '[' && line.find(']') != std::string::npos) {
            section = IniFixture_Lower(IniFixture_Trim(line.substr(1, line.find(']') - 1)));
        }
        lines.emplace_back(section, line);
        begin = end + 1;
    }
    return lines;
}

/* The index (in IniFixture_Lines) of the first line of the written ini that matches wanted ("line",
   "[sec] line" or "[sec]", as for "preserved:"), -1 when there is none. */
long IniFixture_FindLine(const std::string &text, const std::string &wanted)
{
    std::string section;
    std::string line = wanted;
    if (!line.empty() && line[0] == '[' && line.find(']') != std::string::npos) {
        section = IniFixture_Lower(IniFixture_Trim(line.substr(1, line.find(']') - 1)));
        line = IniFixture_Trim(line.substr(line.find(']') + 1));
    }
    long index = 0;
    for (const auto &[lineSection, content] : IniFixture_Lines(text)) {
        if (line.empty()) {
            if (!content.empty() && content[0] == '[' && lineSection == section && !section.empty()) {
                return index;
            }
        }
        else if (content == line && (section.empty() || lineSection == section)) {
            return index;
        }
        index++;
    }
    return -1;
}

/* "preserved:" text found in the written ini. */
bool IniFixture_HasLine(const std::string &text, const std::string &wanted)
{
    return IniFixture_FindLine(text, wanted) >= 0;
}

const IniFixtureKey *IniFixture_FindKey(const std::string &name)
{
    for (const IniFixtureKey &key : IniFixture_Keys()) {
        if (IniFixture_Lower(name) == std::string(key.section) + "." + key.key) {
            return &key;
        }
    }
    return nullptr;
}

/* Directory of the fixtures: OPEN_THANDOR_INI_FIXTURES, else tools/test/fixtures/ini above the executable or
   the current directory. */
fs::path IniFixture_Directory()
{
    const char *variable = getenv("OPEN_THANDOR_INI_FIXTURES");
    if (variable != nullptr && variable[0] != 0) {
        return fs::path(variable);
    }
    char exePath[1024] = {};
    Thandor_GetExecutablePathA(exePath, sizeof exePath);
    std::error_code error;
    for (fs::path start : {fs::path(exePath).parent_path(), fs::current_path(error)}) {
        for (int up = 0; up < 6 && !start.empty(); up++) {
            const fs::path candidate = start / "tools" / "test" / "fixtures" / "ini";
            if (fs::is_directory(candidate, error)) {
                return candidate;
            }
            if (start == start.parent_path()) {
                break;
            }
            start = start.parent_path();
        }
    }
    return fs::path();
}

/* The game's file and memory layers are started by ProcessEntry; the self-test runs before it. */
struct IniFixtureRuntime {
    FileSystemOpenProc *open = g_FileSystemOpen;
    FileSystemCloseProc *close = g_FileSystemClose;
    FileSystemReadExactProc *readExact = g_FileSystemReadExact;
    FileSystemWriteExactOrFlushProc *writeExact = g_FileSystemWriteExactOrFlush;
    FileSystemGetSizeProc *getSize = g_FileSystemGetSize;
    FileSystemSeekProc *seek = g_FileSystemSeek;
    FileSystemGetPositionProc *getPosition = g_FileSystemGetPosition;
    FileSystemDeleteProc *deleteFile = g_FileSystemDelete;
    uint32_t (*alloc)(uint32_t, void **) = g_MemoryApi.alloc;
    uint32_t (*free)(void *) = g_MemoryApi.free;
    uint16_t executableDirectory[std::size(g_ExecutableDirectoryUtf16)];
    fs::path currentDirectory;

    static uint32_t Alloc(uint32_t bytes, void **outPayload)
    {
        void *payload = ::operator new(bytes, std::nothrow);
        if (payload == nullptr) {
            return FATAL_ERROR_ARENA_EXHAUSTED;
        }
        *outPayload = payload;
        return 0;
    }

    static uint32_t Free(void *memory)
    {
        ::operator delete(memory);
        return 0;
    }

    IniFixtureRuntime()
    {
        std::error_code error;
        std::copy(std::begin(g_ExecutableDirectoryUtf16), std::end(g_ExecutableDirectoryUtf16), executableDirectory);
        currentDirectory = fs::current_path(error);
        if (g_FileSystemOpen == nullptr) {
            g_FileSystemOpen = Win32File_Open;
            g_FileSystemClose = Win32File_Close;
            g_FileSystemReadExact = Win32File_ReadExact;
            g_FileSystemWriteExactOrFlush = Win32File_WriteExactOrFlush;
            g_FileSystemGetSize = Win32File_GetSize;
            g_FileSystemSeek = Win32File_Seek;
            g_FileSystemGetPosition = Win32File_GetPosition;
            g_FileSystemDelete = Win32File_Delete;
        }
        if (g_MemoryApi.alloc == nullptr) {
            g_MemoryApi.alloc = Alloc;
            g_MemoryApi.free = Free;
        }
    }

    /* The work directory becomes the current and the executable directory. */
    bool Enter(const fs::path &directory)
    {
        std::error_code error;
        const std::wstring wide = directory.wstring() + L"\\";
        if (wide.size() + 1 > std::size(g_ExecutableDirectoryUtf16)) {
            return false;
        }
        std::fill(std::begin(g_ExecutableDirectoryUtf16), std::end(g_ExecutableDirectoryUtf16), 0);
        std::copy(wide.begin(), wide.end(), g_ExecutableDirectoryUtf16);
        fs::current_path(directory, error);
        return !error;
    }

    ~IniFixtureRuntime()
    {
        std::error_code error;
        fs::current_path(currentDirectory, error);
        std::copy(std::begin(executableDirectory), std::end(executableDirectory), g_ExecutableDirectoryUtf16);
        g_FileSystemOpen = open;
        g_FileSystemClose = close;
        g_FileSystemReadExact = readExact;
        g_FileSystemWriteExactOrFlush = writeExact;
        g_FileSystemGetSize = getSize;
        g_FileSystemSeek = seek;
        g_FileSystemGetPosition = getPosition;
        g_FileSystemDelete = deleteFile;
        /* the last loaded image stays allocated by Alloc (the process ends after the self-test) */
    }
};

/* One .expect case. */
void IniFixture_RunCase(IniFixtureRuntime &runtime, const fs::path &fixtures, const fs::path &expectPath,
                        const fs::path &workRoot, IniFixtureTally *tally, bool strict)
{
    using K = IniFixtureKind;
    const std::string name = expectPath.stem().string();
    IniFixtureCase test(name, tally, strict);
    std::vector<IniFixtureLine> lines;
    std::error_code error;

    if (!IniFixture_LoadExpect(expectPath, "", &lines, 0)) {
        test.Check(false, "", "cannot read " + expectPath.filename().string() + " or one of its includes");
        test.Summary();
        return;
    }
    fs::path source;
    bool staleTemporary = false;
    for (const IniFixtureLine &line : lines) {
        if (line.text.rfind("source:", 0) == 0) {
            source = fixtures / IniFixture_Trim(line.text.substr(7));
        }
        else if (line.text == "setup: stale_tmp") {
            staleTemporary = true;
        }
    }
    if (source.empty()) {
        source = fixtures / (name + ".ini");
        if (!fs::exists(source, error)) {
            source = fixtures / (name + ".dat");
        }
    }
    const fs::path work = workRoot / name;
    fs::remove_all(work, error);
    fs::create_directories(work, error);
    const bool isDat = IniFixture_Lower(source.extension().string()) == ".dat";
    const fs::path ini = work / "thandor.ini";
    const fs::path temporary = work / "thandor.ini.tmp";
    if (!fs::copy_file(source, work / (isDat ? "thandor.dat" : "thandor.ini"), fs::copy_options::overwrite_existing,
                       error)) {
        test.Check(false, "", "cannot copy " + source.string() + " to " + work.string());
        test.Summary();
        return;
    }
    if (staleTemporary) {
        std::ofstream(temporary, std::ios::binary) << "[display]\r\nwidth = 77\r\n; left behind by a crash mid-save, cut";
    }
    if (!runtime.Enter(work)) {
        test.Check(false, "", "work directory path too long or not usable: " + work.string());
        test.Summary();
        return;
    }

    /* 1. load, values */
    IniFixture_Load();
    const std::vector<IniFixtureValue> first = IniFixture_Snapshot();
    std::vector<bool> listed(IniFixture_Keys().size(), false);
    const IniFixtureLine *others = nullptr;
    for (const IniFixtureLine &line : lines) {
        const size_t equals = line.text.find('=');
        if (line.text.find(':') != std::string::npos &&
            (equals == std::string::npos || line.text.find(':') < equals)) {
            continue; /* a directive */
        }
        if (equals == std::string::npos) {
            test.Check(false, "", line.origin + ": cannot parse \"" + line.text + "\"");
            continue;
        }
        const std::string keyName = IniFixture_Trim(line.text.substr(0, equals));
        const std::string wanted = IniFixture_Trim(line.text.substr(equals + 1));
        if (keyName == "others") {
            others = &line;
            continue;
        }
        const IniFixtureKey *key = IniFixture_FindKey(keyName);
        if (key == nullptr) {
            test.Check(false, "", line.origin + ": no key " + keyName + " in the key table");
            continue;
        }
        const size_t index = (size_t)(key - IniFixture_Keys().data());
        const IniFixtureValue &value = first[index];
        listed[index] = true;
        bool ok = false;
        size_t begin = 0;
        while (begin <= wanted.size() && !ok) {
            size_t end = wanted.find('|', begin);
            if (end == std::string::npos) {
                end = wanted.size();
            }
            const std::string alternative = IniFixture_Trim(wanted.substr(begin, end - begin));
            if (alternative == "<default>") {
                ok = IniFixture_IsDefault(*key, value);
            }
            else if (key->kind == K::Name) {
                ok = value.present && value.text == alternative;
            }
            else {
                ok = value.present && IniFixture_Lower(value.text) == IniFixture_Lower(alternative);
            }
            begin = end + 1;
        }
        test.Check(ok, line.pending, keyName + " = " + value.text + ", expected " + wanted);
    }
    if (others != nullptr) {
        for (size_t index = 0; index < IniFixture_Keys().size(); index++) {
            const IniFixtureKey &key = IniFixture_Keys()[index];
            if (!listed[index]) {
                test.Check(IniFixture_IsDefault(key, first[index]), others->pending,
                           std::string(key.section) + "." + key.key + " = " + first[index].text +
                               ", expected <default> (others)");
            }
        }
    }

    /* 2. save, the written file */
    IniFixture_ForceFlush();
    std::string written;
    test.Check(IniFixture_ReadFile(ini, &written), "", "Flush wrote no thandor.ini");
    for (const IniFixtureLine &line : lines) {
        if (line.text.rfind("preserved:", 0) == 0) {
            const std::string wanted = IniFixture_Trim(line.text.substr(10));
            test.Check(IniFixture_HasLine(written, wanted), line.pending, "preserved: " + wanted);
        }
        else if (line.text.rfind("absent:", 0) == 0) {
            const std::string unwanted = IniFixture_Trim(line.text.substr(7));
            bool found = false;
            for (const auto &entry : IniFixture_Lines(written)) {
                found |= entry.second.find(unwanted) != std::string::npos;
            }
            test.Check(!found, line.pending, "absent: " + unwanted);
        }
        else if (line.text.rfind("order:", 0) == 0) {
            const std::string pair = IniFixture_Trim(line.text.substr(6));
            const size_t less = pair.find(" < ");
            if (less == std::string::npos) {
                test.Check(false, "", line.origin + ": order: needs \"first < second\"");
                continue;
            }
            const std::string before = IniFixture_Trim(pair.substr(0, less));
            const std::string after = IniFixture_Trim(pair.substr(less + 3));
            const long beforeAt = IniFixture_FindLine(written, before);
            const long afterAt = IniFixture_FindLine(written, after);
            test.Check(beforeAt >= 0 && afterAt >= 0 && beforeAt < afterAt, line.pending,
                       "order: " + before + " (line " + std::to_string(beforeAt) + ") before " + after + " (line " +
                           std::to_string(afterAt) + ")");
        }
        else if (line.text.rfind("absent-file:", 0) == 0) {
            const std::string file = IniFixture_Trim(line.text.substr(12));
            test.Check(!fs::exists(work / file, error), line.pending, "absent-file: " + file);
        }
    }
    if (!staleTemporary) {
        test.Check(!fs::exists(temporary, error), "", "no thandor.ini.tmp left after the save");
    }

    /* 3. load the written file again: every key reads as before (a key that read as its default may come back
       loaded with the default value or not loaded: Flush always stores the language, and a reserved dword of 0 is
       not written) */
    IniFixture_Load();
    const std::vector<IniFixtureValue> second = IniFixture_Snapshot();
    for (size_t index = 0; index < first.size(); index++) {
        const IniFixtureKey &key = IniFixture_Keys()[index];
        const bool bothDefault = IniFixture_IsDefault(key, first[index]) && IniFixture_IsDefault(key, second[index]);
        test.Check(first[index] == second[index] || bothDefault, "",
                   std::string("round trip ") + key.section + "." + key.key + ": " + first[index].text + " -> " +
                       second[index].text);
    }

    /* 4. save again: the same text */
    IniFixture_ForceFlush();
    std::string rewritten;
    IniFixture_ReadFile(ini, &rewritten);
    test.Check(rewritten == written, "", "the second save changed thandor.ini (" + std::to_string(written.size()) +
                                             " -> " + std::to_string(rewritten.size()) + " bytes)");

    /* 5. a save that fails (read-only file) must be retried by the next Flush */
    for (const IniFixtureLine &line : lines) {
        if (line.text != "check: readonly_retry") {
            continue;
        }
        const uint32_t width = PersistentSettings_Read(640, PERSISTENT_SETTING_DISPLAY_WIDTH) == 1234 ? 1235 : 1234;
        const std::string wantedLine = "width = " + std::to_string(width);
        fs::permissions(ini, fs::perms::owner_write | fs::perms::group_write | fs::perms::others_write,
                        fs::perm_options::remove, error);
        PersistentSettings_Write(width, PERSISTENT_SETTING_DISPLAY_WIDTH);
        PersistentSettings_Flush();
        std::string readOnlyText;
        IniFixture_ReadFile(ini, &readOnlyText);
        fs::permissions(ini, fs::perms::owner_write, fs::perm_options::add, error);
        test.Check(readOnlyText == rewritten, "", "the read-only thandor.ini was changed");
        PersistentSettings_Flush();
        std::string retried;
        IniFixture_ReadFile(ini, &retried);
        test.Check(IniFixture_HasLine(retried, "[display] " + wantedLine), line.pending,
                   "readonly_retry: the next Flush after a failed save writes " + wantedLine);
        test.Check(!fs::exists(temporary, error), "", "readonly_retry: no thandor.ini.tmp left");
    }
    test.Summary();
}

} // namespace

void Thandor_SelfTestIniFixtures()
{
    IniFixtureTally tally;
    std::error_code error;
    const char *strictVariable = getenv("OPEN_THANDOR_INI_STRICT");
    const bool strict = strictVariable != nullptr && strcmp(strictVariable, "1") == 0;
    const char *onlyVariable = getenv("OPEN_THANDOR_INI_FIXTURE");
    const char *outVariable = getenv("OPEN_THANDOR_INI_OUT");
    const fs::path fixtures = IniFixture_Directory();

    /* [keys] belongs to the key bindings, as in the game (registered by the runtime start, which has not run) */
    KeyBindingsIni_Register();
    if (fixtures.empty() || !fs::is_directory(fixtures, error)) {
        Thandor_Log("inifixtures: FAILED, no fixture directory (set OPEN_THANDOR_INI_FIXTURES to "
                    "tools/test/fixtures/ini)");
        return;
    }
    std::vector<fs::path> cases;
    for (const fs::directory_entry &entry : fs::directory_iterator(fixtures, error)) {
        if (IniFixture_Lower(entry.path().extension().string()) != ".expect") {
            continue;
        }
        if (onlyVariable != nullptr && onlyVariable[0] != 0) {
            const std::string only = std::string(",") + onlyVariable + ",";
            if (only.find("," + entry.path().stem().string() + ",") == std::string::npos) {
                continue;
            }
        }
        cases.push_back(entry.path());
    }
    std::sort(cases.begin(), cases.end());
    const bool keep = outVariable != nullptr && outVariable[0] != 0;
    const fs::path workRoot = keep ? fs::path(outVariable)
                                   : fs::temp_directory_path(error) /
                                         ("open-thandor-inifixtures-" + std::to_string(_getpid()));
    fs::create_directories(workRoot, error);
    Thandor_Log("inifixtures: %u cases from %s, work directory %s%s", (unsigned)cases.size(), fixtures.string().c_str(),
                workRoot.string().c_str(), strict ? ", strict (pending checks must pass)" : "");
    {
        IniFixtureRuntime runtime;
        for (const fs::path &expect : cases) {
            IniFixture_RunCase(runtime, fixtures, expect, workRoot, &tally, strict);
        }
    }
    if (cases.empty()) {
        tally.failed++;
    }
    if (tally.failed == 0 && !keep) {
        fs::remove_all(workRoot, error);
    }
    Thandor_Log("inifixtures: %s, %u cases, %u checks, %u failed, %u expected-fail (pending A/B), %u pending "
                "checks pass already%s",
                tally.failed == 0 ? "ok" : "FAILED", (unsigned)cases.size(), tally.checks, tally.failed,
                tally.expectedFail, tally.pendingPass,
                tally.failed == 0 ? "" : (", work directories in " + workRoot.string()).c_str());
}
