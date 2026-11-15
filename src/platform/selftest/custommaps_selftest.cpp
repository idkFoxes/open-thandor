/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/selftest/custommaps_selftest.cpp
 * Project code (not in the original game)
 */

/* OPEN_THANDOR_SELFTEST=custommaps: the custom maps (docs/custom_maps.md).
   Part 1, CustomMap_Validate (assets/scenario/custom_map_validate.h):
     - stock maps: every level\*.lev and level\*.fld entry of PATCH01.PCK, PATCH00.PCK and LEVEL.PCK in the current
       directory (first package with a path wins), decoded natively (the .fld entries by the field grid codec into
       the expanded image a project's .fld holds); every level with its field grid must be accepted, also in the
       layout of a project level (field grid path "level\<name>" in a new 0x100-byte slot at the end of the
       prefix, placements moved behind it);
     - corrupted variants of one stock map (truncated, bad magic, huge dimensions, counts too large, unknown ids,
       ".." / absolute / unterminated paths, a maps\ field grid path, short .fld, ...) must be rejected, each with a
       reason;
     - fuzz: random byte mutations and truncations of the level and the field grid header and cells; the
       validation must return (no crash; the buffers are exactly the file size, so an ASan build reports any read
       past them).
   Part 2, the .thmap projects (custom_project.h, custom_maps.h), in a fresh directory under %TEMP% that stands in
   for the executable directory: ZIP and legacy PCK V3 projects built from Asgard are listed, read and served;
   broken projects (bad ZIP, ".." entries in ZIP and PCK, oversized, bad LEV / FLD / cells, field grid path not the
   project's field entry, no manifest, unknown format), invalid names, a name of an original level and an old map
   folder are not listed; the active project serves only its own files and nothing once released; a fuzz of the
   project reader on mutated ZIP and PCK bytes must return.
   One line per failed check and a summary line; without the packages the test logs "skipped" and fails. */

#include <process.h>
#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <exception>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <thandor/thandor.h>
#include <thandor/assets/package/runtime.h>
#include <thandor/assets/scenario/custom_map_validate.h>
#include <thandor/assets/scenario/custom_maps.h>
#include <thandor/assets/scenario/custom_project.h>
#include <thandor/platform/filesystem/win32.h>
#include <thandor/ui/frontend/scenario_selection.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/selftest/selftest.h>

#include "miniz.h"
#include "thandor/gfx.hpp"
#include "thandor/pck.hpp"
#include "thandor/text_resource.hpp"

namespace {

unsigned s_CustomMapsChecks = 0;
unsigned s_CustomMapsFailures = 0;

constexpr uint32_t CUSTOMMAPS_FUZZ_ITERATIONS = 20000;
constexpr uint32_t CUSTOMMAPS_DERIVED_SLOT_BYTES = 0x100;
constexpr uint32_t CUSTOMMAPS_PROJECT_FUZZ_ITERATIONS = 1500;

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

/* The project level layout (tools/data/custom_map.py build): the field grid path moves into a new
   CUSTOMMAPS_DERIVED_SLOT_BYTES slot at the end of the prefix (the old slot keeps its text, unreferenced), the
   placements move behind it. */
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
  l = CustomMaps_Derive(lev, "level\\..\\..\\evil");
  CustomMaps_ExpectInvalid("derived field path '..'", l, fld);
  l = CustomMaps_Derive(lev, "level\\test\\");
  CustomMaps_ExpectInvalid("derived field path ends '\\'", l, fld);
  l = CustomMaps_Derive(lev, "level\\test.fld");
  CustomMaps_ExpectInvalid("derived field path with '.'", l, fld);
  l = CustomMaps_Derive(lev, "temp\\test");
  CustomMaps_ExpectInvalid("derived field path dir", l, fld);
  l = CustomMaps_Derive(lev, "maps\\test\\test");
  CustomMaps_ExpectInvalid("derived field path maps\\", l, fld);
  l = CustomMaps_Derive(lev, std::string(121, 'a').insert(0, "level\\").c_str());
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

/* ---------------------------------------------------------------- part 2: .thmap projects */

using ProjectEntries = std::vector<std::pair<std::string, std::vector<uint8_t>>>;

std::vector<uint8_t> CustomMaps_TextBytes(const std::string &text)
{
  std::vector<uint8_t> bytes(text.begin(), text.end());
  while (bytes.size() % 4 != 0) {
    bytes.push_back(0); /* the stored PCK writer wants dword sizes; the legacy manifest ends at its first NUL */
  }
  return bytes;
}

/* A map.ini (ZIP manifests must not hold a NUL). */
std::vector<uint8_t> CustomMaps_IniBytes(const std::string &text)
{
  return std::vector<uint8_t>(text.begin(), text.end());
}

std::vector<uint8_t> CustomMaps_FromBytes(const std::vector<std::byte> &bytes)
{
  std::vector<uint8_t> out(bytes.size());
  if (!bytes.empty()) {
    memcpy(out.data(), bytes.data(), bytes.size());
  }
  return out;
}

std::vector<std::byte> CustomMaps_ToBytes(const std::vector<uint8_t> &bytes)
{
  std::vector<std::byte> out(bytes.size());
  if (!bytes.empty()) {
    memcpy(out.data(), bytes.data(), bytes.size());
  }
  return out;
}

/* A ZIP archive of entries with the names as given (no normalization, so broken names can be written). */
std::vector<uint8_t> CustomMaps_Zip(const ProjectEntries &entries)
{
  mz_zip_archive zip{};
  std::vector<uint8_t> result;
  if (!mz_zip_writer_init_heap(&zip, 0, 0)) {
    return result;
  }
  bool ok = true;
  for (const auto &[name, data] : entries) {
    ok = ok && mz_zip_writer_add_mem(&zip, name.c_str(), data.data(), data.size(), MZ_BEST_SPEED);
  }
  void *buffer = nullptr;
  size_t size = 0;
  if (ok && mz_zip_writer_finalize_heap_archive(&zip, &buffer, &size)) {
    result.assign(static_cast<uint8_t *>(buffer), static_cast<uint8_t *>(buffer) + size);
    mz_free(buffer);
  }
  mz_zip_writer_end(&zip);
  return result;
}

/* A legacy stored PCK project (the editor's MapDocument::save_project writer). */
std::vector<uint8_t> CustomMaps_Pck(const ProjectEntries &entries)
{
  std::vector<thandor::PckWriteEntry> items;
  for (const auto &[name, data] : entries) {
    items.push_back({name, CustomMaps_ToBytes(data), 0});
  }
  try {
    return CustomMaps_FromBytes(thandor::build_stored_pck(items));
  }
  catch (const std::exception &error) {
    CustomMaps_Fail("custommaps: FAIL building a PCK project: %s%s", error.what(), "");
    return {};
  }
}

void CustomMaps_Replace(std::vector<uint8_t> &data, const std::string &from, const std::string &to, bool utf16)
{
  auto encode = [utf16](const std::string &text) {
    std::vector<uint8_t> bytes;
    for (char c : text) {
      bytes.push_back((uint8_t)c);
      if (utf16) {
        bytes.push_back(0);
      }
    }
    return bytes;
  };
  const std::vector<uint8_t> a = encode(from);
  const std::vector<uint8_t> b = encode(to);
  for (auto it = std::search(data.begin(), data.end(), a.begin(), a.end()); it != data.end();
       it = std::search(it + 1, data.end(), a.begin(), a.end())) {
    std::copy(b.begin(), b.end(), it);
  }
}

bool CustomMaps_WriteFile(const std::filesystem::path &path, const std::vector<uint8_t> &data)
{
  FILE *file = _wfopen(path.wstring().c_str(), L"wb");
  if (file == nullptr) {
    return false;
  }
  const bool ok = data.empty() || (fwrite(data.data(), 1, data.size(), file) == data.size());
  return (fclose(file) == 0) && ok;
}

std::vector<uint8_t> CustomMaps_Str(const std::string &title, const std::string &description)
{
  thandor::TextResourceAsset text = thandor::TextResourceAsset::minimal_level_compatibility(title);
  text.replace_plain_string_all_locales(1, description);
  return CustomMaps_FromBytes(text.serialize());
}

/* A 256x256 true-colour TGA (a textures/terrain/<letter>.tga replacement). */
std::vector<uint8_t> CustomMaps_Tga()
{
  thandor::RgbaImage image{256, 256, std::vector<uint8_t>(256 * 256 * 4, 0x80)};
  return CustomMaps_FromBytes(thandor::encode_tga_rgba(image));
}

/* The files of a project named name built from a stock level: level/<name>.lev (field grid path level\<name>),
   level/<name>.fld; the caller adds the manifest. */
ProjectEntries CustomMaps_ProjectFiles(const std::string &name, const std::vector<uint8_t> &lev,
                                       const std::vector<uint8_t> &fld)
{
  return {{"level/" + name + ".lev", CustomMaps_Derive(lev, ("level\\" + name).c_str())},
          {"level/" + name + ".fld", fld}};
}

std::string CustomMaps_ZipManifest(const std::string &name, const std::string &extra = "")
{
  return "[map]\r\nformat = 1\r\nlevel_entry = level/" + name + ".lev\r\nfield_entry = level/" + name +
         ".fld\r\n" + extra;
}

std::string CustomMaps_PckManifest(const std::string &name, const std::string &extra = "")
{
  return "THANDOR_MAP_EDITOR_PROJECT_V3\nmap_name=" + name + "\nlevel_entry=level/" + name +
         ".lev\nfield_entry=level/" + name + ".fld\n" + extra;
}

/* The title text of the listed map whose title is text (searching every custom title id); its slot or -1. */
int CustomMaps_FindTitleSlot(const char *text)
{
  for (uint32_t slot = 0; slot < CUSTOM_MAP_TITLE_INDEX_SLOTS; slot++) {
    uint16_t *title = nullptr;
    if (!CustomMaps_TryResolveText(TEXT_ID_LEVEL_TITLE_BASE + CUSTOM_MAP_TITLE_INDEX_BASE + slot, &title) ||
        (title == nullptr)) {
      continue;
    }
    size_t i = 0;
    for (; (text[i] != 0) && (title[i + 1] == (uint8_t)text[i]); i++) {
    }
    if ((text[i] == 0) && (title[i + 1] == 0)) {
      return (int)slot;
    }
  }
  return -1;
}

std::string CustomMaps_Narrow(const uint16_t *text)
{
  std::string out;
  for (; (text != nullptr) && (*text != 0); text++) {
    out.push_back((*text < 0x80) ? (char)*text : '~');
  }
  return out;
}

void CustomMaps_Expect(bool condition, const char *what)
{
  s_CustomMapsChecks++;
  if (!condition) {
    CustomMaps_Fail("custommaps: FAIL %s%s", what, "");
  }
}

bool CustomMaps_Served(const char *path, std::vector<uint8_t> *outData = nullptr)
{
  std::vector<uint16_t> units(path, path + strlen(path) + 1);
  const uint8_t *data = nullptr;
  uint32_t bytes = 0;
  if (!CustomMaps_FindServedAsset(units.data(), &data, &bytes)) {
    return false;
  }
  if (outData != nullptr) {
    outData->assign(data, data + bytes);
  }
  return true;
}

std::vector<uint16_t> CustomMaps_Units(const char *text)
{
  return std::vector<uint16_t>(text, text + strlen(text) + 1);
}

/* Random mutations of a project file: the reader must always return. */
void CustomMaps_FuzzProject(const char *kind, const std::vector<uint8_t> &project)
{
  uint32_t seed = 0xC0FFEE11u;
  auto next = [&seed]() {
    seed = seed * 1103515245u + 12345u;
    return seed >> 8;
  };
  unsigned loaded = 0;
  for (uint32_t iteration = 0; iteration < CUSTOMMAPS_PROJECT_FUZZ_ITERATIONS; iteration++) {
    std::vector<uint8_t> bytes = project;
    const uint32_t mutations = 1 + next() % 6;
    for (uint32_t m = 0; m < mutations; m++) {
      /* mostly the archive structure (the first and last KiB: headers, directory, manifest) */
      const size_t span = std::min<size_t>(bytes.size(), 0x400);
      const uint32_t where = next() % 3;
      const size_t offset = (where == 0) ? next() % span
                            : (where == 1) ? bytes.size() - 1 - next() % span
                                           : next() % bytes.size();
      bytes[offset] = (next() % 3 == 0) ? (uint8_t)(bytes[offset] ^ (1u << (next() % 8))) : (uint8_t)next();
    }
    if (next() % 8 == 0) {
      bytes.resize(next() % (bytes.size() + 1));
    }
    CustomProject result;
    std::string reason;
    if (CustomProject_LoadBytes(std::move(bytes), &result, &reason)) {
      loaded++;
    }
  }
  s_CustomMapsChecks++;
  Thandor_Log("custommaps: project fuzz %s: %u mutated files read without a crash (%u loaded)", kind,
              CUSTOMMAPS_PROJECT_FUZZ_ITERATIONS, loaded);
}

void CustomMaps_ProjectTests(const std::vector<uint8_t> &lev, const std::vector<uint8_t> &fld)
{
  std::error_code error;
  const std::filesystem::path root = std::filesystem::temp_directory_path(error) /
                                     ("ot_custommaps_" + std::to_string(_getpid()));
  const std::filesystem::path maps = root / "maps";
  std::filesystem::remove_all(root, error);
  std::filesystem::create_directories(maps / "oldmap", error);
  if (error) {
    CustomMaps_Fail("custommaps: FAIL cannot create %s: %s", root.string().c_str(), error.message().c_str());
    return;
  }
  CustomMaps_WriteFile(maps / "oldmap" / "oldmap.lev", lev);
  uint16_t savedDirectory[256];
  memcpy(savedDirectory, g_ExecutableDirectoryUtf16, sizeof savedDirectory);
  const std::wstring rootText = root.wstring();
  memset(g_ExecutableDirectoryUtf16, 0, sizeof g_ExecutableDirectoryUtf16);
  for (size_t i = 0; (i < rootText.size()) && (i + 1 < 256); i++) {
    g_ExecutableDirectoryUtf16[i] = (uint16_t)rootText[i];
  }

  /* the projects */
  thandor::RgbaImage tiny{4, 4, std::vector<uint8_t>(64, 0x40)};
  const std::vector<uint8_t> tinyGfx = CustomMaps_FromBytes(thandor::encode_single_rgba_gfx(tiny));
  std::vector<uint8_t> badGfx = tinyGfx;
  CustomMaps_SetU32(badGfx, 4, (uint32_t)badGfx.size() + 0x1000); /* allocation size past the data */
  ProjectEntries zip = CustomMaps_ProjectFiles("good_zip", lev, fld);
  zip.push_back({"map.ini", CustomMaps_IniBytes(CustomMaps_ZipManifest("good_zip",
                    "display_name = Zip Test\r\nauthor = Tester\r\ndefault_human_faction = 1\r\n"))});
  zip.push_back({"gfx/boden/edit_good_zip_a.gfx", tinyGfx});
  zip.push_back({"gfx/boden/bad.gfx", badGfx});
  zip.push_back({"sound/x.sam", std::vector<uint8_t>(64, 1)});
  zip.push_back({"level/level.dat", std::vector<uint8_t>(64, 2)});
  zip.push_back({"textures/terrain/a.tga", CustomMaps_Tga()});
  const std::vector<uint8_t> zipBytes = CustomMaps_Zip(zip);

  ProjectEntries pck = CustomMaps_ProjectFiles("good_pck", lev, fld);
  pck.push_back({"editor/project.txt", CustomMaps_TextBytes(CustomMaps_PckManifest("good_pck",
                    "display_name=\nauthor=PCK%20Autor\nstring_entry=level/good_pck.str\n"))});
  pck.push_back({"level/good_pck.str", CustomMaps_Str("Mein PCK", "PCK description")});
  pck.push_back({"editor/scenario_record.dat", std::vector<uint8_t>(256, 0)});
  const std::vector<uint8_t> pckBytes = CustomMaps_Pck(pck);

  struct BadCase {
    const char *name;
    std::vector<uint8_t> bytes;
  };
  std::vector<BadCase> bad;
  {
    std::vector<uint8_t> b = {'P', 'K', 3, 4};
    for (int i = 0; i < 1000; i++) {
      b.push_back((uint8_t)(i * 37));
    }
    bad.push_back({"bad_zip", b});
  }
  {
    ProjectEntries e = CustomMaps_ProjectFiles("trav_zip", lev, fld);
    e.push_back({"map.ini", CustomMaps_IniBytes(CustomMaps_ZipManifest("trav_zip"))});
    e.push_back({"aa/evil.gfx", tinyGfx});
    std::vector<uint8_t> b = CustomMaps_Zip(e);
    CustomMaps_Replace(b, "aa/evil.gfx", "../evil.gfx", false);
    bad.push_back({"trav_zip", b});
  }
  {
    ProjectEntries e = CustomMaps_ProjectFiles("trav_pck", lev, fld);
    e.push_back({"editor/project.txt", CustomMaps_TextBytes(CustomMaps_PckManifest("trav_pck"))});
    e.push_back({"gfx/boden/evil_x.gfx", tinyGfx});
    std::vector<uint8_t> b = CustomMaps_Pck(e);
    CustomMaps_Replace(b, "gfx\\boden\\evil_x.gfx", "..\\..\\..\\evil_xx.gfx", true);
    bad.push_back({"trav_pck", b});
  }
  {
    ProjectEntries e = CustomMaps_ProjectFiles("bad_lev", lev, fld);
    e[0].second[0] = 'x';
    e.push_back({"map.ini", CustomMaps_IniBytes(CustomMaps_ZipManifest("bad_lev"))});
    bad.push_back({"bad_lev", CustomMaps_Zip(e)});
  }
  {
    ProjectEntries e = CustomMaps_ProjectFiles("bad_place", lev, fld);
    CustomMaps_SetU32(e[0].second, CustomMaps_U32(e[0].second, 0xDC) + 4, 8); /* placement faction 8 */
    e.push_back({"map.ini", CustomMaps_IniBytes(CustomMaps_ZipManifest("bad_place"))});
    bad.push_back({"bad_place", CustomMaps_Zip(e)});
  }
  {
    ProjectEntries e = CustomMaps_ProjectFiles("bad_fld", lev, fld);
    e[1].second.resize(e[1].second.size() - 0x80);
    e.push_back({"map.ini", CustomMaps_IniBytes(CustomMaps_ZipManifest("bad_fld"))});
    bad.push_back({"bad_fld", CustomMaps_Zip(e)});
  }
  {
    ProjectEntries e = CustomMaps_ProjectFiles("bad_cells", lev, fld);
    const size_t cell = 0x200 + (size_t)(CustomMaps_U32(fld, 0xB8) + 1) * 0x80;
    CustomMaps_SetU32(e[1].second, cell + 0x50, (CustomMaps_U32(fld, cell + 0x50) & ~0xFFu) | 30);
    e.push_back({"editor/project.txt", CustomMaps_TextBytes(CustomMaps_PckManifest("bad_cells"))});
    bad.push_back({"bad_cells", CustomMaps_Pck(e)});
  }
  {
    ProjectEntries e = CustomMaps_ProjectFiles("bad_side", lev, fld);
    e.push_back({"map.ini", CustomMaps_IniBytes(CustomMaps_ZipManifest("bad_side"))});
    /* a 55 x 55 grid (below the 59..139 sides of the map check, valid for the reader) */
    std::vector<uint8_t> small(0x200 + 55 * 55 * 0x80, 0);
    memcpy(small.data(), fld.data(), 0x200);
    CustomMaps_SetU32(small, 0x04, (uint32_t)small.size());
    CustomMaps_SetU32(small, 0xB8, 55);
    CustomMaps_SetU32(small, 0xBC, 55);
    e[1].second = small;
    bad.push_back({"bad_side", CustomMaps_Zip(e)});
  }
  {
    ProjectEntries e = {{"level/fm.lev", CustomMaps_Derive(lev, "level\\other")}, {"level/fm.fld", fld}};
    e.push_back({"map.ini", CustomMaps_IniBytes(CustomMaps_ZipManifest("fm"))});
    bad.push_back({"field_mismatch", CustomMaps_Zip(e)});
  }
  {
    ProjectEntries e = {{"level/mf.lev", CustomMaps_Derive(lev, "maps\\mf")}, {"maps/mf.fld", fld}};
    e.push_back({"map.ini", CustomMaps_IniBytes("[map]\r\nformat=1\r\nlevel_entry=level/mf.lev\r\n"
                                                 "field_entry=maps/mf.fld\r\n")});
    bad.push_back({"maps_field", CustomMaps_Zip(e)});
  }
  {
    bad.push_back({"no_manifest", CustomMaps_Zip(CustomMaps_ProjectFiles("no_manifest", lev, fld))});
  }
  {
    ProjectEntries e = CustomMaps_ProjectFiles("bad_format", lev, fld);
    std::string manifest = CustomMaps_ZipManifest("bad_format");
    manifest.replace(manifest.find("format = 1"), 10, "format = 2");
    e.push_back({"map.ini", CustomMaps_IniBytes(manifest)});
    bad.push_back({"bad_format", CustomMaps_Zip(e)});
  }
  {
    ProjectEntries e = CustomMaps_ProjectFiles("old_v9", lev, fld);
    e.push_back({"editor/project.txt", CustomMaps_TextBytes("THANDOR_MAP_EDITOR_PROJECT_V9\nlevel_entry=level/old_v9.lev"
                                                            "\nfield_entry=level/old_v9.fld\n")});
    bad.push_back({"old_v9", CustomMaps_Pck(e)});
  }
  for (const BadCase &c : bad) {
    CustomMaps_WriteFile(maps / (std::string(c.name) + ".thmap"), c.bytes);
  }
  { /* larger than the limit: rejected before it is read */
    FILE *file = _wfopen((maps / "oversized.thmap").wstring().c_str(), L"wb");
    if (file != nullptr) {
      fwrite("PK\3\4", 1, 4, file);
      _fseeki64(file, (int64_t)CustomProject_MaxFileBytes(), SEEK_SET);
      fputc(0, file);
      fclose(file);
    }
  }
  CustomMaps_WriteFile(maps / "good_zip.thmap", zipBytes);
  CustomMaps_WriteFile(maps / "good_pck.thmap", pckBytes);
  CustomMaps_WriteFile(maps / "asgard.thmap", zipBytes);                    /* an original level's name */
  CustomMaps_WriteFile(maps / "S01_INSEL.thmap", zipBytes);                 /* a level.dat identifier */
  CustomMaps_WriteFile(maps / "bad name.thmap", zipBytes);                  /* a space */
  CustomMaps_WriteFile(maps / "abcdefghijklmnopqrstuvwxyz01234.thmap", zipBytes); /* 31 characters */
  CustomMaps_WriteFile(maps / "notes.txt", zipBytes);                       /* not a project */

  /* the checks of each broken file, with their reasons */
  bad.push_back({"oversized", {}});
  for (const BadCase &c : bad) {
    std::string reason;
    s_CustomMapsChecks++;
    if (CustomMaps_CheckProjectForTest(c.name, &reason)) {
      CustomMaps_Fail("custommaps: FAIL project %s accepted%s", c.name, "");
    }
    else if (reason.empty()) {
      CustomMaps_Fail("custommaps: FAIL project %s rejected without a reason%s", c.name, "");
    }
    else {
      Thandor_Log("custommaps: project %-16s rejected: %s", c.name, reason.c_str());
    }
  }
  for (const char *name : {"good_zip", "good_pck"}) {
    std::string reason;
    s_CustomMapsChecks++;
    if (!CustomMaps_CheckProjectForTest(name, &reason)) {
      CustomMaps_Fail("custommaps: FAIL project %s rejected: %s", name, reason.c_str());
    }
  }

  /* the scan with a catalog that has an original "Asgard." and "s01_insel." */
  std::vector<uint8_t> catalogBytes(sizeof(ScenarioCatalogHeader) + 2 * sizeof(ScenarioCatalogRecord), 0);
  ScenarioCatalogHeader *catalog = reinterpret_cast<ScenarioCatalogHeader *>(catalogBytes.data());
  catalog->levelRecordsOffset = sizeof(ScenarioCatalogHeader);
  catalog->levelRecordCount = 2;
  ScenarioCatalogRecord *records = reinterpret_cast<ScenarioCatalogRecord *>(catalogBytes.data() + sizeof *catalog);
  const char *identifiers[2] = {"Asgard.", "s01_insel."};
  for (int r = 0; r < 2; r++) {
    for (size_t i = 0; identifiers[r][i] != 0; i++) {
      records[r].identifier[i] = (uint8_t)identifiers[r][i];
    }
  }
  CustomMaps_Rebuild(catalog);
  std::string listed;
  for (uint32_t i = 0; CustomMaps_ListedName(i) != nullptr; i++) {
    listed += std::string(listed.empty() ? "" : ",") + CustomMaps_ListedName(i);
  }
  Thandor_Log("custommaps: scan listed %u: %s", CustomMaps_Count(), listed.c_str());
  CustomMaps_Expect(listed == "good_pck,good_zip", "scan lists exactly good_pck and good_zip");
  {
    /* the hint texts count the rejected files of the scan (the hint row itself only shows without a listed map) */
    uint16_t *description = nullptr;
    CustomMaps_Expect(CustomMaps_TryResolveText(TEXT_ID_CUSTOM_MAPS_HINT_DESCRIPTION, &description),
                      "the hint description resolves");
    const std::string text = CustomMaps_Narrow(description);
    CustomMaps_Expect(text.find(" abgelehnt, siehe thandor.log)") != std::string::npos ||
                      text.find(" rejected, see thandor.log)") != std::string::npos,
                      "the hint description names the rejected files");
  }

  /* texts: the title from display_name, else STR string 0; the author in the description */
  const int zipSlot = CustomMaps_FindTitleSlot("Zip Test");
  const int pckSlot = CustomMaps_FindTitleSlot("Mein PCK");
  CustomMaps_Expect(zipSlot >= 0, "ZIP title \"Zip Test\" from display_name");
  CustomMaps_Expect(pckSlot >= 0, "PCK title \"Mein PCK\" from the STR");
  if (pckSlot >= 0) {
    uint16_t *description = nullptr;
    CustomMaps_TryResolveText(TEXT_ID_LEVEL_DESCRIPTION_BASE +
                              TEXT_ID_LEVEL_DESCRIPTION_STRIDE * (CUSTOM_MAP_TITLE_INDEX_BASE + pckSlot), &description);
    const std::string text = CustomMaps_Narrow(description);
    Thandor_Log("custommaps: PCK description \"%s\"", text.c_str());
    CustomMaps_Expect(text.find("PCK description") == 0 && text.find("utor: PCK Autor") != std::string::npos,
                      "PCK description from the STR with the author");
  }

  /* the active project: its files only, nothing once released */
  std::vector<uint8_t> served;
  CustomMaps_Expect(!CustomMaps_Served("level\\good_zip.fld"), "nothing served before a project is active");
  CustomMaps_Expect(CustomMaps_ActivateLevelPath(CustomMaps_Units("maps\\good_zip.lev").data()), "activate good_zip");
  CustomMaps_Expect(CustomMaps_Served("MAPS\\Good_Zip.LEV", &served) && served == zip[0].second,
                    "the level served as maps\\good_zip.lev (case ignored)");
  CustomMaps_Expect(CustomMaps_Served("level/good_zip.fld", &served) && served == fld, "the field grid served");
  CustomMaps_Expect(CustomMaps_Served("gfx\\boden\\edit_good_zip_a.gfx", &served) && served == tinyGfx,
                    "a custom terrain texture served");
  CustomMaps_Expect(CustomMaps_Served("gfx\\boden\\soila.gfx"), "the TGA replacement of the ground texture a served");
  CustomMaps_Expect(!CustomMaps_Served("gfx\\boden\\bad.gfx"), "a texture with a wrong size not served");
  CustomMaps_Expect(!CustomMaps_Served("sound\\x.sam"), "a sound not served");
  CustomMaps_Expect(!CustomMaps_Served("level\\level.dat"), "level\\level.dat not served");
  CustomMaps_Expect(!CustomMaps_Served("level\\good_zip.lev"), "the level entry not served under its own name");
  {
    uint32_t (*savedAlloc)(uint32_t, void **) = g_MemoryApi.alloc;
    uint32_t (*savedFree)(void *) = g_MemoryApi.free;
    g_MemoryApi.alloc = CustomMaps_Alloc;
    g_MemoryApi.free = CustomMaps_Free;
    std::vector<uint16_t> path = CustomMaps_Units("level\\good_zip.fld");
    uint32_t bytes = 0;
    uint32_t loadError = 0;
    void *loaded = Package_LoadEntryWithSize(path.data(), &bytes, &loadError);
    CustomMaps_Expect((loaded != nullptr) && (bytes == fld.size()) && (memcmp(loaded, fld.data(), bytes) == 0),
                      "Package_LoadEntryWithSize loads the served field grid");
    if (loaded != nullptr) {
      g_MemoryApi.free(loaded);
    }
    std::vector<uint8_t> small(16);
    path = CustomMaps_Units("gfx\\boden\\edit_good_zip_a.gfx");
    CustomMaps_Expect(!Package_LoadEntryIntoBuffer((PckLoadCapacityFlags)small.size(), small.data(), path.data(),
                                                   &loadError) && (loadError == FATAL_ERROR_OUT_OF_MEMORY),
                      "Package_LoadEntryIntoBuffer refuses a served file larger than the buffer");
    g_MemoryApi.alloc = savedAlloc;
    g_MemoryApi.free = savedFree;
  }
  CustomMaps_Expect(CustomMaps_ActivateIdentifier(CustomMaps_Units("maps\\good_pck.").data()) &&
                    CustomMaps_Served("level\\good_pck.fld") && !CustomMaps_Served("level\\good_zip.fld"),
                    "the identifier maps\\good_pck. switches to good_pck");
  CustomMaps_Expect(CustomMaps_ActivateLevelPath(CustomMaps_Units("level\\asgard.lev").data()) &&
                    !CustomMaps_Served("level\\good_pck.fld") && !CustomMaps_Served("maps\\good_pck.lev"),
                    "an original level releases the project");
  CustomMaps_Expect(!CustomMaps_ActivateLevelPath(CustomMaps_Units("maps\\bad_cells.lev").data()) &&
                    !CustomMaps_Served("level\\bad_cells.fld"), "a broken project is not activated");
  CustomMaps_Expect(!CustomMaps_ActivateLevelPath(CustomMaps_Units("maps\\missing.lev").data()),
                    "a missing project is not activated");
  CustomMaps_ActivateLevelPath(CustomMaps_Units("maps\\good_zip.lev").data());
  CustomMaps_Rebuild(catalog);
  CustomMaps_Expect(!CustomMaps_Served("level\\good_zip.fld"), "the scan releases the project");
  CustomMaps_ActivateLevelPath(CustomMaps_Units("maps\\good_zip.lev").data());
  CustomMaps_Deactivate();
  CustomMaps_Expect(!CustomMaps_Served("gfx\\boden\\soila.gfx"), "nothing served after the release");

  /* the reader on mutated files (a small project: the level, a header-only field grid, a text) */
  ProjectEntries fuzzEntries = {{"level/fz.lev", CustomMaps_Derive(lev, "level\\fz")},
                                {"level/fz.fld", std::vector<uint8_t>(fld.begin(), fld.begin() + 0x200)},
                                {"level/fz.str", CustomMaps_Str("Fuzz", "Fuzz text")}};
  ProjectEntries fuzzZip = fuzzEntries;
  fuzzZip.push_back({"map.ini", CustomMaps_IniBytes(CustomMaps_ZipManifest("fz", "string_entry=level/fz.str\r\n"))});
  fuzzZip.push_back({"textures/terrain/b.tga", CustomMaps_Tga()});
  ProjectEntries fuzzPck = fuzzEntries;
  fuzzPck.push_back({"editor/project.txt",
                     CustomMaps_TextBytes(CustomMaps_PckManifest("fz", "string_entry=level/fz.str\n"))});
  CustomMaps_FuzzProject("ZIP", CustomMaps_Zip(fuzzZip));
  CustomMaps_FuzzProject("PCK", CustomMaps_Pck(fuzzPck));

  std::filesystem::remove_all(root, error);
  CustomMaps_Rebuild(nullptr); /* the directory is gone: nothing listed afterwards */
  {
    /* no maps folder: the hint row behind the stock rows, not counted, no level */
    ScenarioCatalogDisplayRecord stockRecord{};
    Ptr32<ScenarioCatalogDisplayRecord> stockRows[1] = {};
    stockRows[0] = &stockRecord;
    uint32_t rowCount = 0;
    bool hintRow = false;
    Ptr32<ScenarioCatalogDisplayRecord> *rows = CustomMaps_ListRows(stockRows, 1, &rowCount, &hintRow);
    CustomMaps_Expect(hintRow && (rowCount == 1) && (rows[0].get() == &stockRecord) &&
                      CustomMaps_IsHintRecord(rows[1].get()) && !CustomMaps_IsHintRecord(&stockRecord),
                      "without a maps folder the hint row follows the stock rows, uncounted");
    uint16_t levelPath[64] = {};
    CustomMaps_Expect(!CustomMaps_LevelPathOfRecord(levelPath, rows[1].get()) && (rows[1]->identifier[0] == 0x01) &&
                      (rows[1]->identifier[1] == 0), "the hint row has no level path and identifier 0x01");
    uint16_t *text = nullptr;
    CustomMaps_TryResolveText(TEXT_ID_CUSTOM_MAPS_HINT_DESCRIPTION, &text);
    const std::string description = CustomMaps_Narrow(text);
    CustomMaps_TryResolveText(TEXT_ID_CUSTOM_MAPS_HINT_LINK, &text);
    const std::string link = CustomMaps_Narrow(text);
    Thandor_Log("custommaps: hint \"%s\" link \"%s\"", description.c_str(), link.c_str());
    CustomMaps_Expect((description.find("Keine eigenen Karten gefunden. ") == 0 ||
                       description.find("No custom maps found. ") == 0) &&
                      (description.find("thandor.exe.") != std::string::npos) &&
                      (link == "Map Editor auf GitHub" || link == "Map Editor on GitHub"),
                      "the hint texts without rejected files");
  }
  memcpy(g_ExecutableDirectoryUtf16, savedDirectory, sizeof savedDirectory);
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
    CustomMaps_ExpectValid((levelPath + " derived").c_str(), CustomMaps_Derive(lev, "level\\Mein_Level"),
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
                           CustomMaps_Derive(*baseLevel, std::string(114, 'b').insert(0, "level\\").c_str()), *baseField);
    CustomMaps_CorruptCases(*baseLevel, *baseField);
    CustomMaps_Fuzz(*baseLevel, *baseField);
    CustomMaps_ProjectTests(*baseLevel, *baseField);
  }
  Thandor_Log("custommaps: %s (%u checks, %u failed)", s_CustomMapsFailures == 0 ? "PASS" : "FAIL", s_CustomMapsChecks,
              s_CustomMapsFailures);
}
