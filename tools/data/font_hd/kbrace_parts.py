"""fontk.gfx slots 0x7B..0x7E: the pixels in these slots are shifted against ASCII, the upscale shows what the ORIGINAL
pixels show (code meaning ignored). Hook runs before pairs_parts.

  0x7E  real { (source: stem, hooks to the right, point to the left)  -> brace_open(), even stroke W
  0x7B  } (source: same brace, point to the right)                    -> pixel-identical horizontal mirror of 0x7E
  0x7C  tilde (source: one wave)                                      -> one sine period, even stroke W
  0x7D  house (source: straight walls, floor, pointed roof)           -> walls / floor / two straight roof strokes
0x7F (matches the source) is not touched. All coordinates are source px; the tables hold values snapped per N so that
the axis-aligned strokes sit on whole output pixels (3x 3 px, 2x 2 px)."""
import math
import numpy as np
import upscale4 as U4
import pairs_parts as P

CODES = (0x7B, 0x7C, 0x7D, 0x7E)
OX = 3                               # canvas margin (source px)
# brace in the frame of 0x7E: output rows [y0, y1), stem centre xs, point run k (45 deg), hook end xr (relative to the ink
# left edge = 0), output column of the ink left edge in 0x7E / 0x7B
BRACE = {3: dict(rows=(6, 41), xs=5.5 / 3, k=4 / 3, xr=12 / 3, x7e=1, x7b=2),
         2: dict(rows=(5, 27), xs=1.5, k=1.0, xr=8 / 2, x7e=1, x7b=1)}


def applies(M, code):
    return code in CODES and M.g[ord('A')].shape[0] != 15 and M.N in BRACE


def brace_open(M):
    """opening brace raster, full height of the 0x7B cell: hooks with r_small, 45 deg point, vertically symmetric."""
    N, w = M.N, M.W / M.N
    p = BRACE[N]
    y0, y1 = p['rows'][0] / N, p['rows'][1] / N
    yc = (y0 + y1) / 2
    shape = (M.g[0x7B].shape[0], M.g[0x7B].shape[1] + 2 * OX)
    xs, k, xr = OX + p['xs'], p['k'], OX + p['xr']
    xp = xs - k
    ext = w
    pts = [(y0 + w / 2, xr), (y0 + w / 2, xs), (yc - k, xs), (yc + ext, xp - ext)]
    r = U4.union(U4.stroke_raster(M, shape, pts, rad=['small', 'small']))
    r[int(math.ceil(yc * N)):] = 0
    r = U4.mirror_y(r, yc * N)
    r[:, :int(round((xp - w / 2) * N))] = 0             # blunt point
    r[:, int(round(xr * N)):] = 0
    return r


def g_brace(M, code):
    N = M.N
    a = M.g[code]
    h, w = a.shape[0] * N, a.shape[1] * N
    r = brace_open(M)
    if code == 0x7E:
        return P.place(r[:h], h, w, BRACE[N]['x7e'])
    out = P.place(r[:, ::-1], r.shape[0], w, BRACE[N]['x7b'])
    return np.vstack([np.zeros((N, w)), out[:h - N]])         # source 0x7B sits one row lower than 0x7E


# ------------------------------------------------------------------ house
ROOF_T = U4.T3375                     # roof strokes, dx/dy (angle of X / 7; measured in the source: 0.65-0.75)
HOUSE = {3: dict(xc=4.0), 2: dict(xc=4.0)}
ROOF_Y, EAVE_Y, FLOOR_Y = 5.0, 9.0, 12.0   # flat apex cut, eave cut, bottom edge (source px)
WALL_L, WALL_R = 1.0, 7.0                  # outer wall edges


def g_house(M, code):
    N, W = M.N, M.W
    a = M.g[code]
    h, w = a.shape[0] * N, a.shape[1] * N
    xc = HOUSE[N]['xc']
    t, half = ROOF_T, (W / N) / 2 / math.cos(math.atan(ROOF_T))
    ye = EAVE_Y + 1.0
    pts = [(ye, xc - (ye - ROOF_Y) * t), (ROOF_Y, xc), (ye, xc + (ye - ROOF_Y) * t)]
    r = U4.union(U4.stroke_raster(M, a.shape, pts, clip=(ROOF_Y, EAVE_Y)))
    # walls start where the roof's outer edge has passed the wall's outer edge (no step on the outside)
    wt = int(math.ceil((ROOF_Y + (xc - WALL_L - half) / t) * N))
    xl, xr = int(round(WALL_L * N)), int(round(WALL_R * N))
    yb = int(round(FLOOR_Y * N))
    r[wt:yb, xl:xl + W] = 1
    r[wt:yb, xr - W:xr] = 1
    r[yb - W:yb, xl:xr] = 1
    return r


# ------------------------------------------------------------------ tilde
TILDE = {3: dict(x=(0.0, 26 / 3), cy=8.35, a=0.8), 2: dict(x=(0.0, 8.5), cy=8.35, a=0.8)}


def g_tilde(M, code):
    N = M.N
    a = M.g[code]
    p = TILDE[N]
    x0, x1 = p['x']
    n = 96
    # butt ends cut vertically at x0, x1: the centreline runs a little past them
    e = (x1 - x0) * 0.04
    xs = [x0 - e + (x1 - x0 + 2 * e) * i / n for i in range(n + 1)]
    pts = [(p['cy'] - p['a'] * math.sin(2 * math.pi * (x - x0) / (x1 - x0)), x) for x in xs]
    r = U4.lo(U4.stroke_raster(M, a.shape, pts))
    r[:, :int(round(x0 * N))] = 0
    r[:, int(round(x1 * N)):] = 0
    return r


def render(M, code):
    if not applies(M, code):
        return None
    if code in (0x7B, 0x7E):
        return g_brace(M, code)
    return g_house(M, code) if code == 0x7D else g_tilde(M, code)
