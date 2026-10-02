"""Runs all behaviour checks after a build, in parallel, and prints one summary table.

usage: run_checks.py GAME_DIR [--new TEST_EXE] [--old PREVIOUS_TEST_EXE] [--release RELEASE_EXE]
                     [--map-jobs N] [--map-minutes M] [--skip determinism,aihash,...]

GAME_DIR is a game directory with the game data (and thandor_original.exe for imagecmp). Every check runs in its
own linked copy GAME_DIR_chk_<name> (hard links to the data files, own executable, settings, log and save
folder; the tools started from here make further copies next to it), windowed, with its own UDP ports:
  determinism  run_determinism.py --reference tools/test/determinism_reference (3 instances, ports 960-962)
  aihash       compare_map_hash.py stromschnelle 1400: two copies of the new build, plus the --old build if
               given; all must record identical state hashes (ports 980-982)
  pixels       (only with --old) new vs old build: the paused in-game frame (skirmish_pause.txt, -KARTE=
               "mittelpunkt") and the choose-game page (choose_game.txt, -KARTE="-"); the screenshots must be
               pixel-identical (ports 900-903)
  saveload     skirmish_save.txt saves a skirmish (save\\Multi Ahaggar.sve must exist and list 10 entries with
               tools/data/pck.py), then choose_load.txt loads it from the choose-game page; the loaded game must
               reach "in game" and quit by script without crash or hang (port 905)
  multiplayer  run_multiplayer.py 150 s with mp_host_create.txt / mp_client_join.txt: both reach the game, no
               crash, and the client log still has "net recv" lines in its last 20 lines (ports 929/930)
  imagecmp     the release build with OPEN_THANDOR_SELFTEST=imagecmp: 0 pointer and 0 byte mismatches
  maps         run_all_maps.py --only "*[!0-9]" (the single games without a digit at the end); non-ok missions
               fail, except ENDED (the strong computer opponents can win a single map in time): a warning
               (ports 940+)
--new defaults to build-test/thandor.exe, --release to build-rel/thandor.exe of this repository. The output of
each check goes to GAME_DIR/checks/<check>.txt (screenshots / diffs of the pixel check to GAME_DIR/checks/
pixels/). Exit status 1 when any check failed. Only game processes started from the copies of this run are
stopped at the end.

The input scripts (tools/test/*.txt, format: src/platform/debug/script.c; lines that do not start with a number are
comments) use layout 1280x720 in an 800-high window: script y = screen y - 40.
"""
import argparse
import glob
import os
import shutil
import subprocess
import sys
import threading
import time

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
PRIVATE = ('thandor.exe', 'thandor.pdb', 'thandor.dat', 'thandor.log', 'crash.log', 'crash_raw.log', 'hang.log',
           'statehash.txt')
SHARED_DIRS = ('flm', 'setup', 'level')  # read-only data folders: junctions; every other folder is skipped
CHECKS = ['determinism', 'aihash', 'pixels', 'saveload', 'multiplayer', 'imagecmp', 'maps']
INSTANCES = {'determinism': 3, 'aihash': 3, 'pixels': 2, 'saveload': 1, 'multiplayer': 2, 'imagecmp': 0}

args = None
game = None
out_dir = None
copies = []
copies_lock = threading.Lock()


def make_copy(name, exe):
    """GAME_DIR_chk_<name>: hard links to the data files, junctions for the data folders, own executable,
    settings, empty log and save folder."""
    target = '%s_chk_%s' % (game.rstrip('\\/'), name)
    with copies_lock:
        copies.append(target)
    os.makedirs(target, exist_ok=True)
    for entry in os.listdir(game):
        source, link = os.path.join(game, entry), os.path.join(target, entry)
        if entry.lower() in PRIVATE:
            continue
        if os.path.isfile(source) and not os.path.exists(link):
            os.link(source, link)
        elif os.path.isdir(source) and entry.lower() in SHARED_DIRS and not os.path.exists(link):
            subprocess.run('mklink /J "%s" "%s"' % (link, source), shell=True, capture_output=True)
    for entry in ('save', 'shots'):
        path = os.path.join(target, entry)
        if os.path.isdir(path) and not os.path.isjunction(path):
            shutil.rmtree(path)
    os.makedirs(os.path.join(target, 'save'))
    for entry in PRIVATE:
        if os.path.exists(os.path.join(target, entry)):
            os.remove(os.path.join(target, entry))
    shutil.copy(exe, os.path.join(target, 'thandor.exe'))
    pdb = os.path.splitext(exe)[0] + '.pdb'
    if os.path.exists(pdb):
        shutil.copy(pdb, os.path.join(target, 'thandor.pdb'))
    if os.path.exists(os.path.join(game, 'thandor.dat')):
        shutil.copy(os.path.join(game, 'thandor.dat'), os.path.join(target, 'thandor.dat'))
    return target


def run_tool(log, command, timeout):
    """Runs a python tool, its output into log; returns (exit code, output)."""
    with open(log, 'w') as f:
        f.write('> %s\n' % ' '.join(command))
        f.flush()
        process = subprocess.Popen([sys.executable] + command, stdout=f, stderr=subprocess.STDOUT,
                                   env=dict(os.environ, PYTHONIOENCODING='utf-8'))
        try:
            code = process.wait(timeout=timeout)
        except subprocess.TimeoutExpired:  # stop the tool and the games it started
            subprocess.run('taskkill /F /T /PID %d' % process.pid, shell=True, capture_output=True)
            process.wait()
            code = 'timeout'
    return code, open(log, errors='replace').read()


def run_game(folder, arguments, env, timeout, port):
    """Starts folder/thandor.exe, waits until it ends (at most timeout s), stops only this process.
    Returns (exited by itself, crash or hang log written, new log text)."""
    for entry in ('crash.log', 'hang.log', 'thandor.log'):
        if os.path.exists(os.path.join(folder, entry)):
            os.remove(os.path.join(folder, entry))
    exe = os.path.join(folder, 'thandor.exe')
    env = dict(os.environ, OPEN_THANDOR_WINDOWED='1', OPEN_THANDOR_MULTI_INSTANCE='1', OPEN_THANDOR_NET_PORT=str(port),
               **env)
    process = subprocess.Popen('"%s" %s' % (exe, arguments), executable=exe, cwd=folder, env=env)
    try:
        process.wait(timeout=timeout)
        exited = True
    except subprocess.TimeoutExpired:
        subprocess.run('taskkill /F /T /PID %d' % process.pid, shell=True, capture_output=True)
        process.wait()
        exited = False
    crashed = any(os.path.exists(os.path.join(folder, e)) for e in ('crash.log', 'hang.log'))
    log = os.path.join(folder, 'thandor.log')
    return exited, crashed, open(log, errors='replace').read() if os.path.exists(log) else ''


# ---------------------------------------------------------------------------------------------------- checks

def check_determinism():
    folder = make_copy('det', args.new)
    code, text = run_tool(os.path.join(out_dir, 'determinism.txt'),
                          [os.path.join(HERE, 'run_determinism.py'), folder, '--reference',
                           os.path.join(HERE, 'determinism_reference')], 1800)
    lines = [l for l in text.splitlines() if l.split() and l.split()[0] in ('battle', 'turrets', 'production')]
    details = '; '.join(l.split()[0] + ' ok' if l.split()[1] == 'ok:' else ' '.join(l.split(' (rerun')[0].split())
                        for l in lines) or 'no result (exit %s)' % code
    return ('PASS' if code == 0 and len(lines) == 3 else 'FAIL'), details


def check_aihash():
    dirs = [make_copy('hash0', args.new), make_copy('hash1', args.new)]
    if args.old:
        dirs.append(make_copy('hash2', args.old))
    code, text = run_tool(os.path.join(out_dir, 'aihash.txt'),
                          [os.path.join(HERE, 'compare_map_hash.py'), 'stromschnelle', '1400'] + dirs, 2400)
    steps = [l.split(' steps, last ')[0].split()[-1] for l in text.splitlines() if ' steps, last ' in l]
    compares = [l for l in text.splitlines() if l.startswith('dir ') and ' vs dir 0: ' in l]
    ok = (code == 0 and len(compares) == len(dirs) - 1 and all(l.endswith('identical') for l in compares)
          and len(steps) == len(dirs) and all(s != '0' for s in steps))
    names = ['new', 'new'] + (['old'] if args.old else [])
    details = 'steps %s; %s' % ('/'.join(steps) or '-', '; '.join(
        '%s: %s' % (names[int(l.split()[1])], l.split(': ', 1)[1]) for l in compares) or 'no result')
    return ('PASS' if ok else 'FAIL'), details


def check_pixels():
    from PIL import Image, ImageChops
    pixels = os.path.join(out_dir, 'pixels')
    shutil.rmtree(pixels, ignore_errors=True)
    os.makedirs(pixels)
    folders = {'new': make_copy('pix_new', args.new), 'old': make_copy('pix_old', args.old)}
    pages = [('pause', 'skirmish_pause.txt', '-NOINTRO -KARTE="mittelpunkt"'),
             ('choose', 'choose_game.txt', '-NOINTRO -KARTE="-"')]
    report, failed = [], False
    log = open(os.path.join(out_dir, 'pixels.txt'), 'w')
    for page, script, arguments in pages:
        outcome = {}

        def one(build, k):
            folder = folders[build]
            shutil.rmtree(os.path.join(folder, 'shots'), ignore_errors=True)
            # the script command "shot" is not implemented; the page is static (paused) for several seconds
            # before the script quits, so the last periodic shot shows it
            env = {'OPEN_THANDOR_SCRIPT': os.path.join(HERE, script), 'OPEN_THANDOR_AUTOSHOT': '2000',
                   'OPEN_THANDOR_WINDOW_X': str(k * 660), 'OPEN_THANDOR_WINDOW_Y': '420'}
            if page == 'pause':
                # pause at a fixed simulation tick with a fixed seed: the frame no longer depends on the frame rate
                env.update({'OPEN_THANDOR_STATEHASH': '100000', 'OPEN_THANDOR_STATEHASH_SEED': '12345',
                            'OPEN_THANDOR_STATEHASH_PAUSE_AT': '150'})
            outcome[build] = run_game(folder, arguments, env, 240, 900 + k)
            shots = sorted(glob.glob(os.path.join(folder, 'shots', 'shot_*.bmp')))
            if shots:
                shutil.copy(shots[-1], os.path.join(pixels, '%s_%s.bmp' % (page, build)))

        threads = [threading.Thread(target=one, args=(build, k)) for k, build in enumerate(('new', 'old'))]
        for t in threads:
            t.start()
        for t in threads:
            t.join()
        for build in ('new', 'old'):
            exited, crashed, text = outcome[build]
            log.write('==== %s %s: exited %s, crash/hang %s\n%s\n' % (page, build, exited, crashed, text))
        shots = [os.path.join(pixels, '%s_%s.bmp' % (page, b)) for b in ('new', 'old')]
        if not all(os.path.exists(s) for s in shots):
            report.append('%s: no screenshot' % page)
            failed = True
            continue
        a, b = (Image.open(s).convert('RGB') for s in shots)
        box = ImageChops.difference(a, b).getbbox() if a.size == b.size else (0, 0) + a.size
        if box:
            ImageChops.difference(a, b).save(os.path.join(pixels, '%s_diff.png' % page)) if a.size == b.size else None
            report.append('%s: DIFFERS in %s' % (page, box))
            failed = True
        else:
            report.append('%s identical' % page)
    log.close()
    return ('FAIL' if failed else 'PASS'), '; '.join(report)


def check_saveload():
    folder = make_copy('saveload', args.new)
    log = open(os.path.join(out_dir, 'saveload.txt'), 'w')
    save = os.path.join(folder, 'save', 'Multi Ahaggar.sve')
    env = {'OPEN_THANDOR_SCRIPT': os.path.join(HERE, 'skirmish_save.txt'), 'OPEN_THANDOR_WINDOW_X': '1320',
           'OPEN_THANDOR_WINDOW_Y': '420'}
    exited, crashed, text = run_game(folder, '-NOINTRO -KARTE="mittelpunkt"', env, 300, 905)
    log.write('==== save: exited %s, crash/hang %s\n%s\n' % (exited, crashed, text))
    if crashed or not os.path.exists(save):
        log.close()
        return 'FAIL', 'save: %s' % ('crash/hang' if crashed else 'no save file' + ('' if exited else ' (timeout)'))
    listing = subprocess.run([sys.executable, os.path.join(REPO, 'tools', 'data', 'pck.py'), 'list', save],
                             capture_output=True, text=True, encoding='utf-8', errors='replace',
                             env=dict(os.environ, PYTHONIOENCODING='utf-8'))
    log.write('==== pck.py list\n%s%s\n' % (listing.stdout, listing.stderr))
    entries = listing.stdout.strip().splitlines()[-1] if listing.stdout.strip() else 'no listing'
    if entries != '10 entries':
        log.close()
        return 'FAIL', 'save file lists "%s" (expected 10 entries)' % entries
    env['OPEN_THANDOR_SCRIPT'] = os.path.join(HERE, 'choose_load.txt')
    exited, crashed, text = run_game(folder, '-NOINTRO -KARTE="-"', env, 300, 905)
    log.write('==== load: exited %s, crash/hang %s\n%s\n' % (exited, crashed, text))
    log.close()
    in_game = 'script: in game' in text
    if crashed or not in_game or not exited:
        return 'FAIL', 'load: %s' % ('crash/hang' if crashed else 'did not reach the game' if not in_game
                                     else 'did not quit by script (timeout)')
    return 'PASS', 'saved (10 entries), loaded, ran 20 s in game'


def check_multiplayer():
    host = make_copy('mp', args.new)
    client = host + '2'
    with copies_lock:
        copies.append(client)
    times = {d: max([os.path.getmtime(os.path.join(d, n)) for n in ('crash.log', 'hang.log')
                     if os.path.exists(os.path.join(d, n))] + [0]) for d in (host, client)}
    code, text = run_tool(os.path.join(out_dir, 'multiplayer.txt'),
                          [os.path.join(HERE, 'run_multiplayer.py'), host, '150',
                           '--host-script', os.path.join(HERE, 'mp_host_create.txt'),
                           '--client-script', os.path.join(HERE, 'mp_client_join.txt')], 400)
    problems = []
    for label in ('host', 'client'):
        if '[%s] script: in game' % label not in text:
            problems.append('%s did not reach the game' % label)
    for d, label in ((host, 'host'), (client, 'client')):
        for name in ('crash.log', 'hang.log'):
            path = os.path.join(d, name)
            if os.path.exists(path) and os.path.getmtime(path) > times[d]:
                problems.append('%s wrote %s' % (label, name))
    client_log = os.path.join(client, 'thandor.log')
    tail = list(open(client_log, errors='replace'))[-20:] if os.path.exists(client_log) else []
    recv = sum(1 for l in tail if l.startswith('net recv'))
    if not recv:
        problems.append('no "net recv" in the last 20 client log lines (lockstep stopped)')
    if code != 0:
        problems.append('tool exit %s' % code)
    return ('FAIL' if problems else 'PASS'), '; '.join(problems) or 'both in game, %d net recv in last 20 lines' % recv


def check_imagecmp():
    if not args.release or not os.path.exists(args.release):
        return 'SKIP', 'no release exe'
    if not os.path.exists(os.path.join(game, 'thandor_original.exe')):
        return 'SKIP', 'GAME_DIR has no thandor_original.exe'
    folder = make_copy('imagecmp', args.release)
    exited, crashed, text = run_game(folder, '-NOINTRO', {'OPEN_THANDOR_SELFTEST': 'imagecmp'}, 180, 906)
    open(os.path.join(out_dir, 'imagecmp.txt'), 'w').write(text)
    last = text.strip().splitlines()[-1] if text.strip() else 'no log'
    ok = exited and not crashed and last.startswith('imagecmp:') and \
        ' 0 pointer mismatches, 0 byte mismatches' in last
    return ('PASS' if ok else 'FAIL'), last.replace('imagecmp: ', '')


def check_maps():
    folder = make_copy('maps', args.new)
    shutil.rmtree(os.path.join(folder, 'soak'), ignore_errors=True)
    code, text = run_tool(os.path.join(out_dir, 'maps.txt'),
                          [os.path.join(HERE, 'run_all_maps.py'), folder, '--only', '*[!0-9]',
                           '--jobs', str(args.map_jobs), '--minutes', str(args.map_minutes)],
                          3600)
    rows = [l for l in text.splitlines() if l.startswith('[') and '/' in l.split()[0]]
    bad, ended = [], []
    for row in rows:
        content = row[row.index(']') + 2:]  # '%-22s %-12s %s' % (label, status, load)
        label, status = content[:22].strip(), content[23:35].strip()
        if status == 'ok':
            continue
        (ended if status == 'ENDED' else bad).append(label)
    details = '%d missions' % len(rows)
    if bad:
        details += ', FAILED: ' + ', '.join(bad)
    if ended:
        details += ', ENDED (warning): ' + ', '.join(ended)
    if code != 0 or not rows:
        return 'FAIL', details + ' (tool exit %s)' % code
    return ('FAIL' if bad else 'WARN' if ended else 'PASS'), details


# ------------------------------------------------------------------------------------------------------ main

def stray_processes():
    """thandor.exe processes whose executable lies in one of the copies of this run's checks (incl. the tools'
    sub-copies such as _chk_maps_w3 or _chk_mp2); a run on the same GAME_DIR with other checks is left alone."""
    result = subprocess.run(['powershell', '-NoProfile', '-Command',
                             "Get-CimInstance Win32_Process -Filter \"Name='thandor.exe'\" | "
                             "ForEach-Object { \"$($_.ProcessId)|$($_.ExecutablePath)\" }"],
                            capture_output=True, text=True)
    with copies_lock:
        prefixes = tuple(c.lower() for c in copies)
    found = []
    for line in result.stdout.splitlines():
        pid, _, path = line.partition('|')
        if prefixes and os.path.dirname(path).lower().startswith(prefixes):
            found.append((int(pid), path))
    return found


def main():
    global args, game, out_dir
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('game_dir')
    parser.add_argument('--new', default=os.path.join(REPO, 'build-test', 'thandor.exe'),
                        help='test build to check (default: build-test/thandor.exe)')
    parser.add_argument('--old', help='previous test build (comparison for aihash and pixels)')
    parser.add_argument('--release', default=os.path.join(REPO, 'build-rel', 'thandor.exe'),
                        help='release build for imagecmp (default: build-rel/thandor.exe)')
    parser.add_argument('--map-jobs', type=int, help='parallel missions (default: 16 minus the other instances, '
                                                       'at least 4)')
    parser.add_argument('--map-minutes', type=float, default=2)
    parser.add_argument('--skip', default='', help='comma-separated: ' + ','.join(CHECKS))
    args = parser.parse_args()
    game = os.path.abspath(args.game_dir)
    args.new = os.path.abspath(args.new)
    args.old = os.path.abspath(args.old) if args.old else None
    args.release = os.path.abspath(args.release) if args.release else None
    skip = set(s.strip() for s in args.skip.split(',') if s.strip())
    unknown = skip - set(CHECKS)
    if unknown:
        parser.error('unknown check(s): ' + ', '.join(sorted(unknown)))
    if args.old and not os.path.exists(args.old):
        parser.error('--old not found: ' + args.old)
    if not args.old:
        skip.add('pixels')
    selected = [c for c in CHECKS if c not in skip]
    if args.map_jobs is None:
        others = sum(INSTANCES[c] for c in selected if c in INSTANCES)
        if 'aihash' in selected and not args.old:
            others -= 1
        args.map_jobs = max(4, 16 - others)
    args.map_jobs = min(args.map_jobs, 20)  # ports 940..959; 960+ are the determinism instances
    out_dir = os.path.join(game, 'checks')
    os.makedirs(out_dir, exist_ok=True)
    print('checks: %s (maps: %d jobs, %g min); output in %s' % (', '.join(selected), args.map_jobs,
                                                                args.map_minutes, out_dir), flush=True)

    results = {}

    def runner(name):
        start = time.time()
        try:
            status, details = globals()['check_' + name]()
        except Exception as error:  # a broken check must not hide the others
            status, details = 'FAIL', 'exception: %r' % error
        results[name] = (status, details, time.time() - start)
        print('  %-12s %-4s %4.0fs  %s' % (name, status, results[name][2], details), flush=True)

    begin = time.time()
    threads = []
    try:
        for name in selected:
            thread = threading.Thread(target=runner, args=(name,))
            thread.start()
            threads.append(thread)
            time.sleep(3)  # stagger the starts a little
        for thread in threads:
            thread.join()
    finally:
        for pid, path in stray_processes():
            print('stopping leftover %s (pid %d)' % (path, pid))
            subprocess.run('taskkill /F /T /PID %d' % pid, shell=True, capture_output=True)
    for name in CHECKS:
        if name not in selected:
            results[name] = ('SKIP', 'no --old' if name == 'pixels' and not args.old else 'skipped', 0)
    print()
    print('%-12s %-6s %7s  %s' % ('check', 'result', 'time', 'details'))
    for name in CHECKS:
        status, details, seconds = results[name]
        print('%-12s %-6s %6.0fs  %s' % (name, status, seconds, details))
    print('total %.0f s' % (time.time() - begin))
    return 1 if any(r[0] == 'FAIL' for r in results.values()) else 0


if __name__ == '__main__':
    sys.exit(main())
