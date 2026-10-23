"""Downgrade check of thandor.ini: what an older version makes of a newer version's settings file.

usage: ini_downgrade.py --exe OLDER_TEST_EXE [--fixture NAME] [--strict]

Runs OPEN_THANDOR_SELFTEST=inifixtures of OLDER_TEST_EXE (a build of an older version with the developer tools, for
example a dev test build or one made from the v1.0.7 tag) on one fixture of tools/test/fixtures/ini (default
experimental_dev_v2: the thandor.ini this version writes, with [game] language and language_country_code, pathing,
objective_overlay, autosave and a [keys] section), in a temporary directory: the older version loads the file
through its real loader, saves it (Flush) and loads it again. Then it prints which keys of the newer file the older
version reads, and which lines of the newer file survive its save and which are lost - what a player keeps after
starting an older version once and going back.

The expectations are the older version's: <NAME>.downgrade (key names of 1.0.7 / dev: language_country_code,
[reserved] dword_50..58), not <NAME>.expect, which describes this version. The script copies the fixture directory
(the .ini / .dat / .include files) to a temporary one with <NAME>.downgrade as the only case, <NAME>.expect.
Exit status 1 when the case fails; with --strict also while its "pending" checks fail.

What to expect from an older build: dev (and 1.0.8) reads language_country_code (written by this version for
exactly this reason, so the language survives a downgrade), keeps the keys it does not know (language, pathing,
objective_overlay, autosave, [keys]) and writes them back; releases up to 1.0.7 read language_country_code but
drop the unknown keys on their next save (the newer version then reads their defaults).

Why this way and not by re-implementing the old parser in Python: that would test the copy, not the game.
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


def downgrade_fixtures(folder, fixture):
    """A fixture directory for the older build: the data files, and <fixture>.downgrade as <fixture>.expect."""
    target = os.path.join(folder, 'fixtures')
    os.makedirs(target)
    for name in os.listdir(FIXTURES):
        if os.path.splitext(name)[1].lower() in ('.ini', '.dat', '.include'):
            shutil.copy(os.path.join(FIXTURES, name), os.path.join(target, name))
    shutil.copy(os.path.join(FIXTURES, fixture + '.downgrade'), os.path.join(target, fixture + '.expect'))
    return target


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--exe', required=True, help='test build (developer tools) of the older version')
    parser.add_argument('--fixture', default='experimental_dev_v2')
    parser.add_argument('--strict', action='store_true', help='pending checks must pass')
    args = parser.parse_args()
    exe = os.path.abspath(args.exe)
    source = os.path.join(FIXTURES, args.fixture + '.ini')
    expectations = os.path.join(FIXTURES, args.fixture + '.downgrade')
    for path in (exe, source, expectations):
        if not os.path.exists(path):
            parser.error('missing: ' + path)
    folder = tempfile.mkdtemp(prefix='ot_ini_downgrade_')
    try:
        # own copy of the exe: its thandor.log lands next to it
        shutil.copy(exe, os.path.join(folder, 'thandor.exe'))
        dll = os.path.join(os.path.dirname(exe), 'SDL3.dll')
        if os.path.exists(dll):
            shutil.copy(dll, os.path.join(folder, 'SDL3.dll'))
        fixtures = downgrade_fixtures(folder, args.fixture)
        work = os.path.join(folder, 'work')
        env = game_env(OPEN_THANDOR_SELFTEST='inifixtures', OPEN_THANDOR_INI_FIXTURES=fixtures,
                       OPEN_THANDOR_INI_FIXTURE=args.fixture, OPEN_THANDOR_INI_OUT=work,
                       OPEN_THANDOR_INI_STRICT='1' if args.strict else '0')
        subprocess.run([os.path.join(folder, 'thandor.exe')], cwd=folder, env=env, timeout=120)
        log_path = os.path.join(folder, 'thandor.log')
        log = open(log_path, encoding='utf-8', errors='replace').read().splitlines() if os.path.exists(log_path) else []
        written_path = os.path.join(work, args.fixture, 'thandor.ini')
        print('older version: %s' % exe)
        print('newer file:    %s' % source)
        print('expectations:  %s' % expectations)
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
            print('downgrade: the older version drops these keys on its next save; the newer version then reads '
                  'their defaults')
        else:
            print('downgrade: every line of the newer file survives the older version\'s save')
        result = [line for line in log if line.startswith('inifixtures: ')]
        return 0 if result and result[-1].startswith('inifixtures: ok') else 1
    finally:
        shutil.rmtree(folder, ignore_errors=True)


if __name__ == '__main__':
    sys.exit(main())
