"""Stroke construction for glyphs where the contour approach is not clean enough (diagonals, junctions):
2 4 5 7 9 ? A H V W X Y Z , ; (font.gfx). Every glyph is a few centre-line strokes of the font-wide width W
(output px), with the font-wide arc radii (outer r, inner r - W, concentric), butt (rectangular) ends,
diagonals at few fixed angles, clipped flat at cap height / baseline. Positions are measured from the
original glyph (stroke centres, see comments) and snapped to the output grid like the contour hinting
(centre -> floor(c*N - W/2 + 0.5)), so heights match the contour-built glyphs (E, O, P ...).
"""
import math
import numpy as np
import upscale as U
import upscale3 as S

T225 = math.tan(math.radians(22.5))
T3375 = math.tan(math.radians(33.75))
T45 = 1.0


class G:
    """grid helpers for one FontModel."""

    def __init__(self, M):
        self.M, self.N, self.W = M, M.N, M.W
        self.w = M.W / M.N

    def sc(self, v):
        """snap a stroke centre (source px) so that both edges land on output pixel edges."""
        N, W = self.N, self.W
        return (math.floor(v * N - W / 2 + 0.5) + W / 2) / N

    def se(self, v):
        """snap an edge (source px) to the output grid."""
        return round(v * self.N) / self.N

    def axis(self, v, centred_stroke=False):
        """mirror axis (output px, half-pixel grid); with a centred stroke the parity follows W."""
        t = v * self.N
        if centred_stroke:
            return (math.floor(t) + 0.5) if self.W % 2 else float(round(t))
        return round(2 * t) / 2.0

    @property
    def TOPc(self):
        return self.sc(3.9)

    @property
    def BASc(self):
        return self.sc(11.1)

    @property
    def TOPe(self):
        return self.TOPc - self.w / 2

    @property
    def BASe(self):
        return self.BASc + self.w / 2


# ------------------------------------------------------------------ stroke geometry
def centreline(pts, rad, M):
    """polyline with rounded joints: rad[i] for interior point i (None = sharp, 'big' / 'small' / float output
    px = OUTER radius of the turn; the centre line radius is r - W/2, inner = r - W)."""
    N, W = M.N, M.W
    pts = [np.array(p, dtype=np.float64) for p in pts]
    out = [pts[0]]
    for i in range(1, len(pts) - 1):
        r = rad[i - 1] if i - 1 < len(rad) else None
        P, A, B = pts[i], pts[i - 1], pts[i + 1]
        if r is None:
            out.append(P)
            continue
        R_out = {'big': M.r_big, 'small': M.r_small}.get(r, r)
        rc = (R_out - W / 2.0) / N
        d1 = (P - A) / np.linalg.norm(P - A)
        d2 = (B - P) / np.linalg.norm(B - P)
        cosang = float(np.clip(np.dot(-d1, d2), -1, 1))
        theta = math.acos(cosang)                       # inner angle at P
        turn = math.pi - theta
        t = rc / math.tan(theta / 2)                    # distance P -> tangent points
        t = min(t, np.linalg.norm(P - A) * 0.999, np.linalg.norm(B - P) * 0.999)
        T1, T2 = P - d1 * t, P + d2 * t
        # arc centre: from T1 along the normal towards the inside of the turn
        n1 = np.array([d1[1], -d1[0]])
        if np.dot(n1, d2) < 0:
            n1 = -n1
        rc_eff = t * math.tan(theta / 2)
        Cc = T1 + n1 * rc_eff
        a0 = math.atan2(*(T1 - Cc))
        a1 = math.atan2(*(T2 - Cc))
        da = (a1 - a0 + math.pi) % (2 * math.pi) - math.pi
        k = max(4, int(abs(da) / (math.pi / 2) * 24))
        for j in range(k + 1):
            a = a0 + da * j / k
            out.append(Cc + rc_eff * np.array([math.sin(a), math.cos(a)]))
    out.append(pts[-1])
    # drop duplicates
    clean = [out[0]]
    for p in out[1:]:
        if np.linalg.norm(p - clean[-1]) > 1e-6:
            clean.append(p)
    return clean


def offset_polygon(cl, half):
    """closed polygon of a polyline stroke: miter joins, butt ends. points (y, x)."""
    n = len(cl)
    segs = [cl[i + 1] - cl[i] for i in range(n - 1)]
    dirs = [s / np.linalg.norm(s) for s in segs]
    nrm = [np.array([d[1], -d[0]]) for d in dirs]       # left normal
    L, Rr = [], []
    for i in range(n):
        if i == 0:
            m = nrm[0]
            s = 1.0
        elif i == n - 1:
            m = nrm[-1]
            s = 1.0
        else:
            m = nrm[i - 1] + nrm[i]
            ln = np.linalg.norm(m)
            if ln < 1e-9:
                m = nrm[i]
                s = 1.0
            else:
                m = m / ln
                s = 1.0 / max(np.dot(m, nrm[i]), 0.2)
        L.append(cl[i] + m * half * s)
        Rr.append(cl[i] - m * half * s)
    return np.array(L + Rr[::-1])


SSU = 8


class Hi:
    """binary stroke mask at N*SSU resolution; unions are exact (OR), AA is computed once afterwards."""

    def __init__(self, m):
        self.m = m


def stroke_raster(M, shape, pts, rad=(), clip=None, xclip=None):
    h, w = shape
    cl = centreline(pts, list(rad), M)
    poly = offset_polygon(cl, M.W / M.N / 2)
    m = U.raster([poly], M.N * SSU, h, w, 1) > 0.5
    if clip is not None:
        y0, y1 = int(round(clip[0] * M.N)) * SSU, int(round(clip[1] * M.N)) * SSU
        m[:max(0, y0)] = False
        m[y1:] = False
    if xclip is not None:
        x0 = int(round(xclip * M.N * SSU))
        m[:, :max(0, x0)] = False
    return Hi(m)


def lo(x):
    return U.box_down(x.m.astype(np.float64), SSU) if isinstance(x, Hi) else x


def mirror_x(r, C):
    """copy the left half onto the right half about the axis C (output px)."""
    K = int(round(2 * C)) - 1
    cen = np.arange(r.shape[1]) + 0.5
    m = S.V._mirror(r, 1, K)
    return np.where((cen > C)[None, :], m, r)


def mirror_y(r, C):
    K = int(round(2 * C)) - 1
    cen = np.arange(r.shape[0]) + 0.5
    m = S.V._mirror(r, 0, K)
    return np.where((cen > C)[:, None], m, r)


def union(*rs):
    """exact union of stroke masks (OR at high resolution), then max with already rendered rasters."""
    his = [r for r in rs if isinstance(r, Hi)]
    los = [r for r in rs if not isinstance(r, Hi)]
    out = None
    if his:
        m = his[0].m.copy()
        for r in his[1:]:
            m |= r.m
        out = lo(Hi(m))
    for r in los:
        out = r if out is None else np.maximum(out, r)
    return out


# ------------------------------------------------------------------ glyph definitions (font.gfx, line height 15)
def g_H(M, g, shape):
    xl = g.sc(0.85)                                      # left stem cols 0-1
    C = g.axis(3.48)                                     # (0.85 + 6.1) / 2
    xr = 2 * C / g.N - xl
    ym = g.sc(7.5)                                       # = E/F middle bar
    r = union(stroke_raster(M, shape, [(g.TOPe, xl), (g.BASe, xl)]),
              stroke_raster(M, shape, [(g.TOPe, xr), (g.BASe, xr)]),
              stroke_raster(M, shape, [(ym, xl), (ym, xr)]))
    return mirror_x(r, C), [(1, C)]


def legs_v(g, ax, a, t, y_tip, y_far, up):
    """two legs of A (up=True: tip at the top) or V: centre lines through (y_tip, ax -/+ a) with slope t."""
    s = -1 if up else 1
    xl = lambda y: ax - a - (y - y_tip) * t * (1 if up else -1)
    xr = lambda y: ax + a + (y - y_tip) * t * (1 if up else -1)
    return xl, xr


A_BAR_SRC = 9.25                      # bar on the lower edge of the original bar rows 8-9 (larger counter); was 8.9 (centred)


def g_A(M, g, shape):
    C = g.axis(4.1)
    ax = C / g.N
    a = 0.2
    yt = g.TOPc
    xl = lambda y: ax - a - (y - yt) * T225
    xr = lambda y: ax + a + (y - yt) * T225
    yb = g.BASe + 2 * g.w
    leg = stroke_raster(M, shape, [(yb, xl(yb)), (yt, ax - a), (yt, ax + a), (yb, xr(yb))], clip=(g.TOPe, g.BASe))
    # the inner offset of the flat apex self-intersects -> stray hole inside the solid apex; above the point where the
    # inner edges really meet every row is solid between its outer edges
    yi = yt + (g.w / (2 * math.cos(math.radians(22.5))) - a) / T225
    for row in leg.m[:int(math.floor(yi * M.N * SSU))]:
        ink = np.nonzero(row)[0]
        if len(ink):
            row[ink[0]:ink[-1] + 1] = True
    ybar = g.sc(A_BAR_SRC)
    r = union(leg, stroke_raster(M, shape, [(ybar, xl(ybar)), (ybar, xr(ybar))]))
    return mirror_x(r, C), [(1, C)]


def fill_crotch(r, C):
    """V-type crotch (legs meeting at the mirror axis C, output px): the short flat bottom of the centre line makes
    the inner offset self-intersect -> a grey / empty pinhole below the crotch point (V, v at 2x and 3x).
    Below the first row where the axis column is >= 0.5, every pixel between the first and the last solid
    pixel of the row lies inside both legs -> set to full ink."""
    col = int(math.floor(C)) if abs(C - math.floor(C) - 0.5) < 1e-6 else int(round(C)) - 1
    ys = np.nonzero(r[:, col] >= 0.5)[0]
    if not len(ys):
        return r
    r = r.copy()
    for y in range(int(ys[0]) + 1, r.shape[0]):
        full = np.nonzero(r[y] >= 0.999)[0]
        if len(full) < 2 or not (full[0] <= col <= full[-1]):
            continue
        r[y, full[0]:full[-1] + 1] = 1.0
    return r


def g_V(M, g, shape):
    C = g.axis(4.55)
    ax = C / g.N
    a = 0.2
    yb = g.BASc
    xl = lambda y: ax - a - (yb - y) * T225
    xr = lambda y: ax + a + (yb - y) * T225
    yt = g.TOPe - 2 * g.w
    r = union(stroke_raster(M, shape, [(yt, xl(yt)), (yb, ax - a), (yb, ax + a), (yt, xr(yt))], clip=(g.TOPe, g.BASe)))
    return fill_crotch(mirror_x(r, C), C), [(1, C)]


def g_W(M, g, shape):
    C = g.axis(6.7)                                      # original 6.4, shifted so the 22.5 deg legs fit the box
    ax = C / g.N
    yt, yb = g.TOPc, g.BASc
    h = (yb - yt) * T225
    yT = g.TOPe - 2 * g.w
    yB = g.BASe + 2 * g.w
    pts = [(yT, ax - 2 * h - (yt - yT) * T225), (yb, ax - h), (yt, ax), (yb, ax + h),
           (yT, ax + 2 * h + (yt - yT) * T225)]
    # extend the inner vertices a little beyond the clip lines so they are cut flat
    r = union(stroke_raster(M, shape, pts, clip=(g.TOPe, g.BASe)))
    return mirror_x(r, C), [(1, C)]


def g_X(M, g, shape):
    C = g.axis(3.55)
    ax = C / g.N
    yc = (g.TOPe + g.BASe) / 2
    e = 2 * g.w
    y0, y1 = g.TOPe - e, g.BASe + e
    r = union(stroke_raster(M, shape, [(y0, ax - (yc - y0) * T3375), (y1, ax + (y1 - yc) * T3375)],
                            clip=(g.TOPe, g.BASe)),
              stroke_raster(M, shape, [(y0, ax + (yc - y0) * T3375), (y1, ax - (y1 - yc) * T3375)],
                            clip=(g.TOPe, g.BASe)))
    r = mirror_x(r, C)
    r = mirror_y(r, yc * g.N)
    return r, [(1, C), (0, yc * g.N)]


def g_Y(M, g, shape):
    C = g.axis(4.4, centred_stroke=True)
    ax = C / g.N
    yj = g.sc(7.0)
    y0 = g.TOPe - 2 * g.w
    left = union(stroke_raster(M, shape, [(y0, ax - (yj - y0) * T45), (yj, ax), (g.BASe + g.w, ax)],
                               clip=(g.TOPe, g.BASe)))
    return mirror_x(left, C), [(1, C)]


def g_Z(M, g, shape):
    xl, xr = g.se(0.0), g.se(7.6)
    yt, yb = g.TOPc, g.BASc
    top = stroke_raster(M, shape, [(yt, xl), (yt, xr)])
    bot = stroke_raster(M, shape, [(yb, xl), (yb, xr)])
    hw = g.w / 2 * math.sqrt(2)                          # horizontal half width of a 45 deg stroke
    yi0, yi1 = yt + g.w / 2, yb - g.w / 2                # inner edges of the bars
    xa = xr - hw                                         # diagonal centre at the inner top edge
    xb = xa - (yi1 - yi0)                                # ... at the inner bottom edge (45 deg)
    e = g.w
    diag = stroke_raster(M, shape, [(yi0 - e, xa + e), (yi1 + e, xb - e)], clip=(yi0, yi1))
    return union(top, bot, diag), []


def g_7(M, g, shape):
    xl, xr = g.se(0.3), g.se(8.0)
    yt = g.TOPc
    top = stroke_raster(M, shape, [(yt, xl), (yt, xr)])
    hw = g.w / 2 / math.cos(math.atan(T3375))
    yi0 = yt + g.w / 2
    xa = xr - hw
    yb = g.BASe + g.w
    diag = stroke_raster(M, shape, [(yi0 - g.w, xa + g.w * T3375), (yb, xa - (yb - yi0) * T3375)],
                         clip=(yi0, g.BASe))
    return union(top, diag), []


DIAG4_OUT = 9.05                     # original 4: outer (lower left) edge of the diagonal, x + y = const (source px)


def g_4(M, g, shape):
    """4: right stem, 45 deg diagonal, bar. The diagonal is positioned like the original (outer edge x + y =
    DIAG4_OUT), its inner edge snapped onto the 45 deg pixel diagonal (crisp, nothing in the counter); top flat at
    cap height, the bar starts where the diagonal's outer edge meets its top edge (clean corner, no overhang)."""
    N = g.N
    xs = g.sc(7.4)                                       # right stem col 7
    yb = g.sc(9.6)                                       # bar rows 9-10
    hd = g.W * math.sqrt(2)                              # horizontal width of the 45 deg stroke (output px)
    k_in = round(DIAG4_OUT * N + hd)                     # inner edge x + y (output px, integer = crisp)
    kc = (k_in - hd / 2) / N                             # centre line x + y (source px)
    ybar0, ybar1 = yb - g.w / 2, yb + g.w / 2
    xl = max(0.0, g.se(kc - g.w * math.sqrt(2) / 2 - ybar0))   # outer edge at the bar's top edge, on the grid
    stem = stroke_raster(M, shape, [(g.BASe + g.w, xs), (g.TOPe - g.w, xs)], clip=(g.TOPe, g.BASe))
    ya, yz = g.TOPe - g.w, ybar1 + g.w
    diag = stroke_raster(M, shape, [(ya, kc - ya), (yz, kc - yz)], clip=(g.TOPe, ybar1), xclip=xl)
    bar = stroke_raster(M, shape, [(yb, xl), (yb, g.se(9.0))])
    return union(stem, diag, bar), []


def g_0(M, g, shape):
    """0 = the rendered O (same arcs and stroke, pixel-identical left / right halves) narrowed to the digit body:
    flat middle columns of the top / bottom bars removed, outer edges inside DIGIT_L / DIGIT_R (centred in the
    advance like the overview red lines), symmetric (same parity as the O, so both halves keep the same width)."""
    N = M.N
    O = S.render(M, ord('O'))
    h, w = shape[0] * N, shape[1] * N
    cols = np.nonzero(O.max(axis=0) > 0.02)[0]
    lo, hi = int(cols[0]), int(cols[-1]) + 1                    # O ink edges (output px)
    off = (shape[1] - 9) * N / 2.0
    L = math.ceil(round(DIGIT_L * N) + off - 1e-9)
    R = math.floor(round(DIGIT_R * N) + off + 1e-9)
    wt = R - L
    if (hi - lo - wt) % 2:                                     # keep the O's parity -> exact mirror symmetry
        wt -= 1
        R -= 1
    kl = (wt + 1) // 2                                        # columns kept from the left / right side of the O
    kr = wt - kl                                              # (odd width: the extra middle column is flat)
    if wt >= hi - lo or np.abs(O[:, lo + kr:hi - kr] - O[:, [lo + kr]]).max() > 1e-6:
        return None                                           # removed columns not flat -> leave the 0 alone
    out = np.zeros((h, w))
    out[:, L:L + kl] = O[:h, lo:lo + kl]
    out[:, R - kr:R] = O[:h, hi - kr:hi]
    return out, []


DIGIT_L, DIGIT_R = 0.3, 7.7          # common outer edges of the digit body (advance 9: 2 3 5 6 7 8), from the 8


def g_2(M, g, shape):
    L, R = g.se(DIGIT_L), g.se(DIGIT_R)
    xl, xr = L + g.w / 2, R - g.w / 2
    yt, ym, yb = g.TOPc, g.sc(7.5), g.BASc
    # free start (top left) and end of the foot (bottom right) end on the body edges L / R
    pts = [(yt, L), (yt, xr), (ym, xr), (ym, xl), (yb, xl), (yb, R)]
    r = union(stroke_raster(M, shape, pts, rad=['small', 'small', 'small', None]))
    return r, []


def g_5(M, g, shape):
    L, R = g.se(DIGIT_L), g.se(DIGIT_R)
    xl, xr = L + g.w / 2, R - g.w / 2
    yt, ym, yb = g.TOPc, g.sc(7.5), g.BASc
    # top bar ends on R, the free end of the lower bowl on L (as the 2, rotated)
    pts = [(yt, R), (yt, xl), (ym, xl), (ym, xr), (yb, xr), (yb, L)]
    r = union(stroke_raster(M, shape, pts, rad=[None, None, 'small', 'small']))
    return r, []


def g_9(M, g, shape):
    """9 = the 6 rotated by 180 degrees (pixel-identical part), ink box aligned to the original 9."""
    six = S.render(M, ord('6'))
    rot = six[::-1, ::-1]
    h, w = shape[0] * M.N, shape[1] * M.N
    out = np.zeros((h, w))
    # vertical: the 6 box has the same height -> rows map 1:1 after the flip
    r6 = rot[:h]
    c6 = np.nonzero(r6.max(axis=0) > 0.5)[0]
    a9 = M.g[ord('9')]
    cols9 = np.nonzero(a9.max(axis=0) > 0.3)[0]
    # 6 body (advance 9) centred in the advance of the 9 (10): left edge = 6's left edge + half the difference
    six1 = M.g[ord('6')]
    target_left = int(round(DIGIT_L * M.N + (a9.shape[1] - six1.shape[1]) * M.N / 2))
    dx = target_left - c6[0]
    for x in range(w):
        xs = x - dx
        if 0 <= xs < r6.shape[1]:
            out[:, x] = r6[:, xs]
    # the flip moves the rows: rows y -> H-1-y; the 6 spans the same cap/baseline zone so this is exact
    return out, []


def g_question(M, g, shape):
    """? = P bowl (copied, see upscale3.question_from_P) + strokes: middle bar to the stem, small-radius turn,
    straight stem with a rectangular end, the font dot below."""
    N = g.N
    h, w = shape[0] * N, shape[1] * N
    P = S.render(M, ord('P'))
    p1, q1 = M.g[ord('P')], M.g[ord('?')]
    cp = np.nonzero(p1.max(axis=0) > 0.5)[0][-1]
    cq = np.nonzero(q1.max(axis=0) > 0.5)[0][-1]
    dx = int(cq - cp) * N
    # P middle bar rows (output) in a column right of the stem
    col = P[:, int(4 * N)]
    rows = [y for y in range(int(5.5 * N), int(9.5 * N)) if col[y] > 0.5]
    yb0, yb1 = rows[0], rows[-1] + 1
    ybar = (yb0 + yb1) / 2 / N
    stem_r = int(math.ceil((np.nonzero(p1[6] > 0.5)[0][0] + M.W_src) * N))
    out = np.zeros((h, w))
    x0 = max(0, stem_r + dx)
    y_cut = yb1
    for x in range(x0, w):
        xs = x - dx
        if 0 <= xs < P.shape[1]:
            out[:y_cut, x] = P[:y_cut, xs]
    # top bar left part: P's top bar continued to the left end of the original '?' (x = 0)
    trow = P[:, int(4 * N)]
    trows = [y for y in range(0, int(5.5 * N)) if trow[y] > 0.5]
    ytop = (trows[0] + trows[-1] + 1) / 2 / N
    xs_ = g.sc(0.9)
    db = max(1, int(round(0.25 * N)))
    dot_top = M.base_row - M.D
    y_end = (dot_top - max(2, N)) / N
    lower = stroke_raster(M, shape, [((yb0 + yb1) / 2 / N, (x0 + 2 * N) / N), (ybar, xs_), (y_end, xs_)],
                          rad=['small'])
    topbar = stroke_raster(M, shape, [(ytop, g.se(0.0)), (ytop, (x0 + 2 * N) / N)])
    dotx = int(round(xs_ * N - M.D / 2))
    yb_dot = dot_top + M.D
    dot = np.zeros((h, w))
    dot[dot_top:yb_dot, dotx:dotx + M.D] = 1
    return union(out, lower, topbar, dot), []


def g_comma(M, g, shape, code):
    """, and ; : the font dot plus a short tail (right half of the dot, down below the baseline)."""
    a = M.g[code]
    r = None
    if code == ord(';'):
        M.strokes = False
        try:
            r = S.render(M, code)
        finally:
            M.strokes = True
    D, N = M.D, M.N
    h, w = shape[0] * N, shape[1] * N
    rows = np.nonzero(a.max(axis=1) > 0.3)[0]
    cols = np.nonzero(a[rows[-3]:].max(axis=0) > 0.3)[0]
    cx = (cols[0] + cols[-1] + 1) / 2.0
    Cx = math.floor(cx * N) + 0.5 if D % 2 else round(cx * N)
    x0 = int(round(Cx - D / 2))
    yb = M.base_row
    out = np.zeros((h, w)) if r is None else r.copy()
    if r is not None:
        out[int(round(9.5 * N)):] = 0                   # replace the lower part of ';'
    out[yb - D:yb, x0:x0 + D] = 1
    tw = max(1, (D + 1) // 2)
    tl = max(2, int(round(0.8 * N)))
    out[yb:min(h, yb + tl), x0 + D - tw:x0 + D] = 1
    return out, []


def arm_leg(M, g, shape, xs, y_top, y_bot):
    """K / k: stem centre xs; arm and leg at 45 deg, mirror-symmetric about the junction height. Both centre
    lines meet at P = (yj, right stem edge); the inner corner of the '<' lies 0.71 W right of P and stays sharp
    (exact union at 8x8, no doubled AA); everything left of the stem centre is cut (hidden in the stem)."""
    yj = (y_top + y_bot) / 2
    px = xs + g.w / 2
    e = 2 * g.w
    arm = stroke_raster(M, shape, [(yj + g.w, px - g.w), (y_top - e, px + (yj - y_top + e))], clip=(y_top, y_bot),
                        xclip=xs)
    leg = stroke_raster(M, shape, [(yj - g.w, px - g.w), (y_bot + e, px + (y_bot + e - yj))], clip=(y_top, y_bot),
                        xclip=xs)
    stem = stroke_raster(M, shape, [(g.TOPe, xs), (g.BASe, xs)])
    return union(stem, arm, leg), yj


def g_K(M, g, shape):
    xs = g.sc(0.85)
    r, yj = arm_leg(M, g, shape, xs, g.TOPe, g.BASe)
    r = mirror_y(r, yj * g.N)                            # arm and leg exactly mirror-identical
    return r, [(0, yj * g.N)]


def g_k(M, g, shape):
    """k: stem from the ascender, arm and leg 45 deg, mirror-symmetric about the middle of the x-height part."""
    xs = g.sc(0.9)
    yx = g.se(M.snap_zone(4.9))                          # x-height (top edge)
    r, yj = arm_leg(M, g, shape, xs, yx, g.BASe)
    top = r[:int(round(yx * g.N))].copy()                # stem above the x-height is not mirrored
    r = mirror_y(r, yj * g.N)
    r[int(round(g.BASe * g.N)):] = 0                     # the mirrored ascender must not appear below the baseline
    r[:top.shape[0]] = np.maximum(r[:top.shape[0]], top)
    return r, []


def xtop(g):
    return g.sc(5.95) - g.w / 2                          # x-height top edge (z / n top bar rows 5-6)


def g_v(M, g, shape):
    C = g.axis(3.5)
    ax = C / g.N
    a = 0.15
    yb, yt = g.BASc, xtop(g)
    xl = lambda y: ax - a - (yb - y) * T225
    xr = lambda y: ax + a + (yb - y) * T225
    y0 = yt - 2 * g.w
    r = union(stroke_raster(M, shape, [(y0, xl(y0)), (yb, ax - a), (yb, ax + a), (y0, xr(y0))], clip=(yt, g.BASe)))
    return fill_crotch(mirror_x(r, C), C), [(1, C)]


def g_w(M, g, shape):
    C = g.axis(5.3)
    ax = C / g.N
    yt, yb = xtop(g) + g.w / 2, g.BASc
    h = (yb - yt) * T225
    yT = yt - 2 * g.w
    pts = [(yT, ax - 2 * h - (yt - yT) * T225), (yb, ax - h), (yt, ax), (yb, ax + h), (yT, ax + 2 * h + (yt - yT) * T225)]
    r = union(stroke_raster(M, shape, pts, clip=(xtop(g), g.BASe)))
    return mirror_x(r, C), [(1, C)]


def g_x(M, g, shape):
    C = g.axis(2.95)
    ax = C / g.N
    yt = xtop(g)
    yc = (yt + g.BASe) / 2
    e = 2 * g.w
    y0, y1 = yt - e, g.BASe + e
    r = union(stroke_raster(M, shape, [(y0, ax - (yc - y0) * T3375), (y1, ax + (y1 - yc) * T3375)], clip=(yt, g.BASe)),
              stroke_raster(M, shape, [(y0, ax + (yc - y0) * T3375), (y1, ax - (y1 - yc) * T3375)], clip=(yt, g.BASe)))
    r = mirror_x(r, C)
    r = mirror_y(r, yc * g.N)
    return r, [(1, C), (0, yc * g.N)]


def g_z(M, g, shape):
    xl, xr = g.se(0.0), g.se(6.0)
    yt, yb = xtop(g) + g.w / 2, g.BASc
    top = stroke_raster(M, shape, [(yt, xl), (yt, xr)])
    bot = stroke_raster(M, shape, [(yb, xl), (yb, xr)])
    hw = g.w / 2 * math.sqrt(2)
    yi0, yi1 = yt + g.w / 2, yb - g.w / 2
    xa = xr - hw
    xb = xa - (yi1 - yi0)
    e = g.w
    diag = stroke_raster(M, shape, [(yi0 - e, xa + e), (yi1 + e, xb - e)], clip=(yi0, yi1))
    return union(top, bot, diag), []


def g_slash(M, g, shape):
    """/ : one straight stroke at 22.5 deg (original ~21 deg), from the cap height to below the baseline."""
    y0, y1 = g.TOPe, g.se(13.5)                        # top on the common cap top edge
    ym, xm = (y0 + y1) / 2, 2.7
    e = 2 * g.w
    ya, yb_ = y0 - e, y1 + e
    r = union(stroke_raster(M, shape, [(ya, xm + (ym - ya) * T225), (yb_, xm - (yb_ - ym) * T225)], clip=(y0, y1)))
    return r, []


STROKE = {ord('v'): g_v, ord('w'): g_w, ord('x'): g_x, ord('z'): g_z, ord('/'): g_slash,
          ord('K'): g_K, ord('k'): g_k, ord('H'): g_H, ord('A'): g_A, ord('V'): g_V, ord('W'): g_W, ord('X'): g_X, ord('Y'): g_Y,
          ord('Z'): g_Z, ord('7'): g_7, ord('4'): g_4, ord('2'): g_2, ord('5'): g_5, ord('9'): g_9, ord('0'): g_0,
          ord('?'): g_question}


def render(M, code):
    """returns (raster, axes) or None if the glyph is not stroke-built."""
    if M.g[ord('A')].shape[0] != 15:
        return None
    g = G(M)
    shape = M.g[code].shape
    import diag_parts                                     # diagonal parts module (% etc.)
    STROKE.update(diag_parts.STROKE)
    if code in STROKE:
        return STROKE[code](M, g, shape)                  # may be None (g_0 fallback -> contour)
    if code in (ord(','), ord(';')):
        return g_comma(M, g, shape, code)
    return None
