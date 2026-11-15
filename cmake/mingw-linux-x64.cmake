# Cross toolchain: build the 64-bit Windows Open Thandor executable from a Linux host with MinGW-w64 GCC.
#
# Usage is normally through tools/build-linux-mingw.sh. The script supplies SDL3_DIR and forces the committed
# precompiled SDL_GPU shader headers, so no Windows fxc/dxc executable has to run on the Linux host.

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(THANDOR_MINGW_TRIPLE "x86_64-w64-mingw32" CACHE STRING "MinGW-w64 target triple")

find_program(_thandor_mingw_gcc NAMES "${THANDOR_MINGW_TRIPLE}-gcc" REQUIRED)
find_program(_thandor_mingw_gxx NAMES "${THANDOR_MINGW_TRIPLE}-g++" REQUIRED)
find_program(_thandor_mingw_windres NAMES "${THANDOR_MINGW_TRIPLE}-windres" REQUIRED)

set(CMAKE_C_COMPILER "${_thandor_mingw_gcc}")
set(CMAKE_CXX_COMPILER "${_thandor_mingw_gxx}")
set(CMAKE_RC_COMPILER "${_thandor_mingw_windres}")

# Let CMake discover the matching binutils from the compiler prefix. These hints also make the selected tools
# explicit on distributions where the host binutils appear first on PATH.
foreach(_tool AR RANLIB NM OBJCOPY STRIP)
    string(TOLOWER "${_tool}" _tool_lower)
    find_program(_thandor_${_tool_lower} NAMES "${THANDOR_MINGW_TRIPLE}-${_tool_lower}")
    if(_thandor_${_tool_lower})
        set(CMAKE_${_tool} "${_thandor_${_tool_lower}}" CACHE FILEPATH "MinGW-w64 ${_tool_lower}" FORCE)
    endif()
endforeach()

# Programs run during configure/build must be Linux-host programs. Libraries, includes and packages are Windows
# target artifacts. SDL3_DIR may point outside the compiler sysroot and is therefore passed explicitly by the
# wrapper script rather than being found through an implicit host prefix.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
