"""German sharp s (0xDF) for font.gfx, built from parts (work_sz).
stem = plain rectangle (B's stem shifted 1 source px right, as in the original), top-left arc with the big
radius, then two B-style right bowls (rounded-rectangle rings, small radius, inner radius = r - W), upper
bowl smaller and narrower; waist centred at source row 7 like the original. Vertical extent = B's ink rows."""
import numpy as np

SZ = 0xDF
SS = 8


def _rr_inside(X, Y, x0, y0, x1, y1, r):
    """points inside the box [x0,x1]x[y0,y1] with corner radii r = (tl, tr, br, bl)."""
    m = (X >= x0) & (X <= x1) & (Y >= y0) & (Y <= y1)
    for (cx, cy, sx, sy), rad in zip(((x0, y0, 1, 1), (x1, y0, -1, 1), (x1, y1, -1, -1), (x0, y1, 1, -1)), r):
        if rad <= 0:
            continue
        ccx, ccy = cx + sx * rad, cy + sy * rad
        corner = ((X - ccx) * sx < 0) & ((Y - ccy) * sy < 0)
        m &= ~corner | ((X - ccx) ** 2 + (Y - ccy) ** 2 <= rad * rad)
    return m


def _ring(X, Y, x0, y0, x1, y1, r, W):
    inner = _rr_inside(X, Y, x0 + W, y0 + W, x1 - W, y1 - W, tuple(max(0, v - W) for v in r))
    return _rr_inside(X, Y, x0, y0, x1, y1, r) & ~inner


def render(M, code, B):
    """B = finished render of 'B' (same model). Returns the ß raster or None."""
    if code != SZ or M.g[ord('A')].shape[0] != 15 or M.g[SZ].shape != M.g[ord('B')].shape:
        return None
    N, W = M.N, M.W
    h, w = M.g[SZ].shape[0] * N, M.g[SZ].shape[1] * N
    rows = np.nonzero(B.max(axis=1) > 0.5)[0]
    cols = np.nonzero(B.max(axis=0) > 0.5)[0]
    top, bot = rows[0], rows[-1] + 1                 # B's ink rows (top / bottom edge)
    xs = cols[0] + N                                 # stem: B's stem 1 source px further right (original)
    xr_up = int(round(7.3 * N))                      # right edges from the original (upper bowl narrower)
    xr_lo = int(round(7.9 * N))
    ym = int(7.25 * N - W / 2.0 + 0.5)              # waist bar [ym, ym+W), centre at source row 7.25
    rb, rs = M.r_big, M.r_small
    y, x = (np.mgrid[0:h * SS, 0:w * SS] + 0.5) / SS
    m = _ring(x, y, xs, top, xr_up, ym + W, (rb, rs, rs, 0), W)          # top arc + upper bowl
    m |= _ring(x, y, xs, ym, xr_lo, bot, (0, rb, rs, 0), W)              # lower bowl (big arc out of the waist)
    m |= (x >= xs) & (x <= xs + W) & (y >= top + rb) & (y <= bot)        # stem (plain rectangle)
    return m.reshape(h, SS, w, SS).mean(axis=(1, 3))
