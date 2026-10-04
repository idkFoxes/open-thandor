"""Plays a stock map with strong computer opponents under the state hash (fixed seed, game speed 5 from the first
step via OPEN_THANDOR_STATEHASH_SPEED) in several game dirs at once and compares the per-step hashes. This covers
the computer opponents, which the arena scenarios of run_determinism.py do not. Each dir needs the test build as
thandor.exe (with its SDL3.dll); put an older build into one of them to compare two versions (the first dir is the reference).

usage: compare_map_hash.py MAP STEPS DIR [DIR...]    e.g. compare_map_hash.py stromschnelle 1400 ..\\ot-soak ..\\ot-old
A run that ends earlier (a side lost) records fewer steps; the steps all runs recorded are compared, and an
incomplete last line (the game is stopped while writing) is ignored. A run is stopped once its hash file has not
grown for a minute (all steps done or the level ended), at the latest after 30 minutes.
"""
import os
import subprocess
import sys
import threading
import time

MAP, STEPS, DIRS = sys.argv[1], int(sys.argv[2]), [os.path.abspath(d) for d in sys.argv[3:]]
SCRIPT = "0 layout 1280 800\n6000 drag 785 496 900 496\n7000 drag 785 496 900 496\n10000 clickuntilingame 839 539 3000\n0 ingame\n1800000 quit\n"
results = {}


def run(k, folder):
    script = os.path.join(folder, 'ai_hash_script.txt')
    open(script, 'w').write(SCRIPT)
    out = os.path.join(folder, 'statehash.txt')
    if os.path.exists(out):
        os.remove(out)
    env = dict(os.environ, OPEN_THANDOR_SCRIPT=script, OPEN_THANDOR_WINDOWED='1', OPEN_THANDOR_WINDOW_X=str(k * 330),
               OPEN_THANDOR_MULTI_INSTANCE='1', OPEN_THANDOR_NET_PORT=str(980 + k),
               OPEN_THANDOR_STATEHASH=str(STEPS), OPEN_THANDOR_STATEHASH_SEED='12345', OPEN_THANDOR_STATEHASH_SPEED='5')
    exe = os.path.join(folder, 'thandor.exe')
    p = subprocess.Popen('"%s" -NOINTRO -KARTE="%s"' % (exe, MAP), executable=exe, cwd=folder, env=env)
    # stop the run when it has all steps, or when the hash file has not grown for a minute (the level ended)
    lastSize, lastChange, started = -1, time.time(), time.time()
    while p.poll() is None and time.time() - started < 1800:
        time.sleep(5)
        size = os.path.getsize(out) if os.path.exists(out) else 0
        if size != lastSize:
            lastSize, lastChange = size, time.time()
        elif size > 0 and time.time() - lastChange > 60:
            break
    if p.poll() is None:
        p.kill()
    lines = open(out).read().splitlines() if os.path.exists(out) else []
    results[k] = [l.split() for l in lines if not l.startswith(' ') and len(l.split()) >= 8]


threads = [threading.Thread(target=run, args=(k, d)) for k, d in enumerate(DIRS)]
for t in threads:
    t.start()
for t in threads:
    t.join()
for k, d in enumerate(DIRS):
    print(d, len(results[k]), 'steps, last', results[k][-1] if results[k] else None)
# compare by simulation tick (the first column): under load the first recorded tick can differ by one
base = {line[0]: line for line in results[0]}
for k in range(1, len(DIRS)):
    first = next((line[0] for line in results[k] if line[0] in base and base[line[0]] != line), None)
    print('dir %d vs dir 0: %s' % (k, 'identical' if first is None else 'first difference at tick ' + first))
