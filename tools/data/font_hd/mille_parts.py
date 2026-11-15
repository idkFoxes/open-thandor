"""per mille (0x89) = the finished small 0 of fontk.gfx three times + one straight 22.5 deg slash.
Layout from the original glyph: 0 top left, slash, two 0 at the bottom; the rows of the top and the bottom zeros
overlap by one original row. Stroke width of zeros and slash = the fontk stroke (3x: 3 px, 2x: 2 px).
The fontk 0 (18 / 12 rows) is too tall for two stacked zeros, so its flat middle rows are removed (arcs and
width stay, same way the big 0 was derived from O)."""
import copy
import numpy as np
import upscale4 as U4
from upscale4 import stroke_raster, lo, T225

CODE = 0x89
#        output px per scale: zero height, top row of the digits, bottom edge, x of the three zeros
LAYOUT = {3: dict(h=14, y0=10, y1=35, xs=(1, 19, 34)),
          2: dict(h=10, y0=6, y1=24, xs=(0, 13, 23))}
_zero = {}


def small_zero(N):
    """fontk 0 at scale N, cropped to its ink, flat middle rows removed down to LAYOUT height. Cached per scale."""
    if N not in _zero:
        import dumpcur as D
        z = D.S.render(D.model('fontk.gfx', N), ord('0'))
        rows = np.nonzero(z.max(axis=1) > 0)[0]
        cols = np.nonzero(z.max(axis=0) > 0)[0]
        z = z[rows.min():rows.max() + 1, cols.min():cols.max() + 1]
        cut = z.shape[0] - LAYOUT[N]['h']
        mid = z.shape[0] // 2
        a, b = mid - (cut + 1) // 2, mid + cut // 2       # rows a..b-1 are dropped, they must be flat
        if cut < 0 or not all(np.array_equal(z[i], z[a - 1]) for i in range(a, b + 1)):
            raise ValueError('fontk 0 %dx: mittlere Zeilen nicht flach' % N)
        _zero[N] = np.vstack([z[:a], z[b:]])
    return _zero[N]


def compose(M, code, L, zeros):
    """zeros at the (x, y) in `zeros` + the 22.5 deg slash through the middle of the free gap between the top zero and
    the bottom-left zero (xs[0], xs[1] of layout L), clipped to the digit zone. Shared with percent_parts."""
    N = M.N
    z = small_zero(N)
    zh, zw = z.shape
    h, w = M.g[code].shape[0] * N, M.g[code].shape[1] * N
    y0, y1 = L['y0'], L['y1']
    xs = L['xs']
    r = np.zeros((h, w))
    for x, y in zeros:
        r[y:y + zh, x:x + zw] = z
    ym = (y1 - zh + y0 + zh) / 2.0
    xm = (xs[0] + zw + xs[1]) / 2.0
    e = 2.0 * N
    ya, yb = y0 - e, y1 + e
    S = copy.copy(M)
    S.W = N                                               # small-digit stroke: 3x 3 px, 2x 2 px
    pts = [(ya / N, (xm + (ym - ya) * T225) / N), (yb / N, (xm - (yb - ym) * T225) / N)]
    sl = lo(stroke_raster(S, (M.g[code].shape[0], M.g[code].shape[1]), pts, clip=(y0 / N, y1 / N)))
    return np.maximum(r, sl)


def render(M, code):
    N = M.N
    if code != CODE or N not in LAYOUT or M.g[code].max() <= 0:
        return None
    L = LAYOUT[N]
    zh, zw = small_zero(N).shape
    y0, y1, xs = L['y0'], L['y1'], L['xs']
    r = compose(M, code, L, ((xs[0], y0), (xs[1], y1 - zh), (xs[2], y1 - zh)))
    return r, '0x89 = 3 x fontk-0 (%dx%d) + 22.5-Grad-Strich %d px (mille_parts)' % (zw, zh, N)
