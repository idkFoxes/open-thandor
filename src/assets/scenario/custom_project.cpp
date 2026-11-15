/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/scenario/custom_project.cpp
 * Project code (not in the original game)
 */

/* Map editor projects (.thmap, see custom_project.h). Adapted from custom_maps::Project::load of the
   OpenThandor_ZipMaps fork (src/assets/scenario/custom_project.cpp there): the same archive reader and entry rules,
   but no faction or text alias rewrites (the game keeps its own faction choice and text ids, custom_maps.cpp) and
   only the terrain files are kept for serving. */

#include <algorithm>
#include <cstring>
#include <exception>
#include <thandor/assets/scenario/custom_project.h>

#include "thandor/map_project_archive.hpp"
#include "thandor/text_resource.hpp"

namespace {

using namespace thandor;

void CustomProject_Need(bool value,const char *reason)
{
  if (!value) {
    throw FormatError(reason);
  }
}

std::vector<uint8_t> CustomProject_Bytes(const std::vector<std::byte> &bytes)
{
  std::vector<uint8_t> copy(bytes.size());
  if (!bytes.empty()) {
    std::memcpy(copy.data(),bytes.data(),bytes.size());
  }
  return copy;
}

bool CustomProject_EndsWith(const std::string &text,const char *suffix)
{
  const size_t length = std::strlen(suffix);
  return (text.size() >= length) && (text.compare(text.size() - length,length,suffix) == 0);
}

/* A terrain file the game may load: gfx/boden/<...>.gfx, .pal or .dat; a .gfx or .pal must carry its magic and
   an allocation size equal to its byte count (the game's checks of these assets trust that size). */
bool CustomProject_IsServable(const std::string &path,const std::vector<std::byte> &data)
{
  if (path.rfind("gfx/boden/",0) != 0) {
    return false;
  }
  const bool gfx = CustomProject_EndsWith(path,".gfx");
  const bool pal = CustomProject_EndsWith(path,".pal");
  if (!gfx && !pal) {
    return CustomProject_EndsWith(path,".dat");
  }
  return (data.size() >= 8) && (read_u32(data,0) == fourcc(gfx ? "gfx" : "pal")) && (read_u32(data,4) == data.size());
}

void CustomProject_Read(MapProjectArchive archive,CustomProject *project)
{
  const MapProjectManifest meta = archive.manifest();
  std::map<uint32_t,std::vector<std::byte>> editedTerrain = read_edited_terrain_images(archive,meta);

  CustomProject_Need(meta.contains("level_entry") && meta.contains("field_entry"),
                     "manifest lacks level_entry or field_entry");
  project->zip = archive.zip;
  project->levelEntry = normalize_project_path(meta.at("level_entry"));
  project->fieldEntry = normalize_project_path(meta.at("field_entry"));
  std::string stringEntry;
  if (meta.contains("string_entry")) {
    stringEntry = normalize_project_path(meta.at("string_entry"));
  }
  CustomProject_Need(project->levelEntry.starts_with("level/") && CustomProject_EndsWith(project->levelEntry,".lev") &&
                     project->fieldEntry.starts_with("level/") && CustomProject_EndsWith(project->fieldEntry,".fld") &&
                     (stringEntry.empty() || CustomProject_EndsWith(stringEntry,".str")),
                     "invalid project entry types");
  const std::vector<std::byte> &lev = archive.require(project->levelEntry);
  const std::vector<std::byte> &fld = archive.require(project->fieldEntry);

  /* the level's field grid path (+0xB0) names the project's field entry */
  CustomProject_Need(lev.size() >= 0x800 && read_u32(lev,0) == fourcc("lev"),"invalid project LEV");
  const uint32_t fieldPathOffset = read_u32(lev,0xB0);
  CustomProject_Need(fieldPathOffset < lev.size(),"LEV field grid path outside the level");
  const std::string fieldBase = read_utf16le_z(lev,fieldPathOffset,(lev.size() - fieldPathOffset) & ~(size_t)1);
  CustomProject_Need(!fieldBase.empty() && normalize_project_path(fieldBase + ".fld") == project->fieldEntry,
                     "LEV field grid path does not match the project's field entry");

  for (const auto &[path,data] : archive.files) {
    if (CustomProject_IsServable(path,data)) {
      project->servedAssets.emplace(path,CustomProject_Bytes(data));
    }
  }
  if (!editedTerrain.empty()) {
    /* ZIP textures/terrain/<letter>.tga replace the ground textures of the level (+0xB4 path + letter) */
    const uint32_t groundOffset = read_u32(lev,0xB4);
    CustomProject_Need(groundOffset < lev.size(),"LEV ground texture path outside the level");
    std::string base = normalize_project_path(read_utf16le_z(lev,groundOffset,(lev.size() - groundOffset) & ~(size_t)1));
    for (auto &[slot,gfx] : editedTerrain) {
      const std::string path = base + static_cast<char>('a' + slot) + ".gfx";
      CustomProject_Need(CustomProject_IsServable(path,gfx),"replaced terrain texture outside gfx/boden/");
      project->servedAssets.insert_or_assign(path,CustomProject_Bytes(gfx));
    }
  }

  project->lev = CustomProject_Bytes(lev);
  project->fld = CustomProject_Bytes(fld);
  /* older editors wrote the LEV converter version into new field grids; the layout is the same */
  if ((project->fld.size() >= 0x10) && (read_u32(fld,0) == fourcc("fld")) && (read_u32(fld,12) == 0x00070001u)) {
    const uint32_t converter = 0x00060006u;
    std::memcpy(project->fld.data() + 12,&converter,sizeof converter);
  }

  if (!stringEntry.empty()) {
    const TextResourceAsset strings = TextResourceAsset::parse(archive.require(stringEntry));
    std::vector<uint32_t> countries = strings.country_codes();
    countries.push_back(0);
    for (uint32_t country : countries) {
      const uint32_t preferred = (country == 0) ? 44u : country;
      std::vector<std::vector<uint16_t>> &lines = project->texts[country];
      lines.resize(CUSTOM_PROJECT_TEXT_STRINGS);
      const uint32_t count = std::min(strings.string_count(preferred),CUSTOM_PROJECT_TEXT_STRINGS);
      for (uint32_t index = 0; index < count; index++) {
        lines[index] = strings.string_units(index,preferred);
      }
    }
  }
  if (meta.contains("display_name")) {
    project->displayName = meta.at("display_name");
  }
  if (meta.contains("author")) {
    project->author = meta.at("author");
  }
}

} // namespace

uint64_t CustomProject_MaxFileBytes()
{
  return kMapProjectMaxArchiveBytes;
}

bool CustomProject_LoadBytes(std::vector<uint8_t> bytes,CustomProject *outProject,std::string *outReason)
{
  *outProject = CustomProject{};
  try {
    std::vector<std::byte> data(bytes.size());
    if (!bytes.empty()) {
      std::memcpy(data.data(),bytes.data(),bytes.size());
    }
    bytes.clear();
    bytes.shrink_to_fit();
    CustomProject_Read(MapProjectArchive::parse(std::move(data)),outProject);
    return true;
  }
  catch (const std::exception &error) {
    *outProject = CustomProject{};
    *outReason = error.what();
    return false;
  }
}

bool CustomProject_Load(const std::filesystem::path &file,CustomProject *outProject,std::string *outReason)
{
  *outProject = CustomProject{};
  try {
    CustomProject_Read(MapProjectArchive::open(file),outProject);
    return true;
  }
  catch (const std::exception &error) {
    *outProject = CustomProject{};
    *outReason = error.what();
    return false;
  }
}
