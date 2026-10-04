"""Local two-instance network test.

Needs the test build: configure with THANDOR_DEV_TOOLS=ON (CMake preset "test", or
-DTHANDOR_DEV_TOOLS=ON) and copy its thandor.exe and SDL3.dll into GAME_DIR. The default build
does not contain the test aids used below (they would be ignored and the second instance would not start).

usage: run_multiplayer.py GAME_DIR SECONDS [--host-script FILE] [--client-script FILE] [--shots MS]

Starts a host (-HOST) in GAME_DIR and a client (-CLIENT="127.0.0.1") in GAME_DIR + "2" on the same machine,
using the test aids OPEN_THANDOR_MULTI_INSTANCE=1 (second instance allowed) and OPEN_THANDOR_NET_PORT=930
(client socket on another port; it still talks to the host's game port 929). Both run windowed side by side
(OPEN_THANDOR_WINDOWED=1, host at x=0, client at x=1290), so both keep drawing and both produce screenshots.
GAME_DIR + "2" is created on first use with hard links to every file of GAME_DIR (no extra disk space) and
junctions for its folders.
After SECONDS both are stopped; the new log lines of both and a contact sheet per instance are printed/saved.
"""
import argparse
import glob
import os
import shutil
import subprocess
import time

parser = argparse.ArgumentParser()
parser.add_argument('game_dir')
parser.add_argument('seconds', type=float)
parser.add_argument('--host-script')
parser.add_argument('--client-script')
parser.add_argument('--shots', default='4000')
parser.add_argument('--client-delay', type=float, default=6, help='seconds between host and client start')
args = parser.parse_args()

host_dir = os.path.abspath(args.game_dir)
client_dir = host_dir.rstrip('\\/') + '2'


def link_tree():
    os.makedirs(client_dir, exist_ok=True)
    for name in os.listdir(host_dir):
        if name.lower() in ('shots', 'thandor.log', 'crash.log', 'crash_raw.log'):
            continue
        source = os.path.join(host_dir, name)
        target = os.path.join(client_dir, name)
        if os.path.isdir(source):
            if not os.path.exists(target):
                subprocess.run(['cmd', '/c', 'mklink', '/J', target, source], capture_output=True)
        else:
            if os.path.exists(target):
                os.remove(target)
            if name.lower() in ('thandor.dat', 'thandor.ini'):
                shutil.copy(source, target)  # the game writes thandor.ini: no link into the host's copy
            else:
                os.link(source, target)


def log_lines(directory):
    path = os.path.join(directory, 'thandor.log')
    return sum(1 for _ in open(path, errors='replace')) if os.path.exists(path) else 0


def start(directory, arguments, extra_env, script):
    shutil.rmtree(os.path.join(directory, 'shots'), ignore_errors=True)
    env = dict(os.environ, OPEN_THANDOR_AUTOSHOT=args.shots, OPEN_THANDOR_MULTI_INSTANCE='1',
               OPEN_THANDOR_NETLOG='1', OPEN_THANDOR_WINDOWED='1', OPEN_THANDOR_WINDOW_Y='0', **extra_env)
    if script:
        env['OPEN_THANDOR_SCRIPT'] = os.path.abspath(script)
    exe = os.path.join(directory, 'thandor.exe')
    return subprocess.Popen('"%s" %s' % (exe, arguments), executable=exe, cwd=directory, env=env)


def report(directory, first_line, label):
    path = os.path.join(directory, 'thandor.log')
    if os.path.exists(path):
        for line in list(open(path, errors='replace'))[first_line:]:
            if not line.startswith('   ') and 'autoshot' not in line:
                print('[%s] %s' % (label, line.rstrip()))
    files = sorted(glob.glob(os.path.join(directory, 'shots', '*.bmp')))
    if files:
        from PIL import Image
        thumbs = [Image.open(f).convert('RGB').resize((320, 180)) for f in files]
        sheet = Image.new('RGB', (320 * 4, 180 * ((len(thumbs) + 3) // 4)))
        for i, thumb in enumerate(thumbs):
            sheet.paste(thumb, ((i % 4) * 320, (i // 4) * 180))
        sheet.save(os.path.join(directory, 'shots', 'sheet.png'))
        print('[%s] %d snapshots -> %s' % (label, len(files), os.path.join(directory, 'shots', 'sheet.png')))


link_tree()
host_first = log_lines(host_dir)
client_first = log_lines(client_dir)
# A negative --client-delay starts the client first.
HOST_ENV = {'OPEN_THANDOR_WINDOW_X': '0'}
CLIENT_ENV = {'OPEN_THANDOR_NET_PORT': '930', 'OPEN_THANDOR_WINDOW_X': '1290'}
if args.client_delay >= 0:
    host = start(host_dir, '-NOINTRO -NAME="Host" -HOST', HOST_ENV, args.host_script)
    time.sleep(args.client_delay)
    client = start(client_dir, '-NOINTRO -NAME="Client" -CLIENT="127.0.0.1"', CLIENT_ENV,
                   args.client_script)
else:
    client = start(client_dir, '-NOINTRO -NAME="Client" -CLIENT="127.0.0.1"', CLIENT_ENV,
                   args.client_script)
    time.sleep(-args.client_delay)
    host = start(host_dir, '-NOINTRO -NAME="Host" -HOST', HOST_ENV, args.host_script)
time.sleep(args.seconds)
# stop only the two processes started here (and their children); other game instances keep running
for process in (host, client):
    subprocess.run('taskkill /F /T /PID %d' % process.pid, shell=True, capture_output=True)
    process.wait()
report(host_dir, host_first, 'host')
report(client_dir, client_first, 'client')
