"""# (0x23) = two parallel / + two bars (work_hash, work_hash2).
Slashes: stroke_raster like the finished / (22.5 deg, uniform stroke M.W, horizontal ends), each centre line through
a lattice point, so the glyph maps onto itself under the 180 deg rotation about (yc, xc).
font.gfx (work_hash2): the # stands on the baseline and tops out at cap height, both read from the finished H. The
slashes are shortened to that range, not squashed. Bars have the thickness and length of the finished =, placed
symmetrically in the glyph so that the middle counter is about as tall as the free sections above and below.
fontk.gfx: as before, bars on the rows of its =, slash height HALF_H.
From the original #: ink height 3.1..11.9 (8.8 px), bars ink 0.5..8.2 (centre 4.35), slash spacing 2.8 px."""
import numpy as np
import upscale3 as S
import upscale4 as U4

HASH = 0x23
HALF_H = 4.4                # half height of the slashes (source px), fontk.gfx
XC = 4.35                   # horizontal centre of the glyph (source px)
SPACING = 2.8               # horizontal distance of the slash centre lines (source px)


def ink_rows(r):
    return np.nonzero(r.max(axis=1) > 0.5)[0]


def bars_in_range(top, bot, t):
    """bars of thickness t, symmetric in the output rows [top, bot): counter gap as close as possible to a third of
    the free rows, with the parity that keeps the rows above and below equal. Returns ((y0, y1), (y0, y1))."""
    free = bot - top - 2 * t
    gap = int(round(free / 3.0))
    if (gap - free) % 2:
        gap += 1 if free / 3.0 > gap else -1
    outer = (free - gap) // 2
    return (top + outer, top + outer + t), (bot - outer - t, bot - outer)


def render(M, code, eq):
    """eq = finished render of '=' (same model). Returns the # raster or None."""
    if code != HASH:
        return None
    N, W = M.N, M.W
    h, w = M.g[code].shape
    rows = ink_rows(eq)
    cols = np.nonzero(eq.max(axis=0) > 0.5)[0]
    gaps = np.nonzero(np.diff(rows) > 1)[0]
    if len(gaps) != 1:
        return None
    upper = (rows[0], rows[gaps[0]] + 1)                  # output rows [y0, y1) of the bars
    lower = (rows[gaps[0] + 1], rows[-1] + 1)
    if M.g[ord('A')].shape[0] == h:                       # font.gfx: from the cap top to the baseline, like H
        cap = ink_rows(S.render(M, ord('H')))
        y0, y1 = int(cap[0]), int(cap[-1]) + 1
        upper, lower = bars_in_range(y0, y1, upper[1] - upper[0])
        yc = (y0 + y1) / 2.0
    else:
        yc = (upper[0] + lower[1]) / 2.0                  # fontk.gfx: the rotation centre lies on the = centre
        half = int(round(HALF_H * N))
        y0, y1 = yc - half, yc + half
    xc = int(round(XC * N))
    s = int(round(SPACING * N / 2.0)) * 2                 # even: both centre lines hit whole px on yc
    L = cols[-1] - cols[0] + 1
    L += L % 2                                            # even, centred on the whole px xc
    e = 2.0 * W
    ya, yb = y0 - e, y1 + e
    strokes = []
    for dx in (-s / 2.0, s / 2.0):
        xm = xc + dx
        pts = [(ya / N, (xm + (yc - ya) * U4.T225) / N), (yb / N, (xm - (yb - yc) * U4.T225) / N)]
        strokes.append(U4.stroke_raster(M, (h, w), pts, clip=(y0 / N, y1 / N)))
    r = U4.lo(U4.union(*strokes)).copy()
    x0, x1 = xc - L // 2, xc + L // 2
    for b0, b1 in (upper, lower):
        r[b0:b1, x0:x1] = 1.0
    return r
