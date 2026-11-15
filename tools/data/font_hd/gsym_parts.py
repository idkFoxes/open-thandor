"""font.gfx symbols built from parts (work_gsym), 3x and 2x only; fontk.gfx is not touched:
* 0x2A six straight arms of uniform width (the original has six: vertical + two at about 60 deg), raised
TM 0x99 small T and M from font.gfx letters at high resolution, area-downscaled (frac_parts route), raised
P  0xB6 two rectangular stems + top band + solid bowl on the left
f  0x83 slanted stem, top and bottom arc (radii of y_tail_arc), run ends and crossbar rectangles at the x-height"""
import math
import numpy as np

CODES = (0x2A, 0x99, 0xB6, 0x83)
SS = 16
CAP = {2: 6, 3: 10}                      # cap top row
BOTTOM = {2: 29, 3: 43}                  # end of the descender (= y's tail)
XBAR = {2: (10, 13), 3: (16, 20)}        # f's crossbar rows
STAR = {3: (3, 10, 25, 7.5), 2: (2, 6, 16, 5.0)}    # arm width, rows [y0, y1), centre x
PILC = {3: (6, 13, 1, 10, 23, 5), 2: (4, 9, 0, 6, 15, 4)}    # stem 1 x, stem 2 x, bowl left, bowl rows, corner radius
FH = {3: dict(th=14.0, xb=1.5, xe=17, b0=4, b1=17), 2: dict(th=14.0, xb=1.0, xe=12, b0=2, b1=11)}
# TM: high-res scale, high-res stroke, downscale factor, ink height, gap. Final stroke 3 (3x) / 2 (2x)
TMC = {3: (9, 24, 8, 12, 1), 2: (6, 16, 8, 8, 1)}


def _grid(h, w):
    y, x = (np.mgrid[0:h * SS, 0:w * SS] + 0.5) / SS
    return y, x


def _down(m, h, w):
    return m.reshape(h, SS, w, SS).mean(axis=(1, 3))


def star(M):
    N = M.N
    h, w = M.g[0x2A].shape[0] * N, M.g[0x2A].shape[1] * N
    W, y0, y1, cx = STAR[N]
    cy, R = (y0 + y1) / 2.0, (y1 - y0) / 2.0
    y, x = _grid(h, w)
    m = np.zeros(y.shape, bool)
    for deg in (0, 60, 120):                       # one bar through the centre per direction: 6 arms
        a = math.radians(deg)
        dx, dy = math.sin(a), -math.cos(a)
        t = (x - cx) * dx + (y - cy) * dy
        s = (x - cx) * dy - (y - cy) * dx
        m |= (np.abs(t) <= R) & (np.abs(s) <= W / 2.0)
    return _down(m, h, w)


def pilcrow(M):
    N, W = M.N, M.W
    h, w = M.g[0xB6].shape[0] * N, M.g[0xB6].shape[1] * N
    s1, s2, bl, b0, b1, r = PILC[N]
    bot = {3: 35, 2: 24}[N]
    y, x = _grid(h, w)
    m = (y >= b0) & (y <= b1) & (x >= bl) & (x <= s1 + W)
    for sy, cy in ((-1, b0 + r), (1, b1 - r)):     # the bowl's two left corners, circular
        cx = bl + r
        out = (x < cx) & ((y - cy) * sy > 0) & ((x - cx) ** 2 + (y - cy) ** 2 > r * r)
        m &= ~out
    m |= (y >= b0) & (y <= b0 + W) & (x >= s1) & (x <= s2 + W)       # top band joins the stems
    m |= (y >= b0) & (y <= bot) & (x >= s1) & (x <= s1 + W)
    m |= (y >= b0) & (y <= bot) & (x >= s2) & (x <= s2 + W)
    return _down(m, h, w)


def fhook(M):
    import upscale3 as S
    N, W = M.N, M.W
    h, w = M.g[0x83].shape[0] * N, M.g[0x83].shape[1] * N
    arc = S.y_tail_arc(M)
    A = arc.shape[0]                               # outer radius of the f / y arc
    R = A - W / 2.0
    p = FH[N]
    th = math.radians(p['th'])
    st, ct = math.sin(th), math.cos(th)
    top, bot = CAP[N], BOTTOM[N]
    cyt, cyb = top + A, bot - A
    xb = p['xb']
    # internal tangent of the two arcs at slant th: p_b - p_t runs along (-st, ct)
    L = (cyb - cyt + 2 * R * st) / ct
    xt = xb + L * st + 2 * R * ct
    y, x = _grid(h, w)
    pt = (xt - R * ct, cyt - R * st)
    m = (y >= top) & (y <= top + W) & (x >= xt) & (x <= p['xe'])
    m |= (y >= bot - W) & (y <= bot) & (x >= 0) & (x <= xb)
    # top arc: from straight up to the stem's normal; bottom arc: point mirror of it
    for cx, cy, sg in ((xt, cyt, 1), (xb, cyb, -1)):
        dx, dy = (x - cx) * sg, (y - cy) * sg
        d2 = dx * dx + dy * dy
        ring = (d2 <= (R + W / 2.0) ** 2) & (d2 >= (R - W / 2.0) ** 2)
        sector = (dx <= 0) & (dx * st - dy * ct >= 0)   # left of "up", above the stem's normal
        m |= ring & sector
    t = (x - pt[0]) * (-st) + (y - pt[1]) * ct
    s = (x - pt[0]) * ct + (y - pt[1]) * st
    m |= (t >= 0) & (t <= L) & (np.abs(s) <= W / 2.0)
    y0, y1 = XBAR[N]
    m |= (y >= y0) & (y <= y1) & (x >= p['b0']) & (x <= p['b1'])
    return _down(m, h, w)


_tm = {}


def _letters(N):
    if N not in _tm:
        import upscale as U, upscale3 as S, dumpcur as D, frac_parts as F
        NH, WH, k, H, gap = TMC[N]
        G = S.FontModel(U.load('font.gfx'), NH, W=WH, **D.FONT_KW['font.gfx'])
        out = {}
        for ch in 'TM':
            c = F._crop(S.render(G, ord(ch)))
            if ch == 'T':                              # bar ends on whole px: stem + an equal whole number of px per side
                n = (c.shape[1] - WH) // (2 * k)
                d = (c.shape[1] - WH - 2 * n * k) // 2
                c = c[:, d:d + WH + 2 * n * k]
            c = F.hint(c, k, H * k)
            out[ch] = F._crop(min((F._down(c, k, dx) for dx in range(k)), key=F._soft))
        _tm[N] = out
    return _tm[N]


def tm(M):
    N = M.N
    h, w = M.g[0x99].shape[0] * N, M.g[0x99].shape[1] * N
    L = _letters(N)
    T, m = L['T'], L['M']
    gap = TMC[N][4]
    ink = T.shape[1] + gap + m.shape[1]
    if ink > w:
        return None
    x = (w - ink) // 2
    top = CAP[N]
    r = np.zeros((h, w))
    r[top:top + T.shape[0], x:x + T.shape[1]] = T
    x += T.shape[1] + gap
    r[top:top + m.shape[0], x:x + m.shape[1]] = m
    return r


def render(M, code):
    if code not in CODES or M.N not in CAP or M.g[ord('A')].shape[0] != 15:
        return None
    return {0x2A: star, 0x99: tm, 0xB6: pilcrow, 0x83: fhook}[code](M)
