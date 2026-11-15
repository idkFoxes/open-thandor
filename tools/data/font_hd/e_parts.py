"""lowercase e = the o's ring (same superellipse arc style, concentric outer / inner contour -> uniform stroke W)
+ flat crossbar, opening at the bottom right. Box and crossbar rows from the contour build (metrics unchanged);
superellipse exponent fitted to the finished o so the e's arcs read like the o's."""
import numpy as np

SS = 8


def _ink_box(r, t=0.5):
    rows = np.nonzero(r.max(axis=1) > t)[0]
    cols = np.nonzero(r.max(axis=0) > t)[0]
    return rows[0], rows[-1] + 1, cols[0], cols[-1] + 1


def _grid(h, w):
    ys = (np.arange(h * SS) + 0.5) / SS
    xs = (np.arange(w * SS) + 0.5) / SS
    return np.meshgrid(ys, xs, indexing='ij')


def _se(Y, X, cy, cx, a, b, p):
    return (np.abs((X - cx) / a) ** p + np.abs((Y - cy) / b) ** p) <= 1.0


def _down(m, h, w):
    return m.reshape(h, SS, w, SS).mean(axis=(1, 3))


def _fit_p(o, W):
    y0, y1, x0, x1 = _ink_box(o)
    h, w = o.shape
    Y, X = _grid(h, w)
    cy, cx, a, b = (y0 + y1) / 2, (x0 + x1) / 2, (x1 - x0) / 2, (y1 - y0) / 2
    best = None
    for p in np.arange(2.0, 4.01, 0.1):
        m = _se(Y, X, cy, cx, a, b, p) & ~_se(Y, X, cy, cx, a - W, b - W, p)
        e = np.abs(_down(m, h, w) - o).sum()
        if best is None or e < best[0]:
            best = (e, p)
    return best[1]


def render_e(M, base, o):
    """base: the contour-built e (box / crossbar source), o: the finished o. Returns the new raster or None."""
    W = M.W
    h, w = base.shape
    y0, y1, x0, _ = _ink_box(base)
    # crossbar: rows in the middle third that are solid across the body; body right edge = crossbar's right end
    widest = (base[y0:y1] > 0.5).sum(axis=1).max()
    bars = [y for y in range(y0 + W, y1 - W) if (base[y] > 0.5).sum() >= widest - 1]
    if not bars:
        return None
    cb0, cb1 = bars[0], bars[-1] + 1
    x1 = int(np.nonzero(base[cb0] > 0.5)[0][-1]) + 1
    p = _fit_p(o, W)
    Y, X = _grid(h, w)
    cy, cx, a, b = (y0 + y1) / 2, (x0 + x1) / 2, (x1 - x0) / 2, (y1 - y0) / 2
    outer = _se(Y, X, cy, cx, a, b, p)
    inner = _se(Y, X, cy, cx, a - W, b - W, p)
    ring = outer & ~inner
    m = ring.copy()
    m |= outer & (Y >= cb0) & (Y < cb1)                         # crossbar, ends in the ring on both sides
    y_cut = cy + b - W                                          # inner bottom: terminal of the bottom stroke
    m &= ~((Y >= cb1) & (Y < y_cut) & (X > cx))                 # opening at the bottom right
    r = _down(m, h, w)
    r[r < 1.0 / (SS * SS) + 1e-9] = 0.0
    return np.clip(r, 0.0, 1.0)


def applies(M):
    """only a real lowercase e (fontk has capitals in the lowercase slots)"""
    def top(c):
        rr = np.nonzero(M.g[c].max(axis=1) > 0.3)[0]
        return rr[0] if len(rr) else None
    te, tE = top(ord('e')), top(ord('E'))
    return te is not None and tE is not None and te > tE + 1
