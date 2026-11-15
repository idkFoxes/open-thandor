"""fontk.gfx ? and inverted ?, V / W (v / w share the originals), ^ : stroke-built, stroke width M.W (3x 3 px, 2x 2 px).
The contour path reduced ? to bar + stem and filled the lower half of V / W and the whole of ^.
 ?  like the font.gfx ? (upscale4.g_question): top bar, arc whose corners are the corners of the fontk P (pixel copy),
    middle bar back to the stem, the same corner mirrored as turn, stem, dot D x D under the stem with gap N. Same output rows as the
    font.gfx ? (the fontk original has nearly the same pixels, same 15-row cell, ink rows 3..11).
 inverted ?  exact 180 deg rotation of ? in its ink box, columns as dots_parts.rotated.
 V  two straight 22.5 deg diagonals, flat vertex as wide as a stroke (as nm_parts M), mirror symmetric.
 W  four straight diagonals, cell only allows t = 7/24 (16.26 deg): width 24 t + 1 = 8 source px = the original's ink.
 ^  open chevron over the original's ink rows 3..6, t = 3/4 (36.87 deg), flat apex W, arm ends cut flat, mirror symmetric.
"""
import math
import numpy as np
import upscale4 as U4
import nm_parts as NM

CAPS = (2.0, 8.0)                                       # cap top / baseline of the 9-row cell (source px), as N / M
T_V = U4.T225
T_W = 7 / 24.0
T_H = 0.75
HAT = (3.0, 7.0)                                        # source rows of the ^ ink
HAT_C = {3: 11.5, 2: 8.0}                               # mirror axis of ^ (output px)
# ? output rows: top edge, bottom edge of the middle bar, bottom of the dot (= font.gfx ?); right edge of the arc
Q = {3: (10, 26, 35, 24), 2: (6, 17, 24, 16)}


def is_fontk(M):
    return M.g[ord('A')].shape[0] == 9 and M.N in Q


def _legs(M, g, shape, vx, t, y0, y1, down):
    """one vertex at vx (source px, centre of its flat end W wide): the two diagonals meeting there.
    down=True: vertex at y1 (V), down=False: vertex at y0 (^)."""
    hw = g.w * math.sqrt(1 + t * t)
    a = g.w / 2
    H = y1 - y0
    w = shape[1]
    if down:
        cl0 = vx - a + hw / 2 - H * t                   # left leg: centre line at y0
        cr0 = vx + a - hw / 2 + H * t
        return (NM._diag(M, shape, cl0, t, y0, y1, g, 0, w), NM._diag(M, shape, cr0, -t, y0, y1, g, 0, w))
    cl0 = vx - a + hw / 2                               # left leg (going down-left): centre line at y0
    cr0 = vx + a - hw / 2
    return (NM._diag(M, shape, cl0, -t, y0, y1, g, 0, w), NM._diag(M, shape, cr0, t, y0, y1, g, 0, w))


def g_V(M, g, shape):
    C = g.w / 2 + (CAPS[1] - CAPS[0]) * T_V             # left top corner on x = 0, whole px axis below
    Cx = float(round(C * g.N))
    r = U4.union(*_legs(M, g, shape, Cx / g.N, T_V, CAPS[0], CAPS[1], True))
    return U4.mirror_x(r, Cx), [(1, Cx)]


def g_W(M, g, shape):
    y0, y1 = CAPS
    H = y1 - y0
    C = g.w / 2 + 2 * H * T_W                           # = 4 source px: ink 0 .. 8
    Cx = C * g.N
    vl = C - H * T_W                                    # lower left vertex; the leg rising from it reaches the apex
    r = U4.union(*_legs(M, g, shape, vl, T_W, y0, y1, True))
    return U4.mirror_x(r, Cx), [(1, Cx)]


def g_hat(M, g, shape):
    Cx = HAT_C[g.N]
    r = U4.union(*_legs(M, g, shape, Cx / g.N, T_H, HAT[0], HAT[1], False))
    return U4.mirror_x(r, Cx), [(1, Cx)]


def g_question(M, g, shape):
    N, W = g.N, g.W
    top, bar_b, base, right = Q[N]
    yt = (top + W / 2) / N
    ym = (bar_b - W / 2) / N
    xr = (right - W / 2) / N
    xs = g.sc(0.9)
    dot_top = base - W
    y_end = (dot_top - N) / N                           # gap N, as the font.gfx dots
    R = float(M.r_small)
    body = U4.stroke_raster(M, shape, [(yt, 0.0), (yt, xr), (ym, xr), (ym, xs), (y_end, xs)], rad=[R, R, R])
    r = U4.union(body)
    x0 = int(round(xs * N - W / 2))
    # the three corners are the corner of the fontk P, pixel by pixel
    P = U4.S.render(M, ord('P'))
    pr = int(np.nonzero(P.max(axis=0) > 0.5)[0][-1]) + 1
    bars = np.nonzero(P[:, pr - W - 2] > 0.5)[0]
    ptop, pbot = int(bars[0]), int(bars[-1]) + 1
    k = (pbot - ptop) // 2
    ur = P[ptop:ptop + k, pr - k:pr]
    lr = P[pbot - k:pbot, pr - k:pr]
    r[top:top + k, right - k:right] = ur
    r[bar_b - k:bar_b, right - k:right] = lr
    r[bar_b - W:bar_b - W + k, x0:x0 + k] = ur[:, ::-1]
    r[dot_top:base, x0:x0 + W] = 1.0
    return r, []


def g_iq(M, g, shape):
    s, _ = g_question(M, g, M.g[ord('?')].shape)
    ys, xs = np.nonzero(s > 0.02)
    y0, y1, x0, x1 = ys.min(), ys.max() + 1, xs.min(), xs.max() + 1
    box = s[y0:y1, x0:x1][::-1, ::-1]
    a = M.g[0xBF]
    w = a.shape[1] * g.N
    wgt = a.max(axis=0)
    c_o = float((wgt * (np.arange(len(wgt)) + 0.5)).sum() / wgt.sum()) * g.N
    bw = x1 - x0
    nx = max(0, min(w - bw, int(round(c_o - bw / 2))))
    out = np.zeros((a.shape[0] * g.N, w))
    out[y0:y1, nx:nx + bw] = box
    return out, []


FN = {0x3F: g_question, 0xBF: g_iq, 0x56: g_V, 0x76: g_V, 0x57: g_W, 0x77: g_W, 0x5E: g_hat}


def render(M, code):
    """returns (raster, axes) or None."""
    if code not in FN or not is_fontk(M):
        return None
    return FN[code](M, U4.G(M), M.g[code].shape)
