"""Downgrade check of thandor.ini: what an older version makes of a newer version's settings file.

usage: ini_downgrade.py [--exe TEST_EXE] [--fixture NAME] [--strict]

Runs OPEN_THANDOR_SELFTEST=inifixtures of TEST_EXE (default: build-mingw-test/thandor.exe of this repository, a
build with the developer tools) on one case of tools/test/fixtures/ini (default experimental_dev: the thandor.ini
of the experimental-dev branch with [game] language, pathing, objective_overlay, autosave and a [keys] section),
in a temporary directory: the older version loads the file through its real loader, saves it (Flush) and loads it
again. Then it prints which keys of the newer file the older version reads, and which lines of the newer file
survive its save and which are lost - what a player keeps after starting an older version once and going back.
Exit status 1 when the case fails; with --strict also while its "pending" checks fail (unknown keys not kept).

Why this way and not with v1.0.7 itself: the parser and writer of v1.0.7 (and v1.0.6) are the ones of dev - between
v1.0.7 and dev persistent.cpp only gained the accessor PersistentSettings_IniPathUtf16 - so the dev test build
reads and writes a newer file exactly like 1.0.7. A released 1.0.7 has no developer tools (no self-tests), a 1.0.7
build with them would have to be made from the tag, and re-implementing the parser in Python would test the copy,
not the game. The older version is therefore the dev build (before the robustness branch A: unknown keys are
dropped on its next save - the behaviour of 1.0.7 and earlier) or, with --exe, any other build with developer tools.
"""
import argparse
import os
import shutil
import subprocess
import sys
import tempfile

from game_env import game_env

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
FIXTURES = os.path.join(HERE, 'fixtures', 'ini')


def ini_lines(path):
    """(section, 'key = value') of the key lines of an ini file (comments and headers left out; the text only,
    for the report)."""
    data = open(path, 'rb').read()
    for encoding in ('utf-8-sig', 'utf-16'):
        try:
            text = data.decode(encoding)
            break
        except UnicodeDecodeError:
            continue
    else:
        text = data.decode('latin-1')
    section, lines = '', []
    for raw in text.replace('\r\n', '\n').replace('\r', '\n').split('\n'):
        line = raw.strip()
        if not line or line[0] in ';#':
            continue
        if line.startswith('[') and ']' in line:
            section = line[1:line.index(']')].strip().lower()
            continue
        if '=' in line:
            key, value = line.split('=', 1)
            lines.append((section, '%s = %s' % (key.strip(), value.strip())))
    return lines


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--exe', default=os.path.join(REPO, 'build-mingw-test', 'thandor.exe'))
    parser.add_argument('--fixture', default='experimental_dev')
    parser.add_argument('--strict', action='store_true', help='pending checks (unknown keys kept) must pass')
    args = parser.parse_args()
    exe = os.path.abspath(args.exe)
    source = os.path.join(FIXTURES, args.fixture + '.ini')
    if not os.path.exists(exe) or not os.path.exists(source):
        parser.error('missing: ' + (exe if not os.path.exists(exe) else source))
    folder = tempfile.mkdtemp(prefix='ot_ini_downgrade_')
    try:
        # own copy of the exe: its thandor.log lands next to it
        shutil.copy(exe, os.path.join(folder, 'thandor.exe'))
        dll = os.path.join(os.path.dirname(exe), 'SDL3.dll')
        if os.path.exists(dll):
            shutil.copy(dll, os.path.join(folder, 'SDL3.dll'))
        work = os.path.join(folder, 'work')
        env = game_env(OPEN_THANDOR_SELFTEST='inifixtures', OPEN_THANDOR_INI_FIXTURES=FIXTURES,
                       OPEN_THANDOR_INI_FIXTURE=args.fixture, OPEN_THANDOR_INI_OUT=work,
                       OPEN_THANDOR_INI_STRICT='1' if args.strict else '0')
        subprocess.run([os.path.join(folder, 'thandor.exe')], cwd=folder, env=env, timeout=120)
        log_path = os.path.join(folder, 'thandor.log')
        log = open(log_path, encoding='utf-8', errors='replace').read().splitlines() if os.path.exists(log_path) else []
        written_path = os.path.join(work, args.fixture, 'thandor.ini')
        print('older version: %s' % exe)
        print('newer file:    %s' % source)
        print()
        print('self-test (load, values, save, reload):')
        for line in log:
            if line.startswith('inifixtures %s:' % args.fixture) or line.startswith('inifixtures: '):
                print('  ' + line)
        if not os.path.exists(written_path):
            print('no thandor.ini written')
            return 1
        newer = ini_lines(source)
        written = set(ini_lines(written_path))
        kept = [entry for entry in newer if entry in written]
        lost = [entry for entry in newer if entry not in written]
        print()
        print('lines of the newer file after the older version saved it: %d kept, %d lost' % (len(kept), len(lost)))
        for section, line in lost:
            print('  lost  [%s] %s' % (section, line))
        added = sorted(set(written) - set(newer))
        for section, line in added:
            print('  added [%s] %s' % (section, line))
        print()
        if lost:
            print('downgrade: an older version drops these keys on its next save; the newer version then reads '
                  'their defaults (see the "pending A" checks: once unknown keys are kept, nothing is lost)')
        else:
            print('downgrade: every line of the newer file survives the older version\'s save')
        result = [line for line in log if line.startswith('inifixtures: ')]
        return 0 if result and result[-1].startswith('inifixtures: ok') else 1
    finally:
        shutil.rmtree(folder, ignore_errors=True)


if __name__ == '__main__':
    sys.exit(main())
