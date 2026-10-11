"""Pathing measurement: a group of ground units of faction 1 gets one move order on a generated flat test level
(tools/test/make_arena.py --scenario pathing) and the movement trace (OPEN_THANDOR_PATHTRACE, see
include/thandor/platform/debug/pathtrace.h) is evaluated per unit: time to arrive, stops, ticks turning on the spot,
route (re)plans, route points changed by other units' planning, collision blocks, path length.

usage:
  run_pathing.py GAME_DIR [--exe build-mingw-test/thandor.exe] [--cases choke,open,...] [--steps 2200]
                 [--pathing original|improved] [--save JSON] [--compare JSON] [--units]
  run_pathing.py GAME_DIR --before-after [--cases ...] [--threshold NAME=VALUE ...] [--baseline JSON]
                 [--save JSON]

A case is "<layout>:<order>:<unit id>[:<count>]", e.g. "choke:formation:66:10"; layouts open, choke, cross,
crosschoke (make_arena.py), orders formation, point, cross (statehash.cpp DebugArena_PathingOrders). --cases takes
the short names of CASES or full case strings. Every case runs in its own linked copy GAME_DIR_path<k> (files hard
linked, flm/setup junctions, own level directory), all in parallel, minimized, with the fixed random seed of the
determinism test, so a case gives the same numbers every time. Needs the developer-tools build (preset mingw-test).
--save writes the per-case summary as JSON, --compare prints it next to an earlier one (before/after a change).
--pathing sets OPEN_THANDOR_PATHING (the session's pathing mode, PathingMode_DevOverride); the trace records the
mode that ran ("P" line) and a run whose mode differs from the requested one counts as broken (exit status 2).
--copy-tag and --port-base keep parallel users of the same GAME_DIR apart.

--before-after (pathing plan P0, docs/plans/pathfinding.md 5) runs every case twice, with OPEN_THANDOR_PATHING=original
and =improved, all in parallel, prints both side by side with the change in percent and checks the improved mode
against THRESHOLDS (override with --threshold NAME=VALUE, repeatable; --threshold list prints them). With --baseline
the original-mode numbers must also equal that saved run exactly (the original mode is bit-exact; the committed
baseline is tools/test/pathing_baseline.json). Exit status 0 all thresholds met, 1 a threshold missed or the
original numbers moved, 2 a run gave no trace or ran in the wrong mode.
"""
import argparse
import json
import math
import os
import shutil
import subprocess
import sys
import threading
import time
from game_env import game_env

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, '..', '..'))
PRIVATE = ('thandor.exe', 'thandor.pdb', 'thandor.sym', 'thandor.dat', 'thandor.ini', 'thandor.log', 'crash.log',
           'crash_raw.log', 'hang.log', 'statehash.txt', 'pathtrace.txt', 'sdl3.dll')
SCRIPT = "0 layout 1280 800\n6000 clickuntilingame 839 539 3000\n0 ingame\n900000 quit\n"
ORDER_TICK = 102          # ARENA_MOVE_OUT_TICK in statehash.cpp
STEPS_PER_SECOND = 20
WINDOW = 2000             # evaluated steps after the order (the first recorded tick may differ by one under load)
ACTIVE = 0x1
MODE_NAMES = {0: 'original', 1: 'improved'}  # PathingMode values (include/thandor/world/pathing/mode.h)
MIN_STOP_TICKS = 3       # a stand phase of at least this many steps after moving counts as a stop
# short names: tracked tank PNZ-3 Dachs (66), wheeled jeep Wiesel (1), walker WLK-1 Emu (110), glider GLT-1 Moskito
# (172 bare)
CASES = {
    'single': 'open:formation:66:1',
    'open': 'open:formation:66:10',
    'choke': 'choke:formation:66:10',
    'chokepoint': 'choke:point:66:10',
    'cross': 'cross:cross:66:10',
    'jeep': 'choke:formation:1:10',
    'walker': 'choke:formation:110:10',
    'glider': 'choke:formation:172:10',
}
# the cases of the before/after comparison: the baseline cases of the plan's section 2 that the P0-P3 packages
# target (tank alone and in groups, open / choke / point order, jeeps, gliders)
BEFORE_AFTER_CASES = 'single,open,choke,chokepoint,jeep,glider'
# Acceptance of the improved mode against the original mode of the same run (docs/plans/pathfinding.md 5: "Testing
# per package" and the expected effects of P1-P3). A drop is a fraction of the original value (0.4 = at least 40 %
# lower) and is only demanded for the group cases of drop_cases where the original value reaches the matching *_min
# (a lone tank's turn towards its target at the start, or gliders without stops, have nothing to halve); elsewhere
# the value may grow by at most growth_tolerance x max(original, 1).
THRESHOLDS = {
    'drop_cases': 'open,choke,chokepoint,cross,jeep',  # cases (short names) where the drops below are demanded
    'turn_drop': 0.40,              # turn-on-spot s/unit at least 40 % lower (P1: -40..-60 %)
    'turn_min': 0.5,                # ... where the original turns at least 0.5 s/unit
    'stops_drop': 0.50,             # stops/unit at least halved ("Testing per package")
    'stops_min': 1.0,
    'collision_stops_drop': 0.50,   # stops after a collision at least halved (P2: collision stops -50 %+)
    'collision_stops_min': 0.5,
    'blocked_growth': 0.25,         # collision-blocked steps/unit at most 25 % more (E1 moved the stops into the
                                    # collisions; P2's waits may add blocked steps, but no cascade)
    'replans_growth': 0.25,         # re-plans/unit at most 25 % more (halving them is P4's target, not P0-P3's)
    'arrival_slack': 0.05,          # mean and last arrival at most 5 % later than the original
    'choke_mean_max': 60.0,         # choke formation: mean arrival at most 60 s (original 74.6 s)
    'all_arrive': 'chokepoint',     # cases (comma-separated short names) where every unit must arrive (P3)
    'growth_tolerance': 0.10,       # allowed growth of the drop metrics below their *_min
}


def make_copy(game, k, tag='path'):
    target = '%s_%s%d' % (game.rstrip('\\/'), tag, k)
    os.makedirs(os.path.join(target, 'save'), exist_ok=True)
    os.makedirs(os.path.join(target, 'level'), exist_ok=True)
    for name in os.listdir(game):
        source, link = os.path.join(game, name), os.path.join(target, name)
        if name.lower() in PRIVATE:
            continue
        if os.path.isfile(source) and not os.path.exists(link):
            os.link(source, link)
        elif os.path.isdir(source) and name.lower() in ('flm', 'setup') and not os.path.exists(link):
            subprocess.run('mklink /J "%s" "%s"' % (link, source), shell=True, capture_output=True)
    for name in ('thandor.dat', 'thandor.ini'):
        if os.path.exists(os.path.join(target, name)):
            os.remove(os.path.join(target, name))
    if os.path.exists(os.path.join(game, 'thandor.dat')):
        shutil.copy(os.path.join(game, 'thandor.dat'), os.path.join(target, 'thandor.dat'))
    return target


def run_case(folder, k, case, args, results, mode=None):
    layout, order, unit, *rest = case.split(':')
    count = rest[0] if rest else '10'
    exe_source = os.path.abspath(args.exe)
    sdl = os.path.join(os.path.dirname(exe_source), 'SDL3.dll')
    # private copies: never write through a hard link into the game directory
    for source, name in ((exe_source, 'thandor.exe'), (sdl, 'SDL3.dll')):
        if os.path.exists(os.path.join(folder, name)):
            os.remove(os.path.join(folder, name))
        if os.path.exists(source):
            shutil.copy(source, os.path.join(folder, name))
    subprocess.run([sys.executable, os.path.join(HERE, 'make_arena.py'), folder, '--scenario', 'pathing',
                    '--name', 'testpathing', '--layout', layout, '--unit-id', unit, '--count', count],
                   check=True, capture_output=True)
    script = os.path.join(folder, 'pathing_script.txt')
    open(script, 'w').write(SCRIPT)
    trace = os.path.join(folder, 'pathtrace.txt')
    for name in ('pathtrace.txt', 'statehash.txt'):
        if os.path.exists(os.path.join(folder, name)):
            os.remove(os.path.join(folder, name))
    env = game_env(OPEN_THANDOR_SCRIPT=script, OPEN_THANDOR_WINDOWED='1', OPEN_THANDOR_MULTI_INSTANCE='1',
                   OPEN_THANDOR_NET_PORT=str(args.port_base + k),OPEN_THANDOR_STATEHASH=str(args.steps),
                   OPEN_THANDOR_STATEHASH_SEED='12345', OPEN_THANDOR_ARENA_ORDERS='pathing',
                   OPEN_THANDOR_ARENA_PATHING=order, OPEN_THANDOR_PATHTRACE=trace)
    env.update(dict(item.split('=', 1) for item in args.env))
    if mode:
        env['OPEN_THANDOR_PATHING'] = mode
    exe = os.path.join(folder, 'thandor.exe')
    process = subprocess.Popen('"%s" -NOINTRO -KARTE="testpathing."' % exe, executable=exe, cwd=folder, env=env)
    try:
        process.wait(timeout=args.timeout)
    except subprocess.TimeoutExpired:
        process.kill()
    results[k] = analyse(trace) if os.path.exists(trace) else None


def analyse(path):
    """Per-unit and group metrics from a movement trace."""
    info, units, events, modes = {}, {}, {}, []
    for line in open(path):
        part = line.split()
        if not part:
            continue
        if part[0] == 'P':
            modes.append(int(part[2]))
        elif part[0] == 'I':
            info[int(part[1])] = {'asset': int(part[2]), 'class': int(part[3]), 'radius': int(part[4]),
                                  'faction': int(part[8]), 'speed': int(part[6])}
        elif part[0] == 'U':
            tick, slot = int(part[1]), int(part[2])
            units.setdefault(slot, []).append((tick, int(part[3]), int(part[4]), int(part[5]), int(part[6]),
                                               int(part[8], 16), int(part[9]), int(part[10]), int(part[13]),
                                               int(part[14])))
        elif part[0] == 'E':
            tick, kind, slot, other = int(part[1]), part[2], int(part[3]), int(part[4])
            if ORDER_TICK <= tick <= ORDER_TICK + WINDOW:
                events.setdefault(slot, []).append((tick, kind, other))
    mobile = {slot for slot, i in info.items() if i['faction'] == 1 and i['speed'] != 0 and slot in units}
    per_unit = {}
    for slot, rows in units.items():
        rows = [r for r in rows if ORDER_TICK <= r[0] <= ORDER_TICK + WINDOW]
        if not rows or slot not in info:
            continue
        unit_events = events.get(slot, [])
        events_at = {}
        for tick, kind, other in unit_events:
            events_at.setdefault(tick, []).append((kind, other))
        turn_ticks = wait_ticks = moving_ticks = active_ticks = 0
        path_length = 0.0
        arrived = None
        reactivations = 0
        route_points = 0
        started = False
        target = None
        stand = []            # ticks of the current stand phase (after having moved)
        has_moved = False
        stops = {'collision': 0, 'replan': 0, 'turn': 0, 'other': 0}
        was_active = False
        for (prev, row) in zip(rows, rows[1:]):
            tick, x, y, heading, advance, flags, route_x, route_y, target_x, target_y = row
            moved = (x, y) != (prev[1], prev[2])
            turned = heading != prev[3]
            active = bool(flags & ACTIVE)
            if active and arrived is None:
                started = True
                target = (target_x, target_y)
            if arrived is not None and active and not was_active:
                reactivations += 1
            was_active = active
            if (route_x, route_y) != (prev[6], prev[7]):
                route_points += 1
            if not started or arrived is not None:
                continue
            if not active:
                arrived = tick
                continue
            active_ticks += 1
            if moved:
                moving_ticks += 1
                path_length += math.hypot(x - prev[1], y - prev[2])
                if len(stand) >= MIN_STOP_TICKS:
                    # classify the stand phase by what happened in it (and the tick before it)
                    kinds = {k for t in [stand[0] - 1] + stand for k, _ in events_at.get(t, [])}
                    blockers = {o for t in [stand[0] - 1] + stand for k, o in events_at.get(t, []) if k == 'B'}
                    if blockers:
                        stops['collision'] += 1
                    elif 'R' in kinds or 'T' in kinds:
                        stops['replan'] += 1
                    elif 'H' in kinds:
                        stops['turn'] += 1
                    else:
                        stops['other'] += 1
                stand = []
                has_moved = True
            else:
                if turned:
                    turn_ticks += 1
                else:
                    wait_ticks += 1
                if has_moved:
                    stand.append(tick)
        start = rows[0]
        end = rows[-1]
        straight = math.hypot(target[0] - start[1], target[1] - start[2]) if target else 0.0
        remaining = math.hypot(target[0] - end[1], target[1] - end[2]) if target else 0.0
        blocks = [e for e in unit_events if e[1] == 'B']
        speed = info[slot]['speed']
        per_unit[slot] = {
            'arrive_s': None if arrived is None else round((arrived - ORDER_TICK) / STEPS_PER_SECOND, 2),
            'stops': sum(stops.values()),
            'stops_collision': stops['collision'],
            'stops_replan': stops['replan'],
            'stops_turn': stops['turn'],
            'stops_other': stops['other'],
            'turn_s': round(turn_ticks / STEPS_PER_SECOND, 2),
            'wait_s': round(wait_ticks / STEPS_PER_SECOND, 2),
            'move_s': round(moving_ticks / STEPS_PER_SECOND, 2),
            'speed_ratio': round(path_length / (active_ticks * speed), 2) if active_ticks and speed else None,
            'replans': sum(1 for e in unit_events if e[1] == 'R' and (arrived is None or e[0] <= arrived)),
            'retargets': sum(1 for e in unit_events if e[1] == 'T'),
            'heading_stops': sum(1 for e in unit_events if e[1] == 'H'),
            'blocks_unit': sum(1 for e in blocks if e[2] in mobile),
            'blocks_other': sum(1 for e in blocks if e[2] not in mobile),
            'route_points': route_points,
            'reactivations': reactivations,
            'detour': round(path_length / straight, 2) if straight else None,
            'left_q12': int(remaining),
        }
    group = {}
    if per_unit:
        values = list(per_unit.values())
        arrivals = [v['arrive_s'] for v in values if v['arrive_s'] is not None]
        group = {
            'units': len(values),
            'arrived': len(arrivals),
            'last_arrival_s': max(arrivals) if arrivals else None,
            'mean_arrival_s': round(sum(arrivals) / len(arrivals), 2) if arrivals else None,
        }
        for key in ('stops', 'stops_collision', 'stops_replan', 'stops_turn', 'stops_other', 'turn_s', 'wait_s',
                    'move_s', 'replans', 'retargets', 'heading_stops', 'blocks_unit', 'blocks_other', 'route_points',
                    'reactivations'):
            total = sum(v[key] for v in values)
            group[key + '_per_unit'] = round(total / len(values), 2)
        detours = [v['detour'] for v in values if v['detour']]
        group['detour_mean'] = round(sum(detours) / len(detours), 2) if detours else None
        ratios = [v['speed_ratio'] for v in values if v['speed_ratio']]
        group['speed_ratio_mean'] = round(sum(ratios) / len(ratios), 2) if ratios else None
    classes = sorted({info[s]['class'] for s in per_unit if s in info})
    # the session's pathing mode from the "P" lines (None: a build before the mode existed; 'mixed': it changed
    # during the session, which must never happen)
    mode = None if not modes else (MODE_NAMES.get(modes[0], str(modes[0])) if len(set(modes)) == 1 else 'mixed')
    return {'group': group, 'units': per_unit, 'classes': classes, 'mode': mode}


GROUP_KEYS = ('units', 'arrived', 'last_arrival_s', 'mean_arrival_s', 'speed_ratio_mean', 'stops_per_unit',
              'stops_collision_per_unit', 'stops_replan_per_unit', 'stops_turn_per_unit', 'stops_other_per_unit',
              'turn_s_per_unit', 'wait_s_per_unit', 'move_s_per_unit', 'replans_per_unit', 'retargets_per_unit',
              'heading_stops_per_unit', 'blocks_unit_per_unit', 'blocks_other_per_unit', 'route_points_per_unit',
              'reactivations_per_unit', 'detour_mean')


def print_table(summary, other=None):
    for case, result in summary.items():
        if result is None:
            print('%s: no trace (crash, hang or timeout?)' % case)
            continue
        print('%s (runtime classes %s, pathing %s)' % (case, result['classes'], result.get('mode')))
        for key in GROUP_KEYS:
            value = result['group'].get(key)
            line = '  %-24s %10s' % (key, value)
            if other and other.get(case):
                line += '   was %10s' % other[case]['group'].get(key)
            print(line)


def run_cases(game, jobs, args):
    """jobs: list of (case string, pathing mode or None), all run in parallel; returns the results by job."""
    folders = [make_copy(game, k, args.copy_tag) for k in range(len(jobs))]
    results = {}
    threads = [threading.Thread(target=run_case, args=(folders[k], k, jobs[k][0], args, results, jobs[k][1]))
               for k in range(len(jobs))]
    for thread in threads:
        thread.start()
        time.sleep(2)
    for thread in threads:
        thread.join()
    return [results.get(k) for k in range(len(jobs))]


def run_error(result, mode):
    """Why a run cannot be used: no trace, no units, or another pathing mode than requested (None: usable)."""
    if result is None:
        return 'no trace (crash, hang or timeout?)'
    if not result['group']:
        return 'no unit of faction 1 in the trace'
    if mode is not None and result.get('mode') != mode:
        return 'requested pathing %s, the trace shows %s (OPEN_THANDOR_PATHING not applied?)' % (
            mode, result.get('mode'))
    return None


def parse_thresholds(items):
    thresholds = dict(THRESHOLDS)
    for item in items:
        name, _, value = item.partition('=')
        if name not in THRESHOLDS:
            raise SystemExit('unknown threshold %r; known: %s' % (name, ', '.join(THRESHOLDS)))
        thresholds[name] = value if isinstance(THRESHOLDS[name], str) else float(value)
    return thresholds


def collision_blocked(group):
    """Collision-blocked steps per unit ('B' events, blocker mobile or not), as in the plan's section 2 table."""
    return round(group['blocks_unit_per_unit'] + group['blocks_other_per_unit'], 2)


def change_text(value, base):
    if base:
        return '%+.0f %%' % (100.0 * (value - base) / base)
    return '+%g' % value if value else '0'


def check_improved(name, before, after, t):
    """The threshold checks of one case (improved run against the original run): list of (passed, text)."""
    checks = []

    drop_cases = [c.strip() for c in str(t['drop_cases']).split(',') if c.strip()]

    def drop_or_hold(label, key_drop, key_min, b, a):
        if name in drop_cases and b >= t[key_min]:
            limit = b * (1.0 - t[key_drop])
            checks.append((a <= limit, '%s %.2f -> %.2f (%s), needs <= %.2f (-%.0f %%)' % (
                label, b, a, change_text(a, b), limit, 100 * t[key_drop])))
        else:
            limit = b + t['growth_tolerance'] * max(b, 1.0)
            checks.append((a <= limit, '%s %.2f -> %.2f, needs <= %.2f (no growth)' % (label, b, a, limit)))

    def growth(label, key, b, a):
        limit = b + t[key] * max(b, 1.0)
        checks.append((a <= limit, '%s %.2f -> %.2f (%s), needs <= %.2f (+%.0f %%)' % (
            label, b, a, change_text(a, b), limit, 100 * t[key])))

    b, a = before['group'], after['group']
    all_arrive = [c.strip() for c in str(t['all_arrive']).split(',') if c.strip()]
    if name in all_arrive:
        checks.append((a['arrived'] == a['units'], 'arrived %d/%d -> %d/%d, needs all' % (
            b['arrived'], b['units'], a['arrived'], a['units'])))
    else:
        checks.append((a['arrived'] >= b['arrived'], 'arrived %d/%d -> %d/%d, needs no fewer' % (
            b['arrived'], b['units'], a['arrived'], a['units'])))
    for key, label in (('mean_arrival_s', 'mean arrival s'), ('last_arrival_s', 'last arrival s')):
        if b[key] is not None and a[key] is not None:
            limit = b[key] * (1.0 + t['arrival_slack'])
            checks.append((a[key] <= limit, '%s %.1f -> %.1f (%s), needs <= %.1f' % (
                label, b[key], a[key], change_text(a[key], b[key]), limit)))
    if name == 'choke':
        mean = a['mean_arrival_s']
        checks.append((mean is not None and mean <= t['choke_mean_max'],
                       'choke mean arrival %s s, needs <= %g s' % (mean, t['choke_mean_max'])))
    drop_or_hold('turn on spot s/unit', 'turn_drop', 'turn_min', b['turn_s_per_unit'], a['turn_s_per_unit'])
    drop_or_hold('stops/unit', 'stops_drop', 'stops_min', b['stops_per_unit'], a['stops_per_unit'])
    drop_or_hold('collision stops/unit', 'collision_stops_drop', 'collision_stops_min',
                 b['stops_collision_per_unit'], a['stops_collision_per_unit'])
    growth('collision-blocked steps/unit', 'blocked_growth', collision_blocked(b), collision_blocked(a))
    growth('re-plans/unit', 'replans_growth', b['replans_per_unit'], a['replans_per_unit'])
    return checks


COMPARE_ROWS = (('arrived', 'arrived'), ('mean arrival s', 'mean_arrival_s'), ('last arrival s', 'last_arrival_s'),
                ('speed ratio', 'speed_ratio_mean'), ('stops/unit', 'stops_per_unit'),
                ('  after collision', 'stops_collision_per_unit'), ('  after re-plan', 'stops_replan_per_unit'),
                ('turn on spot s/unit', 'turn_s_per_unit'), ('wait s/unit', 'wait_s_per_unit'),
                ('re-plans/unit', 'replans_per_unit'), ('retargets/unit', 'retargets_per_unit'),
                ('blocked steps/unit', None))


def group_value(group, key):
    if key is None:
        return collision_blocked(group)
    if key == 'arrived':
        return '%d/%d' % (group['arrived'], group['units'])
    return group.get(key)


def before_after(game, args):
    thresholds = parse_thresholds(args.threshold)
    names = (args.cases or BEFORE_AFTER_CASES).split(',')
    cases = [CASES.get(name, name) for name in names]
    jobs = [(case, mode) for mode in ('original', 'improved') for case in cases]
    results = run_cases(game, jobs, args)
    original = {cases[i]: results[i] for i in range(len(cases))}
    improved = {cases[i]: results[len(cases) + i] for i in range(len(cases))}
    baseline = json.load(open(args.baseline)) if args.baseline else None
    if baseline and 'original' in baseline and 'improved' in baseline:
        baseline = baseline['original']  # the --save of an earlier --before-after run
    broken, missed = [], []
    for name, case in zip(names, cases):
        before, after = original[case], improved[case]
        print('%-22s %10s %10s %8s   %s' % (name, 'original', 'improved', 'change', case))
        errors = [(mode, run_error(result, mode)) for mode, result in (('original', before), ('improved', after))]
        errors = [(mode, error) for mode, error in errors if error]
        for mode, error in errors:
            print('  BROKEN %s run: %s' % (mode, error))
            broken.append('%s/%s' % (name, mode))
        if errors:
            continue
        for label, key in COMPARE_ROWS:
            b, a = group_value(before['group'], key), group_value(after['group'], key)
            delta = change_text(a, b) if isinstance(b, (int, float)) and isinstance(a, (int, float)) else ''
            print('  %-20s %10s %10s %8s' % (label, b, a, delta))
        for passed, text in check_improved(name, before, after, thresholds):
            print('  %s %s' % ('ok  ' if passed else 'MISS', text))
            if not passed:
                missed.append('%s: %s' % (name, text))
        if baseline is not None:
            expected = baseline.get(case)
            if expected is None or expected.get('group') is None:
                print('  MISS baseline has no entry for this case')
                missed.append('%s: no baseline' % name)
            elif expected['group'] != before['group']:
                differing = [k for k in GROUP_KEYS if expected['group'].get(k) != before['group'].get(k)]
                print('  MISS original mode differs from the baseline: %s' % ', '.join(
                    '%s %s -> %s' % (k, expected['group'].get(k), before['group'].get(k)) for k in differing))
                missed.append('%s: original mode differs from the baseline' % name)
            else:
                print('  ok   original mode equals the baseline')
    if args.save:
        json.dump({'original': original, 'improved': improved}, open(args.save, 'w'), indent=1)
    print()
    if broken:
        print('pathing before/after: BROKEN (%s)' % ', '.join(broken))
        return 2
    if missed:
        print('pathing before/after: FAIL (%d of the checks missed)' % len(missed))
        return 1
    print('pathing before/after: PASS (%d cases)' % len(cases))
    return 0


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('game_dir')
    parser.add_argument('--exe', default=os.path.join(REPO, 'build-mingw-test', 'thandor.exe'))
    parser.add_argument('--cases', help='short names of CASES or case strings (default open,choke,chokepoint,'
                                        'cross; with --before-after %s)' % BEFORE_AFTER_CASES)
    parser.add_argument('--steps', type=int, default=2200, help='recorded steps (at least ORDER_TICK + WINDOW)')
    parser.add_argument('--timeout', type=int, default=600)
    parser.add_argument('--env', action='append', default=[], help='NAME=VALUE for the game (repeatable)')
    parser.add_argument('--pathing', choices=('original', 'improved'),
                        help='OPEN_THANDOR_PATHING for every case (the trace must show that mode)')
    parser.add_argument('--before-after', action='store_true',
                        help='every case with the original and the improved mode, compared and checked')
    parser.add_argument('--threshold', action='append', default=[],
                        help='NAME=VALUE overriding THRESHOLDS for --before-after (repeatable; "list" prints them)')
    parser.add_argument('--baseline', help='--before-after: the original mode must equal this saved run')
    parser.add_argument('--copy-tag', default='path', help='linked copies are GAME_DIR_<tag><k> (default path)')
    parser.add_argument('--port-base', type=int, default=1960, help='UDP port of copy k: base + k (default 1960)')
    parser.add_argument('--save')
    parser.add_argument('--compare')
    parser.add_argument('--analyse', help='only evaluate an existing trace file')
    parser.add_argument('--units', action='store_true', help='also print the per-unit values')
    args = parser.parse_args()
    if 'list' in args.threshold:
        for name, value in THRESHOLDS.items():
            print('%-22s %s' % (name, value))
        return 0
    if args.analyse:
        print(json.dumps(analyse(args.analyse), indent=1))
        return 0
    game = os.path.abspath(args.game_dir)
    if args.before_after:
        return before_after(game, args)
    cases = [CASES.get(name, name) for name in (args.cases or 'open,choke,chokepoint,cross').split(',')]
    results = run_cases(game, [(case, args.pathing) for case in cases], args)
    summary = {cases[k]: results[k] for k in range(len(cases))}
    other = json.load(open(args.compare)) if args.compare else None
    if other and 'original' in other and 'improved' in other:
        other = other[args.pathing or 'original']  # the --save of a --before-after run
    print_table(summary, other)
    if args.units:
        for case, result in summary.items():
            if result:
                print(case)
                for slot, values in sorted(result['units'].items()):
                    print('  %4d %s' % (slot, values))
    if args.save:
        json.dump(summary, open(args.save, 'w'), indent=1)
    errors = ['%s: %s' % (case, run_error(result, args.pathing)) for case, result in summary.items()
              if run_error(result, args.pathing)]
    for error in errors:
        print('BROKEN ' + error)
    return 2 if errors else 0


if __name__ == '__main__':
    sys.exit(main())
