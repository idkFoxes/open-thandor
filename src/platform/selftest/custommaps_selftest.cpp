/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/selftest/custommaps_selftest.cpp
 * Project code (not in the original game)
 */

/* OPEN_THANDOR_SELFTEST=custommaps: CustomMap_Validate (assets/scenario/custom_map_validate.h).
     - stock maps: every level\*.lev and level\*.fld entry of PATCH01.PCK, PATCH00.PCK and LEVEL.PCK in the current
       directory (first package with a path wins), decoded natively (the .fld entries by the field grid codec into
       the expanded image a loose custom .fld holds); every level with its field grid must be accepted, also in the
       layout of a derived custom map (field grid path "maps\<name>\<name>" in a new 0x100-byte slot at the end of
       the prefix, placements moved behind it);
     - corrupted variants of one stock map (truncated, bad magic, huge dimensions, counts too large, unknown ids,
       ".." / absolute / unterminated paths, short .fld, ...) must be rejected, each with a reason;
     - fuzz: random byte mutations and truncations of the level and the field grid header and cells; the
       validation must return (no crash; the buffers are exactly the file size, so an ASan build reports any read
       past them).
   One line per failed check and a summary line; without the packages the test logs "skipped" and fails. */

#include <stdio.h>
#include <string.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <thandor/thandor.h>
#include <thandor/assets/scenario/custom_map_validate.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/selftest/selftest.h>

namespace {

unsigned s_CustomMapsChecks = 0;
unsigned s_CustomMapsFailures = 0;

constexpr uint32_t CUSTOMMAPS_FUZZ_ITERATIONS = 20000;
constexpr uint32_t CUSTOMMAPS_DERIVED_SLOT_BYTES = 0x100;

void CustomMaps_Fail(const char *format, const char *a, const char *b)
{
  s_CustomMapsFailures++;
  Thandor_Log(format, a, b);
}

uint32_t CustomMaps_Alloc(uint32_t bytes, void **outPayload)
{
  void *payload = ::operator new(bytes, std::nothrow);
  if (payload == nullptr) {
    return FATAL_ERROR_ARENA_EXHAUSTED;
  }
  *outPayload = payload;
  return 0;
}

uint32_t CustomMaps_Free(void *memory)
{
  ::operator delete(memory);
  return 0;
}

uint32_t CustomMaps_U32(const std::vector<uint8_t> &data, size_t offset)
{
  uint32_t value = 0;
  memcpy(&value, data.data() + offset, sizeof value);
  return value;
}

void CustomMaps_SetU32(std::vector<uint8_t> &data, size_t offset, uint32_t value)
{
  memcpy(data.data() + offset, &value, sizeof value);
}

void CustomMaps_SetPath(std::vector<uint8_t> &data, size_t offset, const char *text, bool terminate = true)
{
  size_t i = 0;
  for (; text[i] != 0; i++) {
    data[offset + 2 * i] = (uint8_t)text[i];
    data[offset + 2 * i + 1] = 0;
  }
  if (terminate) {
    data[offset + 2 * i] = 0;
    data[offset + 2 * i + 1] = 0;
  }
}

/* Lower-case ASCII of a UTF-16 path (other units as '?'). */
std::string CustomMaps_PathKey(const uint8_t *units, size_t maxUnits)
{
  std::string path;
  for (size_t k = 0; k < maxUnits; k++) {
    const uint16_t unit = (uint16_t)(units[2 * k] | units[2 * k + 1] << 8);
    if (unit == 0) {
      break;
    }
    path += (char)((unit >= 'A' && unit <= 'Z') ? unit + ('a' - 'A') : (unit < 0x80 ? unit : '?'));
  }
  return path;
}

/* Decodes every level\*.lev and level\*.fld entry of one package whose path is new. */
void CustomMaps_LoadPackage(const char *packageName, uint8_t *packed, std::unordered_set<std::string> &seenPaths,
                            std::unordered_map<std::string, std::vector<uint8_t>> &levels,
                            std::unordered_map<std::string, std::vector<uint8_t>> &fields)
{
  FILE *pck = fopen(packageName, "rb");
  if (pck == nullptr) {
    return;
  }
  long position = PCK_ENTRY_HEADER_BYTES;
  for (;;) {
    PckEntryHeader header;
    if ((fseek(pck, position, SEEK_SET) != 0) || (fread(&header, sizeof header, 1, pck) != 1)) {
      break;
    }
    if ((header.packedSize == 0) || (header.packedSize > PACKAGE_SCRATCH_BUFFER_BYTES) ||
        ((uint32_t)header.compressionMethod >= sizeof g_PckDecoderTable / sizeof g_PckDecoderTable[0])) {
      break;
    }
    position += PCK_ENTRY_HEADER_BYTES + (long)header.packedSize;
    const std::string path = CustomMaps_PathKey(reinterpret_cast<const uint8_t *>(header.path), PCK_ENTRY_PATH_UNITS);
    const bool isLevel = (path.size() > 4) && (path.compare(path.size() - 4, 4, ".lev") == 0);
    const bool isField = (path.size() > 4) && (path.compare(path.size() - 4, 4, ".fld") == 0);
    if ((path.rfind("level\\", 0) != 0) || (!isLevel && !isField) || !seenPaths.insert(path).second) {
      continue;
    }
    s_CustomMapsChecks++;
    if ((g_PckDecoderTable[header.compressionMethod] == nullptr) || (header.unpackedSize == 0) ||
        (header.unpackedSize > PACKAGE_SCRATCH_BUFFER_BYTES) ||
        (fread(packed, 1, header.packedSize, pck) != header.packedSize)) {
      CustomMaps_Fail("custommaps: FAIL %s %s: not readable", packageName, path.c_str());
      continue;
    }
    std::vector<uint8_t> image(header.unpackedSize);
    if (!g_PckDecoderTable[header.compressionMethod](header.unpackedSize, image.data(), header.packedSize, packed,
                                                       nullptr, nullptr)) {
      CustomMaps_Fail("custommaps: FAIL %s %s: decoding failed", packageName, path.c_str());
      continue;
    }
    (isLevel ? levels : fields)[path.substr(0, path.size() - 4)] = std::move(image);
  }
  fclose(pck);
}

bool CustomMaps_Validate(const std::vector<uint8_t> &lev, const std::vector<uint8_t> &fld, std::string *reason)
{
  char text[256];
  memset(text, 0x55, sizeof text);
  const bool valid = CustomMap_Validate(lev.data(), lev.size(), fld.data(), fld.size(), text, sizeof text);
  if (reason != nullptr) {
    *reason = (memchr(text, 0, sizeof text) != nullptr) ? text : "<reason not terminated>";
  }
  return valid;
}

/* The derived custom map layout: the field grid path moves into a new CUSTOMMAPS_DERIVED_SLOT_BYTES slot at the end
   of the prefix (the old slot keeps its text, unreferenced), the placements move behind it. */
std::vector<uint8_t> CustomMaps_Derive(const std::vector<uint8_t> &lev, const char *fieldPath)
{
  const uint32_t prefix = CustomMaps_U32(lev, 0xDC);
  std::vector<uint8_t> derived(lev.begin(), lev.begin() + prefix);
  derived.resize(prefix + CUSTOMMAPS_DERIVED_SLOT_BYTES, 0);
  derived.insert(derived.end(), lev.begin() + prefix, lev.end());
  CustomMaps_SetPath(derived, prefix, fieldPath);
  CustomMaps_SetU32(derived, 0xB0, prefix);
  CustomMaps_SetU32(derived, 0xDC, prefix + CUSTOMMAPS_DERIVED_SLOT_BYTES);
  CustomMaps_SetU32(derived, 0x04, (uint32_t)derived.size());
  return derived;
}

void CustomMaps_ExpectValid(const char *name, const std::vector<uint8_t> &lev, const std::vector<uint8_t> &fld)
{
  std::string reason;
  s_CustomMapsChecks++;
  if (!CustomMaps_Validate(lev, fld, &reason)) {
    CustomMaps_Fail("custommaps: FAIL %s rejected: %s", name, reason.c_str());
  }
}

void CustomMaps_ExpectInvalid(const char *name, const std::vector<uint8_t> &lev, const std::vector<uint8_t> &fld)
{
  std::string reason;
  s_CustomMapsChecks++;
  if (CustomMaps_Validate(lev, fld, &reason)) {
    CustomMaps_Fail("custommaps: FAIL corrupt %s accepted%s", name, "");
    return;
  }
  if (reason.empty() || (reason[0] == '<')) {
    CustomMaps_Fail("custommaps: FAIL corrupt %s rejected without a reason%s", name, "");
    return;
  }
  Thandor_Log("custommaps: corrupt %-26s rejected: %s", name, reason.c_str());
}

/* Deliberately corrupted variants of one stock map: each must be rejected. */
void CustomMaps_CorruptCases(const std::vector<uint8_t> &lev, const std::vector<uint8_t> &fld)
{
  const uint32_t prefix = CustomMaps_U32(lev, 0xDC);
  const uint32_t placements = CustomMaps_U32(lev, 0xD8);
  const uint32_t width = CustomMaps_U32(fld, 0xB8);
  const uint32_t groundPath = CustomMaps_U32(lev, 0xB4);
  const uint32_t armTableOffset = CustomMaps_U32(lev, 0xE4);
  std::vector<uint8_t> l;
  std::vector<uint8_t> f;

  { /* null data and size 0 */
    char text[64];
    s_CustomMapsChecks++;
    if (CustomMap_Validate(nullptr, 0, nullptr, 0, text, sizeof text) || (text[0] == 0) ||
        CustomMap_Validate(lev.data(), lev.size(), fld.data(), fld.size(), nullptr, 0) == false ||
        CustomMap_Validate(lev.data(), 0, fld.data(), fld.size(), text, 1) || (text[0] != 0)) {
      CustomMaps_Fail("custommaps: FAIL %s%s", "null / empty / reason buffer cases", "");
    }
  }
  l.assign(lev.begin(), lev.begin() + 0x7FF);
  CustomMaps_ExpectInvalid("lev truncated (0x7FF)", l, fld);
  l.assign(lev.begin(), lev.end() - 1);
  CustomMaps_ExpectInvalid("lev truncated by 1", l, fld);
  l.assign(lev.begin(), lev.end() - 0x20);
  CustomMaps_ExpectInvalid("lev missing a placement", l, fld);
  l = lev; l[0] = 'x';
  CustomMaps_ExpectInvalid("lev bad magic", l, fld);
  l = lev; CustomMaps_SetU32(l, 0x0C, 0x00070002);
  CustomMaps_ExpectInvalid("lev converter version", l, fld);
  l = lev; CustomMaps_SetU32(l, 0xDC, prefix + 2);
  CustomMaps_ExpectInvalid("lev prefix not dword", l, fld);
  l = lev; CustomMaps_SetU32(l, 0xD8, 0x7FFFFFFF);
  CustomMaps_ExpectInvalid("lev placement count huge", l, fld);
  l = lev; CustomMaps_SetU32(l, 0xD8, placements + 1);
  CustomMaps_ExpectInvalid("lev placement count + 1", l, fld);
  l = lev; CustomMaps_SetU32(l, 0xE0, 0x40000);
  CustomMaps_ExpectInvalid("lev ARM count huge", l, fld);
  l = lev; CustomMaps_SetU32(l, 0xE4, prefix - 0x20);
  CustomMaps_ExpectInvalid("lev ARM table past prefix", l, fld);
  l = lev; CustomMaps_SetU32(l, 0xE4, 0x300);
  CustomMaps_ExpectInvalid("lev ARM table in the header", l, fld);
  l = lev; CustomMaps_SetPath(l, armTableOffset, "arm\\..\\..\\x");
  CustomMaps_ExpectInvalid("lev ARM entry '..'", l, fld);
  l = lev; CustomMaps_SetPath(l, armTableOffset, "mdl\\unit");
  CustomMaps_ExpectInvalid("lev ARM entry wrong dir", l, fld);
  l = lev; CustomMaps_SetPath(l, armTableOffset, "arm\\abcdefghijklmnopqrstuvwxyz0");
  CustomMaps_ExpectInvalid("lev ARM entry too long", l, fld);
  l = lev; CustomMaps_SetPath(l, groundPath, "gfx\\..\\..\\windows\\x");
  CustomMaps_ExpectInvalid("lev path '..'", l, fld);
  l = lev; CustomMaps_SetPath(l, groundPath, "c:\\windows\\x");
  CustomMaps_ExpectInvalid("lev path absolute (drive)", l, fld);
  l = lev; CustomMaps_SetPath(l, groundPath, "\\\\server\\x");
  CustomMaps_ExpectInvalid("lev path absolute (UNC)", l, fld);
  l = lev; CustomMaps_SetPath(l, groundPath, "gfx/boden/soil");
  CustomMaps_ExpectInvalid("lev path slash", l, fld);
  l = lev; CustomMaps_SetPath(l, groundPath, "sound\\soil");
  CustomMaps_ExpectInvalid("lev path wrong prefix", l, fld);
  l = lev;
  for (uint32_t offset = groundPath; offset < prefix; offset += 2) {
    l[offset] = 'a';
    l[offset + 1] = 0;
  }
  CustomMaps_ExpectInvalid("lev path unterminated", l, fld);
  l = lev; CustomMaps_SetU32(l, 0xB4, prefix + 0x100);
  CustomMaps_ExpectInvalid("lev path offset outside", l, fld);
  l = lev; CustomMaps_SetU32(l, 0xB4, CustomMaps_U32(lev, 0xB8));
  CustomMaps_ExpectInvalid("lev paths overlap", l, fld);
  l = lev; CustomMaps_SetU32(l, 0xB0, prefix - 4);
  CustomMaps_ExpectInvalid("lev field path at prefix end", l, fld);
  l = lev; CustomMaps_SetU32(l, 0x170, 0x100000);
  CustomMaps_ExpectInvalid("lev title index", l, fld);
  l = lev; CustomMaps_SetU32(l, 0x314, 8); CustomMaps_SetU32(l, 0x310, 8);
  CustomMaps_ExpectInvalid("lev faction count 8", l, fld);
  l = lev; CustomMaps_SetU32(l, 0x310, 0);
  CustomMaps_ExpectInvalid("lev assignable 0", l, fld);
  l = lev; CustomMaps_SetU32(l, 0x21C, 100);
  CustomMaps_ExpectInvalid("lev colour offset 100", l, fld);
  l = lev; CustomMaps_SetU32(l, 0x200, 0x7FFFFFFF);
  CustomMaps_ExpectInvalid("lev camera far away", l, fld);
  l = lev; CustomMaps_SetU32(l, 0x324, 5000);
  CustomMaps_ExpectInvalid("lev intro movie 5000", l, fld);
  l = lev; CustomMaps_SetU32(l, 0x350, 100);
  CustomMaps_ExpectInvalid("lev music sample 100", l, fld);
  l = lev; l[0x380] = 30;
  CustomMaps_ExpectInvalid("lev condition kind 30", l, fld);
  l = lev; l[0x380] = INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY; CustomMaps_SetU32(l, 0x384, 9);
  CustomMaps_ExpectInvalid("lev condition faction 9", l, fld);
  l = lev; l[0x380] = INGAME_SCHEDULED_CONDITION_BOOLEAN_POSTFIX_EXPRESSION; memset(&l[0x381], 1, 15);
  CustomMaps_ExpectInvalid("lev postfix without end", l, fld);
  l = lev; l[0x380] = INGAME_SCHEDULED_CONDITION_BOOLEAN_POSTFIX_EXPRESSION; l[0x381] = 64; l[0x382] = 0xFC;
  CustomMaps_ExpectInvalid("lev postfix operand 64", l, fld);
  l = lev; l[0x780] = 1; l[0x784] = 9; l[0x786] = 0;
  CustomMaps_ExpectInvalid("lev trigger faction 9", l, fld);
  l = lev; l[0x780] = 1; l[0x784] = 1; l[0x786] = 200;
  CustomMaps_ExpectInvalid("lev trigger condition 200", l, fld);
  l = lev; l[0x780] = 1; l[0x784] = 1; l[0x785] = 9; l[0x786] = 0;
  CustomMaps_ExpectInvalid("lev trigger end movie 9", l, fld);
  if (placements != 0) {
    l = lev; CustomMaps_SetU32(l, prefix, 9999);
    CustomMaps_ExpectInvalid("lev placement unknown id", l, fld);
    l = lev; CustomMaps_SetU32(l, prefix, 299);
    CustomMaps_ExpectInvalid("lev placement id gap 299", l, fld);
    l = lev; CustomMaps_SetU32(l, prefix + 4, 8);
    CustomMaps_ExpectInvalid("lev placement faction 8", l, fld);
    l = lev; CustomMaps_SetU32(l, prefix + 8, 0x7FFFFFFF);
    CustomMaps_ExpectInvalid("lev placement X outside", l, fld);
    l = lev; CustomMaps_SetU32(l, prefix + 12, 0x1000);
    CustomMaps_ExpectInvalid("lev placement Y outside", l, fld);
    l = lev; CustomMaps_SetU32(l, prefix + 16, 0x12345);
    CustomMaps_ExpectInvalid("lev placement rotation", l, fld);
  }
  l = CustomMaps_Derive(lev, "maps\\..\\..\\evil");
  CustomMaps_ExpectInvalid("derived field path '..'", l, fld);
  l = CustomMaps_Derive(lev, "maps\\test\\");
  CustomMaps_ExpectInvalid("derived field path ends '\\'", l, fld);
  l = CustomMaps_Derive(lev, "maps\\test\\test.fld");
  CustomMaps_ExpectInvalid("derived field path with '.'", l, fld);
  l = CustomMaps_Derive(lev, "temp\\test");
  CustomMaps_ExpectInvalid("derived field path dir", l, fld);
  l = CustomMaps_Derive(lev, std::string(121, 'a').insert(0, "maps\\").c_str());
  CustomMaps_ExpectInvalid("derived field path too long", l, fld);

  f.assign(fld.begin(), fld.end() - 1);
  CustomMaps_ExpectInvalid("fld truncated by 1", lev, f);
  f.assign(fld.begin(), fld.begin() + 0x1FF);
  CustomMaps_ExpectInvalid("fld shorter than header", lev, f);
  f = fld; f.resize(f.size() + 0x80);
  CustomMaps_ExpectInvalid("fld longer", lev, f);
  f = fld; f[0] = 'x';
  CustomMaps_ExpectInvalid("fld bad magic", lev, f);
  f = fld; CustomMaps_SetU32(f, 0x0C, 0x00060007);
  CustomMaps_ExpectInvalid("fld converter version", lev, f);
  f = fld; CustomMaps_SetU32(f, 0xB8, 0x10003); CustomMaps_SetU32(f, 0xBC, 0x10003);
  CustomMaps_ExpectInvalid("fld huge dimensions", lev, f);
  f = fld; CustomMaps_SetU32(f, 0xB8, 0x40000003); CustomMaps_SetU32(f, 0xBC, 0x40000003);
  CustomMaps_ExpectInvalid("fld overflowing dimensions", lev, f);
  f = fld; CustomMaps_SetU32(f, 0xB8, CustomMaps_U32(fld, 0xBC)); CustomMaps_SetU32(f, 0xBC, width);
  if (CustomMaps_U32(fld, 0xB8) != CustomMaps_U32(fld, 0xBC)) {
    CustomMaps_ExpectInvalid("fld swapped dimensions", lev, f);
  }
  f = fld; CustomMaps_SetU32(f, 0xB8, width + 1);
  CustomMaps_ExpectInvalid("fld width not 4k+3", lev, f);
  f = fld; CustomMaps_SetU32(f, 0x04, (uint32_t)fld.size() + 0x80);
  CustomMaps_ExpectInvalid("fld allocation size", lev, f);
  const size_t cell = 0x200 + (size_t)(width + 1) * 0x80;
  f = fld; CustomMaps_SetU32(f, cell + 0x40, CustomMaps_U32(fld, cell + 0x40) + 1);
  CustomMaps_ExpectInvalid("fld cell off the lattice", lev, f);
  f = fld; CustomMaps_SetU32(f, cell + 0x50, (CustomMaps_U32(fld, cell + 0x50) & ~0xFFu) | 30);
  CustomMaps_ExpectInvalid("fld cell material 30", lev, f);
  f = fld; CustomMaps_SetU32(f, cell + 0x50, CustomMaps_U32(fld, cell + 0x50) | 0x00010000);
  CustomMaps_ExpectInvalid("fld cell region-visited bit", lev, f);
  f = fld; CustomMaps_SetU32(f, cell + 0x48, 0x40000000);
  CustomMaps_ExpectInvalid("fld cell height", lev, f);
  f = fld; CustomMaps_SetU32(f, cell + 0x68, 0x01000000);
  CustomMaps_ExpectInvalid("fld cell lighting index", lev, f);
  f = fld; CustomMaps_SetU32(f, cell + 0x7C, 1);
  CustomMaps_ExpectInvalid("fld cell extraction", lev, f);
  f = fld; CustomMaps_SetU32(f, 0xB0, CustomMaps_U32(fld, 0xB0) | 0x80000000u);
  CustomMaps_ExpectInvalid("fld material flags", lev, f);
}

/* Random mutations: the validation must always return. */
void CustomMaps_Fuzz(const std::vector<uint8_t> &lev, const std::vector<uint8_t> &fld)
{
  uint32_t seed = 0x5EED1234u;
  auto next = [&seed]() {
    seed = seed * 1103515245u + 12345u;
    return seed >> 8;
  };
  std::vector<uint8_t> f = fld;
  unsigned accepted = 0;
  for (uint32_t iteration = 0; iteration < CUSTOMMAPS_FUZZ_ITERATIONS; iteration++) {
    std::vector<uint8_t> l = lev;
    const uint32_t mode = next() % 8;
    const uint32_t mutations = 1 + next() % 8;
    std::vector<std::pair<size_t, uint8_t>> fieldUndo;
    for (uint32_t m = 0; m < mutations; m++) {
      if (mode < 5) {
        /* the level: mostly its header, tables, paths and script */
        const size_t offset = (next() % 4 != 0) ? next() % std::min<size_t>(l.size(), 0x1000) : next() % l.size();
        l[offset] = (next() % 3 == 0) ? (uint8_t)(l[offset] ^ (1u << (next() % 8))) : (uint8_t)next();
      }
      else {
        /* the field grid: header or a cell */
        const size_t offset = (next() % 2 != 0) ? next() % 0x200 : next() % f.size();
        fieldUndo.emplace_back(offset, f[offset]);
        f[offset] = (uint8_t)next();
      }
    }
    if (mode == 7) {
      l.resize(next() % (l.size() + 1));
    }
    if (CustomMap_Validate(l.data(), l.size(), f.data(), f.size(), nullptr, 0)) {
      accepted++;
    }
    if ((mode == 6) && (next() % 4 == 0)) {
      /* a truncated field grid copy */
      std::vector<uint8_t> shortField(f.begin(), f.begin() + next() % f.size());
      CustomMap_Validate(lev.data(), lev.size(), shortField.data(), shortField.size(), nullptr, 0);
    }
    for (auto undo = fieldUndo.rbegin(); undo != fieldUndo.rend(); ++undo) {
      f[undo->first] = undo->second;
    }
  }
  s_CustomMapsChecks++;
  Thandor_Log("custommaps: fuzz %u mutated maps validated without a crash (%u accepted)", CUSTOMMAPS_FUZZ_ITERATIONS,
              accepted);
}

} // namespace

void Thandor_SelfTestCustomMaps()
{
  static const char *packages[] = {"PATCH01.PCK", "PATCH00.PCK", "LEVEL.PCK"};
  uint32_t (*savedAlloc)(uint32_t, void **) = g_MemoryApi.alloc;
  uint32_t (*savedFree)(void *) = g_MemoryApi.free;
  g_MemoryApi.alloc = CustomMaps_Alloc;
  g_MemoryApi.free = CustomMaps_Free;
  auto packedBuffer = std::make_unique_for_overwrite<uint8_t[]>(PACKAGE_SCRATCH_BUFFER_BYTES);
  std::unordered_set<std::string> seenPaths;
  std::unordered_map<std::string, std::vector<uint8_t>> levels;
  std::unordered_map<std::string, std::vector<uint8_t>> fields;
  for (const char *package : packages) {
    CustomMaps_LoadPackage(package, packedBuffer.get(), seenPaths, levels, fields);
  }
  g_MemoryApi.alloc = savedAlloc;
  g_MemoryApi.free = savedFree;
  if (levels.empty()) {
    Thandor_Log("custommaps: FAIL stock maps skipped (no level packages in the current directory)");
    return;
  }

  const std::vector<uint8_t> *baseLevel = nullptr;
  const std::vector<uint8_t> *baseField = nullptr;
  unsigned validCount = 0;
  for (const auto &[levelPath, lev] : levels) {
    s_CustomMapsChecks++;
    if ((lev.size() < 0x800) || (CustomMaps_U32(lev, 0xB0) >= lev.size())) {
      CustomMaps_Fail("custommaps: FAIL %s: no field grid path%s", levelPath.c_str(), "");
      continue;
    }
    const std::string fieldPath = CustomMaps_PathKey(lev.data() + CustomMaps_U32(lev, 0xB0),
                                                     (lev.size() - CustomMaps_U32(lev, 0xB0)) / 2);
    const auto field = fields.find(fieldPath);
    if (field == fields.end()) {
      CustomMaps_Fail("custommaps: FAIL %s: field grid %s not found", levelPath.c_str(), fieldPath.c_str());
      continue;
    }
    std::string reason;
    if (!CustomMaps_Validate(lev, field->second, &reason)) {
      s_CustomMapsFailures++;
      Thandor_Log("custommaps: FAIL stock %s rejected: %s", levelPath.c_str(), reason.c_str());
      continue;
    }
    CustomMaps_ExpectValid((levelPath + " derived").c_str(), CustomMaps_Derive(lev, "maps\\Mein Level\\Mein Level"),
                           field->second);
    validCount++;
    if ((levelPath == "level\\asgard") || (baseLevel == nullptr)) {
      baseLevel = &lev;
      baseField = &field->second;
    }
  }
  Thandor_Log("custommaps: %u of %u stock levels valid (%u field grids)", validCount, (unsigned)levels.size(),
              (unsigned)fields.size());
  if (baseLevel != nullptr) {
    CustomMaps_ExpectValid("derived 120-unit field path",
                           CustomMaps_Derive(*baseLevel, std::string(115, 'b').insert(0, "maps\\").c_str()), *baseField);
    CustomMaps_CorruptCases(*baseLevel, *baseField);
    CustomMaps_Fuzz(*baseLevel, *baseField);
  }
  Thandor_Log("custommaps: %s (%u checks, %u failed)", s_CustomMapsFailures == 0 ? "PASS" : "FAIL", s_CustomMapsChecks,
              s_CustomMapsFailures);
}
