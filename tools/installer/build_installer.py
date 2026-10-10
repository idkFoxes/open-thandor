"""Builds the patch installer Thandor-Patch-6.exe from a release build (Inno Setup 7, tools/installer/thandor-patch.iss).

usage: build_installer.py BUILD_DIR [--out DIR] [--iscc PATH] [--version X.Y.Z]
                          [--wizard-image FILES] [--wizard-small-image FILES] [--icon FILE] [-D NAME[=VALUE] ...]

BUILD_DIR is a release build directory (preset mingw-release, or an MSVC release build) with thandor.exe and
SDL3.dll; a build with the developer tools (THANDOR_DEV_TOOLS=ON in its CMakeCache.txt) is refused. The files are
staged in BUILD_DIR/installer/stage: thandor.exe, SDL3.dll, thandor.sym (if the build has one) and the SDL3 licence
(LICENSE-SDL3.txt, from the vcpkg package next to SDL3_DIR, if found). A staged executable or DLL that still carries
DWARF debug sections is stripped with `strip --strip-debug` (keeps the symbol table and every address, so
thandor.sym still matches); the GCC build strips thandor.exe itself already. Then ISCC compiles the installer into
--out (default BUILD_DIR/installer).

--version defaults to CMAKE_PROJECT_VERSION from the CMakeCache.txt, else 1.0.7. The wizard images default to the
BMPs in tools/installer/images (chosen by thandor-patch.iss), else Inno's own placeholders; the icon to
src/platform/bootstrap/thandor.ico when it exists. The CMake target `installer` (release builds) calls this script.
-D passes further ISCC defines
(e.g. -D TestLowPriv for the install tests without admin rights; never for a release).
"""
import argparse
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
SCRIPT = os.path.join(HERE, 'thandor-patch.iss')
ISCC_CANDIDATES = [
    os.path.join(os.environ.get('LOCALAPPDATA', ''), 'Programs', 'Inno Setup 7', 'ISCC.exe'),
    os.path.join(os.environ.get('ProgramFiles', ''), 'Inno Setup 7', 'ISCC.exe'),
    os.path.join(os.environ.get('ProgramFiles(x86)', ''), 'Inno Setup 7', 'ISCC.exe'),
]


def cmake_cache(build_dir):
    values = {}
    path = os.path.join(build_dir, 'CMakeCache.txt')
    if os.path.isfile(path):
        with open(path, encoding='utf-8', errors='replace') as f:
            for line in f:
                m = re.match(r'^([A-Za-z0-9_]+):[A-Z]+=(.*)$', line.rstrip('\n'))
                if m:
                    values[m.group(1)] = m.group(2)
    return values


def find_iscc(given):
    if given:
        return given
    found = shutil.which('ISCC')
    if found:
        return found
    for candidate in ISCC_CANDIDATES:
        if os.path.isfile(candidate):
            return candidate
    sys.exit('build_installer: ISCC.exe (Inno Setup 7) not found, pass --iscc')


def find_tool(cache, key, name):
    path = cache.get(key)
    if path and os.path.isfile(path):
        return path
    return shutil.which(name)


def has_debug_sections(objdump, path):
    if not objdump:
        return False
    out = subprocess.run([objdump, '-h', path], capture_output=True, text=True).stdout
    return re.search(r'\s\.debug_info\s', out) is not None


def main():
    parser = argparse.ArgumentParser(description='Stage a release build and compile Thandor-Patch-6.exe.')
    parser.add_argument('build_dir')
    parser.add_argument('--out', help='output folder of the installer (default BUILD_DIR/installer)')
    parser.add_argument('--iscc', help='path of ISCC.exe (default: PATH, then the usual Inno Setup 7 folders)')
    parser.add_argument('--version', help='numeric version X.Y.Z (default: CMAKE_PROJECT_VERSION or 1.0.7)')
    parser.add_argument('--wizard-image', help='large wizard image file(s), comma separated')
    parser.add_argument('--wizard-small-image', help='small wizard image file(s), comma separated')
    parser.add_argument('--icon', help='Setup icon (.ico)')
    parser.add_argument('-D', dest='defines', action='append', default=[], help='further ISCC define NAME[=VALUE]')
    args = parser.parse_args()

    build_dir = os.path.abspath(args.build_dir)
    cache = cmake_cache(build_dir)
    if cache.get('THANDOR_DEV_TOOLS', 'OFF').upper() in ('ON', '1', 'TRUE', 'YES'):
        sys.exit('build_installer: %s is a developer-tools build (THANDOR_DEV_TOOLS=ON); use a release preset such '
                 'as mingw-release' % build_dir)
    exe = os.path.join(build_dir, 'thandor.exe')
    sdl = os.path.join(build_dir, 'SDL3.dll')
    for path in (exe, sdl):
        if not os.path.isfile(path):
            sys.exit('build_installer: missing %s' % path)
    version = args.version or cache.get('CMAKE_PROJECT_VERSION') or '1.0.7'
    if not re.fullmatch(r'\d+\.\d+\.\d+', version):
        sys.exit('build_installer: version %r is not X.Y.Z' % version)

    out_dir = os.path.abspath(args.out) if args.out else os.path.join(build_dir, 'installer')
    stage = os.path.join(build_dir, 'installer', 'stage')
    if os.path.isdir(stage):
        shutil.rmtree(stage)
    os.makedirs(stage)
    os.makedirs(out_dir, exist_ok=True)

    shutil.copy2(exe, stage)
    shutil.copy2(sdl, stage)
    sym = os.path.join(build_dir, 'thandor.sym')
    if os.path.isfile(sym):
        shutil.copy2(sym, stage)
    else:
        print('build_installer: no thandor.sym (MSVC build?), shipping without it')
    sdl_dir = cache.get('SDL3_DIR')
    if sdl_dir and os.path.isfile(os.path.join(sdl_dir, 'copyright')):
        shutil.copyfile(os.path.join(sdl_dir, 'copyright'), os.path.join(stage, 'LICENSE-SDL3.txt'))
    else:
        print('build_installer: SDL3 licence (vcpkg share/sdl3/copyright) not found, shipping without it')

    objdump = find_tool(cache, 'CMAKE_OBJDUMP', 'objdump')
    strip = find_tool(cache, 'CMAKE_STRIP', 'strip')
    for name in ('thandor.exe', 'SDL3.dll'):
        path = os.path.join(stage, name)
        if has_debug_sections(objdump, path):
            if not strip:
                sys.exit('build_installer: %s has debug sections and no strip was found' % name)
            subprocess.run([strip, '--strip-debug', path], check=True)
            print('build_installer: stripped the debug sections of %s' % name)

    command = [find_iscc(args.iscc), '/DAppVersion=' + version, '/DStage=' + stage, '/O' + out_dir]
    if args.wizard_image:
        command.append('/DWizardImage=' + args.wizard_image)
    if args.wizard_small_image:
        command.append('/DWizardSmallImage=' + args.wizard_small_image)
    if args.icon:
        command.append('/DSetupIcon=' + os.path.abspath(args.icon))
    command += ['/D' + define for define in args.defines]
    command.append(SCRIPT)
    print(' '.join('"%s"' % part if ' ' in part else part for part in command), flush=True)
    result = subprocess.run(command)
    if result.returncode != 0:
        sys.exit('build_installer: ISCC failed (exit code %d)' % result.returncode)
    print('build_installer: %s' % os.path.join(out_dir, 'Thandor-Patch-6.exe'))


if __name__ == '__main__':
    main()
