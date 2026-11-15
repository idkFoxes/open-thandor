"""font.gfx ^ (0x5E), ae (0xE6), ordfeminine (0xAA). Other fonts / scales fall through to the normal path.
 ^   open chevron of two straight arms, stroke W, flat apex W wide, arm ends cut flat, mirror symmetric (kqvw_parts._legs).
 ae  3x only: the contour build rounds the underside of the right top bar separately, one extra row (5 rows thick);
     that row is cut right of the stem, the rest of the finished glyph stays.
 ordfeminine  the contour build loses the counter: small a composed from font.gfx's finished a at high N, hinted,
     area-downscaled (frac_parts), final stroke W as the small digits. The original has no underline, so none is drawn.
 ordmasculine (0xBA, v9)  elliptic ring in the ink box of the contour build (size and place unchanged), ring width =
     the final stroke of the ordfeminine, so both have the same weight (the contour build gave it the heavier stroke
     of the full-size letters).
"""
import math
import numpy as np
import upscale4 as U4
import kqvw_parts as KQ
import frac_parts as FR

T = 0.75                                                # ^ arm slope dx/dy (36.87 deg from vertical)
HAT = {3: (10, 21, 12.0), 2: (6, 13, 7.5)}              # ^ output rows top, bottom (exclusive), apex centre x
AE_XH = 16                                              # 3x: first row of the top bar of ae
# ordfeminine: output scale -> high N, its stroke, downscale factor, final stroke, ink rows top / bottom
ORD = {3: (7, 9, 3, 3, 10, 24), 2: (5, 6, 3, 2, 6, 16)}
_ord = {}


def is_font(M):
    return M.g[ord('A')].shape[0] == 15


def g_hat(M):
    N = M.N
    top, bot, c = HAT[N]
    g = U4.G(M)
    r = U4.union(*KQ._legs(M, g, M.g[0x5E].shape, c / N, T, top / float(N), bot / float(N), False))
    return U4.mirror_x(r, c)


def g_ae(M):
    M._gcaret_busy = True
    try:
        import upscale3 as S
        r = S.render(M, 0xE6).copy()
    finally:
        M._gcaret_busy = False
    W = M.W
    cols = np.nonzero(r[AE_XH + W + 2] > 0.5)[0]        # stem between top bar and crossbar
    if cols[-1] - cols[0] + 1 != W:
        return None
    r[AE_XH + W:AE_XH + 2 * W, cols[-1] + 1:] = 0.0
    return r


def small_a(N, letter='a'):
    if (N, letter) not in _ord:
        import upscale as U, upscale3 as S, dumpcur as D
        NH, WH, k, W, top, bot = ORD[N]
        M = S.FontModel(U.load('font.gfx'), NH, W=WH, **D.FONT_KW['font.gfx'])
        c = FR.hint(FR._crop(S.render(M, ord(letter))), k, (bot - top) * k)
        _ord[(N, letter)] = FR._crop(min((FR._down(c, k, dx) for dx in range(k)), key=FR._soft))
    return _ord[(N, letter)]


def g_ordm(M):
    import upscale3 as S
    import accents_parts as AP
    M._gcaret_busy = True
    try:
        r = S.render(M, 0xBA)
    finally:
        M._gcaret_busy = False
    rows = np.nonzero(r.max(axis=1) > 0.5)[0]
    cols = np.nonzero(r.max(axis=0) > 0.5)[0]
    y0, y1, x0, x1 = rows[0], rows[-1] + 1, cols[0], cols[-1] + 1
    W = ORD[M.N][3]
    cy, cx = (y0 + y1) / 2.0, (x0 + x1) / 2.0
    ry, rx = (y1 - y0) / 2.0, (x1 - x0) / 2.0
    n = 96
    ell = lambda a, b: [(cy + a * math.sin(2 * math.pi * i / n), cx + b * math.cos(2 * math.pi * i / n))
                        for i in range(n)]
    return AP._poly_raster([ell(ry, rx), ell(ry - W, rx - W)], r.shape[0], r.shape[1])


def g_ordf(M, code=0xAA):
    if code == 0xBA:
        return g_ordm(M)
    N = M.N
    a = small_a(N)
    src = M.g[code]
    h, w = src.shape[0] * N, src.shape[1] * N
    top = ORD[N][4]
    cs = (src.sum(axis=0) * (np.arange(src.shape[1]) + 0.5)).sum() / src.sum() * N
    ca = (a.sum(axis=0) * (np.arange(a.shape[1]) + 0.5)).sum() / a.sum()
    x = max(0, min(w - a.shape[1], int(round(cs - ca))))
    r = np.zeros((h, w))
    r[top:top + a.shape[0], x:x + a.shape[1]] = a
    return r


def render(M, code):
    """finished raster or None."""
    if code not in (0x5E, 0xE6, 0xAA, 0xBA) or M.N not in (2, 3) or not is_font(M):
        return None
    if code == 0x5E:
        return g_hat(M)
    if code == 0xBA and getattr(M, '_gcaret_busy', False):
        return None
    if code in (0xAA, 0xBA):
        return g_ordf(M, code)
    if M.N == 3 and not getattr(M, '_gcaret_busy', False):
        return g_ae(M)
    return None
