"""font.gfx @ (0x40) at 3x / 2x built from the font's own O (work_gat): uniform stroke M.W (3x 4 px, 2x 3 px).
Same open form as the fontk @ (ksym_parts.g_at): ring = O arcs stretched, open at the lower right, its right wall curving
into the foot of the inner a (round bowl on the left, straight stem on the right; the O has too few flat lines to
shrink to the a's size, so the bowl is drawn with circular arcs, outer radius r, inner r - W). The ink box is the one of the v7 @.
fontk.gfx is not touched."""
import numpy as np

AT = 0x40
SS = 16
CODES = (AT,)
# ring: top row, left column, width, height; inner a: width, height, outer radius (gap ring - a: 3x 3 px, 2x 2 px on all sides)
GEO = {3: dict(top=10, left=0, w=34, h=32, aw=20, ah=18, r=7), 2: dict(top=6, left=0, w=23, h=22, aw=13, ah=12, r=5)}


def applies(M, code):
    return (code == AT and M.N in GEO and M.W == M.N + 1 and M.g[ord('A')].shape[0] == 15
            and M.g[code].shape == (15, 12))


def _ink_crop(r):
    rows = np.nonzero(r.max(axis=1) > 0.02)[0]
    cols = np.nonzero(r.max(axis=0) > 0.02)[0]
    return r[rows[0]:rows[-1] + 1, cols[0]:cols[-1] + 1]


def _flat(a):
    """first / last index of the run of identical lines around the middle of a (lines along axis 0)."""
    m = a.shape[0] // 2
    i, j = m, m
    while i > 0 and np.abs(a[i - 1] - a[m]).max() < 1e-6:
        i -= 1
    while j < a.shape[0] - 1 and np.abs(a[j + 1] - a[m]).max() < 1e-6:
        j += 1
    return i, j


def _fit_axis(a, n):
    """a resized to n lines by repeating / removing identical middle lines only (arcs and stroke unchanged)."""
    d = n - a.shape[0]
    i, j = _flat(a)
    if d >= 0:
        return np.insert(a, [i] * d, a[i], axis=0)
    assert j - i + 1 >= -d, 'not enough flat lines'
    return np.delete(a, range(i, i - d), axis=0)


def _fit(a, w, h):
    return _fit_axis(_fit_axis(a, h).T, w).T


def _inner_a(aw, ah, W, r):
    """bowl with circular left corners (outer radius r, inner r - W), flat bands into a straight stem on the right."""
    y, x = (np.mgrid[0:ah * SS, 0:aw * SS] + 0.5) / SS

    def rounded_left(x0, y0, x1, y1, rad):
        m = (x >= x0) & (x < x1) & (y >= y0) & (y < y1)
        for cy, sy in ((y0 + rad, -1), (y1 - rad, 1)):
            corner = (x < x0 + rad) & ((y - cy) * sy > 0)
            m &= ~corner | ((x - x0 - rad) ** 2 + (y - cy) ** 2 <= rad * rad)
        return m
    m = rounded_left(0, 0, aw, ah, r) & ~rounded_left(W, W, aw - W, ah - W, r - W)
    return m.reshape(ah * SS, aw * SS).reshape(ah, SS, aw, SS).mean(axis=(1, 3))


def g_at(M, code):
    import upscale3 as S
    N, W = M.N, M.W
    h, w = M.g[code].shape[0] * N, M.g[code].shape[1] * N
    G = GEO[N]
    O = _ink_crop(S.render(M, ord('O')))
    top, left, rw, rh, aw, ah = G['top'], G['left'], G['w'], G['h'], G['aw'], G['ah']
    ay, ax = top + (rh - ah) // 2, left + (rw - aw) // 2
    a = _inner_a(aw, ah, W, G['r'])
    out = np.zeros((h, w))
    # left half of the ring + bottom band, square cut before the lower right arc
    ring = _fit(O, rw, rh)
    yfoot = ay + ah - top                       # ring rows down to the foot of the a
    xcut = rw - (2 * W + 3)                     # bottom band ends below the stem, as in the fontk @ (before the arc)
    keep = ring.copy()
    keep[yfoot:, xcut:] = 0.0
    keep[:yfoot, rw // 2:] = 0.0
    out[top:top + rh, left:left + rw] = keep
    # top band, right wall and lower right arc (O arcs, ring only as high as the foot); the arc runs into the stem
    up = _fit(O, rw, yfoot)
    up[:, :rw // 2] = 0.0
    up[yfoot - W:, :ax + aw - W - left] = 0.0
    out[top:top + yfoot, left:left + rw] = np.maximum(out[top:top + yfoot, left:left + rw], up)
    out[ay:ay + ah, ax:ax + aw] = np.maximum(out[ay:ay + ah, ax:ax + aw], a)
    return out


def render(M, code):
    return g_at(M, code) if applies(M, code) else None
