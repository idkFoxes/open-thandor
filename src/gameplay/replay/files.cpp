/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/replay/files.cpp
 * Project code (not in the original game)
 */

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <thandor/gameplay/replay/files.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <iterator>

#include <thandor/platform/bootstrap/image.h>

namespace thandor::replay {

namespace {

constexpr std::uint64_t FINGERPRINT_SEED = 0xcbf29ce484222325ull;

/* 64-bit FNV-style hash over 8-byte words (fast enough for the some 140 MB of packages) */
std::uint64_t HashBytes(std::uint64_t hash, const std::uint8_t *data, std::size_t size)
{
  std::size_t index = 0;
  for (; index + 8 <= size; index += 8) {
    std::uint64_t word;
    std::memcpy(&word, data + index, 8);
    hash = (hash ^ word) * 0x100000001b3ull;
    hash ^= hash >> 29;
  }
  for (; index < size; index++) {
    hash = (hash ^ data[index]) * 0x100000001b3ull;
  }
  return hash;
}

bool HashFile(const std::wstring &path, std::uint64_t &hash)
{
  HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
                            FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
  if (file == INVALID_HANDLE_VALUE) {
    return false;
  }
  std::vector<std::uint8_t> buffer(1u << 20);
  bool ok = true;
  for (;;) {
    DWORD got = 0;
    if (!ReadFile(file, buffer.data(), static_cast<DWORD>(buffer.size()), &got, nullptr)) {
      ok = false;
      break;
    }
    if (got == 0) {
      break;
    }
    hash = HashBytes(hash, buffer.data(), got);
  }
  CloseHandle(file);
  return ok;
}

} // namespace

std::string Narrow(const std::wstring &text)
{
  if (text.empty()) {
    return {};
  }
  const int length = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), nullptr, 0,
                                         nullptr, nullptr);
  std::string out(static_cast<std::size_t>(std::max(length, 0)), '\0');
  if (length > 0) {
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), out.data(), length, nullptr,
                        nullptr);
  }
  return out;
}

std::wstring Widen(const std::string &text)
{
  if (text.empty()) {
    return {};
  }
  const int length = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), nullptr, 0);
  std::wstring out(static_cast<std::size_t>(std::max(length, 0)), L'\0');
  if (length > 0) {
    MultiByteToWideChar(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), out.data(), length);
  }
  return out;
}

std::wstring EnvironmentValue(const wchar_t *name)
{
  wchar_t buffer[2048];
  const DWORD length = GetEnvironmentVariableW(name, buffer, static_cast<DWORD>(std::size(buffer)));
  if (length == 0 || length >= std::size(buffer)) {
    return {};
  }
  return std::wstring(buffer, length);
}

bool EnvironmentFlag(const wchar_t *name)
{
  const std::wstring value = EnvironmentValue(name);
  return !value.empty() && value != L"0" && _wcsicmp(value.c_str(), L"off") != 0;
}

std::wstring CommandLineReplayPath()
{
  const wchar_t *line = GetCommandLineW();
  const wchar_t *found = line != nullptr ? wcsstr(line, L"-REPLAY=\"") : nullptr;
  if (found == nullptr) {
    return {};
  }
  found += wcslen(L"-REPLAY=\"");
  const wchar_t *end = wcschr(found, L'"');
  return end == nullptr ? std::wstring(found) : std::wstring(found, end);
}

std::wstring ExecutableDirectory()
{
  wchar_t path[MAX_PATH * 4];
  const DWORD length = GetModuleFileNameW(nullptr, path, static_cast<DWORD>(std::size(path)));
  std::wstring directory(path, length);
  const std::size_t slash = directory.find_last_of(L"\\/");
  return slash == std::wstring::npos ? std::wstring() : directory.substr(0, slash + 1);
}

std::wstring AbsolutePath(const std::wstring &path)
{
  wchar_t full[MAX_PATH * 4];
  const DWORD length = GetFullPathNameW(path.c_str(), static_cast<DWORD>(std::size(full)), full, nullptr);
  return length == 0 || length >= std::size(full) ? path : std::wstring(full, length);
}

bool FileExists(const std::wstring &path)
{
  const DWORD attributes = GetFileAttributesW(path.c_str());
  return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

bool ReadFileBytes(const std::wstring &path, std::vector<std::uint8_t> &bytes)
{
  HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
  if (file == INVALID_HANDLE_VALUE) {
    return false;
  }
  LARGE_INTEGER size;
  bool ok = GetFileSizeEx(file, &size) && size.QuadPart > 0 && size.QuadPart < (256ll << 20);
  if (ok) {
    bytes.resize(static_cast<std::size_t>(size.QuadPart));
    DWORD got = 0;
    ok = ReadFile(file, bytes.data(), static_cast<DWORD>(bytes.size()), &got, nullptr) && got == bytes.size();
  }
  CloseHandle(file);
  return ok;
}

bool WriteFileBytes(const std::wstring &path, const std::vector<std::uint8_t> &bytes)
{
  HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (file == INVALID_HANDLE_VALUE) {
    return false;
  }
  DWORD wrote = 0;
  const bool ok = WriteFile(file, bytes.data(), static_cast<DWORD>(bytes.size()), &wrote, nullptr) &&
                  wrote == bytes.size();
  CloseHandle(file);
  return ok;
}

bool CreateDirectoryIfMissing(const std::wstring &path)
{
  return CreateDirectoryW(path.c_str(), nullptr) || GetLastError() == ERROR_ALREADY_EXISTS;
}

std::wstring CreateTemporaryDirectory()
{
  wchar_t base[MAX_PATH + 1];
  const DWORD length = GetTempPathW(static_cast<DWORD>(std::size(base)), base);
  std::wstring directory = length != 0 && length < std::size(base) ? std::wstring(base, length) : ExecutableDirectory();
  wchar_t leaf[80];
  swprintf(leaf, std::size(leaf), L"open-thandor-recorder-%lu-%llu\\", GetCurrentProcessId(),
           static_cast<unsigned long long>(GetTickCount64()));
  directory += leaf;
  if (!CreateDirectoryW(directory.c_str(), nullptr)) {
    return {};
  }
  return directory;
}

void RemoveTemporaryDirectory(const std::wstring &directory)
{
  if (directory.empty()) {
    return;
  }
  WIN32_FIND_DATAW found;
  HANDLE search = FindFirstFileW((directory + L"*").c_str(), &found);
  if (search != INVALID_HANDLE_VALUE) {
    do {
      if ((found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
        DeleteFileW((directory + found.cFileName).c_str());
      }
    } while (FindNextFileW(search, &found));
    FindClose(search);
  }
  RemoveDirectoryW(directory.c_str());
}

std::string LocalTimeText(bool forFileName)
{
  SYSTEMTIME now;
  char text[32];
  GetLocalTime(&now);
  snprintf(text, sizeof text, forFileName ? "%04u-%02u-%02u_%02u-%02u-%02u" : "%04u-%02u-%02u %02u:%02u:%02u",
           now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond);
  return text;
}

void RotateRecordings(const std::wstring &directory, unsigned keepCount, unsigned keepFree, std::uint64_t maxBytes)
{
  struct Found {
    std::wstring name;
    std::uint64_t size;
  };
  std::vector<Found> files;
  WIN32_FIND_DATAW found;
  HANDLE search = FindFirstFileW((directory + L"*.otr").c_str(), &found);
  if (search == INVALID_HANDLE_VALUE) {
    return;
  }
  do {
    if ((found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
      files.push_back({found.cFileName, (static_cast<std::uint64_t>(found.nFileSizeHigh) << 32) | found.nFileSizeLow});
    }
  } while (FindNextFileW(search, &found));
  FindClose(search);
  std::sort(files.begin(), files.end(), [](const Found &a, const Found &b) { return _wcsicmp(a.name.c_str(), b.name.c_str()) > 0; });
  std::uint64_t total = 0;
  unsigned kept = 0;
  for (const Found &file : files) {
    if (kept + keepFree < keepCount && total + file.size <= maxBytes) {
      total += file.size; /* kept: counts towards the total; a deleted one does not */
      kept++;
      continue;
    }
    if (DeleteFileW((directory + file.name).c_str())) {
      Thandor_Log("recorder: removed the old recording recordings\\%s (%llu bytes)", Narrow(file.name).c_str(),
                  static_cast<unsigned long long>(file.size));
    }
  }
}

AssetFingerprint ComputeAssetFingerprint(const std::wstring &levelFile)
{
  struct CacheEntry {
    std::wstring path;
    std::uint64_t size;
    std::uint64_t writeTime;
    std::uint64_t hash;
  };
  static std::vector<CacheEntry> s_cache;
  AssetFingerprint result;
  std::vector<std::pair<std::wstring, std::wstring>> files; /* key (lower-case name), full path */
  const std::wstring directory = ExecutableDirectory();
  WIN32_FIND_DATAW found;
  HANDLE search = FindFirstFileW((directory + L"*.pck").c_str(), &found);
  if (search != INVALID_HANDLE_VALUE) {
    do {
      if ((found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
        continue;
      }
      std::wstring name = found.cFileName;
      std::transform(name.begin(), name.end(), name.begin(), [](wchar_t c) { return static_cast<wchar_t>(towlower(c)); });
      files.emplace_back(name, directory + found.cFileName);
    } while (FindNextFileW(search, &found));
    FindClose(search);
  }
  std::sort(files.begin(), files.end());
  if (!levelFile.empty() && FileExists(levelFile)) {
    std::wstring name = levelFile.substr(levelFile.find_last_of(L"\\/") + 1);
    std::transform(name.begin(), name.end(), name.begin(), [](wchar_t c) { return static_cast<wchar_t>(towlower(c)); });
    files.emplace_back(L"level:" + name, levelFile);
  }
  result.hash = FINGERPRINT_SEED;
  for (const auto &[key, path] : files) {
    WIN32_FILE_ATTRIBUTE_DATA data;
    if (!GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &data)) {
      continue;
    }
    const std::uint64_t size = (static_cast<std::uint64_t>(data.nFileSizeHigh) << 32) | data.nFileSizeLow;
    const std::uint64_t writeTime =
        (static_cast<std::uint64_t>(data.ftLastWriteTime.dwHighDateTime) << 32) | data.ftLastWriteTime.dwLowDateTime;
    std::uint64_t fileHash = 0;
    bool cached = false;
    for (const CacheEntry &entry : s_cache) {
      if (entry.path == path && entry.size == size && entry.writeTime == writeTime) {
        fileHash = entry.hash;
        cached = true;
        break;
      }
    }
    if (!cached) {
      fileHash = FINGERPRINT_SEED;
      if (!HashFile(path, fileHash)) {
        Thandor_Log("recorder: cannot read %s for the asset fingerprint", Narrow(path).c_str());
        continue;
      }
      s_cache.push_back({path, size, writeTime, fileHash});
    }
    const std::string name = Narrow(key);
    result.hash = HashBytes(result.hash, reinterpret_cast<const std::uint8_t *>(name.data()), name.size());
    result.hash = HashBytes(result.hash, reinterpret_cast<const std::uint8_t *>(&size), sizeof size);
    result.hash = HashBytes(result.hash, reinterpret_cast<const std::uint8_t *>(&fileHash), sizeof fileHash);
    result.fileCount++;
  }
  return result;
}

void ExitProcessWith(unsigned exitCode)
{
  ExitProcess(exitCode);
}

} // namespace thandor::replay
