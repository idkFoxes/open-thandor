"""fontk.gfx digits 4 (0x34) and 2 (0x32), rebuilt with stroke_raster (work_k4). Both reached the contour path, whose
iso line closed the small counters: the 4 became a solid block with a stub (triangle counter gone, "24" read as "28"),
the 2 got a filled top left and a square top right corner.
Digit body as the other fontk digits: source rows 2..8 (3x output rows 6..24, 2x 4..16), output columns 0 .. 4*N.
4: design of the font.gfx 4 (upscale4.g_4) scaled to this height: rectangular right stem, straight diagonal, straight
   bar from the left body edge through the stem, 1 px overhang on the right (font.gfx 3/27 of its width), 1 stroke of
   stem below the bar. Slope t = dx/dy = 1/2 instead of 45 deg, the narrow body has no room for 45 deg; repeating AA.
2: top = pixel copy of the finished 0 (same arcs and stroke, left terminal cut flat like the original's 2 source rows),
   right stem continues, 45 deg diagonal into a rectangular foot bar. Stroke M.W throughout."""
import math
import numpy as np
import upscale3 as S
import upscale4 as U4

FOUR, TWO = 0x34, 0x32
Y0, Y1 = 2.0, 8.0            # digit top / baseline (source px)
TERM = 4.0                   # 2: the top copied from the 0 ends here (left terminal; source px)


def applies(M, code):
    return code in (FOUR, TWO) and M.g[ord('A')].shape[0] == 9 and M.g[code].shape[0] == 9 and M.N in (2, 3)


def g_4(M, shape):
    N, W = M.N, M.W
    w = W / N
    R = 4 * N                                             # right body edge (output px)
    xs = (R - 1 - W / 2.0) / N                            # stem centre: 1 px overhang of the bar
    b1 = Y1 - w                                           # 1 stroke of stem below the bar
    b0 = b1 - w
    yb = (b0 + b1) / 2
    t = 0.5
    hw = w * math.sqrt(1 + t * t)                         # horizontal width of the diagonal
    cx = hw / 2                                           # centre line at y = b0: outer edge on the left body edge
    e = 2 * w
    stem = U4.stroke_raster(M, shape, [(Y0 - e, xs), (Y1 + e, xs)], clip=(Y0, Y1))
    ya, yz = Y0 - e, b1 + e
    diag = U4.stroke_raster(M, shape, [(ya, cx + (b0 - ya) * t), (yz, cx + (b0 - yz) * t)], clip=(Y0, b0))
    bar = U4.stroke_raster(M, shape, [(yb, 0.0), (yb, R / float(N))])
    r = U4.union(stem, diag, bar)
    return r


def g_2(M, shape):
    N, W = M.N, M.W
    w = W / N
    R = 4 * N
    h, wd = shape[0] * N, shape[1] * N
    O = S.render(M, ord('0'))
    r = np.zeros((h, wd))
    yt = int(round(TERM * N))
    r[:yt] = O[:yt]                                       # top arcs + left terminal + right stem part
    xs = (R - W / 2.0) / N
    b0 = Y1 - w                                           # foot bar rows
    hw = w * math.sqrt(2)
    # 45 deg centre line x + y = k: the upper edge meets the top left corner of the foot (as the 4: clean corner)
    k = b0 + hw / 2
    yj = k - xs                                           # where the centre line meets the stem centre
    e = 2 * w
    st = U4.stroke_raster(M, shape, [(TERM - e, xs), (yj, xs), (Y1 + e, k - (Y1 + e))], rad=['small'], clip=(TERM, Y1))
    y_ = (b0 + Y1) / 2
    foot = U4.stroke_raster(M, shape, [(y_, 0.0), (y_, R / float(N))])
    return np.maximum(r, U4.union(st, foot))


def render(M, code):
    if not applies(M, code):
        return None
    return (g_4 if code == FOUR else g_2)(M, M.g[code].shape)
