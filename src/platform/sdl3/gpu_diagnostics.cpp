/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/sdl3/gpu_diagnostics.cpp
 * Project code (not in the original game)
 */

/* Diagnostics of the SDL_GPU renderers, see gpu_diagnostics.h. */

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <dxgi.h>

#include "gpu_diagnostics.h"

#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/bootstrap/low_memory.h>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_version.h>

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <cstring>

namespace {

constexpr LONG kAddressSpaceLogBudget = 80;
constexpr LONG kFailureLogBudget = 40;
constexpr uint64_t kBigResourceBytes = 4u << 20; /* textures and buffers from 4 MiB on log the address space */
constexpr uint32_t kMarkerFramesWithoutSwapchain = 120;

volatile LONG s_addressSpaceLogs = 0;
volatile LONG s_failureLogs = 0;
bool s_adaptersLogged = false;

wchar_t s_markerPath[MAX_PATH] = {};
bool s_markerPending = false;
uint32_t s_markerFrames = 0;

constexpr uint64_t MiB(uint64_t bytes)
{
  return bytes >> 20;
}

/* The marker's path: thandor.ini's directory (the current directory for a bare file name) and the marker name. */
bool MarkerPath(const uint16_t *iniPath, wchar_t *out, size_t capacity)
{
  static const wchar_t kMarkerName[] = L"thandor-gpu-start.txt";
  size_t length = 0;
  size_t directoryLength = 0;
  if (iniPath != nullptr) {
    while ((iniPath[length] != 0) && (length < MAX_PATH)) {
      if ((iniPath[length] == '\\') || (iniPath[length] == '/')) {
        directoryLength = length + 1;
      }
      length++;
    }
  }
  if (directoryLength + (sizeof kMarkerName / sizeof kMarkerName[0]) > capacity) {
    return false;
  }
  for (size_t index = 0; index < directoryLength; index++) {
    out[index] = static_cast<wchar_t>(iniPath[index]);
  }
  std::memcpy(out + directoryLength, kMarkerName, sizeof kMarkerName);
  return true;
}

uint32_t BytesPerTexel(SDL_GPUTextureFormat format)
{
  switch (format) {
  case SDL_GPU_TEXTUREFORMAT_R8_UNORM:
    return 1;
  case SDL_GPU_TEXTUREFORMAT_D16_UNORM:
    return 2;
  default:
    return 4;
  }
}

} // namespace

namespace thandor::sdl3 {

void LogAddressSpace(const char *when) noexcept
{
  if (InterlockedIncrement(&s_addressSpaceLogs) > kAddressSpaceLogBudget) {
    return;
  }
  SYSTEM_INFO system;
  GetSystemInfo(&system);
  const auto lowest = reinterpret_cast<uintptr_t>(system.lpMinimumApplicationAddress);
  const auto highest = reinterpret_cast<uintptr_t>(system.lpMaximumApplicationAddress);
  constexpr uintptr_t k2G = 0x80000000u;
  uint64_t freeBytes = 0;
  uint64_t largestFree = 0;
  uint64_t freeBelow2G = 0;
  uint64_t largestFreeBelow2G = 0;
  uint64_t reservedBytes = 0;
  uint64_t committedBytes = 0;
  uintptr_t address = lowest;
  MEMORY_BASIC_INFORMATION region;
  while ((address < highest) && (VirtualQuery(reinterpret_cast<void *>(address), &region, sizeof region) != 0)) {
    const uintptr_t start = std::max(reinterpret_cast<uintptr_t>(region.BaseAddress), lowest);
    const uintptr_t end = std::min(reinterpret_cast<uintptr_t>(region.BaseAddress) + region.RegionSize, highest + 1);
    if (end <= address) {
      break;
    }
    const uint64_t size = end - start;
    if (region.State == MEM_FREE) {
      freeBytes += size;
      largestFree = std::max(largestFree, size);
      if (start < k2G) {
        const uint64_t low = std::min<uintptr_t>(end, k2G) - start;
        freeBelow2G += low;
        largestFreeBelow2G = std::max(largestFreeBelow2G, low);
      }
    }
    else if (region.State == MEM_RESERVE) {
      reservedBytes += size;
    }
    else {
      committedBytes += size;
    }
    address = end;
  }
  Thandor_Log("address space %s: user limit 0x%llX, free %llu MiB (largest block %llu MiB), below 2 GB free %llu MiB "
              "(largest block %llu MiB), committed %llu MiB, reserved %llu MiB",
              when, static_cast<unsigned long long>(highest), static_cast<unsigned long long>(MiB(freeBytes)),
              static_cast<unsigned long long>(MiB(largestFree)), static_cast<unsigned long long>(MiB(freeBelow2G)),
              static_cast<unsigned long long>(MiB(largestFreeBelow2G)),
              static_cast<unsigned long long>(MiB(committedBytes)),
              static_cast<unsigned long long>(MiB(reservedBytes)));
}

void LogGpuAdapters() noexcept
{
  if (s_adaptersLogged) {
    return;
  }
  s_adaptersLogged = true;
  const int version = SDL_GetVersion();
  Thandor_Log("SDL version %d.%d.%d (%s)", SDL_VERSIONNUM_MAJOR(version), SDL_VERSIONNUM_MINOR(version),
              SDL_VERSIONNUM_MICRO(version), SDL_GetRevision());
  HMODULE dxgi = LoadLibraryW(L"dxgi.dll");
  if (dxgi == nullptr) {
    Thandor_Log("display adapters: dxgi.dll not loaded (error %lu)", GetLastError());
    return;
  }
  using CreateFactory1 = HRESULT(WINAPI *)(REFIID, void **);
  auto create = reinterpret_cast<CreateFactory1>(reinterpret_cast<void *>(GetProcAddress(dxgi, "CreateDXGIFactory1")));
  IDXGIFactory1 *factory = nullptr;
  HRESULT result = (create != nullptr) ? create(__uuidof(IDXGIFactory1), reinterpret_cast<void **>(&factory)) : E_FAIL;
  if (FAILED(result) || (factory == nullptr)) {
    Thandor_Log("display adapters: CreateDXGIFactory1 failed (0x%08lX)", static_cast<unsigned long>(result));
    FreeLibrary(dxgi);
    return;
  }
  IDXGIAdapter1 *adapter = nullptr;
  for (UINT index = 0; factory->EnumAdapters1(index, &adapter) != DXGI_ERROR_NOT_FOUND; index++) {
    DXGI_ADAPTER_DESC1 description;
    std::memset(&description, 0, sizeof description);
    if (FAILED(adapter->GetDesc1(&description))) {
      adapter->Release();
      continue;
    }
    char name[256];
    if (WideCharToMultiByte(CP_UTF8, 0, description.Description, -1, name, sizeof name, nullptr, nullptr) == 0) {
      std::snprintf(name, sizeof name, "?");
    }
    char driver[64];
    LARGE_INTEGER umdVersion;
    if (SUCCEEDED(adapter->CheckInterfaceSupport(__uuidof(IDXGIDevice), &umdVersion))) {
      std::snprintf(driver, sizeof driver, "%u.%u.%u.%u", static_cast<unsigned>(HIWORD(umdVersion.HighPart)),
                    static_cast<unsigned>(LOWORD(umdVersion.HighPart)), static_cast<unsigned>(HIWORD(umdVersion.LowPart)),
                    static_cast<unsigned>(LOWORD(umdVersion.LowPart)));
    }
    else {
      std::snprintf(driver, sizeof driver, "unknown");
    }
    Thandor_Log("display adapter %u: %s (vendor 0x%04X device 0x%04X), dedicated video memory %llu MiB, dedicated "
                "system memory %llu MiB, shared system memory %llu MiB, driver %s%s",
                index, name, description.VendorId, description.DeviceId,
                static_cast<unsigned long long>(MiB(description.DedicatedVideoMemory)),
                static_cast<unsigned long long>(MiB(description.DedicatedSystemMemory)),
                static_cast<unsigned long long>(MiB(description.SharedSystemMemory)), driver,
                ((description.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0) ? " (software)" : "");
    adapter->Release();
  }
  factory->Release();
  FreeLibrary(dxgi);
}

void LogGpuFailure(const char *format, ...) noexcept
{
  const LONG count = InterlockedIncrement(&s_failureLogs);
  if (count > kFailureLogBudget) {
    return;
  }
  char text[512];
  va_list args;
  va_start(args, format);
  std::vsnprintf(text, sizeof text, format, args);
  va_end(args);
  Thandor_Log("SDL_GPU failure %ld: %s: %s%s", static_cast<long>(count), text, SDL_GetError(),
              (count == kFailureLogBudget) ? " (further failures are not logged)" : "");
}

SDL_GPUTexture *CreateGpuTextureLogged(SDL_GPUDevice *device, const SDL_GPUTextureCreateInfo *info,
                                       const char *what) noexcept
{
  const uint64_t bytes = static_cast<uint64_t>(info->width) * info->height * info->layer_count_or_depth *
                         BytesPerTexel(info->format);
  char when[160];
  if (bytes >= kBigResourceBytes) {
    std::snprintf(when, sizeof when, "before texture %s %ux%u", what, info->width, info->height);
    LogAddressSpace(when);
  }
  SDL_GPUTexture *texture = SDL_CreateGPUTexture(device, info);
  if (texture == nullptr) {
    LogGpuFailure("SDL_CreateGPUTexture %s %ux%u format %d usage 0x%X", what, info->width, info->height,
                  static_cast<int>(info->format), static_cast<unsigned>(info->usage));
  }
  if (bytes >= kBigResourceBytes) {
    std::snprintf(when, sizeof when, "after texture %s %ux%u (%s)", what, info->width, info->height,
                  (texture != nullptr) ? "created" : "failed");
    LogAddressSpace(when);
  }
  return texture;
}

SDL_GPUBuffer *CreateGpuBufferLogged(SDL_GPUDevice *device, const SDL_GPUBufferCreateInfo *info,
                                     const char *what) noexcept
{
  char when[160];
  if (info->size >= kBigResourceBytes) {
    std::snprintf(when, sizeof when, "before buffer %s of %u bytes", what, info->size);
    LogAddressSpace(when);
  }
  SDL_GPUBuffer *buffer = SDL_CreateGPUBuffer(device, info);
  if (buffer == nullptr) {
    LogGpuFailure("SDL_CreateGPUBuffer %s of %u bytes", what, info->size);
  }
  if (info->size >= kBigResourceBytes) {
    std::snprintf(when, sizeof when, "after buffer %s of %u bytes (%s)", what, info->size,
                  (buffer != nullptr) ? "created" : "failed");
    LogAddressSpace(when);
  }
  return buffer;
}

SDL_GPUTransferBuffer *CreateGpuTransferBufferLogged(SDL_GPUDevice *device, const SDL_GPUTransferBufferCreateInfo *info,
                                                     const char *what) noexcept
{
  char when[160];
  if (info->size >= kBigResourceBytes) {
    std::snprintf(when, sizeof when, "before transfer buffer %s of %u bytes", what, info->size);
    LogAddressSpace(when);
  }
  SDL_GPUTransferBuffer *buffer = SDL_CreateGPUTransferBuffer(device, info);
  if (buffer == nullptr) {
    LogGpuFailure("SDL_CreateGPUTransferBuffer %s of %u bytes", what, info->size);
  }
  if (info->size >= kBigResourceBytes) {
    std::snprintf(when, sizeof when, "after transfer buffer %s of %u bytes (%s)", what, info->size,
                  (buffer != nullptr) ? "created" : "failed");
    LogAddressSpace(when);
  }
  return buffer;
}

SDL_GPUSampler *CreateGpuSamplerLogged(SDL_GPUDevice *device, const SDL_GPUSamplerCreateInfo *info,
                                       const char *what) noexcept
{
  SDL_GPUSampler *sampler = SDL_CreateGPUSampler(device, info);
  if (sampler == nullptr) {
    LogGpuFailure("SDL_CreateGPUSampler %s", what);
  }
  return sampler;
}

SDL_GPUShader *CreateGpuShaderLogged(SDL_GPUDevice *device, const SDL_GPUShaderCreateInfo *info,
                                     const char *what) noexcept
{
  SDL_GPUShader *shader = SDL_CreateGPUShader(device, info);
  if (shader == nullptr) {
    LogGpuFailure("SDL_CreateGPUShader %s", what);
  }
  return shader;
}

SDL_GPUGraphicsPipeline *CreateGpuPipelineLogged(SDL_GPUDevice *device, const SDL_GPUGraphicsPipelineCreateInfo *info,
                                                 const char *what) noexcept
{
  SDL_GPUGraphicsPipeline *pipeline = SDL_CreateGPUGraphicsPipeline(device, info);
  if (pipeline == nullptr) {
    LogGpuFailure("SDL_CreateGPUGraphicsPipeline %s", what);
  }
  return pipeline;
}

void *MapGpuTransferBufferLogged(SDL_GPUDevice *device, SDL_GPUTransferBuffer *buffer, bool cycle,
                                 const char *what) noexcept
{
  void *mapped = SDL_MapGPUTransferBuffer(device, buffer, cycle);
  if (mapped == nullptr) {
    LogGpuFailure("SDL_MapGPUTransferBuffer %s", what);
  }
  return mapped;
}

SDL_GPUCommandBuffer *AcquireGpuCommandBufferLogged(SDL_GPUDevice *device, const char *what) noexcept
{
  SDL_GPUCommandBuffer *commands = SDL_AcquireGPUCommandBuffer(device);
  if (commands == nullptr) {
    LogGpuFailure("SDL_AcquireGPUCommandBuffer %s", what);
  }
  return commands;
}

bool GpuStartMarker_Take(const uint16_t *iniPath, uint32_t *renderer) noexcept
{
  wchar_t path[MAX_PATH];
  if (!MarkerPath(iniPath, path, MAX_PATH)) {
    return false;
  }
  HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (file == INVALID_HANDLE_VALUE) {
    return false;
  }
  char text[128] = {};
  DWORD read = 0;
  const bool ok = ReadFile(file, text, sizeof text - 1, &read, nullptr) != 0;
  CloseHandle(file);
  DeleteFileW(path);
  unsigned value = 0;
  if (!ok || (std::sscanf(text, "renderer=%u", &value) != 1)) {
    Thandor_Log("GPU start marker found but not readable, ignored");
    return false;
  }
  *renderer = value;
  return true;
}

void GpuStartMarker_Write(const uint16_t *iniPath, uint32_t renderer, const char *name) noexcept
{
  if (!MarkerPath(iniPath, s_markerPath, MAX_PATH)) {
    return;
  }
  HANDLE file =
      CreateFileW(s_markerPath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (file == INVALID_HANDLE_VALUE) {
    Thandor_Log("GPU start marker not written (error %lu)", GetLastError());
    return;
  }
  char text[128];
  const int length = std::snprintf(text, sizeof text, "renderer=%u\r\nname=%s\r\n", renderer, name);
  DWORD written = 0;
  WriteFile(file, text, static_cast<DWORD>(length), &written, nullptr);
  FlushFileBuffers(file);
  CloseHandle(file);
  s_markerPending = true;
  s_markerFrames = 0;
}

void GpuStartMarker_Clear() noexcept
{
  if (!s_markerPending) {
    return;
  }
  s_markerPending = false;
  DeleteFileW(s_markerPath);
}

void GpuNoteFrameSubmitted(bool withSwapchain) noexcept
{
  if (!s_markerPending) {
    return;
  }
  if (!withSwapchain && (++s_markerFrames < kMarkerFramesWithoutSwapchain)) {
    return;
  }
  GpuStartMarker_Clear();
  Thandor_Log("SDL_GPU: first frame %s, start marker removed",
              withSwapchain ? "presented" : "submitted (no swapchain yet)");
  LogAddressSpace("after the first frame");
#ifdef THANDOR_LARGE_ADDRESS_AWARE
  LowMemory_LogState("after the first frame");
#endif
}

} // namespace thandor::sdl3
