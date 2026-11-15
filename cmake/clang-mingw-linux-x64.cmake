# Cross toolchain: build Open Thandor's 64-bit Windows target on Linux with Clang + LLD, using the MinGW-w64
# headers/runtimes and GNU ABI. LLD emits an MSVC-compatible CodeView PDB while the executable keeps the MinGW ABI.
#
# Normal entry point: tools/build-linux-mingw.sh release --pdb

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(THANDOR_MINGW_TRIPLE "x86_64-w64-mingw32" CACHE STRING "MinGW-w64 target triple")
set(THANDOR_CLANG_TARGET "x86_64-w64-windows-gnu" CACHE STRING "Clang Windows GNU target triple")

find_program(_thandor_clang NAMES clang REQUIRED)
find_program(_thandor_clangxx NAMES clang++ REQUIRED)
find_program(_thandor_lld NAMES ld.lld REQUIRED)
# Clang supplies the frontend and LLD the linker, but the target headers, libstdc++, libgcc and CRT still come from
# the installed MinGW-w64 GCC toolchain. Requiring its driver also gives us a reliable toolchain root.
find_program(_thandor_mingw_gxx NAMES "${THANDOR_MINGW_TRIPLE}-g++" REQUIRED)
find_program(_thandor_mingw_windres NAMES "${THANDOR_MINGW_TRIPLE}-windres" REQUIRED)

get_filename_component(_thandor_mingw_bin "${_thandor_mingw_gxx}" DIRECTORY)
get_filename_component(_thandor_mingw_root "${_thandor_mingw_bin}" DIRECTORY)

# Distro Clang does not reliably discover a cross MinGW GCC installation from --gcc-toolchain alone. Arch in
# particular installs libgcc/libstdc++ below a versioned /usr/lib/gcc/x86_64-w64-mingw32/<version> directory,
# which LLD will otherwise miss with "unable to find library -lgcc" during CMake's compiler probe. Ask the
# actual MinGW driver for its runtime and sysroot so this follows the installed GCC version instead of guessing.
execute_process(
    COMMAND "${_thandor_mingw_gxx}" -print-libgcc-file-name
    OUTPUT_VARIABLE _thandor_libgcc
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE _thandor_libgcc_rc
)
if(NOT _thandor_libgcc_rc EQUAL 0 OR NOT EXISTS "${_thandor_libgcc}")
    message(FATAL_ERROR "Could not locate the MinGW-w64 libgcc runtime through ${_thandor_mingw_gxx}")
endif()
get_filename_component(_thandor_gcc_install_dir "${_thandor_libgcc}" DIRECTORY)

execute_process(
    COMMAND "${_thandor_mingw_gxx}" -dumpfullversion
    OUTPUT_VARIABLE _thandor_gcc_version
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE _thandor_gcc_version_rc
)
if(NOT _thandor_gcc_version_rc EQUAL 0 OR NOT _thandor_gcc_version)
    execute_process(
        COMMAND "${_thandor_mingw_gxx}" -dumpversion
        OUTPUT_VARIABLE _thandor_gcc_version
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE _thandor_gcc_version_rc
    )
endif()
if(NOT _thandor_gcc_version_rc EQUAL 0 OR NOT _thandor_gcc_version)
    message(FATAL_ERROR "Could not determine the MinGW-w64 GCC version through ${_thandor_mingw_gxx}")
endif()

execute_process(
    COMMAND "${_thandor_mingw_gxx}" -print-sysroot
    OUTPUT_VARIABLE _thandor_mingw_reported_sysroot
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(CMAKE_C_COMPILER "${_thandor_clang}")
set(CMAKE_CXX_COMPILER "${_thandor_clangxx}")
set(CMAKE_C_COMPILER_TARGET "${THANDOR_CLANG_TARGET}")
set(CMAKE_CXX_COMPILER_TARGET "${THANDOR_CLANG_TARGET}")
set(CMAKE_RC_COMPILER "${_thandor_mingw_windres}")

# Point Clang at the MinGW sysroot and explicitly add the cross libstdc++ headers. Arch installs those below
# /usr/x86_64-w64-mingw32/include/c++/<gcc-version>; Clang's Windows-GNU driver does not discover them from the
# versioned libgcc directory. The three paths mirror g++'s generic, target-specific and backward C++ searches.
# -L is kept as a defensive linker hint because CMake's first compiler probe must resolve libgcc/libstdc++.
if(_thandor_mingw_reported_sysroot AND EXISTS "${_thandor_mingw_reported_sysroot}/include")
    set(_thandor_mingw_sysroot "${_thandor_mingw_reported_sysroot}")
else()
    set(_thandor_mingw_sysroot "${_thandor_mingw_root}/${THANDOR_MINGW_TRIPLE}")
endif()
if(EXISTS "${_thandor_mingw_sysroot}/include" AND EXISTS "${_thandor_mingw_sysroot}/lib")
    set(CMAKE_SYSROOT "${_thandor_mingw_sysroot}")
endif()

set(_thandor_cxx_include "${_thandor_mingw_sysroot}/include/c++/${_thandor_gcc_version}")
set(_thandor_cxx_target_include "${_thandor_cxx_include}/${THANDOR_MINGW_TRIPLE}")
set(_thandor_cxx_backward_include "${_thandor_cxx_include}/backward")
if(NOT EXISTS "${_thandor_cxx_include}/string" OR NOT EXISTS "${_thandor_cxx_target_include}/bits/c++config.h")
    message(FATAL_ERROR
        "Could not locate MinGW-w64 libstdc++ headers for GCC ${_thandor_gcc_version}. "
        "Expected ${_thandor_cxx_include}/string and ${_thandor_cxx_target_include}/bits/c++config.h")
endif()

set(_thandor_cxx_header_args
    "-nostdinc++ -isystem${_thandor_cxx_include} -isystem${_thandor_cxx_target_include} -isystem${_thandor_cxx_backward_include}")
set(CMAKE_CXX_FLAGS_INIT "${_thandor_cxx_header_args}")
set(_thandor_lld_runtime_args "-fuse-ld=lld -L${_thandor_gcc_install_dir}")
set(CMAKE_EXE_LINKER_FLAGS_INIT "${_thandor_lld_runtime_args}")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "${_thandor_lld_runtime_args}")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "${_thandor_lld_runtime_args}")

message(STATUS "Clang/MinGW GCC runtime: ${_thandor_gcc_install_dir}")
message(STATUS "Clang/MinGW sysroot: ${_thandor_mingw_sysroot}")
message(STATUS "Clang/MinGW libstdc++ headers: ${_thandor_cxx_include}")

# Use target-aware binutils for the compact thandor.sym table and any CMake archive work. The PDB path itself does
# not depend on objcopy/strip; LLD writes it directly from CodeView records.
foreach(_tool AR RANLIB NM OBJCOPY STRIP)
    string(TOLOWER "${_tool}" _tool_lower)
    find_program(_thandor_${_tool_lower} NAMES "${THANDOR_MINGW_TRIPLE}-${_tool_lower}" "llvm-${_tool_lower}")
    if(_thandor_${_tool_lower})
        set(CMAKE_${_tool} "${_thandor_${_tool_lower}}" CACHE FILEPATH "Cross ${_tool_lower}" FORCE)
    endif()
endforeach()

# Programs run at configure/build time are Linux-host tools. Libraries, includes and packages are Windows target
# artifacts. SDL3_DIR is supplied explicitly by tools/build-linux-mingw.sh.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
