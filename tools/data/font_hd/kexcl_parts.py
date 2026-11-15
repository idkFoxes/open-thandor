"""fontk.gfx ! inverted ! and *, 3x and 2x only, stroke width M.W (3x 3 px, 2x 2 px); font.gfx is not touched.
The contour path lost the dot of ! / inverted ! and filled * to a hexagon.
 !  rectangular stem + W x W dot exactly under it, gap N; same output rows as the fontk ? (kqvw_parts.Q).
 inverted !  exact 180 deg rotation of ! in its ink box, columns on the original's ink centre (as kqvw_parts.g_iq).
 *  six straight arms of uniform width (vertical + two at 60 deg, as the original's pixels and the font.gfx *),
    top on the top row of the fontk ?, height = the original's 5 ink rows, centre = the original's ink centre."""
import math
import numpy as np
import kqvw_parts as KQ

SS = 16
STAR = {3: (10, 25, 7.5), 2: (6, 16, 5.0)}              # rows [y0, y1), centre x (output px)


def _ink_col(M, code, bw):
    a = M.g[code]
    wgt = a.max(axis=0)
    c = float((wgt * (np.arange(len(wgt)) + 0.5)).sum() / wgt.sum()) * M.N
    return max(0, min(a.shape[1] * M.N - bw, int(round(c - bw / 2.0))))


def exclam(M):
    N, W = M.N, M.W
    top, _, base, _ = KQ.Q[N]
    a = M.g[0x21]
    r = np.zeros((a.shape[0] * N, a.shape[1] * N))
    x0 = _ink_col(M, 0x21, W)
    dot = base - W
    r[top:dot - N, x0:x0 + W] = 1.0
    r[dot:base, x0:x0 + W] = 1.0
    return r


def inv_exclam(M):
    s = exclam(M)
    ys, xs = np.nonzero(s > 0.02)
    y0, y1, x0, x1 = ys.min(), ys.max() + 1, xs.min(), xs.max() + 1
    a = M.g[0xA1]
    out = np.zeros((a.shape[0] * M.N, a.shape[1] * M.N))
    nx = _ink_col(M, 0xA1, x1 - x0)
    out[y0:y1, nx:nx + x1 - x0] = s[y0:y1, x0:x1][::-1, ::-1]
    return out


def star(M):
    N, W = M.N, M.W
    a = M.g[0x2A]
    h, w = a.shape[0] * N, a.shape[1] * N
    y0, y1, cx = STAR[N]
    cy, R = (y0 + y1) / 2.0, (y1 - y0) / 2.0
    y, x = (np.mgrid[0:h * SS, 0:w * SS] + 0.5) / SS
    m = np.zeros(y.shape, bool)
    for deg in (0, 60, 120):                            # one bar through the centre per direction: 6 arms
        d = math.radians(deg)
        dx, dy = math.sin(d), -math.cos(d)
        t = (x - cx) * dx + (y - cy) * dy
        s = (x - cx) * dy - (y - cy) * dx
        m |= (np.abs(t) <= R) & (np.abs(s) <= W / 2.0)
    return m.reshape(h, SS, w, SS).mean(axis=(1, 3))


FN = {0x21: exclam, 0xA1: inv_exclam, 0x2A: star}


def render(M, code):
    """returns the raster or None (other code / font / scale)."""
    if code not in FN or not KQ.is_fontk(M) or M.N not in STAR:
        return None
    return FN[code](M)
