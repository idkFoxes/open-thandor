"""Determinism test: plays the generated test arena (tools/test/make_arena.py) for a fixed number of simulation
steps with a fixed random seed and compares the per-step state hashes (test aid OPEN_THANDOR_STATEHASH, see
include/thandor/platform/debug/statehash.h).

usage:
  run_determinism.py GAME_DIR [--steps 600] [--runs 2] [--seed 12345]      two runs must match each other
  run_determinism.py GAME_DIR --save-reference FILE                         store the hashes of one run
  run_determinism.py GAME_DIR --reference FILE                              a run must match a stored reference

Needs the test build (CMake preset "test") as GAME_DIR/thandor.exe. Runs go in parallel in linked copies
GAME_DIR_d<k> (windowed, one instance each), so their frame rates differ - which must not change the result.
On a mismatch the first differing step is reported; rerun with --detail <tick> to get every army's values at that
tick in both runs (statehash.txt next to each copy)."""
import argparse
import os
import shutil
import subprocess
import sys
import threading
import time

HERE = os.path.dirname(os.path.abspath(__file__))
PRIVATE = ('thandor.exe', 'thandor.pdb', 'thandor.dat', 'thandor.log', 'crash.log', 'crash_raw.log', 'hang.log',
           'statehash.txt')
# mission page (1280x800): "Beginnen", then wait until the level runs; the arena needs no further input
SCRIPT = "0 layout 1280 800\n6000 clickuntilingame 839 539 3000\n0 ingame\n600000 quit\n"


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
    for name in ('thandor.exe', 'thandor.pdb', 'thandor.dat'):
        if os.path.exists(os.path.join(game, name)):
            shutil.copy(os.path.join(game, name), os.path.join(target, name))
    return target


def run_once(folder, k, args, results):
    exe = os.path.join(folder, 'thandor.exe')
    script = os.path.join(folder, 'determinism_script.txt')
    open(script, 'w').write(SCRIPT)
    output = os.path.join(folder, 'statehash.txt')
    if os.path.exists(output):
        os.remove(output)
    env = dict(os.environ, OPEN_THANDOR_SCRIPT=script, OPEN_THANDOR_WINDOWED='1',
               OPEN_THANDOR_WINDOW_X=str(k * 400), OPEN_THANDOR_WINDOW_Y='0', OPEN_THANDOR_MULTI_INSTANCE='1',
               OPEN_THANDOR_NET_PORT=str(960 + k), OPEN_THANDOR_STATEHASH=str(args.steps),
               OPEN_THANDOR_STATEHASH_SEED=str(args.seed))
    if args.detail:
        env['OPEN_THANDOR_STATEHASH_DETAIL'] = str(args.detail)
    process = subprocess.Popen('"%s" -NOINTRO -KARTE="%s."' % (exe, args.name), executable=exe, cwd=folder, env=env)
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
    parser.add_argument('--steps', type=int, default=600)
    parser.add_argument('--runs', type=int, default=2)
    parser.add_argument('--seed', type=int, default=12345)
    parser.add_argument('--name', default='testarena')
    parser.add_argument('--detail', type=int, default=0)
    parser.add_argument('--timeout', type=int, default=600)
    parser.add_argument('--reference')
    parser.add_argument('--save-reference')
    args = parser.parse_args()
    game = os.path.abspath(args.game_dir)
    subprocess.run([sys.executable, os.path.join(HERE, 'make_arena.py'), game, '--name', args.name], check=True)
    runs = 1 if args.save_reference or args.reference else args.runs
    folders = [make_copy(game, k) for k in range(runs)]
    results = {}
    threads = [threading.Thread(target=run_once, args=(f, k, args, results)) for k, f in enumerate(folders)]
    for thread in threads:
        thread.start()
        time.sleep(2)
    for thread in threads:
        thread.join()
    for k in range(runs):
        print('run %d: %d steps recorded%s' % (k, len(results[k]),
                                                ', last ' + results[k][-1] if results[k] else ''))
        if len(results[k]) < args.steps:
            print('run %d did not reach %d steps (crash, hang or timeout?)' % (k, args.steps))
            return 1
    if args.save_reference:
        open(args.save_reference, 'w').write('\n'.join(results[0]) + '\n')
        print('reference saved:', args.save_reference)
        return 0
    expected = hashes(open(args.reference).read().splitlines()) if args.reference else hashes(results[0])
    for k in range(0 if args.reference else 1, runs):
        got = hashes(results[k])
        for (tick_a, hash_a), (tick_b, hash_b) in zip(expected, got):
            if (tick_a, hash_a) != (tick_b, hash_b):
                print('MISMATCH run %d at tick %s (expected %s %s, got %s %s)' % (k, tick_b, tick_a, hash_a,
                                                                                  tick_b, hash_b))
                print('rerun with --detail %s to compare every army at that tick' % tick_b)
                return 1
    print('deterministic: %d steps identical%s' % (len(expected), ' to the reference' if args.reference else
                                                   ' in %d runs' % runs))
    return 0


if __name__ == '__main__':
    sys.exit(main())
