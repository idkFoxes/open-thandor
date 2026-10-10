# Unit pathfinding and movement: analysis and improvement plan

Status: analysis done, baseline measured, plan proposed (branch `experimental/pathfinding`, from `dev` 113ce57f).
Nothing in the simulation is changed on this branch; it adds only a developer-tools movement trace, a pathing test
level and a measurement script (see [Tools](#tools-added-on-this-branch)). The follow-up measurements (the prototype
variants and the after-runs of each package) are **pending, to run later** - see
[Pending measurements](#pending-measurements).

Owner complaint: units block each other and move in a stop - turn - drive - stop - turn - drive rhythm
("bleiben stehen, drehen, fahren, stehen bleiben, drehen, weiter fahren").

Line numbers below refer to `dev` 113ce57f.

## 1. How it works today

### 1.1 The scratch grid (`src/world/pathing/scratch_grid.cpp`, `include/thandor/world/pathing/grid.h`)

Pathing runs on a grid of 4x4 scratch cells per field cell (skewed hex lattice, 6 neighbours: two in the row above,
left/right, two in the row below), 8 bytes per cell: `stateMask` and `pathCost`. Two copies exist (primary and
secondary, plus a path-cost queue of 6 x 65536 entries).

`stateMask` bits:

| Bits | Meaning | Written by |
|---|---|---|
| 0 | visited (flood fills, propagation) | the walkers |
| 1..7 | faction presence: the faction has explored/occupied the source field cell (occupancy presence bits `0xF9`) | `GridScratch_PropagateFieldOccupancyMaskNeighborhood`, tick-wheel case 7; only OR-ed in, cleared only by a terrain rebuild |
| 8..15 | low distance bands = **blocking** for the matching radius class | buildings and other static classes 4, 10..16, 20, 22 (`GridInfluence_AddLowDistanceBands`); planned route segments during group planning (below) |
| 16..23 | high distance bands = **high cost** for the matching radius class | mobile classes 0..3, 17..19, 23 (`GridInfluence_AddHighDistanceBands`) |
| 24..30 | terrain classes (slope/water per traversal class) and cut-off regions | `GridScratch_RebuildTerrainAndRuntimeClassificationMasks` (startup.cpp:406 and the reduced update only) |
| 31 | blocked (map edge ring) | terrain rebuild |

A model stamps eight concentric rings, ring n = its `footprintRadius` + `g_GridInfluenceRadiusOffset[n]` (1000, 1250,
1500, 1600, 1920, 2240, 2600, 4100) + 499 (influence.cpp:13-35, 182-296). A mover only tests the ring of its own
`footprintRadiusClass`, so the bands are a Minkowski sum "obstacle radius + my radius".

Per planning call the masks are set from the mover: `g_GridPathEntityClassMask` = its faction bit,
`g_GridPathBlockingMask` = low band of its class | terrain bit of its traversal class, `g_GridPathHighCostMask` =
high band of its class (route.cpp:180-185).

### 1.2 When the grid is refreshed (`src/gameplay/session/tick.cpp`)

20 simulation steps per second; world job `simulationTick & 7`:

- case 4 (tick.cpp:169-170): `GridInfluence_ClearDistanceBandsAndRefreshEntities` - all bands cleared, every model
  re-stamps at its current position: every 8 steps (0.4 s). Between refreshes a moving unit's high-cost disc stays
  where it was.
- case 7 (tick.cpp:178-193): occupancy rebuild and faction presence into the scratch grid.
- terrain classes only at session start and in the reduced update (tick.cpp:247-253).

### 1.3 Planning one unit: `EntityPathing_ResolveDestinationAndRebuildRoutes` (route.cpp:67)

1. Start and target cell (clamped), the mover's own influence (and that of the model it overlaps) is lifted.
2. Blocked start cell -> `GridPathCost_RelocateFromBlockedCell` (nearest open cell within +-16): head there.
3. Otherwise `GridPathLine_TestHexSegmentBlocked` (reachability.cpp:479) on the straight line start -> target with
   the **high-cost mask as blocker** (route.cpp:208). A cell blocks the line when it is map edge, has a blocking
   band, a caller-mask (high-cost) band, **or lacks the mover's faction presence bit** (reachability.cpp:437).
4. Line clear: the route point is the target itself.
5. Line blocked: `GridPathCost_PropagateWeightedHexNeighbors` (path_cost.cpp:194) fills costs from the target
   outwards (FIFO label correcting, not Dijkstra; step cost 4, 3 on own-presence cells, 12 with a high-cost band;
   cells with presence + blocking band are walls; stops when the start is reached or after 6 passes). If the start is
   cut off, the target is moved to the nearest reachable cell and costs are propagated again.
   `GridPathCost_BacktrackBestHexRoute` (path_cost.cpp:84) walks downhill from the start **as long as the straight
   line from the start stays clear** (again with the high-cost mask, route.cpp:243) and returns the farthest such
   cell: that cell centre is the route point.
6. `EntityPathing_RebuildOverlappingGroupRoutes` (below) with that point; influence restored.

Result: **a route is always one straight segment** (`movementWorld*` = the next route point, `fallbackPosition` =
the final or retargeted target). Nothing of the propagated cost field is kept; the next segment is planned from
scratch when this one ends. The 8-entry waypoint queue is the player's shift-waypoint list, not a path.

### 1.4 Re-planning the neighbours: `EntityPathing_RebuildOverlappingGroupRoutes` (route.cpp:294)

Every planning call copies the whole primary grid into the secondary one and works on the copy (route.cpp:327-328).
It collects up to 32 mobile models whose depth bins overlap the box around the move (route.cpp:351-370), sorts them
by priority (locked 0, other faction 1, own faction moving 2, own faction idle 2 + movementSpeed; processed in
ascending order, route.cpp:371-416), re-adds their influence, then:

- other faction: stamped as an obstacle (low bands at its position, route.cpp:430-437);
- the planning unit and every other own-faction model: `EntityPathing_UpdateRouteSegment` (route.cpp:572): its
  influence is lifted, its **current** route point is taken, snapped to a cell centre if its start or its line is
  blocked in the copy, the segment is stamped as **low (blocking) bands** for all models processed later
  (route.cpp:671-710), and if the point changed the other model gets it as new route point via
  `ArmyRuntime_SetPendingMoveTarget` (route.cpp:712-716).

`ArmyRuntime_SetPendingMoveTarget` (move_orders.cpp:175-203) also **sets `ARMY_MOVEMENT_ACTIVE`**, so an idle unit
that stands in a planned corridor is pushed to the nearest open cell centre: this is the original game's only
"make way" mechanism, and it fires for every planning call of any unit near it.

### 1.5 Following the route: `ArmyRuntime_UpdateMovementAndWaypoints` (move_orders.cpp:573)

Called by every movement class each step.

- `ACTIVE` and not `ROUTE_POINT_REACHED`: return the route point (move_orders.cpp:597-604).
- `ROUTE_POINT_REACHED` (set by the drive code on reaching the route point **and on every collision**):
  - at the fallback position (+-0x40): pop the next queued waypoint;
  - otherwise, if the model has **not moved** since the last check and the retry countdown runs: keep waiting
    (move_orders.cpp:634-640);
  - otherwise **re-plan** (move_orders.cpp:641-654), retry countdown `ARMY_MOVEMENT_RETRY_TICKS` = 64 steps
    (move_orders.h:19), counted down once per step (class_dispatch.cpp:69-71).
- not active: within the 0x1000 box of the final target -> arrived; else `ArmyRuntime_StartDirectMoveCommand`
  (another planning call).

### 1.6 Driving (`src/gameplay/army/drive_ground.cpp`, `drive_banking.cpp`, `walker.cpp`, `collision.cpp`)

Ground classes 1 (wheeled), 2 (tracked), 18, 19 - `ArmyGroundMovement_SteerAndDrive` (drive_ground.cpp:154):

1. Accelerated turn towards the route point (`ArmyGroundMovement_TurnTowardsHeading`, drive_ground.cpp:101-147;
   turn rate limit / acceleration from the definition).
2. Allowed heading error: `farHeadingErrorLimitAngle`, blended down to `nearHeadingErrorLimitAngle` inside
   `headingErrorInterpolationDistanceQ12` of the **route point** (drive_ground.cpp:198-207). Measured values: jeep,
   Dachs tank and glider all have far 8192 (45 deg), near 256 (1.4 deg), distance 6000.
3. Error above the limit: **advance = 0, the unit stands and turns on the spot** (drive_ground.cpp:208-211).
4. Otherwise accelerate (`ArmyRuntime_UpdateActivationMetricAndPlayStartSound`, drive_common.cpp:52-61; no
   deceleration, the speed only ever drops to 0) and move along the facing; closer than twice this step's travel
   it jumps onto the route point and sets `ROUTE_POINT_REACHED` (drive_ground.cpp:214-229).
5. `ArmyCollision_FindBlockingRuntimeForCurrentUnit` (collision.cpp:60) tests the next position against every
   model (circle radius sum, any faction, no direction or relative-velocity test). Blocked: the unit **stays,
   advance = 0, `ROUTE_POINT_REACHED`** (drive_ground.cpp:233-250). `ArmyRuntime_HandleCollisionPartner`
   (drive_common.cpp:88) only docks platforms (class 23) and crushes class 0; the blocker is never asked to move.

Banking/glider class 17 (drive_banking.cpp:199-292): slides straight to the route point whatever the heading, but a
**new route point resets speed and turn velocity to 0** (drive_banking.cpp:222-228; acceleration 10 per step for
the Moskito, so 24 steps to full speed). Same collision handling.

Walker class 3 (walker.cpp): step-wise; walks on when the route point is within 45 deg (walker.cpp:224-245), else
turns on the spot (walker.cpp:253-271); a blocked foot is set down beside the other foot (walker.cpp:659-675,
802-818).

## 2. Measurements (baseline, done)

Scenario: flat generated level (`tools/test/make_arena.py --scenario pathing`), faction 1 group of 10 units in
rows of 5, 3 cells apart, 17 rows before the middle; `choke` layouts put a wall of Xenite silos (331) across the
middle row with a gap (about 9500 Q12 free, three tanks side by side); `cross` puts half the group 34 rows further
and the halves swap sides. One move order at simulation tick 102 (statehash test aid, fixed seed 12345):
`formation` = target 40 rows beyond the group centre plus each unit's offset (`SelectionPointerArray_ApplyMoveCommand`),
`point` = all to the same point (what the game does when the selection spread exceeds 5.0 / 7.0,
selection_set.cpp:314 - a 10-tank group in this layout already exceeds it). Evaluated 2000 steps (100 s) after
the order. Runs are deterministic (identical numbers on re-run).

Metrics: arrival = `ARMY_MOVEMENT_ACTIVE` cleared; stop = a stand phase of at least 3 steps after moving, classified
by the event in it (collision 'B', re-plan or retarget 'R'/'T'); turn s = steps standing while the heading changes;
speed ratio = path length / (active steps x movementSpeed).

| Case (unit) | arrived | mean / last arrival s | speed ratio | stops/unit (collision / replan) | turn on spot s/unit | replans/unit | retargets/unit | collision-blocked steps/unit |
|---|---|---|---|---|---|---|---|---|
| single tank, open (66) | 1/1 | 48.9 / 48.9 | 0.98 | 0 | 1.05 | 6 | 0 | 0 |
| open, formation (66) | 10/10 | 55.1 / 65.9 | 0.92 | 3.7 (1.3 / 2.4) | 3.7 | 13.0 | 1.8 | 25 |
| choke, formation (66) | 10/10 | 74.6 / 99.0 | 0.79 | 12.1 (3.8 / 8.3) | 14.1 | 18.8 | 4.0 | 64 |
| choke, point (66) | **5/10** | 70.0 / 102.7 | 0.73 | 20.7 (7.0 / 13.7) | 20.9 | 30.6 | 12.3 | 126 |
| cross (66) | 10/10 | 46.7 / 58.3 | 0.92 | 2.3 (0.8 / 1.5) | 3.5 | 5.5 | 1.1 | 11 |
| choke, jeep (1) | 10/10 | 38.8 / 47.3 | 0.81 | 7.5 (2.4 / 5.1) | 6.7 | 16.8 | 1.8 | 27 |
| choke, walker Emu (110) | 10/10 | 52.5 / 62.1 | 0.78 | (walker steps, not comparable) | - | 15.0 | 1.8 | 7 |
| choke, glider Moskito (172) | 10/10 | 29.6 / 36.9 | 0.74 | 0 | 0 | 14.8 | 1.6 | 2 |

The ideal time of a tank is the single-unit run (48.9 s, 6 re-plans only at the edge of the explored area). Ten
tanks through a gap take on average 53 % longer (last one 2x), and with a point order half of them never arrive:
they keep re-planning towards a target that their neighbours already occupy.

Further evaluation of the traces (choke, formation, tanks):

- **Route segments are short and kinked.** Median segment 3830 Q12 (single tank: 18000), 67 % shorter than the
  6000 heading-interpolation distance, **56 % of new route points need a heading change above 45 deg** (mean
  73 deg) - so the unit stops and turns on the spot. Point order: median 3345, 82 % short.
- **Re-plans are triggered by reaching the route point** (126 of 178 re-plans), after a collision once the 64-step
  retry countdown runs out (about 50), rarely directly after a collision (2).
- **Long waits are the retry countdown**: 14 stand phases of 0.5 s or more, the longest 3.0 s = 64 steps.
- **Collisions are a cascade of turning units**: the blocker was turning on the spot in 66 % of the blocked steps
  (open 51 %, cross 90 %, jeep 86 %), standing (itself blocked/waiting) in 27 %, moving in 5 %.
- **Arrived units are pushed around**: retargets by other units' planning 4 per unit (point order 12.3) and 1.2
  re-activations per arrived unit (point order).
- Glider: 14 speed drops to 0 per unit (one per new route point), speed ratio 0.74.
- A per-unit trace reads like the complaint: `M35 Mr w T38 M2 T15 M16 Mr w T41 M35 Mr w2 T5 M50` (M = moving,
  r = re-plan, w = standing, T = turning on the spot; number = steps).

Prototype E1 (throwaway, not committed: near heading limit only for the final target, far limit 45 deg for
intermediate route points) on the same cases: choke formation mean 74.6 -> 68.3 s, last 99.0 -> 85.2 s, turning
14.1 -> 8.1 s/unit, but collision-blocked steps 64 -> 92, and open/jeep/point got slightly worse (point: 3 instead of
5 arrived). One knob alone moves the problem into the collisions; the packages below must come together
(movement while turning **and** yielding).

## 3. Causes of the stop - turn - drive rhythm

Ranked by measured weight.

1. **One straight segment per plan, re-plan at every route point, no look-ahead** (route.cpp:208-266,
   path_cost.cpp:84-120, move_orders.cpp:641-654). The backtrack stops at the first cell whose line of sight is
   cut by another unit's high-cost disc (route.cpp:243), so in a group the route points are 1-2 field cells apart
   and each new one points somewhere else. Every route point ends in `ROUTE_POINT_REACHED` and a fresh plan.
2. **Turn on the spot instead of driving a curve** (drive_ground.cpp:198-211): any heading error above 45 deg
   zeroes the speed, and inside 6000 Q12 of *any* route point - also an intermediate one - the allowed error shrinks
   to 1.4 deg. With 67 % of segments shorter than 6000 nearly every segment start is a full stop.
3. **Collision = full stop, no yielding** (collision.cpp:60-107, drive_ground.cpp:233-250, drive_common.cpp:88-123).
   A collision is treated like "route point reached": a unit that moved since the last check re-plans at once,
   one that did not waits the whole 64-step retry countdown (move_orders.cpp:634-640). The blocker - most of the
   time a friendly that is itself turning on the spot (cause 2) - is never told to move or to hurry. The
   collision test is symmetric: two overlapping units cannot move apart either.
4. **Other units' planning rewrites my route point and re-activates idle units** (route.cpp:712-716,
   move_orders.cpp:175-203). Each plan of any unit re-stamps and possibly snaps the segments of up to 31
   neighbours; snapped points are cell centres, often behind or beside the unit, so it turns again. Arrived units
   standing in a planned corridor are pushed away (retarget + `ACTIVE`).
5. **Planned segments become walls for units planned later** (route.cpp:694) and standing units' high-cost discs
   cut the line of sight (route.cpp:243, path_cost.cpp:96-106): the later units in selection order get detours and
   short segments.
6. **No group concept** (commands.cpp:215-256): each selected unit plans independently in selection order; with a
   spread above 5.0 / 7.0 (selection_set.cpp:314) all get the same target point, which only a few can reach inside
   the 0x1000 arrival box (move_orders.cpp:665). The others never arrive and keep re-planning (point order: 5/10
   arrive in 100 s, 30 re-plans and 21 s turning per unit).
7. **No hysteresis on re-plans**: a re-plan is triggered by any position change since the last check; there is no
   "my old plan is still fine" test and no minimum time between plans except the 64-step countdown for units that
   did not move.
8. **Glider speed reset on each new route point** (drive_banking.cpp:222-228): 14 restarts from 0 per unit,
   speed ratio 0.74.
9. Minor: bands lag up to 8 steps behind moving units (tick.cpp:169-170); route lines may not leave the explored
   area (reachability.cpp:437), so in unexplored terrain a single unit re-plans about every 9 s (harmless, no stop);
   the speed jumps from full to 0 (no deceleration) which makes the stops look abrupt.

## 4. Constraints for any change

- **Pathing is simulation.** Every peer runs it in lockstep; any change in it breaks multiplayer with the original
  game 1.05 and with older Open Thandor builds, changes the determinism references
  (`tools/test/determinism_reference`), the AI state hash on stromschnelle (`compare_map_hash.py`) and possibly
  campaign balance (AI attack timing, unit losses at choke points).
- Therefore every improvement is an **opt-in pathing mode** (`PathingMode::Original` = 0, `PathingMode::Improved`
  = 1, later more). It is fixed per session (chosen before the session is built, never changed during it). With
  `Original` the code path must be **bit-exact**: the new code sits behind one branch per entry point
  (preferably in its own module, e.g. `src/world/pathing/improved/`), draws no extra random numbers, changes no
  struct layout that is saved or hashed, and the determinism references, the AI hash and the multiplayer test must
  stay identical. Checked by `run_determinism.py --reference` and `run_checks.py` with the mode off.
- Single player: the mode comes from the settings (`thandor.ini`), default to be decided (D1).
- Multiplayer: `Improved` only when **every** peer is Open Thandor with a matching pathing protocol; otherwise
  `Original` is forced. Today the version string in the session title is display only (lobby.cpp:21-30,
  `g_GameVersionUtf16` patched into text 0x211A, lobby.cpp:408) and nothing is negotiated
  (`FRONTEND_PROTOCOL_MAGIC` and the sequence token are the only checks). Proposal, wire format unchanged for the
  original:
  1. Each OT client sets an extra bit in the capability heartbeat 0x10006 (`capabilityFlags`, today only
     `FRONTEND_CAPABILITY_CD` 0x100, lobby.cpp:752) e.g. `0x00010000 OT_EXTENDED` and puts its pathing protocol
     version and supported modes into the packet's 8 reserved bytes (`reserved18_1F`). The original host only tests
     0x100 (lobby.cpp:541-549, player.cpp:621) and stores the dword; extra bits are ignored there.
  2. An OT host that sees the bit from every player (and is OT itself) offers the "improved pathing" option in the
     lobby; otherwise the option is greyed out with the reason.
  3. The host's decision travels with the game start to every client, e.g. in the reserved bytes of the join ack
     0x10003 (`reserved18_1F`) and again in the roster snapshot 0x40008 (`reserved1C` / `reserved2C_37`) so late
     joiners and the start see the same value; a client that is not OT ignores reserved bytes and therefore must
     never be in a session with the mode on (step 2 guarantees that).
  4. Both directions must be tested by hand against the original 1.05 (OT host + original client, original host +
     OT client): the session must start in `Original` and play as today.
  The title version text can additionally show the mode for humans; it must not be parsed.
- **Savegames** must store the mode (and any new per-unit state of the improved mode), so loading restores the
  same behaviour and a multiplayer save stays in sync; the save format is the original's, so this needs an OT
  extension (D3). With the mode off saves stay byte-identical.
- New per-unit state (wait counters, a short path) must not change `ArmyRuntimeSlot`'s layout; use a side table
  indexed by army slot (saved in the extension) or the reserved bytes `reservedE0_EB` only after checking that the
  original never touches them.

## 5. Improvement plan

Ordered by value / risk. Each package is a branch from `dev`, behind `PathingMode::Improved` only. "Effect" is the
expectation from the measured causes; the numbers are to be confirmed by the pending after-runs.

| # | Package | Fixes cause | Expected effect | Effort | Risk |
|---|---|---|---|---|---|
| P0 | **Mode switch and test harness**: `PathingMode` global set at session build (settings for single player, lobby negotiation for multiplayer as in section 4, savegame extension), `run_pathing.py` before/after report with thresholds, determinism references for mode on (new scenario `pathing` in `run_determinism.py`) | - | none by itself | M (lobby part M, rest S) | low; lobby part needs the hand test against 1.05 |
| P1 | **Move while turning**: heading error up to 90 deg -> keep driving with speed scaled by the error (e.g. cos), above 90 deg turn on the spot; near-limit narrowing only for the final target; decelerate instead of jumping to 0 | 2, 9 | turn-on-spot time -40..-60 %; stops by replan -50 % | S | low; units cut corners -> rely on the existing collision test, check narrow passages |
| P2 | **Yield to friendly blockers**: a collision with an own-faction mobile unit that is moving or turning does not set `ROUTE_POINT_REACHED`; wait up to N steps (about 20-30) without re-planning, then re-plan with the blocker as an obstacle; an idle friendly blocker gets a side-step order (perpendicular, away from my direction) instead of the snap of cause 4; head-on: the lower slot index side-steps to the right; allow moving apart when two units already overlap | 3, 4 | collision stops -50 %+, waits of 3.2 s gone | S-M | medium: deadlock rules must be tested (cross and choke cases) |
| P3 | **Arrival for groups**: a unit whose target is occupied by an arrived friendly counts as arrived within (radius sum + margin), or after K failed plans; point orders get formation slots around the point (see P5) | 6 | point order: all arrive, no endless jitter | S | low |
| P4 | **Look-ahead and hysteresis**: keep the backtracked cell chain (2-4 points) instead of only the first visible cell, continue to the next point without stopping (no `ROUTE_POINT_REACHED` at intermediate points), re-plan only when the next point is invalid, the unit was blocked past the P2 budget or the plan is older than T; other units' planning may not rewrite a moving unit's route (only idle units in the way, via P2's side step); string-pulling over the chain with the high-cost mask relaxed for units moving the same way | 1, 4, 5, 7 | re-plans/unit -50 %, segments longer, turn angles smaller | M | medium: more state per unit (side table, saved) |
| P5 | **Group movement**: one plan per selection (leader or centroid), formation slots at the destination (keep relative order; generated slots for point orders), followers follow the leader's chain with their slot offset; ordered passage through choke points by path cost (units behind wait via P2 instead of re-planning) | 5, 6 | choke: last arrival close to single-unit time + queue time | L | medium; touches selection commands and AI orders |
| P6 | **Glider speed**: keep speed on a new route point when the direction change is small | 8 | glider speed ratio 0.74 -> about 0.9 | XS | low |
| P7 | **Flow fields for large groups**: the propagation already computes the full cost field from the target; cache it per (target cell, radius class) for groups above N units and let units follow the gradient plus local separation | 1, 5, 6 at scale | big groups and long distances | L | medium-high (performance: grid copies of 2+ MB per plan, cache invalidation) |
| P8 | Influence at the predicted position / refresh on plan | 9 | small | S | low |

Order: P0 -> P1 + P2 + P6 together (E1 showed that P1 alone shifts the stops into collisions) -> P3 -> P4 -> P5
-> P7, P8 when needed.

Testing per package:

1. `python tools/test/run_pathing.py <game dir> --cases single,open,choke,chokepoint,cross,jeep,walker,glider
   --save after.json --compare baseline.json` with the mode on: acceptance e.g. choke formation mean arrival <= 60 s,
   point order 10/10 arrive, stops/unit and turn s/unit at least halved, no new stuck unit; with the mode off the
   numbers must equal the baseline exactly.
2. `run_determinism.py --reference` and `run_checks.py` with the mode off: unchanged.
3. Mode on: two runs must match each other (`run_determinism.py` without reference, new scenario `pathing`), a
   saved reference for the mode, and `run_multiplayer.py` host + client both OT with the mode on (identical state
   hash), plus the hand test against the original 1.05 (mode forced off).
4. Campaign spot checks with the mode on (`run_all_maps.py`), AI behaviour by eye (it uses the same movement).

## 6. Owner decisions needed

- **D1** Default of the improved mode in single player and in the campaign (balance: the campaign was tuned with the
  original movement) - on, off, or skirmish only?
- **D2** Multiplayer negotiation as proposed (heartbeat capability bit + reserved bytes, host lobby toggle,
  forced off with any non-OT peer) - or simpler: automatically on when all peers are OT of the same version?
- **D3** Savegame extension for the mode and the per-unit state (an OT block after the original data?) and how an
  old OT build treats such a save.
- **D4** Visual change: tanks driving curves instead of turning on the spot (P1) - acceptable for the look of the
  game?
- **D5** Should the AI use the improved movement too (follows automatically from the mode; changes AI strength)?
- **D6** Scope order: P0-P3 first as a small release, P4/P5 later, P7 only on demand?
- **D7** Keep the movement trace (`OPEN_THANDOR_PATHTRACE`), the `pathing` arena and `run_pathing.py` in the
  repository (they are dev-tools only and committed on this branch)?

## Tools added on this branch

- `OPEN_THANDOR_PATHTRACE=<file>` (developer tools only; `src/platform/debug/pathtrace.cpp`,
  `include/thandor/platform/debug/pathtrace.h`): per step one line per mobile unit of a faction, info lines with the
  steering values, and events from `DebugHook_PathingEvent` in route.cpp (re-plan 'R', retarget 'T'),
  drive_ground.cpp / drive_banking.cpp / walker.cpp (collision 'B', turn-on-spot stop 'H'). In a build without the
  developer tools the hook is an empty macro. Checked: the determinism references still match with the hooks
  (`run_determinism.py --reference`: battle, turrets, production ok).
- `make_arena.py --scenario pathing [--unit-id] [--count] [--layout open|choke|cross|crosschoke] [--gap]
  [--wall-id] [--wall-step]`, catalog slot `level03.dat`.
- statehash.cpp: `OPEN_THANDOR_ARENA_ORDERS=pathing` with `OPEN_THANDOR_ARENA_PATHING=formation|point|cross` and
  `OPEN_THANDOR_ARENA_PATHING_ROWS`.
- `tools/test/run_pathing.py`: runs the cases in parallel linked copies `<game dir>_path<k>` and prints the metrics
  of section 2 (`--save`, `--compare`, `--analyse <trace>`, `--units`, `--env NAME=VALUE`).

## Pending measurements

Stopped on 2026-10-10 because the PC was needed; to run in a later batch (build with
`python ot-scratch/build_gate.py <worktree>`, game dir = a linked copy of `ot-soak`, never Program Files):

1. Baseline with the committed tool (the numbers above were taken with an earlier revision of the evaluation; only
   the evaluation window changed):
   `python tools/test/run_pathing.py <ot-soak> --cases single,open,choke,chokepoint,cross,jeep,walker,glider --save baseline.json --units`
2. Prototype variants of P1/P2 (E2 = friendly-blocker wait of 30 steps without `ROUTE_POINT_REACHED`, E4 = drive at
   half speed while the heading error is below 90 deg, E7 = E1+E2+E4), to size the packages before P0 is built.
   The prototype was a throwaway patch of `ArmyGroundMovement_SteerAndDrive` read from `OT_PATH_EXP`; redo it on a
   scratch branch and run the command of 1 with `--env OT_PATH_EXP=<n> --compare baseline.json`.
3. Visual check: the choke case visible (`OPEN_THANDOR_TEST_VISIBLE=1`, >= 1080p window) with the game's own
   screenshots (`shot` script lines or `OPEN_THANDOR_AUTOSHOT`).
