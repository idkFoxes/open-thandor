"""fontk.gfx digits 7 (0x37) and 3 (0x33), rebuilt (work_k73). Digit body as k4_parts: source rows 2..8, columns 0 .. 4*N.
7: design of the font.gfx 7 (upscale4.g_7): top bar over the whole body, one straight diagonal of uniform width whose
   right edge leaves the right bar end at the bar's lower edge, clipped flat at the baseline. The narrow fontk body has
   no room for the 33.75 deg of font.gfx; 22.5 deg (the angle of the fontk slash) keeps the foot where the original has it.
3: pixel copy of the finished 8: right side, both arcs and the inner radii unchanged. The left side stops at flat
   terminals on the rows of the original's 2 source rows at top and bottom; the middle bar starts at the body centre.
   The soft inner radius pixel beside the last terminal row is cleared so the terminal ends exactly at stroke width."""
import math
import numpy as np
import upscale3 as S
import upscale4 as U4

SEVEN, THREE = 0x37, 0x33
Y0, Y1 = 2.0, 8.0            # digit top / baseline (source px)
TERM_TOP, TERM_BOT = 4.0, 6.0  # 3: top terminal ends, bottom terminal starts (source px)


def applies(M, code):
    return code in (SEVEN, THREE) and M.g[ord('A')].shape[0] == 9 and M.g[code].shape[0] == 9 and M.N in (2, 3)


def g_7(M, shape):
    N, W = M.N, M.W
    w = W / float(N)
    R = 4.0                                               # right body edge (source px)
    yt = Y0 + w / 2
    top = U4.stroke_raster(M, shape, [(yt, 0.0), (yt, R)])
    t = U4.T225
    yi0 = Y0 + w                                          # lower edge of the bar
    hw = w / 2 / math.cos(math.atan(t))                   # half the horizontal width of the diagonal
    xa = R - hw                                           # centre line at yi0: right edge on the right bar end
    e = 2 * w
    diag = U4.stroke_raster(M, shape, [(yi0 - e, xa + e * t), (Y1 + e, xa - (Y1 + e - yi0) * t)], clip=(yi0, Y1))
    return U4.union(top, diag)


def g_3(M, shape):
    N, W = M.N, M.W
    r = S.render(M, ord('8')).copy()
    y0, y1 = int(round(TERM_TOP * N)), int(round(TERM_BOT * N))
    xm = 2 * N                                            # body centre: the middle bar and the right side stay
    r[y0:y1, :xm] = 0.0
    r[y0 - 1, W:xm] = 0.0                                 # flat terminal ends: exactly W wide on their last row
    r[y1, W:xm] = 0.0
    return r


def render(M, code):
    if not applies(M, code):
        return None
    return (g_7 if code == SEVEN else g_3)(M, M.g[code].shape)
