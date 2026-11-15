#!/usr/bin/env bash
set -euo pipefail

# Build Open Thandor's 64-bit Windows executable on a Linux host.
# Default: MinGW-w64 GCC + DWARF split debug file.
# --pdb: Clang + LLD + MinGW-w64 GNU ABI/runtime + CodeView PDB.
# SDL3 is pinned/checksum-verified unless SDL3_PREFIX is supplied.

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
PROFILE="release"
CLEAN=0
PDB_MODE=0
JOBS="${JOBS:-}"
SDL3_PREFIX="${SDL3_PREFIX:-}"
SDL3_VERSION="${SDL3_VERSION:-3.4.18}"
SDL3_SHA256="${SDL3_SHA256:-}"
DEPS_DIR="${THANDOR_CROSS_DEPS_DIR:-${ROOT_DIR}/.cross-deps}"

usage() {
    cat <<USAGE
Usage: tools/build-linux-mingw.sh [release|test|gpu-test] [--pdb] [--clean] [--jobs N] [--sdl-prefix DIR]

Builds a 64-bit Windows thandor.exe from Linux. The default compiler is MinGW-w64 GCC. --pdb switches the
frontend/linker to Clang + LLD while keeping the MinGW-w64 GNU ABI and runtime, and emits thandor.pdb.

Profiles:
  release   RelWithDebInfo, developer tools off (default)
  test      RelWithDebInfo, developer tools on
  gpu-test  same as test; retained as the familiar project profile name (SDL_GPU is on by default)

Options:
  --pdb                    Clang + LLD CodeView build; emits thandor.pdb for Visual Studio/WinDbg.
  --clean                  Remove this profile/toolchain build directory before configuring.
  --jobs N                 Parallel build jobs.
  --sdl-prefix DIR         Existing SDL3 MinGW x86_64 prefix; skips download.

Environment:
  SDL3_PREFIX             Existing SDL3 MinGW x86_64 prefix; skips download.
  SDL3_VERSION            SDL3 release to bootstrap (default: 3.4.18).
  SDL3_SHA256             Required when SDL3_VERSION is not the pinned default.
  THANDOR_CROSS_DEPS_DIR  Download/extraction cache (default: .cross-deps).
  JOBS                    Parallel jobs; defaults to nproc.

Required host tools: cmake >= 3.25, ninja, python3, MinGW-w64 (x86_64-w64-mingw32-g++/gcc/nm/windres), tar,
sha256sum, and either curl or wget when SDL3_PREFIX is not supplied. --pdb additionally needs clang, clang++ and
LLD (`ld.lld`).
USAGE
}

while (($#)); do
    case "$1" in
        release|test|gpu-test) PROFILE="$1"; shift ;;
        --pdb) PDB_MODE=1; shift ;;
        --clean) CLEAN=1; shift ;;
        --jobs)
            [[ $# -ge 2 ]] || { echo "--jobs needs a value" >&2; exit 2; }
            JOBS="$2"; shift 2 ;;
        --sdl-prefix)
            [[ $# -ge 2 ]] || { echo "--sdl-prefix needs a directory" >&2; exit 2; }
            SDL3_PREFIX="$2"; shift 2 ;;
        -h|--help) usage; exit 0 ;;
        *) echo "unknown argument: $1" >&2; usage >&2; exit 2 ;;
    esac
done

need() {
    command -v "$1" >/dev/null 2>&1 || { echo "missing required host tool: $1" >&2; exit 1; }
}

need cmake
need ninja
need python3
need x86_64-w64-mingw32-g++
need x86_64-w64-mingw32-gcc
need x86_64-w64-mingw32-nm
need x86_64-w64-mingw32-windres
need tar
need sha256sum
if ((PDB_MODE)); then
    need clang
    need clang++
    need ld.lld
fi

cmake_version="$(cmake --version | awk 'NR==1 {print $3}')"
python3 - "$cmake_version" <<'PYVER'
import sys
v=tuple(int(x) for x in sys.argv[1].split('.')[:3])
if v < (3,25,0):
    raise SystemExit(f"CMake 3.25+ required, found {sys.argv[1]}")
PYVER

if [[ -z "$JOBS" ]]; then
    JOBS="$(nproc 2>/dev/null || echo 2)"
fi

case "$PROFILE" in
    release) DEV_TOOLS=OFF ;;
    test|gpu-test) DEV_TOOLS=ON ;;
esac

if ((PDB_MODE)); then
    # The PDB path still uses the MinGW GCC C++ headers/runtime. Distro Clang does not always discover a cross GCC
    # installation automatically (Arch keeps libgcc in a versioned /usr/lib/gcc/x86_64-w64-mingw32/<version>
    # directory), so resolve it through the actual MinGW driver and test the complete C++/CRT/LLD path up front.
    MINGW_GXX="$(command -v x86_64-w64-mingw32-g++)"
    MINGW_LIBGCC="$($MINGW_GXX -print-libgcc-file-name)"
    if [[ ! -f "$MINGW_LIBGCC" ]]; then
        echo "x86_64-w64-mingw32-g++ did not return a usable libgcc path: $MINGW_LIBGCC" >&2
        exit 1
    fi
    MINGW_GCC_INSTALL_DIR="$(dirname "$MINGW_LIBGCC")"
    MINGW_GCC_VERSION="$($MINGW_GXX -dumpfullversion 2>/dev/null || true)"
    if [[ -z "$MINGW_GCC_VERSION" ]]; then
        MINGW_GCC_VERSION="$($MINGW_GXX -dumpversion)"
    fi
    if [[ -z "$MINGW_GCC_VERSION" ]]; then
        echo "could not determine the MinGW-w64 GCC version" >&2
        exit 1
    fi
    MINGW_SYSROOT="$($MINGW_GXX -print-sysroot)"
    if [[ -z "$MINGW_SYSROOT" || ! -d "$MINGW_SYSROOT/include" || ! -d "$MINGW_SYSROOT/lib" ]]; then
        mingw_bin_dir="$(dirname "$MINGW_GXX")"
        mingw_root="$(dirname "$mingw_bin_dir")"
        MINGW_SYSROOT="${mingw_root}/x86_64-w64-mingw32"
    fi
    if [[ ! -d "$MINGW_SYSROOT/include" || ! -d "$MINGW_SYSROOT/lib" ]]; then
        echo "could not locate the MinGW-w64 sysroot (got: $MINGW_SYSROOT)" >&2
        exit 1
    fi

    # Arch installs cross libstdc++ headers below the target sysroot rather than below libgcc. Clang's
    # Windows-GNU driver does not discover those GCC C++ header roots reliably, so provide the same three
    # directories that g++ uses: generic headers, target-specific bits/c++config.h, and backward headers.
    MINGW_CXX_INCLUDE="$MINGW_SYSROOT/include/c++/$MINGW_GCC_VERSION"
    MINGW_CXX_TARGET_INCLUDE="$MINGW_CXX_INCLUDE/x86_64-w64-mingw32"
    MINGW_CXX_BACKWARD_INCLUDE="$MINGW_CXX_INCLUDE/backward"
    if [[ ! -f "$MINGW_CXX_INCLUDE/string" || ! -f "$MINGW_CXX_TARGET_INCLUDE/bits/c++config.h" ]]; then
        echo "could not locate the MinGW-w64 libstdc++ headers for GCC $MINGW_GCC_VERSION" >&2
        echo "expected: $MINGW_CXX_INCLUDE/string" >&2
        echo "expected: $MINGW_CXX_TARGET_INCLUDE/bits/c++config.h" >&2
        echo "Check that mingw-w64-gcc is fully installed and that x86_64-w64-mingw32-g++ reports the same version." >&2
        exit 1
    fi

    echo "Clang/PDB MinGW GCC runtime: $MINGW_GCC_INSTALL_DIR"
    echo "Clang/PDB MinGW sysroot:     $MINGW_SYSROOT"
    echo "Clang/PDB libstdc++ headers: $MINGW_CXX_INCLUDE"

    probe_dir="$(mktemp -d)"
    trap 'rm -rf "$probe_dir"' EXIT
    cat >"${probe_dir}/pdb_probe.cpp" <<'PDBCPP'
#include <string>
#include <windows.h>

int main() {
    std::string value = "thandor";
    return (value.size() == 7 && GetCurrentProcessId() != 0) ? 0 : 1;
}
PDBCPP
    if ! clang++ --target=x86_64-w64-windows-gnu --sysroot="$MINGW_SYSROOT" -nostdinc++ \
        -isystem "$MINGW_CXX_INCLUDE" -isystem "$MINGW_CXX_TARGET_INCLUDE" -isystem "$MINGW_CXX_BACKWARD_INCLUDE" \
        -fuse-ld=lld -L"$MINGW_GCC_INSTALL_DIR" \
        -g -gcodeview -Wl,--pdb="${probe_dir}/pdb_probe.pdb" -Wl,-S \
        "${probe_dir}/pdb_probe.cpp" -o "${probe_dir}/pdb_probe.exe"; then
        cat >&2 <<EOF
Clang/LLD could not link against the installed MinGW-w64 C++ runtime.
Resolved GCC runtime: $MINGW_GCC_INSTALL_DIR
Resolved sysroot:     $MINGW_SYSROOT
Resolved C++ headers: $MINGW_CXX_INCLUDE
Check that the Arch mingw-w64-gcc package is complete and that its libstdc++ headers/runtime match x86_64-w64-mingw32-g++.
EOF
        exit 1
    fi
    python3 - "${probe_dir}/pdb_probe.pdb" <<'PYCHECK'
from pathlib import Path
import sys
p = Path(sys.argv[1])
if not p.is_file() or not p.read_bytes().startswith(b"Microsoft C/C++ MSF 7.00"):
    raise SystemExit("Clang/LLD probe did not produce an MSF 7.00 PDB")
PYCHECK
    if ! x86_64-w64-mingw32-nm "${probe_dir}/pdb_probe.exe" 2>/dev/null | grep -q ' main$'; then
        echo "Clang/LLD probe PDB worked, but the PE/COFF symbol table was not retained (-S); thandor.sym would fail" >&2
        exit 1
    fi
    rm -rf "$probe_dir"
    trap - EXIT
fi

if [[ -z "$SDL3_PREFIX" ]]; then
    mkdir -p "$DEPS_DIR"
    archive="${DEPS_DIR}/SDL3-devel-${SDL3_VERSION}-mingw.tar.gz"
    unpack_dir="${DEPS_DIR}/SDL3-${SDL3_VERSION}-mingw"
    url="https://github.com/libsdl-org/SDL/releases/download/release-${SDL3_VERSION}/SDL3-devel-${SDL3_VERSION}-mingw.tar.gz"

    if [[ "$SDL3_VERSION" == "3.4.18" && -z "$SDL3_SHA256" ]]; then
        SDL3_SHA256="049b9dd711e3fb3f8d84feb998dfec54a75e17cf539c4a80ff4d058d839771cf"
    fi
    if [[ -z "$SDL3_SHA256" ]]; then
        echo "SDL3_SHA256 is required when overriding SDL3_VERSION (requested ${SDL3_VERSION})" >&2
        exit 1
    fi

    if [[ ! -f "$archive" ]]; then
        echo "Downloading SDL3 ${SDL3_VERSION} MinGW development package..."
        tmp="${archive}.part"
        rm -f "$tmp"
        if command -v curl >/dev/null 2>&1; then
            curl -fL --retry 3 --retry-delay 2 -o "$tmp" "$url"
        elif command -v wget >/dev/null 2>&1; then
            wget -O "$tmp" "$url"
        else
            echo "need curl or wget to bootstrap SDL3; alternatively pass --sdl-prefix DIR" >&2
            exit 1
        fi
        mv "$tmp" "$archive"
    fi

    echo "${SDL3_SHA256}  ${archive}" | sha256sum -c -
    if [[ ! -d "$unpack_dir" ]]; then
        mkdir -p "$unpack_dir"
        tar -xzf "$archive" -C "$unpack_dir"
    fi

    SDL3_CONFIG="$(find "$unpack_dir" -type f -path '*/x86_64-w64-mingw32/lib/cmake/SDL3/SDL3Config.cmake' -print -quit)"
    if [[ -z "$SDL3_CONFIG" ]]; then
        echo "could not find x86_64-w64-mingw32 SDL3Config.cmake under $unpack_dir" >&2
        exit 1
    fi
    SDL3_DIR="$(dirname "$SDL3_CONFIG")"
    SDL3_PREFIX="$(cd "$SDL3_DIR/../../.." && pwd)"
else
    SDL3_PREFIX="$(cd "$SDL3_PREFIX" && pwd)"
    SDL3_CONFIG="$(find "$SDL3_PREFIX" -type f -path '*/lib/cmake/SDL3/SDL3Config.cmake' -print -quit)"
    if [[ -z "$SDL3_CONFIG" ]]; then
        echo "could not find lib/cmake/SDL3/SDL3Config.cmake below SDL3_PREFIX=$SDL3_PREFIX" >&2
        exit 1
    fi
    SDL3_DIR="$(dirname "$SDL3_CONFIG")"
fi

if ((PDB_MODE)); then
    BUILD_TAG="clang-pdb"
    TOOLCHAIN_FILE="${ROOT_DIR}/cmake/clang-mingw-linux-x64.cmake"
else
    BUILD_TAG="mingw"
    TOOLCHAIN_FILE="${ROOT_DIR}/cmake/mingw-linux-x64.cmake"
fi
BUILD_DIR="${ROOT_DIR}/build-linux-${BUILD_TAG}-${PROFILE}"
DIST_DIR="${BUILD_DIR}/dist"
if ((CLEAN)); then
    rm -rf "$BUILD_DIR"
fi

cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DSDL3_DIR="$SDL3_DIR" \
    -DTHANDOR_DEV_TOOLS="$DEV_TOOLS" \
    -DTHANDOR_RENDERER_SDL_GPU=ON \
    -DTHANDOR_GPU_PRECOMPILED_SHADERS=ON

cmake --build "$BUILD_DIR" --parallel "$JOBS"

mkdir -p "$DIST_DIR"
for file in thandor.exe SDL3.dll thandor.sym thandor.debug thandor.pdb thandor.map; do
    if [[ -f "${BUILD_DIR}/${file}" ]]; then
        cp -f "${BUILD_DIR}/${file}" "$DIST_DIR/"
    fi
done

if [[ ! -f "${DIST_DIR}/thandor.exe" || ! -f "${DIST_DIR}/SDL3.dll" || ! -f "${DIST_DIR}/thandor.sym" ]]; then
    echo "build completed but thandor.exe, SDL3.dll or thandor.sym is missing from ${DIST_DIR}" >&2
    exit 1
fi
if ((PDB_MODE)) && [[ ! -f "${DIST_DIR}/thandor.pdb" ]]; then
    echo "--pdb build completed without ${DIST_DIR}/thandor.pdb" >&2
    exit 1
fi

cat <<DONE

Cross-build complete.
  executable: ${DIST_DIR}/thandor.exe
  runtime DLL: ${DIST_DIR}/SDL3.dll
  symbols:     ${DIST_DIR}/thandor.sym
  linker map:  ${DIST_DIR}/thandor.map
DONE
if ((PDB_MODE)); then
    cat <<DONE
  PDB:         ${DIST_DIR}/thandor.pdb

The PDB contains CodeView symbols/source lines for Visual Studio, WinDbg and DbgHelp. Keep thandor.pdb with the exact
matching thandor.exe when debugging; thandor.sym remains useful as the game's compact in-process name fallback.
DONE
else
    cat <<DONE
  debug file:  ${DIST_DIR}/thandor.debug

The GCC build keeps DWARF source information in thandor.debug.
DONE
fi
cat <<DONE

Copy thandor.exe, SDL3.dll and (recommended for crash names) thandor.sym into a copy of the installed game directory.
DONE
