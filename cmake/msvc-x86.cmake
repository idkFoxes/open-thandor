# Toolchain file: MSVC for 32-bit x86 without a vcvars32 environment.
#
# Finds the newest Visual Studio with the C++ tools (vswhere), its default MSVC toolset and the newest
# Windows 10/11 SDK, and points CMake at the x86 compiler with the matching include and library
# directories. Used by the presets in CMakePresets.json, so IDEs like CLion can configure the project
# with their own CMake/Ninja. From a vcvars32 prompt the plain `cmake -G Ninja` still works as before.

if(DEFINED ENV{VSINSTALLDIR} AND DEFINED ENV{VCToolsInstallDir} AND "$ENV{VSCMD_ARG_TGT_ARCH}" STREQUAL "x86")
    # already inside a vcvars32 environment: nothing to do
    return()
endif()

set(_vswhere "$ENV{ProgramFiles\(x86\)}/Microsoft Visual Studio/Installer/vswhere.exe")
if(NOT EXISTS "${_vswhere}")
    message(FATAL_ERROR "msvc-x86.cmake: vswhere.exe not found; install Visual Studio with the C++ workload.")
endif()
execute_process(
    COMMAND "${_vswhere}" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64
            -property installationPath
    OUTPUT_VARIABLE _vs_root OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NOT _vs_root)
    message(FATAL_ERROR "msvc-x86.cmake: no Visual Studio with the C++ x86/x64 tools found.")
endif()
file(TO_CMAKE_PATH "${_vs_root}" _vs_root)

file(READ "${_vs_root}/VC/Auxiliary/Build/Microsoft.VCToolsVersion.default.txt" _msvc_version)
string(STRIP "${_msvc_version}" _msvc_version)
set(_msvc "${_vs_root}/VC/Tools/MSVC/${_msvc_version}")

set(_sdk_root "$ENV{ProgramFiles\(x86\)}/Windows Kits/10")
file(TO_CMAKE_PATH "${_sdk_root}" _sdk_root)
file(GLOB _sdk_versions RELATIVE "${_sdk_root}/Include" "${_sdk_root}/Include/10.*")
list(SORT _sdk_versions COMPARE NATURAL ORDER DESCENDING)
list(GET _sdk_versions 0 _sdk_version)
if(NOT _sdk_version)
    message(FATAL_ERROR "msvc-x86.cmake: no Windows SDK found under ${_sdk_root}.")
endif()

set(CMAKE_C_COMPILER "${_msvc}/bin/HostX86/x86/cl.exe")
set(CMAKE_LINKER "${_msvc}/bin/HostX86/x86/link.exe")
set(CMAKE_AR "${_msvc}/bin/HostX86/x86/lib.exe")
set(CMAKE_RC_COMPILER "${_sdk_root}/bin/${_sdk_version}/x86/rc.exe")
set(CMAKE_MT "${_sdk_root}/bin/${_sdk_version}/x86/mt.exe")

set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES
    "${_msvc}/include"
    "${_sdk_root}/Include/${_sdk_version}/ucrt"
    "${_sdk_root}/Include/${_sdk_version}/um"
    "${_sdk_root}/Include/${_sdk_version}/shared")
set(CMAKE_RC_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_C_STANDARD_INCLUDE_DIRECTORIES})

set(_libpaths
    "/LIBPATH:\"${_msvc}/lib/x86\""
    "/LIBPATH:\"${_sdk_root}/Lib/${_sdk_version}/ucrt/x86\""
    "/LIBPATH:\"${_sdk_root}/Lib/${_sdk_version}/um/x86\"")
string(JOIN " " _libpaths ${_libpaths})
set(CMAKE_EXE_LINKER_FLAGS_INIT "${_libpaths}")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "${_libpaths}")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "${_libpaths}")

message(STATUS "msvc-x86: MSVC ${_msvc_version}, Windows SDK ${_sdk_version}")
