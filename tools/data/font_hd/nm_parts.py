"""N / M: rectangular stems + straight diagonals of uniform width W (the contour path merged the diagonal with the
stems: slit along the stem, vertical notch, grey pixels in the M vertex, in fontk a solid block with a cross bar).
Stem columns are the ones of the contour version. Diagonals: rational slope t = dx/dy (repeating AA pattern),
horizontal width W*sqrt(1+t^2), so the perpendicular width is exactly W. N is point symmetric (exact pixel copy),
M mirror symmetric. Clipped flat at cap height and baseline. fontk: n = N and m = M (identical originals)."""
import math
import numpy as np
import upscale4 as U4

# font -> (y0 src, y1 src, N: xl, xr, t, M: xl, xr, t)
TAB = {15: (None, None, 1.3, 7.6, {3: 0.8, 2: 0.75}, 1.3, 9.3, 0.5),
       9: (2.0, 8.0, 0.5, 3.5, 0.5, 0.5, 4.5, 1 / 3.0)}


def _stem(M, shape, x, y0, y1):
    return U4.stroke_raster(M, shape, [(y0, x), (y1, x)])


def _diag(M, shape, x_at_y0, t, y0, y1, g, xa, xb):
    """stroke whose centre line is x_at_y0 + t * (y - y0) (source px), cut flat at y0 / y1 and at the outer stem
    edges xa / xb (nothing sticks out of the stems)."""
    e = 2 * g.w
    d = U4.stroke_raster(M, shape, [(y0 - e, x_at_y0 - e * t), (y1 + e, x_at_y0 + (y1 - y0 + e) * t)], clip=(y0, y1))
    d.m[:, :int(round(xa * g.N)) * U4.SSU] = False
    d.m[:, int(round(xb * g.N)) * U4.SSU:] = False
    return d


def _params(M, g):
    y0t, y1t, nl, nr, nt, ml, mr, mt = TAB[M.g[ord('A')].shape[0]]
    y0 = g.TOPe if y0t is None else g.se(y0t)
    y1 = g.BASe if y1t is None else g.se(y1t)
    return y0, y1, nl, nr, nt, ml, mr, mt


def g_N(M, g, shape):
    y0, y1, nl, nr, t, _, _, _ = _params(M, g)
    t = t[g.N] if isinstance(t, dict) else t
    xl, xr = g.sc(nl), g.sc(nr)
    C = (xl + xr) / 2                                    # horizontal centre, diagonal runs through the glyph centre
    yc = (y0 + y1) / 2
    r = U4.union(_stem(M, shape, xl, y0, y1), _stem(M, shape, xr, y0, y1),
                 _diag(M, shape, C - (yc - y0) * t, t, y0, y1, g, xl - g.w / 2, xr + g.w / 2))
    # exact 180 degree rotation: lower half = rotated upper half
    ky, kx = int(round((y0 + y1) * g.N)) - 1, int(round((xl + xr) * g.N)) - 1
    ymid = int(round(yc * g.N))
    for y in range(ymid, int(round(y1 * g.N))):
        row = np.zeros(r.shape[1])
        for x in range(r.shape[1]):
            if 0 <= kx - x < r.shape[1]:
                row[x] = r[ky - y, kx - x]
        r[y] = row
    return r, []


def g_M(M, g, shape):
    y0, y1, _, _, _, ml, mr, t = _params(M, g)
    xl, xr = g.sc(ml), g.sc(mr)
    C = (xl + xr) / 2
    hw = g.w * math.sqrt(1 + t * t)                      # horizontal width of the diagonal (source px)
    a = g.w / 2                                          # flat vertex on the baseline: as wide as a stroke (W)
    # lower edge of the left diagonal reaches C - a at the baseline; its centre line is hw / 2 to the right of that
    cx_y0 = C - a + hw / 2 - (y1 - y0) * t
    xa, xb = xl - g.w / 2, xr + g.w / 2
    r = U4.union(_stem(M, shape, xl, y0, y1), _stem(M, shape, xr, y0, y1),
                 _diag(M, shape, cx_y0, t, y0, y1, g, xa, xb), _diag(M, shape, 2 * C - cx_y0, -t, y0, y1, g, xa, xb))
    Cx = C * g.N
    return U4.mirror_x(r, Cx), [(1, Cx)]


def render(M, code):
    """returns (raster, axes) or None."""
    h = M.g[ord('A')].shape[0]
    if h not in TAB or M.g[code].shape[0] != h:
        return None
    if h == 15:
        fn = {ord('N'): g_N, ord('M'): g_M}.get(code)
    else:
        fn = {ord('N'): g_N, ord('n'): g_N, ord('M'): g_M, ord('m'): g_M}.get(code)
    if fn is None:
        return None
    return fn(M, U4.G(M), M.g[code].shape)
