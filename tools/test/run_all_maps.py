"""Starts every mission and checks that it loads and runs, several missions in parallel.

Needs the test build (CMake preset "test" / -DTHANDOR_DEV_TOOLS=ON) as GAME_DIR/thandor.exe with its SDL3.dll
(the worker copies link GAME_DIR's): it uses the test
aids OPEN_THANDOR_CAMPAIGN / OPEN_THANDOR_CAMPAIGN_LEVEL (start a campaign at a given level),
OPEN_THANDOR_WINDOWED, OPEN_THANDOR_MULTI_INSTANCE and OPEN_THANDOR_NET_PORT, plus the input script
(OPEN_THANDOR_SCRIPT).

usage: run_all_maps.py GAME_DIR [--jobs 10] [--max-jobs N --jobs-file PATH] [--minutes 1] [--only PATTERN] [--missions A,B,...]
                       [--shots MS]

Order: the tutorial campaign, the other campaigns level by level, then the single games. For each mission:
start it, drag the computer-opponent slider to "stark", click "Beginnen" until the level runs, press G until the
game speed is at its maximum, let it run --minutes, quit. A mission fails when it does not reach the game within
LOAD_TIMEOUT seconds, the level ends before --minutes are over (ENDED: lost or won at once; the in-game frames
stop), the process ends early, or crash.log / hang.log is written.

--jobs workers always run; up to --max-jobs more join as the number in --jobs-file allows, each only while the
machine's CPU load is below cpu_load.CPU_LIMIT (80 %), one at a time, and a busy machine makes them pause again before
their next mission. Each worker runs in its own game folder GAME_DIR_w<k> (hard links to GAME_DIR's files, own
thandor.exe copy, log, save folder and screenshots), windowed at its own screen position. Screenshots and log of
every run go to GAME_DIR/soak/<nn>_<name>/, the results to GAME_DIR/soak/results.txt (appended as missions
finish, so progress can be followed); every run folder gets sheet.png, the screenshot chain of that run.
"""
import argparse
import fnmatch
import os
import queue
import shutil
import subprocess
import threading
import time

import cpu_load

# hansolo level 1 (hansolo\s00_tut) is listed in the campaign but its level file is not in the game data;
# the campaign itself starts at level 2.
TUTORIAL = [('tutorial', level) for level in range(1, 4)]
CAMPAIGNS = ([('hansolo', level) for level in range(2, 27)] + [('nimm2', level) for level in range(1, 6)] +
             [('luke', level) for level in range(1, 5)])
SINGLE = ['mittelpunkt', 'asgard', 'map08', 'schweinebucht', 'fata morgana', 'gemezel', 'hargonatoll', 'ignis',
          'badewanne', 'kreuzgang', 'lavam', 'lucas03', 'midgard', 'muspelheim', 'niflheim', 'springquell',
          'stromschnelle', 'tunguska', 'multi1']
LOAD_TIMEOUT = 240

# Coordinates on the 1280x800 mission page ("Missionsbeschreibung"): slider knob in its default middle position,
# a point beyond the right end of the slider ("stark"), the "Beginnen" button (also "Weiter" of a single game's
# faction page). VK 71 is G: one game speed step faster (5 is the maximum).
SCRIPT = """0 layout 1280 800
6000 drag 785 496 900 496
7000 drag 785 496 900 496
10000 clickuntilingame 839 539 3000
0 ingame
2000 key 71
2500 key 71
3000 key 71
3500 key 71
4000 key 71
5000 key 71
{run_ms} quit
"""

parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
parser.add_argument('game_dir')
parser.add_argument('--jobs', type=int, default=10)
parser.add_argument('--max-jobs', type=int, help='workers that may join later (default: --jobs)')
parser.add_argument('--jobs-file', help='file holding the number of workers allowed right now (read before every '
                                        'mission; run_checks.py raises it as its other checks finish)')
parser.add_argument('--minutes', type=float, default=1)
parser.add_argument('--only', help='fnmatch pattern on the mission label, e.g. "hansolo*"')
parser.add_argument('--missions', help='comma-separated mission labels, e.g. "tutorial 1,hansolo 5,mittelpunkt"')
parser.add_argument('--shots', default='5000')
args = parser.parse_args()

game = os.path.abspath(args.game_dir)
soak = os.path.join(game, 'soak')
os.makedirs(soak, exist_ok=True)
script = os.path.join(soak, 'script.txt')
with open(script, 'w') as f:
    f.write(SCRIPT.format(run_ms=int(args.minutes * 60000)))
results = os.path.join(soak, 'results.txt')

missions = ([('tutorial %d' % level, campaign, level) for campaign, level in TUTORIAL] +
            [('%s %d' % (campaign, level), campaign, level) for campaign, level in CAMPAIGNS] +
            [(name, None, name) for name in SINGLE])
if args.only:
    missions = [m for m in missions if fnmatch.fnmatch(m[0], args.only)]
if args.missions:
    wanted = [w.strip() for w in args.missions.split(',') if w.strip()]
    unknown = set(wanted) - set(m[0] for m in missions)
    if unknown:
        raise SystemExit('unknown missions: ' + ', '.join(sorted(unknown)))
    missions = [m for m in missions if m[0] in wanted]

PRIVATE = ('thandor.exe', 'thandor.pdb', 'thandor.dat', 'thandor.ini', 'thandor.log', 'crash.log',
           'crash_raw.log', 'hang.log')


def make_worker_dir(k):
    """GAME_DIR_w<k>: hard links to the data files, own copies of the executable and settings."""
    target = '%s_w%d' % (game.rstrip('\\/'), k)
    os.makedirs(os.path.join(target, 'save'), exist_ok=True)
    for name in os.listdir(game):
        source = os.path.join(game, name)
        link = os.path.join(target, name)
        if name.lower() in PRIVATE:
            continue
        if os.path.isfile(source) and not os.path.exists(link):
            os.link(source, link)
        elif os.path.isdir(source) and name.lower() in ('flm', 'setup') and not os.path.exists(link):
            subprocess.run('mklink /J "%s" "%s"' % (link, source), shell=True, capture_output=True)
    # settings: a thandor.ini the game wrote in an earlier run would override the fresh thandor.dat (and may not be
    # a hard link into the game dir)
    for name in ('thandor.dat', 'thandor.ini'):
        if os.path.exists(os.path.join(target, name)):
            os.remove(os.path.join(target, name))
    for name in ('thandor.exe', 'thandor.pdb', 'thandor.dat', 'thandor.ini'):
        if os.path.exists(os.path.join(game, name)):
            shutil.copy(os.path.join(game, name), os.path.join(target, name))
    return target


def make_sheet(folder):
    files = sorted(f for f in os.listdir(folder) if f.startswith('shot_') and f.endswith('.bmp'))
    if not files:
        return
    from PIL import Image
    sheet = Image.new('RGB', (320 * 6, 200 * ((len(files) + 5) // 6)))
    for i, name in enumerate(files):
        thumb = Image.open(os.path.join(folder, name)).convert('RGB').resize((320, 200))
        sheet.paste(thumb, ((i % 6) * 320, (i // 6) * 200))
    sheet.save(os.path.join(folder, 'sheet.png'))


def mtime(path):
    return os.path.getmtime(path) if os.path.exists(path) else 0


def read_new_lines(log, skip):
    return list(open(log, errors='replace'))[skip:] if os.path.exists(log) else []


lock = threading.Lock()
done = [0]
work = queue.Queue()
for number, mission in enumerate(missions, 1):
    work.put((number, mission))


def run_mission(k, folder, number, label, campaign, level):
    exe = os.path.join(folder, 'thandor.exe')
    log = os.path.join(folder, 'thandor.log')
    env = dict(os.environ, OPEN_THANDOR_SCRIPT=script, OPEN_THANDOR_AUTOSHOT=args.shots, OPEN_THANDOR_WINDOWED='1',
               OPEN_THANDOR_WINDOW_X=str((k % 5) * 250), OPEN_THANDOR_WINDOW_Y=str((k // 5 % 2) * 350),
               OPEN_THANDOR_MULTI_INSTANCE='1', OPEN_THANDOR_NET_PORT=str(940 + k))
    for key in ('OPEN_THANDOR_CAMPAIGN', 'OPEN_THANDOR_CAMPAIGN_LEVEL'):
        env.pop(key, None)
    if campaign:
        env['OPEN_THANDOR_CAMPAIGN'] = campaign
        env['OPEN_THANDOR_CAMPAIGN_LEVEL'] = str(level)
        command = '"%s" -NOINTRO -KARTE="-"' % exe  # -KARTE opens the "Choose game" page, the test aid starts the campaign
    else:
        command = '"%s" -NOINTRO -KARTE="%s"' % (exe, level)
    log_lines = sum(1 for _ in open(log, errors='replace')) if os.path.exists(log) else 0
    crash_time = mtime(os.path.join(folder, 'crash.log'))
    hang_time = mtime(os.path.join(folder, 'hang.log'))
    shutil.rmtree(os.path.join(folder, 'shots'), ignore_errors=True)
    start = time.time()
    process = subprocess.Popen(command, executable=exe, cwd=folder, env=env)
    in_game_at = None
    ended = False
    status = None
    while status is None:
        time.sleep(2)
        if in_game_at is None and any('script: in game' in line for line in read_new_lines(log, log_lines)):
            in_game_at = time.time()
        if not ended and any('in-game session ended' in line for line in read_new_lines(log, log_lines)):
            ended = True
        if mtime(os.path.join(folder, 'crash.log')) > crash_time:
            status = 'CRASH'
        elif mtime(os.path.join(folder, 'hang.log')) > hang_time:
            status = 'HANG'
        elif process.poll() is not None:
            status = 'ENDED' if ended else 'ok' if in_game_at and time.time() - in_game_at >= args.minutes * 60 - 5 else 'EXITED EARLY'
        elif in_game_at is None and time.time() - start > LOAD_TIMEOUT:
            status = 'NOT LOADED'
        elif in_game_at and time.time() - in_game_at > args.minutes * 60 + 60:
            status = 'NO QUIT'
    if process.poll() is None:
        process.kill()
        process.wait()
    time.sleep(1)
    target = os.path.join(soak, '%02d_%s' % (number, label.replace(' ', '_')))
    shutil.rmtree(target, ignore_errors=True)
    if os.path.isdir(os.path.join(folder, 'shots')):
        shutil.copytree(os.path.join(folder, 'shots'), target)
    else:
        os.makedirs(target)
    with open(os.path.join(target, 'thandor.log'), 'w') as f:
        f.writelines(read_new_lines(log, log_lines))
    for name in ('crash.log', 'crash_raw.log', 'hang.log'):
        if status in ('CRASH', 'HANG') and os.path.exists(os.path.join(folder, name)):
            shutil.copy(os.path.join(folder, name), target)
    load = '%.0fs load' % (in_game_at - start) if in_game_at else 'no game'
    make_sheet(target)
    with lock:
        done[0] += 1
        line = '[%d/%d] %-22s %-12s %s' % (done[0], len(missions), label, status, load)
        print(line, flush=True)
        with open(results, 'a') as f:
            f.write(time.strftime('%H:%M:%S ') + line + '\n')


def allowed_jobs():
    """Workers allowed right now: the number in --jobs-file, else --jobs."""
    try:
        with open(args.jobs_file) as f:
            return max(args.jobs, min(int(f.read().strip()), max_jobs))
    except (TypeError, OSError, ValueError):
        return args.jobs


def worker(k):
    folder = None
    while not work.empty():
        if k >= allowed_jobs() or (k >= args.jobs and not cpu_load.try_start()):
            time.sleep(5)  # a later worker waits for free places and CPU headroom
            continue
        try:
            number, (label, campaign, level) = work.get_nowait()
        except queue.Empty:
            return
        if folder is None:
            folder = make_worker_dir(k)
        run_mission(k, folder, number, label, campaign, level)


max_jobs = max(args.jobs, args.max_jobs or args.jobs)
threads = [threading.Thread(target=worker, args=(k,)) for k in range(max_jobs)]
for k, thread in enumerate(threads):
    thread.start()
    if k < args.jobs:
        time.sleep(3)  # stagger the starts a little
for thread in threads:
    thread.join()
