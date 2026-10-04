"""Runs one scripted game in the GPU compare mode and summarises the comparisons of the software and GPU frames.

usage: gpu_compare.py GAME_DIR [--exe TEST_EXE] [--api vulkan|d3d12] [--script FILE] [--args "..."]
                      [--interval MS] [--timeout S] [--out DIR]

Makes a linked copy GAME_DIR_chk_gpucmp_<api> (run_checks.make_copy) of the test build (default:
build-mingw-test/thandor.exe, else build-test/thandor.exe; the developer tools are needed) and runs the script
(default tools/test/skirmish_pause.txt with -NOINTRO -KARTE="mittelpunkt" and the fixed seed and pause tick of the
pixel check) with OPEN_THANDOR_GPU=compare-<api> and OPEN_THANDOR_GPU_COMPARE_MS=MS (default 2000), minimized
unless OPEN_THANDOR_TEST_VISIBLE=1. The game then draws every frame both in software and on the GPU and every MS
milliseconds writes shots/gpucmp_NNNN_sw.bmp, _gpu.bmp, _diff.bmp and a "SDL_GPU compare" line in thandor.log with
the statistics of the whole frame and of the UI alone (outside the 3D scene rectangles) and PASS or FAIL (UI mean
channel difference < 0.5 and UI pixels differing by more than 8 under 0.1 %). The log, the compare pictures and
the summary go to --out (default GAME_DIR_gpucmp_<api>); the copy is removed afterwards.
Exit status 1 when a comparison failed, none was made or the game crashed."""
import argparse
import glob
import os
import re
import shutil
import subprocess
import sys

import run_checks

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
LINE = re.compile(r'SDL_GPU compare (\d+): (\d+)x(\d+), (\d+) 3D scenes; frame: mean ([\d.]+), max (\d+), > 8: '
                  r'([\d.]+)%; UI only \((\d+) pixels\): mean ([\d.]+), max (\d+), > 8: ([\d.]+)% -> (PASS|FAIL)')
PORTS = {'vulkan': 990, 'd3d12': 991}


def remove_copy(folder):
    """Removes a game copy: the junctions first (so their targets stay), then the folder."""
    for entry in run_checks.SHARED_DIRS:
        for name in os.listdir(folder):
            path = os.path.join(folder, name)
            if name.lower() == entry and os.path.isjunction(path):
                subprocess.run('cmd /c rmdir "%s"' % path, shell=True, capture_output=True)
    if any(os.path.isjunction(os.path.join(folder, name)) for name in os.listdir(folder)):
        print('copy %s still has junctions, kept' % folder)
        return
    shutil.rmtree(folder, ignore_errors=True)


def main():
    default_exe = os.path.join(REPO, 'build-mingw-test', 'thandor.exe')
    if not os.path.exists(default_exe):
        default_exe = os.path.join(REPO, 'build-test', 'thandor.exe')
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('game_dir')
    parser.add_argument('--exe', default=default_exe, help='test build (default: %(default)s)')
    parser.add_argument('--api', choices=sorted(PORTS), default='vulkan')
    parser.add_argument('--script', default=os.path.join(HERE, 'skirmish_pause.txt'))
    parser.add_argument('--args', default='-NOINTRO -KARTE="mittelpunkt"')
    parser.add_argument('--interval', type=int, default=2000, help='OPEN_THANDOR_GPU_COMPARE_MS (default 2000)')
    parser.add_argument('--timeout', type=int, default=240)
    parser.add_argument('--out')
    args = parser.parse_args()

    run_checks.game = os.path.abspath(args.game_dir)
    out = os.path.abspath(args.out or '%s_gpucmp_%s' % (run_checks.game.rstrip('\\/'), args.api))
    folder = run_checks.make_copy('gpucmp_%s' % args.api, os.path.abspath(args.exe))
    shutil.rmtree(os.path.join(folder, 'shots'), ignore_errors=True)
    env = {'OPEN_THANDOR_GPU': 'compare-%s' % args.api, 'OPEN_THANDOR_GPU_COMPARE_MS': str(args.interval),
           'OPEN_THANDOR_SCRIPT': os.path.abspath(args.script),
           # the pixel check's paused skirmish: fixed seed, paused at a fixed simulation tick
           'OPEN_THANDOR_STATEHASH': '100000', 'OPEN_THANDOR_STATEHASH_SEED': '12345',
           'OPEN_THANDOR_STATEHASH_PAUSE_AT': '150'}
    try:
        exited, crashed, log = run_checks.run_game(folder, args.args, env, args.timeout, PORTS[args.api])
        shutil.rmtree(out, ignore_errors=True)
        os.makedirs(out)
        for picture in glob.glob(os.path.join(folder, 'shots', 'gpucmp_*.bmp')):
            shutil.copy(picture, out)
        with open(os.path.join(out, 'thandor.log'), 'w', encoding='utf-8', errors='replace') as f:
            f.write(log)
    finally:
        remove_copy(folder)

    device = [l for l in log.splitlines() if 'SDL_GPU renderer: compare mode' in l]
    rows = [m.groups() for m in (LINE.search(l) for l in log.splitlines()) if m]
    summary = ['gpu_compare %s: %s' % (args.api, device[0].split('SDL_GPU renderer: ', 1)[1] if device
                                       else 'compare mode did not start'),
               'exited by itself: %s, crash/hang log: %s' % (exited, crashed),
               ' nr   size       scenes | frame mean   max  >8 %  | UI mean   max  >8 %  | verdict']
    for number, width, height, scenes, mean, largest, over8, _, ui_mean, ui_largest, ui_over8, verdict in rows:
        summary.append('%4s  %4sx%-4s  %3s    | %8s %5s %7s | %7s %5s %7s | %s' % (
            number, width, height, scenes, mean, largest, over8, ui_mean, ui_largest, ui_over8, verdict))
    failed = [r for r in rows if r[-1] == 'FAIL']
    if rows:
        summary.append('%d comparisons, %d PASS, %d FAIL; worst UI mean %.3f, worst UI > 8 %.3f %%; worst frame '
                       'mean %.3f, worst frame > 8 %.3f %%' % (
                           len(rows), len(rows) - len(failed), len(failed), max(float(r[8]) for r in rows),
                           max(float(r[10]) for r in rows), max(float(r[4]) for r in rows),
                           max(float(r[6]) for r in rows)))
    ok = bool(rows) and not failed and bool(device) and not crashed
    summary.append('result: %s (pictures and log in %s)' % ('PASS' if ok else 'FAIL', out))
    with open(os.path.join(out, 'summary.txt'), 'w', encoding='utf-8') as f:
        f.write('\n'.join(summary) + '\n')
    print('\n'.join(summary))
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main())
