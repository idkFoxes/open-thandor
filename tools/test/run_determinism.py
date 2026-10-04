"""Determinism test: plays generated test levels (tools/test/make_arena.py) for a fixed number of simulation steps
with a fixed random seed and compares the per-step state hashes (test aid OPEN_THANDOR_STATEHASH, see
include/thandor/platform/debug/statehash.h).

Scenarios (all run in parallel, each in its own linked copy of the game directory, windowed):
  battle      every unit and building of both sides; at step 100 player 1 drives down and player 2 up
              (20 rows), at step 500 all drive back (OPEN_THANDOR_ARENA_ORDERS=move)
  turrets     every armed static defence of both sides facing each other, with power plants
  production  factories, construction yard, labs and power plants of both sides; at step 20 both queue units
              and every lab starts a technology (OPEN_THANDOR_ARENA_ORDERS=production)

usage:
  run_determinism.py GAME_DIR [--scenarios battle,turrets,production] [--steps 1200] [--runs 2]
  run_determinism.py GAME_DIR --save-reference DIR     store one run per scenario as DIR/<scenario>.txt
  run_determinism.py GAME_DIR --reference DIR          every scenario must match its stored reference

Needs the test build (CMake preset "test") as GAME_DIR/thandor.exe with its SDL3.dll. With --runs 2 the runs of a scenario must
match each other (their frame rates differ, which must not change the result). On a mismatch the first differing
step is reported; rerun with --detail <tick> to get every army's values at that tick (statehash.txt in each copy).
"""
import argparse
import os
import shutil
import subprocess
import sys
import threading
import time

HERE = os.path.dirname(os.path.abspath(__file__))
PRIVATE = ('thandor.exe', 'thandor.pdb', 'thandor.dat', 'thandor.ini', 'thandor.log', 'crash.log',
           'crash_raw.log', 'hang.log',
           'statehash.txt')
# mission page (1280x800): "Beginnen", then wait until the level runs; the scenarios need no further input
SCRIPT = "0 layout 1280 800\n6000 clickuntilingame 839 539 3000\n0 ingame\n900000 quit\n"
ORDERS = {'battle': 'move', 'turrets': '', 'production': 'production'}


def make_copy(game, k):
    target = '%s_d%d' % (game.rstrip('\\/'), k)
    os.makedirs(os.path.join(target, 'save'), exist_ok=True)
    for name in os.listdir(game):
        source, link = os.path.join(game, name), os.path.join(target, name)
        if name.lower() in PRIVATE:
            continue
        if os.path.isfile(source) and not os.path.exists(link):
            os.link(source, link)
        elif os.path.isdir(source) and name.lower() in ('flm', 'setup', 'level') and not os.path.exists(link):
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


def run_once(folder, k, scenario, args, results):
    exe = os.path.join(folder, 'thandor.exe')
    script = os.path.join(folder, 'determinism_script.txt')
    open(script, 'w').write(SCRIPT)
    output = os.path.join(folder, 'statehash.txt')
    if os.path.exists(output):
        os.remove(output)
    env = dict(os.environ, OPEN_THANDOR_SCRIPT=script, OPEN_THANDOR_WINDOWED='1',
               OPEN_THANDOR_WINDOW_X=str((k % 4) * 320), OPEN_THANDOR_WINDOW_Y=str((k // 4) * 260),
               OPEN_THANDOR_MULTI_INSTANCE='1', OPEN_THANDOR_NET_PORT=str(960 + k),
               OPEN_THANDOR_STATEHASH=str(args.steps), OPEN_THANDOR_STATEHASH_SEED=str(args.seed))
    if ORDERS[scenario]:
        env['OPEN_THANDOR_ARENA_ORDERS'] = ORDERS[scenario]
    if args.detail:
        env['OPEN_THANDOR_STATEHASH_DETAIL'] = str(args.detail)
    process = subprocess.Popen('"%s" -NOINTRO -KARTE="test%s."' % (exe, scenario), executable=exe, cwd=folder,
                               env=env)
    try:
        process.wait(timeout=args.timeout)
    except subprocess.TimeoutExpired:
        process.kill()
    lines = open(output).read().splitlines() if os.path.exists(output) else []
    results[k] = [line for line in lines if not line.startswith(' ')]


def hashes(lines):
    return [(line.split()[0], line.split()[1]) for line in lines]


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('game_dir')
    parser.add_argument('--scenarios', default='battle,turrets,production')
    parser.add_argument('--steps', type=int, default=1200)
    parser.add_argument('--runs', type=int, default=2)
    parser.add_argument('--seed', type=int, default=12345)
    parser.add_argument('--detail', type=int, default=0)
    parser.add_argument('--timeout', type=int, default=900)
    parser.add_argument('--reference', help='directory with <scenario>.txt')
    parser.add_argument('--save-reference', help='directory to write <scenario>.txt into')
    args = parser.parse_args()
    game = os.path.abspath(args.game_dir)
    scenarios = args.scenarios.split(',')
    for scenario in scenarios:
        subprocess.run([sys.executable, os.path.join(HERE, 'make_arena.py'), game, '--scenario', scenario,
                        '--name', 'test' + scenario], check=True)
    runs = 1 if args.save_reference or args.reference else args.runs
    jobs = [(scenario, r) for scenario in scenarios for r in range(runs)]
    folders = [make_copy(game, k) for k in range(len(jobs))]
    results = {}
    threads = [threading.Thread(target=run_once, args=(folders[k], k, jobs[k][0], args, results))
               for k in range(len(jobs))]
    for thread in threads:
        thread.start()
        time.sleep(2)
    for thread in threads:
        thread.join()
    failed = False
    for scenario in scenarios:
        mine = [k for k in range(len(jobs)) if jobs[k][0] == scenario]
        broken = [k for k in mine if len(results.get(k, [])) < args.steps]
        if broken:
            print('%-11s FAIL: run did not reach %d steps (crash, hang or timeout?): %s'
                  % (scenario, args.steps, ', '.join('%d recorded' % len(results.get(k, []))
                                                      for k in broken)))
            failed = True
            continue
        last = results[mine[0]][-1]
        if args.save_reference:
            os.makedirs(args.save_reference, exist_ok=True)
            open(os.path.join(args.save_reference, scenario + '.txt'), 'w').write('\n'.join(results[mine[0]]) + '\n')
            print('%-11s reference saved (%d steps, last %s)' % (scenario, args.steps, last))
            continue
        if args.reference:
            expected = hashes(open(os.path.join(args.reference, scenario + '.txt')).read().splitlines())
            others = mine
        else:
            expected = hashes(results[mine[0]])
            others = mine[1:]
        # compare by simulation tick: under load the first recorded tick can differ by one between runs
        mismatch = None
        expected_by_tick = dict(expected)
        for k in others:
            for tick, value in hashes(results[k]):
                if tick in expected_by_tick and expected_by_tick[tick] != value:
                    mismatch = tick
                    break
            if mismatch:
                break
        if mismatch:
            print('%-11s MISMATCH at tick %s (rerun with --detail %s)' % (scenario, mismatch, mismatch))
            failed = True
        else:
            print('%-11s ok: %d steps identical (%s), last %s' % (scenario, args.steps,
                  'reference' if args.reference else '%d runs' % runs, last))
    return 1 if failed else 0


if __name__ == '__main__':
    sys.exit(main())
