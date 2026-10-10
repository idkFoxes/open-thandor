/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/selftest/nativefile_selftest.cpp
 * Project code (not in the original game)
 */

/* OPEN_THANDOR_SELFTEST=nativefile: the native file helpers FileSystem_ReadWholeFileNative and
   FileSystem_WriteFileReplacing (src/platform/filesystem/win32.cpp) on files in a fresh directory under %TEMP%
   (the current directory without TEMP):
   a missing file, a new file, replacing an existing file (with shorter content), an empty file, a file of exactly
   FILESYSTEM_NATIVE_READ_LIMIT_BYTES and one byte over it, and a replace whose target is a directory (must fail
   and leave no ".tmp" behind). No game data is needed. One line per failed check, then
   "nativefile: <n> of <m> checks failed". The directory is removed afterwards. */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/selftest/selftest.h>

constexpr auto NATIVEFILE_TEST_PATH_BYTES = 0x200;

/* A path under the test directory, as UTF-16 for the helpers and as narrow text for the checks. */
struct NativeFileTestPath {
    char narrow[NATIVEFILE_TEST_PATH_BYTES];
    uint16_t wide[THANDOR_PATH_CAPACITY];
};

static unsigned s_NativeFileChecks;
static unsigned s_NativeFileFailures;

static void NativeFileTest_Check(bool passed, const char *what)
{
    s_NativeFileChecks++;
    if (!passed) {
        s_NativeFileFailures++;
        Thandor_Log("nativefile FAILED: %s", what);
    }
}

static void NativeFileTest_MakePath(NativeFileTestPath *path, const char *directory, const char *leaf)
{
    snprintf(path->narrow, sizeof path->narrow, "%s\\%s", directory, leaf);
    Text_CopyNarrowToUtf16(sizeof path->wide, path->wide, reinterpret_cast<uint8_t *>(path->narrow));
}

/* The attributes of a file or directory, or false when it does not exist. */
static bool NativeFileTest_Find(const char *path, DWORD *outAttributes)
{
    _WIN32_FIND_DATAA findData;
    HANDLE findHandle = FindFirstFileA(path, &findData);
    if (findHandle == INVALID_HANDLE_VALUE) {
        return false;
    }
    FindClose(findHandle);
    *outAttributes = findData.dwFileAttributes;
    return true;
}

static bool NativeFileTest_Exists(const NativeFileTestPath *path)
{
    DWORD attributes;
    return NativeFileTest_Find(path->narrow, &attributes);
}

/* Deterministic content of the given size (LCG bytes). */
static std::vector<uint8_t> NativeFileTest_Bytes(size_t size, uint32_t seed)
{
    std::vector<uint8_t> bytes(size);
    for (uint8_t &value : bytes) {
        seed = seed * 1664525u + 1013904223u;
        value = static_cast<uint8_t>(seed >> 24);
    }
    return bytes;
}

/* Writes bytes with FileSystem_WriteFileReplacing, checks that it succeeded and left no ".tmp", reads the file
   back and checks the content. */
static void NativeFileTest_RoundTrip(NativeFileTestPath *path, const NativeFileTestPath *temporary,
                                     const std::vector<uint8_t> &bytes, const char *what)
{
    char label[128];
    std::optional<std::vector<uint8_t>> readBack;

    snprintf(label, sizeof label, "%s: write", what);
    NativeFileTest_Check(FileSystem_WriteFileReplacing(path->wide, bytes) == 0, label);
    snprintf(label, sizeof label, "%s: no .tmp left", what);
    NativeFileTest_Check(!NativeFileTest_Exists(temporary), label);
    readBack = FileSystem_ReadWholeFileNative(path->wide);
    snprintf(label, sizeof label, "%s: read back", what);
    NativeFileTest_Check(readBack.has_value() && *readBack == bytes, label);
}

static void NativeFileTest_RemoveAll(const char *directory, const NativeFileTestPath *target,
                                     const NativeFileTestPath *temporary, const NativeFileTestPath *blockedTarget,
                                     const NativeFileTestPath *blockedTemporary)
{
    RemoveDirectoryA(blockedTarget->narrow);
    DeleteFileA(target->narrow);
    DeleteFileA(temporary->narrow);
    DeleteFileA(blockedTemporary->narrow);
    RemoveDirectoryA(directory);
}

void Thandor_SelfTestNativeFile()
{
    const char *tempRoot;
    char directory[NATIVEFILE_TEST_PATH_BYTES - 0x20]; /* leaves room for the leaf names */
    DWORD attributes;
    NativeFileTestPath target;
    NativeFileTestPath temporary;
    NativeFileTestPath missing;
    NativeFileTestPath blockedTarget;
    NativeFileTestPath blockedTemporary;

    s_NativeFileChecks = 0;
    s_NativeFileFailures = 0;
    tempRoot = getenv("TEMP");
    if (tempRoot == nullptr || tempRoot[0] == 0) {
        tempRoot = ".";
    }
    if (snprintf(directory, sizeof directory, "%s\\open-thandor-nativefile", tempRoot) >= (int)sizeof directory) {
        Thandor_Log("nativefile: TEMP path too long");
        return;
    }
    NativeFileTest_MakePath(&target, directory, "progress.txt");
    NativeFileTest_MakePath(&temporary, directory, "progress.txt.tmp");
    NativeFileTest_MakePath(&missing, directory, "missing.txt");
    NativeFileTest_MakePath(&blockedTarget, directory, "blocked");
    NativeFileTest_MakePath(&blockedTemporary, directory, "blocked.tmp");
    NativeFileTest_RemoveAll(directory, &target, &temporary, &blockedTarget, &blockedTemporary); /* an aborted run */
    if (CreateDirectoryA(directory, nullptr) == 0) {
        Thandor_Log("nativefile: cannot create %s", directory);
        return;
    }

    NativeFileTest_Check(!FileSystem_ReadWholeFileNative(missing.wide).has_value(), "missing file reads as nullopt");

    NativeFileTest_RoundTrip(&target, &temporary, NativeFileTest_Bytes(5000, 1), "new file");
    NativeFileTest_RoundTrip(&target, &temporary, NativeFileTest_Bytes(300, 2), "replace existing (shorter)");
    NativeFileTest_RoundTrip(&target, &temporary, std::vector<uint8_t>(), "empty file");
    NativeFileTest_RoundTrip(&target, &temporary, NativeFileTest_Bytes(FILESYSTEM_NATIVE_READ_LIMIT_BYTES, 3),
                             "file at the size limit");

    NativeFileTest_Check(FileSystem_WriteFileReplacing(target.wide,
                                                       NativeFileTest_Bytes(FILESYSTEM_NATIVE_READ_LIMIT_BYTES + 1, 4)) == 0,
                         "over-limit file: write");
    NativeFileTest_Check(!FileSystem_ReadWholeFileNative(target.wide).has_value(), "over-limit file reads as nullopt");

    /* a directory as the target: the replace fails, the temporary file is deleted and the directory stays */
    CreateDirectoryA(blockedTarget.narrow, nullptr);
    NativeFileTest_Check(FileSystem_WriteFileReplacing(blockedTarget.wide, NativeFileTest_Bytes(64, 5)) != 0,
                         "replace onto a directory fails");
    NativeFileTest_Check(!NativeFileTest_Exists(&blockedTemporary), "failed replace leaves no .tmp");
    NativeFileTest_Check(NativeFileTest_Find(blockedTarget.narrow, &attributes) &&
                         (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0, "failed replace keeps the target");

    NativeFileTest_RemoveAll(directory, &target, &temporary, &blockedTarget, &blockedTemporary);
    Thandor_Log("nativefile: %u of %u checks failed", s_NativeFileFailures, s_NativeFileChecks);
}
