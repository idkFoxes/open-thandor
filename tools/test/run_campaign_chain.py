"""Plays campaign levels one after another through the real level change, to test the carry-over of units.

Needs the test build (CMake preset "test") as GAME_DIR/thandor.exe with its SDL3.dll. Uses the test aids OPEN_THANDOR_CAMPAIGN /
OPEN_THANDOR_CAMPAIGN_LEVEL (start level), OPEN_THANDOR_AUTOWIN (end a level as won after N seconds: the local
faction's mobile units are moved into the level's exit zone, then an end trigger that leads to the next level is
fired; the original code then plays the end movie, collects the units in the exit zone, loads the next level and
places them), OPEN_THANDOR_AUTOWIN_LEVELS, OPEN_THANDOR_WINDOWED / _MULTI_INSTANCE / _NET_PORT and the input
script (clickuntilnextlevel).

usage:
  run_campaign_chain.py GAME_DIR pairs     [--jobs 10] [--win-after 30] [--minutes 1] [--port-base 940]
  run_campaign_chain.py GAME_DIR campaigns [--jobs 4]  [--win-after 40] [--only tutorial,luke]
  run_campaign_chain.py GAME_DIR segments  [--jobs 12] [--win-after 120] [--only hansolo]

Worker k plays in its own linked copy GAME_DIR_w<k> with UDP port --port-base + k.

pairs:     for each level that needs the previous level's units (tutorial 2/3, hansolo 9/13/23) start early enough
           (tutorial 3 from tutorial 1), win the levels before it, then let the target level run --minutes. OK when the target level received units and
           did not end before the time was up.
campaigns: each campaign from its first level to the end, every level won after --win-after seconds, the last
           one too (campaign end). The path follows the end triggers and may skip levels (hansolo: 7, 15-17, 20).
           OK when the campaign end was fired without crash or hang; the run stops 40 s after it. Reports
           every level change (from, to, units carried over, of them the local faction's).
segments:  every level of every campaign's winning path, split into parallel runs (SEGMENTS): tutorial whole,
           nimm2 and luke in two parts each, hansolo in seven (2-4, 5-8, 8-10, 11-13, 14-19, 21-23, 24-26).
           No part starts at a level that needs the previous level's units
           (those are reached through a real level change inside a part). Every level is won after
           --win-after seconds; a part ending a campaign must fire the campaign end, the others must win their
           last level. Levels off the winning path (hansolo 7, 15-17, 20) are started by run_all_maps.py.
quick:     5 levels in 3 parts (QUICK_SEGMENTS): tutorial 1-2 and hansolo 22-23 (levels that take over the previous
           level's units) and nimm2 5 to the campaign end.
Results in GAME_DIR/chain/results.txt, screenshots and log per run in GAME_DIR/chain/<run>/.
"""
import argparse
import os
import queue
import re
import shutil
import subprocess
import threading
import time

# (campaign, first level, levels to play): the target is the last one; tutorial 2 itself needs tutorial 1's units
PAIRS = [('tutorial', 1, 2), ('tutorial', 1, 3), ('hansolo', 8, 2), ('hansolo', 12, 2), ('hansolo', 22, 2)]
# first playable level and level count of each campaign (hansolo level 1 is listed but its file is missing)
CAMPAIGNS = [('tutorial', 1, 3), ('hansolo', 2, 26), ('nimm2', 1, 5), ('luke', 1, 4)]
# (campaign, first level, levels played on the winning path, ends the campaign). Hansolo's winning path:
# 2 3 4 5 6 8 9 10 11 12 13 14 18 19 21 22 23 24 25 26; carried units are needed in 9, 13 and 23.
SEGMENTS = [('hansolo', 2, 3, False), ('hansolo', 5, 3, False), ('hansolo', 8, 3, False), ('hansolo', 11, 3, False),
            ('hansolo', 14, 3, False), ('hansolo', 21, 3, False), ('hansolo', 24, 3, True),
            ('nimm2', 1, 3, False), ('nimm2', 4, 2, True), ('luke', 1, 2, False), ('luke', 3, 2, True),
            ('tutorial', 1, 3, True)]
# quick: 5 levels in 3 parts - two levels that take over the previous level's units (tutorial 2, hansolo 23) through
# a real level change, and a campaign end (nimm2)
QUICK_SEGMENTS = [('tutorial', 1, 2, False), ('hansolo', 22, 2, False), ('nimm2', 5, 1, True)]
PRIVATE = ('thandor.exe', 'thandor.pdb', 'thandor.dat', 'thandor.log', 'crash.log', 'crash_raw.log', 'hang.log')

parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
parser.add_argument('game_dir')
parser.add_argument('mode', choices=('pairs', 'campaigns', 'segments', 'quick'))
parser.add_argument('--jobs', type=int, default=10)
parser.add_argument('--win-after', type=int, default=30)
parser.add_argument('--minutes', type=float, default=1)
parser.add_argument('--shots', default='5000')
parser.add_argument('--port-base', type=int, default=940, help='UDP port of worker 0 (worker k: + k)')
parser.add_argument('--only', help='comma-separated campaign names, e.g. tutorial,luke')
args = parser.parse_args()

game = os.path.abspath(args.game_dir)
out = os.path.join(game, 'chain')
os.makedirs(out, exist_ok=True)
results = os.path.join(out, 'results.txt')

# Mission page (1280x800): computer-opponent slider to "stark", "Beginnen"; G five times = highest game speed.
START = ("0 layout 1280 800\n6000 drag 785 496 900 496\n7000 drag 785 496 900 496\n"
         "10000 clickuntilingame 839 539 3000\n0 ingame\n")
SPEED = "2000 key 71\n2500 key 71\n3000 key 71\n3500 key 71\n4000 key 71\n"
# after a level change: the mission page of the next level shows the slider again
NEXT = "%d clickuntilnextlevel 839 539 3000\n6000 drag 785 496 900 496\n" + SPEED


def script_for(levels, win_after, last_run_ms):
    text = START + SPEED
    for _ in range(levels - 1):
        text += NEXT % ((win_after + 3) * 1000)
    return text + "%d quit\n" % last_run_ms


def make_worker_dir(k):
    target = '%s_w%d' % (game.rstrip('\\/'), k)
    os.makedirs(os.path.join(target, 'save'), exist_ok=True)
    for name in os.listdir(game):
        source, link = os.path.join(game, name), os.path.join(target, name)
        if name.lower() in PRIVATE:
            continue
        if os.path.isfile(source) and not os.path.exists(link):
            os.link(source, link)
        elif os.path.isdir(source) and name.lower() in ('flm', 'setup') and not os.path.exists(link):
            subprocess.run('mklink /J "%s" "%s"' % (link, source), shell=True, capture_output=True)
    for name in ('thandor.exe', 'thandor.pdb', 'thandor.dat'):
        if os.path.exists(os.path.join(game, name)):
            shutil.copy(os.path.join(game, name), os.path.join(target, name))
    return target


def mtime(path):
    return os.path.getmtime(path) if os.path.exists(path) else 0


if args.mode == 'pairs':
    # win the level before, then run the target level --minutes (no auto-win there)
    runs = [('%s %d->%d' % (c, l, l + n - 1), c, l, n, args.minutes * 60) for c, l, n in PAIRS]
elif args.mode in ('segments', 'quick'):
    # every level won after --win-after seconds; label: first level and number of levels
    runs = [('%s %d+%d%s' % (c, l, n, ' end' if end else ''), c, l, n, 0)
            for c, l, n, end in (SEGMENTS if args.mode == 'segments' else QUICK_SEGMENTS)]
else:
    # every level won after --win-after seconds; the last level too, so the campaign end is reached
    runs = [('%s %d-%d' % (c, first, last), c, first, last - first + 1, 0) for c, first, last in CAMPAIGNS]
if args.only:
    runs = [run for run in runs if run[1] in args.only.split(',')]

work = queue.Queue()
for number, run in enumerate(runs, 1):
    work.put((number, run))
lock = threading.Lock()


def run_one(k, folder, number, label, campaign, level, levels, final_seconds):
    exe, log = os.path.join(folder, 'thandor.exe'), os.path.join(folder, 'thandor.log')
    script = os.path.join(folder, 'chain_script.txt')
    last_ms = int(final_seconds * 1000) if final_seconds else (args.win_after + 20) * 1000
    with open(script, 'w') as f:
        f.write(script_for(levels, args.win_after, last_ms))
    env = dict(os.environ, OPEN_THANDOR_SCRIPT=script, OPEN_THANDOR_AUTOSHOT=args.shots, OPEN_THANDOR_WINDOWED='1',
               OPEN_THANDOR_WINDOW_X=str((k % 5) * 250), OPEN_THANDOR_WINDOW_Y=str((k // 5 % 2) * 350),
               OPEN_THANDOR_MULTI_INSTANCE='1', OPEN_THANDOR_NET_PORT=str(args.port_base + k),
               OPEN_THANDOR_CAMPAIGN=campaign, OPEN_THANDOR_CAMPAIGN_LEVEL=str(level),
               OPEN_THANDOR_AUTOWIN=str(args.win_after))
    if final_seconds:
        env['OPEN_THANDOR_AUTOWIN_LEVELS'] = str(levels - 1)
    skip = sum(1 for _ in open(log, errors='replace')) if os.path.exists(log) else 0
    crash_time, hang_time = mtime(os.path.join(folder, 'crash.log')), mtime(os.path.join(folder, 'hang.log'))
    shutil.rmtree(os.path.join(folder, 'shots'), ignore_errors=True)
    limit = 240 + levels * (args.win_after + 90) + final_seconds
    start = time.time()
    process = subprocess.Popen('"%s" -NOINTRO -KARTE="-"' % exe, executable=exe, cwd=folder, env=env)
    status = None
    campaign_end = None
    while status is None:
        time.sleep(2)
        if mtime(os.path.join(folder, 'crash.log')) > crash_time:
            status = 'CRASH'
        elif mtime(os.path.join(folder, 'hang.log')) > hang_time:
            status = 'HANG'
        elif process.poll() is not None:
            status = 'exited'
        elif time.time() - start > limit:
            status = 'TIMEOUT'
        elif campaign_end is None and not final_seconds and os.path.exists(log):
            # the path through a campaign may skip levels: stop 40 s after the campaign end was fired
            try:
                with open(log, errors='replace') as f:
                    if any('firing the campaign end' in line for line in list(f)[skip:]):
                        campaign_end = time.time()
            except OSError:
                pass  # the game holds the log open for a moment: read it on the next round
        elif campaign_end is not None and time.time() - campaign_end > 40:
            status = 'ended'
    if process.poll() is None:
        process.kill()
        process.wait()
    time.sleep(1)
    lines = []
    for _ in range(10):  # the killed game may still hold the log for a moment
        try:
            lines = list(open(log, errors='replace'))[skip:] if os.path.exists(log) else []
            break
        except OSError:
            time.sleep(1)
    target = os.path.join(out, '%02d_%s' % (number, re.sub(r'[^\w-]+', '_', label)))
    shutil.rmtree(target, ignore_errors=True)
    if os.path.isdir(os.path.join(folder, 'shots')):
        shutil.copytree(os.path.join(folder, 'shots'), target)
    else:
        os.makedirs(target)
    with open(os.path.join(target, 'thandor.log'), 'w') as f:
        f.writelines(lines)
    for name in ('crash.log', 'crash_raw.log', 'hang.log'):
        if status in ('CRASH', 'HANG') and os.path.exists(os.path.join(folder, name)):
            shutil.copy(os.path.join(folder, name), target)
    # per session: units carried in, how it ended
    sessions = []
    for line in lines:
        m = re.search(r'campaign carries over (\d+) units', line)
        if m:
            sessions.append({'carried': int(m.group(1)), 'own': 0, 'end': None, 'level': None})
        elif sessions and 'carried per faction' in line:
            m = re.search(r'local faction (\d+), carried per faction ([\d ]+)', line)
            if m:
                sessions[-1]['own'] = int(m.group(2).split()[int(m.group(1)) & 7])
        elif sessions and 'firing the campaign end' in line:
            sessions[-1]['final'] = True
        elif sessions and re.search(r'auto-win: level \d+, end selection', line):
            m = re.search(r'auto-win: level (\d+), end selection \d+ -> level (-?\d+).*?(\d+) mobile units', line)
            sessions[-1]['end'] = 'won->%s (%s moved)' % (m.group(2), m.group(3)) if m else 'won'
            sessions[-1]['level'] = m.group(1) if m else None
        elif sessions and 'level script: end trigger' in line and sessions[-1]['end'] is None:
            sessions[-1]['end'] = 'SCRIPT END'
    # every level start of a campaign logs its carry-over (also 0 units)
    started = len(sessions)
    if args.mode in ('segments', 'quick'):
        ends_campaign = label.endswith(' end')
        # a level counts as passed when the next level started (it may end by its own script before the auto-win)
        # or when the auto-win ended it
        passed = max(0, len(sessions) - 1) + (1 if sessions and (sessions[-1]['end'] or '').startswith('won') else 0)
        lost = [s['level'] or '?' for s in sessions if s['end'] == 'SCRIPT END']
        if ends_campaign:
            ok = status in ('exited', 'ended') and bool(sessions) and bool(sessions[-1].get('final')) and not lost
        else:
            ok = status in ('exited', 'ended') and passed >= levels and not lost
        verdict = ('ok' if ok else 'FAIL(%s)' % status) + ' %d levels' % min(started, levels)
    elif args.mode == 'pairs':
        ok = (status == 'exited' and len(sessions) >= levels and sessions[-1]['carried'] > 0 and
              sessions[-1]['end'] is None)
        verdict = 'ok' if ok else 'FAIL(%s)' % status
    else:
        ok = status in ('exited', 'ended') and bool(sessions) and bool(sessions[-1].get('final'))
        verdict = ('ok' if ok else 'FAIL(%s)' % status) + ' %d levels' % started
    detail = '; '.join('L%s carried %d (%d own) %s' % (s['level'] or '?', s['carried'], s['own'],
                                                        'campaign end' if s.get('final') else s['end'] or 'ran')
                       for s in sessions)
    with lock:
        line = '[%d/%d] %-18s %-18s %s' % (number, len(runs), label, verdict, detail)
        print(line, flush=True)
        with open(results, 'a') as f:
            f.write(time.strftime('%H:%M:%S ') + line + '\n')


def worker(k):
    folder = make_worker_dir(k)
    while True:
        try:
            number, (label, campaign, level, levels, final_seconds) = work.get_nowait()
        except queue.Empty:
            return
        run_one(k, folder, number, label, campaign, level, levels, final_seconds)


threads = [threading.Thread(target=worker, args=(k,)) for k in range(min(args.jobs, len(runs)))]
for thread in threads:
    thread.start()
    time.sleep(3)
for thread in threads:
    thread.join()
