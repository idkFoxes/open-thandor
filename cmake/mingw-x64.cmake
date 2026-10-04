# Toolchain file: MinGW-w64 GCC for 64-bit x64 (x86_64-w64-mingw32, SEH), the second compiler next to MSVC.
#
# Finds the toolchain in this order: the cache variable MINGW_ROOT, the environment variable MINGW_ROOT, the
# g++ on the PATH, then the usual install locations (C:/mingw64, C:/msys64/mingw64, C:/msys64/ucrt64). MINGW_ROOT is
# the directory that holds bin/g++.exe. Used by the mingw-* presets in CMakePresets.json.

set(MINGW_ROOT "" CACHE PATH "MinGW-w64 root (the directory with bin/g++.exe); empty: search")

set(_mingw_root "${MINGW_ROOT}")
if(NOT _mingw_root AND DEFINED ENV{MINGW_ROOT})
    file(TO_CMAKE_PATH "$ENV{MINGW_ROOT}" _mingw_root)
endif()
if(NOT _mingw_root)
    find_program(_mingw_gxx NAMES x86_64-w64-mingw32-g++ g++ NO_CACHE)
    if(_mingw_gxx)
        get_filename_component(_mingw_root "${_mingw_gxx}" DIRECTORY)
        get_filename_component(_mingw_root "${_mingw_root}" DIRECTORY)
    endif()
endif()
if(NOT _mingw_root)
    foreach(_candidate "C:/mingw64" "C:/msys64/mingw64" "C:/msys64/ucrt64")
        if(EXISTS "${_candidate}/bin/g++.exe")
            set(_mingw_root "${_candidate}")
            break()
        endif()
    endforeach()
endif()
if(NOT _mingw_root OR NOT EXISTS "${_mingw_root}/bin/g++.exe")
    message(FATAL_ERROR "mingw-x64.cmake: no MinGW-w64 g++ found; set MINGW_ROOT (cache or environment) to the "
                        "directory that holds bin/g++.exe, or put it on the PATH.")
endif()
# remembered for later configure runs (the toolchain file runs again for try_compile projects)
set(MINGW_ROOT "${_mingw_root}" CACHE PATH "MinGW-w64 root (the directory with bin/g++.exe); empty: search" FORCE)
list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES MINGW_ROOT)

set(CMAKE_C_COMPILER "${_mingw_root}/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "${_mingw_root}/bin/g++.exe")
set(CMAKE_RC_COMPILER "${_mingw_root}/bin/windres.exe")
# Configure-time tool runs (try_compile) find the toolchain's DLLs
set(ENV{PATH} "${_mingw_root}/bin;$ENV{PATH}")

message(STATUS "mingw-x64: ${_mingw_root}")
