"""Campaign map test (docs/plans/campaign_map.md section 10, CT1-CT8): campaign progress, starts from the map,
carry-over snapshots, broken progress files, retry, the network rule and the campaign tab's pixels.

usage: run_campaign_map.py GAME_DIR [--new TEST_EXE] [--cases CT1,CT2,...] [--jobs 6] [--games 8]
                           [--port-base 870] [--save-golden]

Needs the test build (CMake preset "mingw-test" or "test": the developer tools). Every game runs in a linked copy
GAME_DIR_cmap_<k> (hard links to the data files, junctions for flm/setup/level, own executable, settings, log and
save folder; never the player's save\\): its progress file is GAME_DIR_cmap_<k>\\progress\\kampagne.dat, given
by OPEN_THANDOR_CAMPAIGN_PROGRESS, so no run touches save\\kampagne.dat. Worker k uses UDP port --port-base + k;
CT7 starts tools/test/run_multiplayer.py (host port 929, client 930: not at the same time as another network
test). Results in GAME_DIR/campaignmap/results.txt, per run its log, progress files and screenshots in
GAME_DIR/campaignmap/<case>/.

The progress files the cases start from are made here (tools/data/campaign_progress.py encode_progress, with the
fingerprint of the campaign's .cgn from LEVEL.PCK) or by a chain run of the real game. Test aids:
OPEN_THANDOR_CAMPAIGN (+ _LEVEL, _MISSION), OPEN_THANDOR_AUTOWIN (+ _LEVELS, _SELECTION), OPEN_THANDOR_AUTOLOSE,
OPEN_THANDOR_CAMPAIGN_PROGRESS (+ _DUMP), OPEN_THANDOR_CAMPAIGN_LEVEL_INFO_DUMP, OPEN_THANDOR_STATEHASH and the
input script (docs/BUILDING.md).

  CT1  chain: tutorial 1-3 from an empty progress, every level won (auto-win, selection 1). The file must hold
       done 1/2/3 with selection 1 and the snapshots "carry 2 from 1" and "carry 3 from 2" with the unit counts
       the chain logged at the starts of levels 2 and 3 ("campaign carries over N units").
  CT2  start a carry target directly: a chain (tutorial 1-3, and hansolo 22-23) wins the levels before the
       target after 15 s and records the target's first 600 simulation steps (state hash, fixed seed); then the
       target (tutorial 3, hansolo 23) is started from that chain's progress through the map's start path
       (OPEN_THANDOR_CAMPAIGN_MISSION). The carried unit count, the per-faction line and the state hash of every
       step up to 600 must equal the chain's, and the level must not end (no end trigger).
  CT3  broken progress files (truncated, wrong magic, payload version 2, CRC32 mismatch, a snapshot with 600 units,
       faction 9, duplicate sections, binary garbage, empty, 2 MB): the campaign tab opens without crash, the file
       is renamed to kampagne.bad with one log line, kampagne.dat is gone. Valid files with a bad snapshot: a
       snapshot whose source does not lead to its target keeps Start disabled (a click on the target and Start
       starts nothing); a snapshot with a unit outside the map and one with asset id 0xFFFF (a position of the
       chain's own snapshot) start without crash (outside: "lies outside the level", nothing restored).
  CT4  replay keeps progress: hansolo done up to m9 (ids 2-10, carry 9 from 8); m4 (id 5) started from the map
       by script clicks and won with selection 1: the wins of m5-m9 and the snapshot entry are byte-identical.
  CT5  branch unlock: hansolo m4 (id 5) won with selection 1 only, m6 (id 7) locked; m4 replayed from the map
       with OPEN_THANDOR_AUTOWIN_SELECTION=2: level 5 then holds selections 1 and 2 (last 2), m6 (id 7) starts.
  CT6  retry and restart record nothing: a chain hansolo 8-9 makes the snapshot of m8 (id 9); m8 is started
       from the map, lost (OPEN_THANDOR_AUTOLOSE), the retry runs, then the in-game "Mission neu starten": all
       three sessions get the same carried units and the progress file stays byte-identical.
  CT7  network: run_multiplayer.py with mp_host_campaign.txt (host: Kampagnen, Tutorial, mission 2 - a carry
       target, Start disabled in a network game -, then Kyrrdon mission 3, Start) and mp_client_join.txt; both
       logs must load nimm2 mission 3 (kometenhagel) and nothing of the tutorial.
  CT8  pixels: choose_campaign.txt (Kampagnen, Die Invasion, mission 9) with a fixed progress file, software
       renderer; the panel (the page's 640x360 rectangle) must equal tools/test/campaign_map_reference/
       choose_campaign.png (--save-golden writes it).
  info level info dump (OPEN_THANDOR_CAMPAIGN_LEVEL_INFO_DUMP) against "tools/data/cgn.py dump <level dir>
       --triggers" on the extracted LEVEL.PCK: per level file existence, the end triggers and the win mask.
Exit status 1 when a case failed.
"""
import argparse
import glob
import os
import random
import re
import shutil
import struct
import subprocess
import sys
import threading
import time

from game_env import game_env

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, os.path.join(REPO, 'tools', 'data'))
import campaign_progress  # noqa: E402
import pck  # noqa: E402

PRIVATE = ('thandor.exe', 'thandor.pdb', 'thandor.sym', 'sdl3.dll', 'thandor.dat', 'thandor.ini', 'thandor.log',
           'crash.log', 'crash_raw.log', 'hang.log', 'statehash.txt')
SHARED_DIRS = ('flm', 'setup', 'level')
CASES = ['CT1', 'CT2', 'CT3', 'CT4', 'CT5', 'CT6', 'CT7', 'CT8', 'info']
GOLDEN = os.path.join(HERE, 'campaign_map_reference', 'choose_campaign.png')
PANEL = (320, 220, 960, 580)  # the scenario page at 1280x800 (thandor.dat of the stock game)

# Scripts (layout 1280x800, the saved display mode; script format: src/platform/debug/script.cpp). The choose-game
# page: "Kampagnen" 392,364; campaign buttons Tutorial 517,272, Die Invasion 644,272, Kyrrdon 772,272, Piraten
# 889,272; "Starten" (and on the mission page "Beginnen") 839,539. Map nodes (mission number -> centre).
TAB = {'tutorial': (517, 272), 'hansolo': (644, 272), 'nimm2': (772, 272), 'luke': (889, 272)}
NODES = {'tutorial': {1: (616, 367), 2: (702, 367), 3: (788, 367)},
         'hansolo': {1: (491, 327), 2: (535, 327), 3: (578, 327), 4: (620, 327), 5: (663, 309), 6: (663, 346),
                     7: (706, 327), 8: (749, 327), 9: (792, 327), 10: (835, 327), 11: (878, 327)}}
START = (839, 539)


def script_open_campaign(campaign, mission, start=True):
    """choose-game page -> Kampagnen -> campaign button -> map node of mission [-> Starten, Beginnen, in game]."""
    x, y = NODES[campaign][mission]
    lines = ['0 layout 1280 800', '7000 move 392 364', '7500 click 392 364', '9000 move %d %d' % TAB[campaign],
             '9500 click %d %d' % TAB[campaign], '11000 move %d %d' % (x, y), '11500 click %d %d' % (x, y),
             '13000 shot']
    if start:
        lines += ['13500 move %d %d' % START, '14000 click %d %d' % START,
                  '17000 clickuntilingame %d %d 3000' % START, '0 ingame']
    return lines


def script_chain(levels, win_after, last_ms, campaign_end=False):
    """A campaign started by OPEN_THANDOR_CAMPAIGN: "Beginnen" on the mission page, then for every further level
    clicks until it runs (end movie, the results page's continue button, mission page); quit last_ms after the
    last level started. campaign_end: the last level is won too; clicks on until the results page of the campaign
    end is closed (the caller stops the game). No speed keys and no slider: the runs that compare state hashes
    must get the same input."""
    lines = ['0 layout 1280 800', '5000 clickuntilingame %d %d 3000' % START, '0 ingame']
    for _ in range(levels - (0 if campaign_end else 1)):
        lines.append('%d clickuntilnextlevel %d %d 3000' % ((win_after + 3) * 1000, START[0], START[1]))
    return lines + ['%d quit' % last_ms]


args = None
game = None
out_root = None
level_dir = None
print_lock = threading.Lock()
port_lock = threading.Lock()
next_worker = [0]
game_slots = threading.Semaphore(8)  # games at once (--games)


def say(text):
    with print_lock:
        print(text, flush=True)


# ------------------------------------------------------------------------------------------------- game copies

def make_copy(index):
    """GAME_DIR_cmap_<index> (index: number or name): hard links to the data files, junctions for the data
    folders, own executable, settings, empty log, save and progress folders."""
    target = '%s_cmap_%s' % (game.rstrip('\\/'), index)
    os.makedirs(target, exist_ok=True)
    for entry in os.listdir(game):
        source, link = os.path.join(game, entry), os.path.join(target, entry)
        if entry.lower() in PRIVATE:
            continue
        if os.path.isfile(source) and not os.path.exists(link):
            os.link(source, link)
        elif os.path.isdir(source) and entry.lower() in SHARED_DIRS and not os.path.exists(link):
            subprocess.run('mklink /J "%s" "%s"' % (link, source), shell=True, capture_output=True)
    for entry in ('save', 'shots', 'progress'):
        path = os.path.join(target, entry)
        if os.path.isdir(path) and not os.path.isjunction(path):
            shutil.rmtree(path)
    os.makedirs(os.path.join(target, 'save'))
    os.makedirs(os.path.join(target, 'progress'))
    for entry in PRIVATE:
        if os.path.exists(os.path.join(target, entry)):
            os.remove(os.path.join(target, entry))
    shutil.copy(args.new, os.path.join(target, 'thandor.exe'))
    for extension in ('.pdb', '.sym'):
        symbols = os.path.splitext(args.new)[0] + extension
        if os.path.exists(symbols):
            shutil.copy(symbols, os.path.join(target, 'thandor' + extension))
    for dll in (os.path.join(os.path.dirname(args.new), 'SDL3.dll'), os.path.join(game, 'SDL3.dll')):
        if os.path.exists(dll):
            shutil.copy(dll, os.path.join(target, 'SDL3.dll'))
            break
    for entry in ('thandor.dat', 'thandor.ini'):
        if os.path.exists(os.path.join(game, entry)):
            shutil.copy(os.path.join(game, entry), os.path.join(target, entry))
    return target


class Worker:
    """A game copy with its own UDP port, taken by one run at a time."""
    pool = []
    pool_lock = threading.Lock()

    def __init__(self):
        with port_lock:
            self.index = next_worker[0]
            next_worker[0] += 1
        self.folder = make_copy(self.index)
        self.port = args.port_base + self.index

    @classmethod
    def take(cls):
        with cls.pool_lock:
            if cls.pool:
                return cls.pool.pop()
        return cls()

    def give_back(self):
        with Worker.pool_lock:
            Worker.pool.append(self)

    @property
    def progress(self):
        return os.path.join(self.folder, 'progress', 'kampagne.dat')

    def reset_progress(self, data=None):
        """Empties the progress folder; writes data (bytes) as kampagne.dat when given."""
        folder = os.path.dirname(self.progress)
        shutil.rmtree(folder, ignore_errors=True)
        os.makedirs(folder)
        if data is not None:
            with open(self.progress, 'wb') as f:
                f.write(data)

    def run(self, arguments, env, timeout, script=None, until=None, settle=0):
        """Starts the game, waits until it exits, crashes or hangs (crash.log / hang.log), until(log text) is true
        (then settle seconds more) or timeout s passed; stops only this process. Returns (status, log text)
        with status 'exited', 'until', 'CRASH', 'HANG' or 'TIMEOUT'."""
        for entry in ('crash.log', 'crash_raw.log', 'hang.log', 'thandor.log', 'statehash.txt'):
            if os.path.exists(os.path.join(self.folder, entry)):
                os.remove(os.path.join(self.folder, entry))
        shutil.rmtree(os.path.join(self.folder, 'shots'), ignore_errors=True)
        variables = {'OPEN_THANDOR_WINDOWED': '1', 'OPEN_THANDOR_MULTI_INSTANCE': '1',
                     'OPEN_THANDOR_NET_PORT': str(self.port), 'OPEN_THANDOR_CAMPAIGN_PROGRESS': self.progress,
                     'OPEN_THANDOR_CAMPAIGN_PROGRESS_DUMP': '1'}
        if script is not None:
            path = os.path.join(self.folder, 'cmap_script.txt')
            with open(path, 'w') as f:
                f.write('\n'.join(script) + '\n')
            variables['OPEN_THANDOR_SCRIPT'] = path
        variables.update(env)
        with game_slots:
            return self.run_process(arguments, variables, timeout, until, settle)

    def run_process(self, arguments, variables, timeout, until, settle):
        exe = os.path.join(self.folder, 'thandor.exe')
        process = subprocess.Popen('"%s" %s' % (exe, arguments), executable=exe, cwd=self.folder,
                                   env=game_env(**variables))
        start = time.time()
        status = None
        while status is None:
            time.sleep(1)
            if os.path.exists(os.path.join(self.folder, 'crash.log')):
                status = 'CRASH'
            elif os.path.exists(os.path.join(self.folder, 'hang.log')):
                status = 'HANG'
            elif process.poll() is not None:
                status = 'exited'
            elif time.time() - start > timeout:
                status = 'TIMEOUT'
            elif until is not None and until(self.log()):
                time.sleep(settle)
                status = 'until'
        if process.poll() is None:
            subprocess.run('taskkill /F /T /PID %d' % process.pid, shell=True, capture_output=True)
        process.wait()
        if status == 'exited' and os.path.exists(os.path.join(self.folder, 'crash.log')):
            status = 'CRASH'
        time.sleep(1)
        return status, self.log()

    def log(self):
        path = os.path.join(self.folder, 'thandor.log')
        for _ in range(5):  # the game holds the log open for a moment
            try:
                with open(path, errors='replace') as f:
                    return f.read()
            except OSError:
                time.sleep(0.5)
        return ''

    def keep(self, case, label, extra=()):
        """Copies log, screenshots, progress files, statehash and crash logs to campaignmap/<case>/<label>/."""
        target = os.path.join(out_root, case, label)
        shutil.rmtree(target, ignore_errors=True)
        os.makedirs(target)
        for name in ('thandor.log', 'statehash.txt', 'crash.log', 'hang.log') + tuple(extra):
            if os.path.exists(os.path.join(self.folder, name)):
                shutil.copy(os.path.join(self.folder, name), target)
        for path in glob.glob(os.path.join(self.folder, 'progress', '*')):
            shutil.copy(path, target)
        for path in glob.glob(os.path.join(self.folder, 'shots', '*.bmp')):
            shutil.copy(path, target)
        return target


class Case:
    """Collects a case's checks: ok(condition, text) records a failure text when the condition is false."""

    def __init__(self, name):
        self.name = name
        self.failures = []
        self.notes = []

    def ok(self, condition, text):
        if not condition:
            self.failures.append(text)
        return condition

    def note(self, text):
        self.notes.append(text)


def with_worker(function):
    worker = Worker.take()
    try:
        return function(worker)
    finally:
        worker.give_back()


# ----------------------------------------------------------------------------------------------- progress files

def cgn_bytes(name):
    with open(os.path.join(level_dir, name + '.cgn'), 'rb') as f:
        return f.read()


def section(name, wins=None, unlocked=(), carries=None):
    return {'name': name, 'fingerprint': campaign_progress.fingerprint(cgn_bytes(name)), 'wins': dict(wins or {}),
            'unlocked': list(unlocked), 'carries': dict(carries or {})}


def path_wins(levels, selection=1):
    return {level: (1 << selection, selection) for level in levels}


def read_progress(path):
    """The sections by name, or None when the file is missing or unreadable."""
    try:
        return {s['name']: s for s in campaign_progress.read_progress(path)}
    except (OSError, ValueError, KeyError, struct.error) as error:
        say('  cannot read %s: %r' % (path, error))
        return None


def file_bytes(path):
    if not os.path.exists(path):
        return None
    with open(path, 'rb') as f:
        return f.read()


# ------------------------------------------------------------------------------------------------ log helpers

def carried_counts(log):
    return [int(m) for m in re.findall(r'campaign carries over (\d+) units', log)]


def faction_lines(log):
    return re.findall(r'local faction \d+, carried per faction [\d ]+', log)


def statehash_lines(folder):
    """{tick: hash} of statehash.txt (the first hashed tick depends on the load: 2 or 3)."""
    path = os.path.join(folder, 'statehash.txt')
    if not os.path.exists(path):
        return {}
    with open(path, errors='replace') as f:
        return dict(line.split()[:2] for line in f if line[:1].isdigit() and len(line.split()) > 1)


def crashed(status):
    return status in ('CRASH', 'HANG')


# ------------------------------------------------------------------------------------------------------- cases

def case_ct1(case):
    def run(worker):
        worker.reset_progress()
        status, log = worker.run('-NOINTRO -KARTE="-"', {
            'OPEN_THANDOR_CAMPAIGN': 'tutorial', 'OPEN_THANDOR_CAMPAIGN_LEVEL': '1', 'OPEN_THANDOR_AUTOWIN': '40'},
            900, script_chain(3, 40, 120000, campaign_end=True),
            until=lambda text: 'progress dump (session ended)' in text.split('firing the campaign end', 1)[-1]
            and 'firing the campaign end' in text, settle=3)
        worker.keep(case.name, 'chain')
        case.ok(not crashed(status), 'chain %s' % status)
        counts = carried_counts(log)
        case.ok(len(counts) >= 3, 'chain started %d levels, expected 3' % len(counts))
        case.ok('firing the campaign end' in log, 'no campaign end')
        progress = read_progress(worker.progress)
        if not case.ok(progress is not None and 'tutorial' in progress, 'no tutorial section in the progress'):
            return
        tutorial = progress['tutorial']
        for level in (1, 2, 3):
            case.ok(tutorial['wins'].get(level) == (2, 1), 'level %d: %s, expected done with selection 1' % (
                level, tutorial['wins'].get(level)))
        for target, source in ((2, 1), (3, 2)):
            carry = tutorial['carries'].get(target)
            if case.ok(carry is not None, 'no snapshot for level %d' % target):
                case.ok(carry['source'] == source and carry['selection'] == 1,
                        'snapshot %d from %d sel %d' % (target, carry['source'], carry['selection']))
                if len(counts) >= target:
                    case.ok(len(carry['units']) == counts[target - 1],
                            'snapshot %d holds %d units, the chain carried %d' % (
                                target, len(carry['units']), counts[target - 1]))
        case.note('carried %s; snapshots %s' % (counts[:3], {t: len(c['units']) for t, c in
                                                            tutorial['carries'].items()}))
    with_worker(run)


def compare_direct_start(case, label, campaign, first_level, levels, target, chain_worker, direct_worker):
    """CT2 for one target: the chain (levels won after 15 s, the target's first 600 steps hashed), then the
    target started directly from the chain's progress."""
    hashing = {'OPEN_THANDOR_STATEHASH': '600', 'OPEN_THANDOR_STATEHASH_SEED': '12345'}
    chain_worker.reset_progress()
    status, chain_log = chain_worker.run('-NOINTRO -KARTE="-"', dict(hashing, **{
        'OPEN_THANDOR_CAMPAIGN': campaign, 'OPEN_THANDOR_CAMPAIGN_LEVEL': str(first_level),
        'OPEN_THANDOR_AUTOWIN': '15', 'OPEN_THANDOR_AUTOWIN_LEVELS': str(levels - 1)}), 900,
        script_chain(levels, 15, 300000))
    chain_worker.keep(case.name, label + '_chain')
    chain_hash = statehash_lines(chain_worker.folder)
    case.ok(status == 'exited', '%s chain %s (expected the exit after 600 hashed steps)' % (label, status))
    chain_counts = carried_counts(chain_log)
    if not case.ok(len(chain_counts) == levels, '%s chain started %d levels, expected %d' % (
            label, len(chain_counts), levels)):
        return
    progress = file_bytes(chain_worker.progress)
    if not case.ok(progress is not None, '%s: the chain wrote no progress' % label):
        return
    direct_worker.reset_progress(progress)
    status, direct_log = direct_worker.run('-NOINTRO -KARTE="-"', dict(hashing, **{
        'OPEN_THANDOR_CAMPAIGN': campaign, 'OPEN_THANDOR_CAMPAIGN_MISSION': str(target)}), 600,
        script_chain(1, 0, 300000))
    direct_worker.keep(case.name, label + '_direct')
    direct_hash = statehash_lines(direct_worker.folder)
    case.ok(status == 'exited', '%s direct start %s (expected the exit after 600 hashed steps)' % (label, status))
    case.ok('campaign start: restored the carry-over snapshot' in direct_log,
            '%s: the snapshot was not restored' % label)
    direct_counts = carried_counts(direct_log)
    case.ok(direct_counts[:1] == chain_counts[-1:], '%s: direct start carried %s, the chain %d' % (
        label, direct_counts, chain_counts[-1]))
    chain_factions, direct_factions = faction_lines(chain_log), faction_lines(direct_log)
    case.ok(bool(direct_factions) and direct_factions[:1] == chain_factions[-1:],
            '%s: per-faction line %s, the chain %s' % (label, direct_factions[:1], chain_factions[-1:]))
    case.ok('level script: end trigger' not in direct_log.split('campaign carries over', 1)[-1],
            '%s: the directly started level ended' % label)
    common = sorted(set(chain_hash) & set(direct_hash), key=int)
    differing = [tick for tick in common if chain_hash[tick] != direct_hash[tick]]
    case.ok('600' in chain_hash and chain_hash.get('600') == direct_hash.get('600'),
            '%s: state hash at tick 600: chain %s, direct %s' % (label, chain_hash.get('600'),
                                                                 direct_hash.get('600')))
    case.ok(not differing, '%s: state hashes differ from tick %s on' % (label, differing[:1]))
    case.note('%s: carried %d, %d/%d steps hashed, first difference %s' % (
        label, chain_counts[-1], len(direct_hash), len(chain_hash), differing[0] if differing else 'none'))


def case_ct2(case):
    def one(label, campaign, first, levels, target):
        a, b = Worker.take(), Worker.take()
        try:
            compare_direct_start(case, label, campaign, first, levels, target, a, b)
        finally:
            a.give_back()
            b.give_back()
    threads = [threading.Thread(target=one, args=('tutorial', 'tutorial', 1, 3, 3)),
               threading.Thread(target=one, args=('hansolo', 'hansolo', 22, 2, 23))]
    for t in threads:
        t.start()
    for t in threads:
        t.join()


def broken_files():
    """(label, bytes) of every broken progress file."""
    tutorial = section('tutorial', path_wins([1]), carries={2: {'source': 1, 'selection': 1,
                                                                'units': [(1, 1, 4096, -4096, 0)]}})
    valid = campaign_progress.encode_progress([tutorial])
    main = campaign_progress.encode_sections([tutorial])
    carry = campaign_progress.encode_carry(2, tutorial['carries'][2])

    def package(main_payload, carry_payloads=(carry,)):
        items = [('progress.bin', main_payload)]
        items += [('carry_tutorial_2.hex', payload) for payload in carry_payloads]
        return pck.write_package(items)
    bad_count = bytearray(carry)
    struct.pack_into('<I', bad_count, 16 + 16, 600)
    crc_data = bytes(bad_count[16:struct.unpack_from('<I', bad_count, 8)[0]])
    bad_count = campaign_progress.write_payload(campaign_progress.CARRY_TAG, crc_data)
    bad_faction = campaign_progress.encode_carry(2, {'source': 1, 'selection': 1, 'units': [(1, 9, 4096, -4096, 0)]})
    duplicate = campaign_progress.encode_sections([tutorial, section('tutorial', path_wins([1]))])
    duplicate = struct.pack('<I', 2) + duplicate[4:]  # the second tutorial section (no snapshot)
    random.seed(1)
    garbage = bytes(random.randrange(256) for _ in range(4096))
    return [
        ('truncated', valid[:len(valid) // 2]),
        ('wrong magic', b'pcx' + valid[3:]),
        ('version 2', package(campaign_progress.write_payload(campaign_progress.PROGRESS_TAG, main, version=2))),
        ('CRC32 mismatch', package(campaign_progress.write_payload(campaign_progress.PROGRESS_TAG, main,
                                                                   crc=0x12345678))),
        ('600 units', package(campaign_progress.write_payload(campaign_progress.PROGRESS_TAG, main), (bad_count,))),
        ('faction 9', package(campaign_progress.write_payload(campaign_progress.PROGRESS_TAG, main),
                              (bad_faction,))),
        ('duplicate sections', package(campaign_progress.write_payload(campaign_progress.PROGRESS_TAG, duplicate))),
        ('garbage', garbage),
        ('empty', b''),
        ('2 MB', valid + bytes(2 * 1024 * 1024 - len(valid))),
    ]


def case_ct3(case):
    files = broken_files()
    lock = threading.Lock()

    def one_broken(label, data):
        def run(worker):
            worker.reset_progress(data)
            status, log = worker.run('-NOINTRO -KARTE="-"', {'OPEN_THANDOR_CAMPAIGN_PROGRESS_DUMP': ''}, 120,
                                     script_open_campaign('tutorial', 1, start=False) + ['15000 quit'])
            worker.keep(case.name, re.sub(r'\W+', '_', label))
            lines = re.findall(r'campaign progress: .*kept as .*', log)
            bad = os.path.join(worker.folder, 'progress', 'kampagne.bad')
            with lock:
                case.ok(status == 'exited', '%s: %s' % (label, status))
                case.ok(len(lines) == 1, '%s: %d "kept as" log lines' % (label, len(lines)))
                case.ok(os.path.exists(bad) and file_bytes(bad) == data, '%s: no kampagne.bad with the bytes' % label)
                case.ok(not os.path.exists(worker.progress), '%s: kampagne.dat still there' % label)
        with_worker(run)

    threads = [threading.Thread(target=one_broken, args=item) for item in files]
    # valid files with a bad snapshot
    threads.append(threading.Thread(target=ct3_start_disabled, args=(case, lock)))
    threads.append(threading.Thread(target=ct3_bad_units, args=(case, lock)))
    for t in threads:
        t.start()
    for t in threads:
        t.join()


def ct3_start_disabled(case, lock):
    """A snapshot of tutorial level 2 whose source is level 2 itself: not valid, so mission 2 (a carry target) is
    not startable: a click on it and on Starten loads nothing."""
    tutorial = section('tutorial', path_wins([1]), carries={2: {'source': 2, 'selection': 1,
                                                                'units': [(1, 1, 4096, -4096, 0)]}})

    def run(worker):
        data = campaign_progress.encode_progress([tutorial])
        worker.reset_progress(data)
        status, log = worker.run('-NOINTRO -KARTE="-"', {}, 120,
                                 script_open_campaign('tutorial', 2) + ['20000 quit'],
                                 until=lambda text: 'campaign carries over' in text, settle=2)
        worker.keep(case.name, 'start_disabled')
        with lock:
            case.ok(status == 'TIMEOUT' or status == 'exited', 'start disabled: %s' % status)
            case.ok('campaign carries over' not in log and 'test aid: starting campaign' not in log,
                    'start disabled: a level was started')
            case.ok(file_bytes(worker.progress) == data, 'start disabled: the progress file changed')
    with_worker(run)


def ct3_bad_units(case, lock):
    """Snapshots of tutorial level 2 with a unit outside the map, and with a unit of asset id 0xFFFF at the
    position of a real carried unit: both start without crash; the first restores nothing."""
    def run(worker):
        # a real snapshot: a chain tutorial 1 -> 2
        worker.reset_progress()
        status, log = worker.run('-NOINTRO -KARTE="-"', {
            'OPEN_THANDOR_CAMPAIGN': 'tutorial', 'OPEN_THANDOR_AUTOWIN': '15', 'OPEN_THANDOR_AUTOWIN_LEVELS': '1'},
            600, script_chain(2, 15, 10000), until=lambda text: len(carried_counts(text)) >= 2, settle=3)
        worker.keep(case.name, 'bad_units_chain')
        progress = read_progress(worker.progress)
        if not (progress and 'tutorial' in progress and 2 in progress['tutorial']['carries']):
            with lock:
                case.ok(False, 'bad units: the chain made no snapshot for tutorial level 2 (%s)' % status)
            return
        real = progress['tutorial']
        unit = real['carries'][2]['units'][0]
        for label, units, expect in (
                ('outside', [unit, (unit[0], unit[1], 0x7FFFFFF0, 0x7FFFFFF0, 0)], 'lies outside the level'),
                ('asset 0xFFFF', [(0xFFFF,) + tuple(unit[1:])], 'restored the carry-over snapshot')):
            changed = dict(real, carries={2: dict(real['carries'][2], units=units)})
            worker.reset_progress(campaign_progress.encode_progress([changed]))
            status, log = worker.run('-NOINTRO -KARTE="-"', {
                'OPEN_THANDOR_CAMPAIGN': 'tutorial', 'OPEN_THANDOR_CAMPAIGN_MISSION': '2'}, 300,
                script_chain(1, 0, 30000))
            worker.keep(case.name, 'bad_units_' + re.sub(r'\W+', '_', label))
            with lock:
                case.ok(status == 'exited', 'bad units %s: %s' % (label, status))
                case.ok(expect in log, 'bad units %s: no "%s"' % (label, expect))
                case.ok('script: in game' in log, 'bad units %s: the level did not run' % label)
    with_worker(run)


def hansolo_up_to_m9():
    """hansolo done m1-m9 (ids 2-10) on the winning path (m6 = id 7 not), with a snapshot 9 from 8."""
    carry = {'source': 8, 'selection': 1, 'units': [(1, 1, 4096 * i, -4096 * i, 0) for i in range(1, 4)]}
    return section('hansolo', path_wins([2, 3, 4, 5, 6, 8, 9, 10]), carries={9: carry})


def case_ct4(case):
    def run(worker):
        before = hansolo_up_to_m9()
        worker.reset_progress(campaign_progress.encode_progress([before]))
        status, log = worker.run('-NOINTRO -KARTE="-"', {
            'OPEN_THANDOR_AUTOWIN': '20', 'OPEN_THANDOR_AUTOWIN_SELECTION': '1', 'OPEN_THANDOR_AUTOWIN_LEVELS': '1'},
            600, script_open_campaign('hansolo', 4) + ['23000 clickuntilnextlevel %d %d 3000' % START,
                                                       '10000 quit'],
            until=lambda text: len(carried_counts(text)) >= 2, settle=3)
        worker.keep(case.name, 'replay')
        case.ok(not crashed(status), 'replay %s' % status)
        case.ok('campaign progress: hansolo level 5 won with selection 1' in log, 'm4 (id 5) not won')
        after = read_progress(worker.progress)
        if not case.ok(after is not None and 'hansolo' in after, 'no hansolo section after the replay'):
            return
        after = after['hansolo']
        for level in (6, 8, 9, 10):
            case.ok(after['wins'].get(level) == before['wins'][level], 'level %d: %s -> %s' % (
                level, before['wins'][level], after['wins'].get(level)))
        expected = campaign_progress.encode_carry(9, before['carries'][9])
        case.ok(9 in after['carries'] and after['carries'][9]['payload'] == expected,
                'the snapshot of level 9 changed')
        case.ok(after['wins'].get(5) == (2, 1), 'level 5: %s' % (after['wins'].get(5),))
    with_worker(run)


def case_ct5(case):
    def run(worker):
        before = section('hansolo', path_wins([2, 3, 4, 5]))
        worker.reset_progress(campaign_progress.encode_progress([before]))
        status, log = worker.run('-NOINTRO -KARTE="-"', {
            'OPEN_THANDOR_AUTOWIN': '20', 'OPEN_THANDOR_AUTOWIN_SELECTION': '2', 'OPEN_THANDOR_AUTOWIN_LEVELS': '1'},
            600, script_open_campaign('hansolo', 4) + ['23000 clickuntilnextlevel %d %d 3000' % START,
                                                       '10000 quit'],
            until=lambda text: len(carried_counts(text)) >= 2, settle=3)
        worker.keep(case.name, 'branch')
        case.ok(not crashed(status), 'branch %s' % status)
        case.ok('campaign progress: hansolo level 5 won with selection 2' in log, 'm4 (id 5) not won with sel 2')
        case.ok(re.search(r'campaign level \d+: id 7 ', log) is not None, 'campaign records not logged')
        after = read_progress(worker.progress)
        if not case.ok(after is not None and 'hansolo' in after, 'no hansolo section after the replay'):
            return
        after = after['hansolo']
        case.ok(after['wins'].get(5) == (6, 2), 'level 5: %s, expected selections 1 and 2, last 2' % (
            after['wins'].get(5),))
        for level in (2, 3, 4):
            case.ok(after['wins'].get(level) == before['wins'][level], 'level %d changed' % level)
        # the level after the win is m6 (id 7): it started, i.e. it is unlocked; m5 (id 6) stays a successor of
        # the selection-1 win
        case.ok('level 5, end selection 2 -> level 7' in log, 'the win did not lead to level 7')
    with_worker(run)


def case_ct6(case):
    def run(worker):
        worker.reset_progress()
        status, log = worker.run('-NOINTRO -KARTE="-"', {
            'OPEN_THANDOR_CAMPAIGN': 'hansolo', 'OPEN_THANDOR_CAMPAIGN_LEVEL': '8', 'OPEN_THANDOR_AUTOWIN': '20',
            'OPEN_THANDOR_AUTOWIN_LEVELS': '1'}, 600, script_chain(2, 20, 10000),
            until=lambda text: 'progress dump (level started)' in text.split('campaign carries over')[-1]
            and len(carried_counts(text)) >= 2, settle=3)
        worker.keep(case.name, 'chain')
        chain_counts = carried_counts(log)
        if not case.ok(len(chain_counts) >= 2 and chain_counts[1] > 0, 'chain 8-9 carried %s (%s)' % (
                chain_counts, status)):
            return
        before = file_bytes(worker.progress)
        status, log = worker.run('-NOINTRO -KARTE="-"', {'OPEN_THANDOR_AUTOLOSE': '20'}, 900,
                                 script_open_campaign('hansolo', 8) + [
                                     '23000 clickuntilnextlevel %d %d 3000' % START,
                                     # the retry runs: options window (Esc), "Beenden", "Neustart"
                                     '8000 key 27', '10000 move %d %d' % QUIT_BUTTON,
                                     '10500 click %d %d' % QUIT_BUTTON, '12500 move %d %d' % RESTART_BUTTON,
                                     '13000 click %d %d' % RESTART_BUTTON,
                                     '15000 clickuntilnextlevel %d %d 3000' % START, '15000 quit'])
        worker.keep(case.name, 'retry')
        counts = carried_counts(log)
        case.ok(not crashed(status), 'retry %s' % status)
        case.ok('test aid: auto-lose' in log, 'no auto-lose')
        case.ok(counts == [chain_counts[1]] * 3, 'sessions carried %s, expected 3 x %d' % (counts, chain_counts[1]))
        case.ok(file_bytes(worker.progress) == before, 'the progress file changed')
        case.note('chain carried %s, map start/retry/restart %s' % (chain_counts, counts))
    with_worker(run)


# in-game (layout 1280x800): Esc opens the options window, its "Beenden" button the "Spiel verlassen" window with
# "Neustart" (the restart of InGameQuitMenu_RestartMission)
QUIT_BUTTON = (406, 363)
RESTART_BUTTON = (710, 475)


def case_ct7(case):
    """Host and client through run_multiplayer.py (fixed ports 929/930); the client copy is the host's + "2"."""
    host = make_copy('mp')
    client = host + '2'
    shutil.rmtree(os.path.join(host, 'progress'), ignore_errors=True)
    os.makedirs(os.path.join(host, 'progress'))
    host_progress = os.path.join(host, 'progress', 'kampagne.dat')
    client_progress = os.path.join(host, 'progress', 'client.dat')  # the client's progress folder is a junction
    tutorial = section('tutorial', path_wins([1]), carries={2: {'source': 1, 'selection': 1,
                                                                'units': [(1, 1, 4096, -4096, 0)]}})
    data = campaign_progress.encode_progress([tutorial, section('nimm2', path_wins([1, 2]))])
    with open(host_progress, 'wb') as f:
        f.write(data)
    for folder in (host, client):
        for name in ('thandor.log', 'crash.log', 'hang.log'):
            if os.path.exists(os.path.join(folder, name)):
                os.remove(os.path.join(folder, name))
    log_path = os.path.join(out_root, case.name, 'multiplayer.txt')
    os.makedirs(os.path.dirname(log_path), exist_ok=True)
    with game_slots, game_slots, open(log_path, 'w', encoding='utf-8', errors='replace') as log:
        code = subprocess.run([sys.executable, os.path.join(HERE, 'run_multiplayer.py'), host, '160',
                               '--host-script', os.path.join(HERE, 'mp_host_campaign.txt'),
                               '--client-script', os.path.join(HERE, 'mp_client_join.txt'),
                               '--host-env', 'OPEN_THANDOR_CAMPAIGN_PROGRESS=' + host_progress,
                               '--host-env', 'OPEN_THANDOR_CAMPAIGN_PROGRESS_DUMP=1',
                               '--client-env', 'OPEN_THANDOR_CAMPAIGN_PROGRESS=' + client_progress],
                              stdout=log, stderr=subprocess.STDOUT, timeout=600,
                              env=dict(os.environ, PYTHONIOENCODING='utf-8')).returncode
    for folder, label in ((host, 'host'), (client, 'client')):
        target = os.path.join(out_root, case.name, label)
        shutil.rmtree(target, ignore_errors=True)
        os.makedirs(target)
        for name in ('thandor.log', 'crash.log', 'hang.log'):
            if os.path.exists(os.path.join(folder, name)):
                shutil.copy(os.path.join(folder, name), target)
        for path in glob.glob(os.path.join(folder, 'shots', '*.bmp')):
            shutil.copy(path, target)
        text = file_bytes(os.path.join(folder, 'thandor.log')) or b''
        text = text.decode('latin-1')
        starts = re.findall(r'level start: campaign level .*', text)
        case.ok(not any(os.path.exists(os.path.join(folder, n)) for n in ('crash.log', 'hang.log')),
                '%s crashed or hung' % label)
        case.ok(any('id 3 "nimm2\\kometenhagel"' in line for line in starts),
                '%s did not start nimm2 mission 3 (%s)' % (label, starts))
        case.ok(not any('"tutorial\\' in line for line in starts), '%s started a tutorial mission' % label)
        case.ok('script: in game' in text, '%s did not reach the game' % label)
    case.ok(code == 0, 'run_multiplayer.py exit %s' % code)
    case.ok(file_bytes(host_progress) == data, 'the host progress file changed')


def case_ct8(case):
    from PIL import Image, ImageChops

    def run(worker):
        worker.reset_progress(campaign_progress.encode_progress([hansolo_up_to_m9(), section(
            'tutorial', path_wins([1]))]))
        status, _ = worker.run('-NOINTRO -KARTE="-"', {
            'OPEN_THANDOR_SCRIPT': os.path.join(HERE, 'choose_campaign.txt')}, 120)
        target = worker.keep(case.name, 'shot')
        case.ok(status == 'exited', 'choose_campaign %s' % status)
        shots = sorted(glob.glob(os.path.join(worker.folder, 'shots', 'script_*.bmp')))
        if not case.ok(bool(shots), 'no screenshot'):
            return
        panel = Image.open(shots[-1]).convert('RGB').crop(PANEL)
        panel.save(os.path.join(target, 'panel.png'))
        if args.save_golden:
            os.makedirs(os.path.dirname(GOLDEN), exist_ok=True)
            panel.save(GOLDEN, optimize=True)
            case.note('golden saved')
            return
        if not case.ok(os.path.exists(GOLDEN), 'no golden %s (--save-golden)' % GOLDEN):
            return
        golden = Image.open(GOLDEN).convert('RGB')
        box = ImageChops.difference(panel, golden).getbbox() if golden.size == panel.size else (0, 0) + panel.size
        if box and golden.size == panel.size:
            ImageChops.difference(panel, golden).save(os.path.join(target, 'diff.png'))
        case.ok(box is None, 'the panel differs from the golden in %s' % (box,))
    with_worker(run)


def case_info(case):
    """The game's level info dump against cgn.py dump --triggers."""
    def run(worker):
        worker.reset_progress()
        status, log = worker.run('-NOINTRO -KARTE="-"', {'OPEN_THANDOR_CAMPAIGN_LEVEL_INFO_DUMP': '1'}, 120,
                                 script_open_campaign('tutorial', 1, start=False) + ['14000 quit'])
        worker.keep(case.name, 'dump')
        case.ok(status == 'exited', 'level info run %s' % status)
        dump = subprocess.run([sys.executable, os.path.join(REPO, 'tools', 'data', 'cgn.py'), 'dump', level_dir,
                               '--triggers'], capture_output=True, text=True, encoding='utf-8', errors='replace',
                              env=dict(os.environ, PYTHONIOENCODING='utf-8'))
        expected = {}
        first = None
        for line in dump.stdout.splitlines():
            m = re.match(r'\s+firstLevelId (-?\d+)', line)
            if m:
                first = int(m.group(1))
                continue
            m = re.match(r'\s+id\s+(\d+) (\S+)', line)
            if m:
                key = (first, int(m.group(1)), fold(m.group(2)))
                continue
            if line.strip().startswith('triggers'):
                expected[key] = (None if 'level file missing' in line else
                                 sorted(set(tuple(int(v) for v in t) for t in
                                            re.findall(r'\((\d+),(\d+),(\d+)\)', line.split('SELECTION')[0]))))
        rows = re.findall(r'campaign level info: first (-?\d+), id\s+(\d+) (\S+)\s+exists (\d) header (\d) script '
                          r'(\d) title (-?\d+) win 0x([0-9A-F]+) triggers \(faction, selection, variant\): ?(.*)',
                          log)
        case.ok(len(rows) > 0, 'no "campaign level info" lines')
        seen = set()
        for first, level, name, exists, header, script, _title, win, triggers in rows:
            key = (int(first), int(level), fold(name))
            if key in seen:
                continue
            seen.add(key)
            if not case.ok(key in expected, 'level %s not in the cgn.py dump' % (key,)):
                continue
            want = expected[key]
            got = sorted(tuple(int(v) for v in t) for t in re.findall(r'\((\d+),(\d+),(\d+)\)', triggers))
            if want is None:
                case.ok(exists == '0', '%s: exists %s, cgn.py: level file missing' % (key, exists))
                continue
            case.ok(exists == '1' and header == '1' and script == '1', '%s: exists %s header %s script %s' % (
                key, exists, header, script))
            case.ok(got == want, '%s: triggers %s, cgn.py %s' % (key, got, want))
            mask = 0
            for _faction, selection, variant in want:
                if variant:
                    mask |= 1 << selection
            case.ok(int(win, 16) == mask, '%s: win mask 0x%s, from the triggers 0x%02X' % (key, win, mask))
        firsts = sorted(set(key[0] for key in seen))
        case.note('%d levels compared (campaign first levels %s)' % (len(seen), firsts))
    with_worker(run)


def fold(name):
    """A level file name for matching across the two dumps: lowercase ASCII letters, digits, '\\' and '_'."""
    return re.sub(r'[^a-z0-9_\\]', '', name.lower())


# ------------------------------------------------------------------------------------------------------- main

def main():
    global args, game, out_root, level_dir
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('game_dir')
    default_new = os.path.join(REPO, 'build-mingw-test', 'thandor.exe')
    parser.add_argument('--new', default=default_new, help='test build (default: build-mingw-test/thandor.exe)')
    parser.add_argument('--cases', default=','.join(CASES), help='comma-separated: ' + ','.join(CASES))
    parser.add_argument('--jobs', type=int, default=6, help='cases at once')
    parser.add_argument('--games', type=int, default=8, help='game instances at once')
    parser.add_argument('--port-base', type=int, default=870)
    parser.add_argument('--save-golden', action='store_true', help='CT8: save the panel as the new golden')
    args = parser.parse_args()
    global game_slots
    game_slots = threading.Semaphore(args.games)
    game = os.path.abspath(args.game_dir)
    args.new = os.path.abspath(args.new)
    cases = [c.strip() for c in args.cases.split(',') if c.strip()]
    unknown = set(cases) - set(CASES)
    if unknown:
        parser.error('unknown case(s): ' + ', '.join(sorted(unknown)))
    out_root = os.path.join(game, 'campaignmap')
    os.makedirs(out_root, exist_ok=True)
    # the level files (campaigns, level files for cgn.py --triggers) from LEVEL.PCK
    extracted = os.path.join(out_root, 'level_pck')
    level_dir = os.path.join(extracted, 'level')
    if not os.path.exists(os.path.join(level_dir, 'campagne.dat')):
        subprocess.run([sys.executable, os.path.join(REPO, 'tools', 'data', 'pck.py'), 'extract',
                        os.path.join(game, 'LEVEL.PCK'), extracted, 'level\\'], capture_output=True)
    results = os.path.join(out_root, 'results.txt')
    begin = time.time()
    outcome = {}
    semaphore = threading.Semaphore(args.jobs)

    def runner(name):
        with semaphore:
            case = Case(name)
            start = time.time()
            try:
                globals()['case_' + name.lower()](case)
            except Exception as error:  # a broken case must not hide the others
                import traceback
                case.failures.append('exception: %r' % error)
                traceback.print_exc()
            verdict = 'FAIL' if case.failures else 'PASS'
            outcome[name] = (verdict, case, time.time() - start)
            line = '%-5s %-4s %4.0fs  %s' % (name, verdict, time.time() - start,
                                             '; '.join(case.failures + case.notes))
            say(line)
            with open(results, 'a') as f:
                f.write(time.strftime('%Y-%m-%d %H:%M:%S ') + line + '\n')

    say('campaign map test: %s with %s; output in %s' % (', '.join(cases), args.new, out_root))
    # the network case uses fixed ports: alone, after the others
    threads = [threading.Thread(target=runner, args=(name,)) for name in cases if name != 'CT7']
    for thread in threads:
        thread.start()
        time.sleep(2)
    for thread in threads:
        thread.join()
    if 'CT7' in cases:
        runner('CT7')
    say('')
    for name in cases:
        verdict, case, seconds = outcome[name]
        say('%-5s %-4s %5.0fs  %s' % (name, verdict, seconds, '; '.join(case.failures + case.notes)))
    say('total %.0f s' % (time.time() - begin))
    return 1 if any(outcome[name][0] == 'FAIL' for name in cases) else 0


if __name__ == '__main__':
    sys.exit(main())
