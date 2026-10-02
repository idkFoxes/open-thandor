"""Runs the game unattended and collects what happened.

usage: run_game.py GAME_DIR SECONDS [--args "..."] [--script FILE] [--shots MS]

Starts GAME_DIR/thandor.exe with the exact argument string (quotes are passed through, so
-KARTE="mittelpunkt" works), optional OPEN_THANDOR_SCRIPT input replay and OPEN_THANDOR_AUTOSHOT
snapshots, stops it after SECONDS, then prints the new thandor.log lines, reports whether crash.log
changed and writes GAME_DIR/shots/sheet.png with all snapshots as thumbnails.

Example (start a skirmish on Ahaggar and play 2 minutes):
  run_game.py ../ot-run 120 --args '-NOINTRO -KARTE="mittelpunkt"' --script tools/test/skirmish_start.txt"""
import argparse
import glob
import os
import shutil
import subprocess

parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
parser.add_argument('game_dir')
parser.add_argument('seconds', type=float)
parser.add_argument('--args', default='-NOINTRO')
parser.add_argument('--script')
parser.add_argument('--shots', default='5000')
args = parser.parse_args()

game = os.path.abspath(args.game_dir)
exe = os.path.join(game, 'thandor.exe')
log = os.path.join(game, 'thandor.log')
crash = os.path.join(game, 'crash.log')
log_lines = sum(1 for _ in open(log, errors='replace')) if os.path.exists(log) else 0
crash_time = os.path.getmtime(crash) if os.path.exists(crash) else 0
shutil.rmtree(os.path.join(game, 'shots'), ignore_errors=True)
env = dict(os.environ, OPEN_THANDOR_AUTOSHOT=args.shots)
if args.script:
    env['OPEN_THANDOR_SCRIPT'] = os.path.abspath(args.script)
process = subprocess.Popen('"%s" %s' % (exe, args.args), executable=exe, cwd=game, env=env)
try:
    process.wait(timeout=args.seconds)
except subprocess.TimeoutExpired:
    pass
# stop only the process started here (and its children); other game instances keep running
subprocess.run('taskkill /F /T /PID %d' % process.pid, shell=True, capture_output=True)
process.wait()

if os.path.exists(log):
    for line in list(open(log, errors='replace'))[log_lines:]:
        if not line.startswith('   ') and 'autoshot' not in line:
            print(line.rstrip())
crashed = os.path.exists(crash) and os.path.getmtime(crash) > crash_time
print('CRASH: see crash.log / crash_raw.log' if crashed else 'no crash')
files = sorted(glob.glob(os.path.join(game, 'shots', '*.bmp')))
if files:
    from PIL import Image
    thumbs = [Image.open(f).convert('RGB').resize((320, 180)) for f in files]
    sheet = Image.new('RGB', (320 * 4, 180 * ((len(thumbs) + 3) // 4)))
    for i, thumb in enumerate(thumbs):
        sheet.paste(thumb, ((i % 4) * 320, (i // 4) * 180))
    sheet.save(os.path.join(game, 'shots', 'sheet.png'))
    print('%d snapshots -> shots/sheet.png' % len(files))
