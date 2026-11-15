/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/replay/files.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_GAMEPLAY_REPLAY_FILES_H
#define THANDOR_GAMEPLAY_REPLAY_FILES_H

/* Not in the original: the operating system side of the game recorder (gameplay/replay/runtime.h), kept apart
   because <windows.h> and the game headers do not mix: environment, command line, paths, files, the temporary
   checkpoint directory, the recordings\ rotation and the asset fingerprint. */

#include <cstdint>
#include <string>
#include <vector>

namespace thandor::replay {

std::string Narrow(const std::wstring &text); /* UTF-8 */
std::wstring Widen(const std::string &text);  /* from UTF-8 */

/* The variable's value; empty when it is not set (or too long). */
std::wstring EnvironmentValue(const wchar_t *name);
/* Set and neither "0" nor "off". */
bool EnvironmentFlag(const wchar_t *name);
/* -REPLAY="<path>" on the command line; empty when absent. */
std::wstring CommandLineReplayPath();

std::wstring ExecutableDirectory(); /* with the trailing backslash */
std::wstring AbsolutePath(const std::wstring &path);
bool FileExists(const std::wstring &path);
bool ReadFileBytes(const std::wstring &path, std::vector<std::uint8_t> &bytes);
bool WriteFileBytes(const std::wstring &path, const std::vector<std::uint8_t> &bytes);
bool CreateDirectoryIfMissing(const std::wstring &path);

/* %TEMP%\open-thandor-recorder-<process>-<ticks>\ (with the backslash); empty when it cannot be created. */
std::wstring CreateTemporaryDirectory();
/* Deletes the files in directory and the directory itself. */
void RemoveTemporaryDirectory(const std::wstring &directory);

/* "YYYY-MM-DD HH:MM:SS", or for file names "YYYY-MM-DD_HH-MM-SS" */
std::string LocalTimeText(bool forFileName);

/* directory (with the backslash) keeps the newest keepCount - keepFree *.otr files and at most maxBytes of them
   together; the older ones are deleted (logged). The names start with date and time, so the order is by name. */
void RotateRecordings(const std::wstring &directory, unsigned keepCount, unsigned keepFree, std::uint64_t maxBytes);

struct AssetFingerprint {
  std::uint64_t hash = 0;
  std::uint32_t fileCount = 0;
};
/* The packages next to thandor.exe (*.PCK; their bytes only: PATCH01.PCK of some installations is a RAR file, not a
   package) and levelFile when it is a file of its own (a custom map); cached per process by path, size and time. */
AssetFingerprint ComputeAssetFingerprint(const std::wstring &levelFile);

[[noreturn]] void ExitProcessWith(unsigned exitCode);

} // namespace thandor::replay

#endif /* THANDOR_GAMEPLAY_REPLAY_FILES_H */
