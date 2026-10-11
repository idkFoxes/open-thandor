"""Pathing measurement: a group of ground units of faction 1 gets one move order on a generated flat test level
(tools/test/make_arena.py --scenario pathing) and the movement trace (OPEN_THANDOR_PATHTRACE, see
include/thandor/platform/debug/pathtrace.h) is evaluated per unit: time to arrive, stops, ticks turning on the spot,
route (re)plans, route points changed by other units' planning, collision blocks, path length.

usage:
  run_pathing.py GAME_DIR [--exe build-mingw-test/thandor.exe] [--cases choke,open,...] [--steps 1500]
                 [--save JSON] [--compare JSON] [--keep]

A case is "<layout>:<order>:<unit id>[:<count>]", e.g. "choke:formation:66:10"; layouts open, choke, cross,
crosschoke (make_arena.py), orders formation, point, cross (statehash.cpp DebugArena_PathingOrders). --cases takes
the short names of CASES or full case strings. Every case runs in its own linked copy GAME_DIR_path<k> (files hard
linked, flm/setup junctions, own level directory), all in parallel, minimized, with the fixed random seed of the
determinism test, so a case gives the same numbers every time. Needs the developer-tools build (preset mingw-test).
--save writes the per-case summary as JSON, --compare prints it next to an earlier one (before/after a change).
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
MIN_STOP_TICKS = 3        # a stand phase of at least this many steps after moving counts as a stop
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


def make_copy(game, k):
    target = '%s_path%d' % (game.rstrip('\\/'), k)
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


def run_case(folder, k, case, args, results):
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
                   OPEN_THANDOR_NET_PORT=str(1960 + k), OPEN_THANDOR_STATEHASH=str(args.steps),
                   OPEN_THANDOR_STATEHASH_SEED='12345', OPEN_THANDOR_ARENA_ORDERS='pathing',
                   OPEN_THANDOR_ARENA_PATHING=order, OPEN_THANDOR_PATHTRACE=trace)
    env.update(dict(item.split('=', 1) for item in args.env))
    exe = os.path.join(folder, 'thandor.exe')
    process = subprocess.Popen('"%s" -NOINTRO -KARTE="testpathing."' % exe, executable=exe, cwd=folder, env=env)
    try:
        process.wait(timeout=args.timeout)
    except subprocess.TimeoutExpired:
        process.kill()
    results[k] = analyse(trace) if os.path.exists(trace) else None


def analyse(path):
    """Per-unit and group metrics from a movement trace."""
    info, units, events = {}, {}, {}
    for line in open(path):
        part = line.split()
        if not part:
            continue
        if part[0] == 'I':
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
    return {'group': group, 'units': per_unit, 'classes': classes}


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
        print('%s (runtime classes %s)' % (case, result['classes']))
        for key in GROUP_KEYS:
            value = result['group'].get(key)
            line = '  %-24s %10s' % (key, value)
            if other and other.get(case):
                line += '   was %10s' % other[case]['group'].get(key)
            print(line)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('game_dir')
    parser.add_argument('--exe', default=os.path.join(REPO, 'build-mingw-test', 'thandor.exe'))
    parser.add_argument('--cases', default='open,choke,chokepoint,cross')
    parser.add_argument('--steps', type=int, default=2200, help='recorded steps (at least ORDER_TICK + WINDOW)')
    parser.add_argument('--timeout', type=int, default=600)
    parser.add_argument('--env', action='append', default=[], help='NAME=VALUE for the game (repeatable)')
    parser.add_argument('--save')
    parser.add_argument('--compare')
    parser.add_argument('--analyse', help='only evaluate an existing trace file')
    parser.add_argument('--units', action='store_true', help='also print the per-unit values')
    args = parser.parse_args()
    if args.analyse:
        print(json.dumps(analyse(args.analyse), indent=1))
        return 0
    game = os.path.abspath(args.game_dir)
    cases = [CASES.get(name, name) for name in args.cases.split(',')]
    folders = [make_copy(game, k) for k in range(len(cases))]
    results = {}
    threads = [threading.Thread(target=run_case, args=(folders[k], k, cases[k], args, results))
               for k in range(len(cases))]
    for thread in threads:
        thread.start()
        time.sleep(2)
    for thread in threads:
        thread.join()
    summary = {cases[k]: results.get(k) for k in range(len(cases))}
    other = json.load(open(args.compare)) if args.compare else None
    print_table(summary, other)
    if args.units:
        for case, result in summary.items():
            if result:
                print(case)
                for slot, values in sorted(result['units'].items()):
                    print('  %4d %s' % (slot, values))
    if args.save:
        json.dump(summary, open(args.save, 'w'), indent=1)
    return 0


if __name__ == '__main__':
    sys.exit(main())
