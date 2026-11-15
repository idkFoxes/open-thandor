"""Diagonal topics (K/k junction, continuous diagonals): extra stroke glyphs + an automatic check.
Registered into upscale4.STROKE (see upscale4.render). Only depends on upscale4 helpers."""
import math
import numpy as np
import upscale4 as U4
from upscale4 import stroke_raster, union, mirror_x, mirror_y, T225

# every diagonal centre line used by the stroke glyphs is recorded here (for the width check)
DIAG_LOG = []


def ring(M, g, shape, x0, x1, y0, y1):
    """closed rounded rectangle stroke (centre-line box x0..x1, y0..y1), small corner radius."""
    xm = (x0 + x1) / 2
    pts = [(y0, xm), (y0, x1), (y1, x1), (y1, x0), (y0, x0), (y0, xm + 1e-3)]
    # the small radius, but never more than half of the shorter side (otherwise the arcs overlap at 2x)
    side = min(x1 - x0, y1 - y0) * M.N
    R = min(float(M.r_small), side / 2 + M.W / 2 - 0.01)
    return stroke_raster(M, shape, pts, rad=[R, R, R, R])


def g_percent(M, g, shape):
    """% : two rings and one straight 22.5 deg stroke (same angle as / )."""
    top = ring(M, g, shape, g.sc(0.85), g.sc(3.55), g.sc(3.9), g.sc(6.7))
    bot = ring(M, g, shape, g.sc(6.95), g.sc(9.65), g.sc(7.9), g.BASc)
    y0, y1 = g.TOPe, g.BASe
    ym, xm = (y0 + y1) / 2, 5.0
    e = 2 * g.w
    ya, yb = y0 - e, y1 + e
    sl = stroke_raster(M, shape, [(ya, xm + (ym - ya) * T225), (yb, xm - (yb - ym) * T225)], clip=(y0, y1))
    return union(top, bot, sl), []


STROKE = {ord('%'): g_percent}


# ------------------------------------------------------------------ check: cross-section width along diagonals
def _bilin(r, y, x):
    h, w = r.shape
    y -= 0.5
    x -= 0.5
    iy, ix = int(math.floor(y)), int(math.floor(x))
    if iy < 0 or ix < 0 or iy + 1 >= h or ix + 1 >= w:
        return 0.0
    fy, fx = y - iy, x - ix
    return (r[iy, ix] * (1 - fy) * (1 - fx) + r[iy + 1, ix] * fy * (1 - fx) + r[iy, ix + 1] * (1 - fy) * fx
            + r[iy + 1, ix + 1] * fy * fx)


def cross_widths(r, p0, p1, W, step=0.5, margin=None):
    """perpendicular ink width (output px) at points along the segment p0 -> p1 (output px, (y, x)),
    skipping `margin` px at both ends (junctions). Width = integral of coverage across the stroke."""
    p0, p1 = np.array(p0, float), np.array(p1, float)
    d = p1 - p0
    L = np.linalg.norm(d)
    d /= L
    n = np.array([d[1], -d[0]])
    m = margin if margin is not None else 1.5 * W
    out = []
    t = m
    while t <= L - m:
        c = p0 + d * t
        s = 0.0
        u = -2.0 * W
        while u <= 2.0 * W:
            q = c + n * u
            s += _bilin(r, q[0], q[1]) * 0.25
            u += 0.25
        # only clean cross sections: no other ink just outside the stroke on both sides
        side = max(_bilin(r, *(c + n * (W / 2 + 1.5))), _bilin(r, *(c - n * (W / 2 + 1.5))))
        if side < 0.05:
            out.append(s)
        t += step
    return np.array(out)
