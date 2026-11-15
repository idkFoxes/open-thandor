"""Fractions and superscripts (work_frac): 1/4 1/2 3/4 (0xBC 0xBD 0xBE) and sup 1 2 3 (0xB9 0xB2 0xB3) composed from
font.gfx's finished digits, rendered at high resolution and area-downscaled per digit (see digits()), pasted on whole
output px so equal digits are pixel-identical, + a slash of the font's construction (22.5 deg from vertical, uniform
width = the small digits' stroke width, flat cut at cap top and baseline, supersampled).
Numerator / superscript: top edge on the cap top. Denominator: bottom edge on the baseline. The cell size and advance
stay the original ones. Both fonts carry these glyphs in the same 15-row cell, so both get the same raster."""
import math
import numpy as np

FRAC = {0xBC: ('1', '4'), 0xBD: ('1', '2'), 0xBE: ('3', '4')}
SUP = {0xB9: '1', 0xB2: '2', 0xB3: '3'}
CODES = set(FRAC) | set(SUP)
# ink rows of font.gfx's full-size digits: [cap top, baseline) in output px (checked against the finished '0')
CAP = {2: (6, 24), 3: (10, 35), 4: (13, 47)}
# output scale -> high-resolution render scale, its stroke width, integer downscale factor, final stroke width
CFG = {2: (4, 6, 3, 2), 3: (4, 6, 2, 3), 4: (8, 12, 3, 4)}
DH = {2: 12, 3: 18, 4: 24}         # final digit height
GAP = (1,)                         # empty pixels (8-neighbourhood) between slash and digits
INK = 0.1                          # coverage below this does not count as ink for the gap
T225 = math.tan(math.radians(22.5))
SS = 8
_digits = {}


def _crop(r):
    rows = np.nonzero(r.max(axis=1) > 0.02)[0]
    cols = np.nonzero(r.max(axis=0) > 0.02)[0]
    return r[rows[0]:rows[-1] + 1, cols[0]:cols[-1] + 1]


def hint(c, k, H):
    """vertical hinting at high resolution: scale the digit to H rows by stretching the parts between its horizontal
    bars (linear between rows: stems stay exact, diagonals stay straight), with every bar copied unchanged and starting
    on a multiple of the downscale factor k (so bars stay crisp)."""
    h, w = c.shape
    full = (c > 0.5).sum(axis=1) >= 0.55 * w
    bars, y = [], 0
    while y < h:
        if full[y]:
            y0 = y
            while y < h and full[y]:
                y += 1
            bars.append((y0, y))
        else:
            y += 1
    if len(bars) == h or (len(bars) == 1 and bars[0] == (0, h)):        # plain stem
        bars = []
    out, sy, ty = [], 0, 0
    for y0, y1 in bars + [(h, h)]:
        L = y1 - y0
        if y1 == h and L:
            t = H - L                                                   # last bar ends on the bottom edge
        elif y0 == h:
            t = H
        elif y0 == 0:
            t = 0
        else:
            t = int(round(y0 * H / float(h) / k)) * k
        n, G = t - ty, y0 - sy
        for i in range(max(0, n)):
            p = min(max((i + 0.5) * G / float(n) - 0.5, 0.0), G - 1.0)
            f = int(math.floor(p))
            g = min(f + 1, G - 1)
            out.append(c[sy + f] * (1 - (p - f)) + c[sy + g] * (p - f))
        out += [c[y] for y in range(y0, y1)]
        sy, ty = y1, t + L
    return np.array(out)


def _down(c, k, dx):
    h, w = c.shape
    W = int(math.ceil((w + dx) / float(k)))
    p = np.zeros((h, W * k))
    p[:, dx:dx + w] = c
    return p.reshape(h // k, k, W, k).mean(axis=(1, 3))


def _soft(a):
    return ((a > 0.02) & (a < 0.98)).sum()


def digits(N):
    """small digits for output scale N: font.gfx's finished digits rendered at high resolution, hinted, area-downscaled
    (box filter) by an integer factor. Column offset: the one with the fewest partial pixels (stems on whole px)."""
    if N not in _digits:
        import upscale as U, upscale3 as S, dumpcur as D
        NH, WH, k, W = CFG[N]
        M = S.FontModel(U.load('font.gfx'), NH, W=WH, **D.FONT_KW['font.gfx'])
        out = {}
        for ch in '1234':
            c = hint(_crop(S.render(M, ord(ch))), k, DH[N] * k)
            out[ch] = _crop(min((_down(c, k, dx) for dx in range(k)), key=_soft))
        _digits[N] = (out, W)
    return _digits[N]


def slash(h, w, top, bot, xm, W):
    """stroke of perpendicular width W through (ym, xm), 22.5 deg from vertical, cut flat at rows top / bot."""
    y, x = (np.mgrid[0:h * SS, 0:w * SS] + 0.5) / SS
    hw = W / 2.0 / math.cos(math.radians(22.5))        # half width measured along a row
    cx = xm - (y - (top + bot) / 2.0) * T225
    m = (np.abs(x - cx) <= hw) & (y >= top) & (y <= bot)
    return m.reshape(h, SS, w, SS).mean(axis=(1, 3))


def _grow(m, n):
    for _ in range(n):
        p = np.pad(m, 1)
        m = p[:-2, :-2] | p[:-2, 1:-1] | p[:-2, 2:] | p[1:-1, :-2] | p[1:-1, 1:-1] | p[1:-1, 2:] | p[2:, :-2] | p[2:, 1:-1] | p[2:, 2:]
    return m


def _between(s, gn, gd):
    """on every slash row: all (grown) numerator ink left of the slash, all (grown) denominator ink right of it."""
    for y in np.nonzero((s > INK).any(axis=1))[0]:
        xs = np.nonzero(s[y] > INK)[0]
        n, d = np.nonzero(gn[y])[0], np.nonzero(gd[y])[0]
        if (len(n) and n[-1] >= xs[0]) or (len(d) and d[0] <= xs[-1]):
            return False
    return True


def layout(h, w, top, bot, dn, dd, W, gap):
    """tightest placement: numerator at x = 0, smallest denominator x for which a slash fits between the two with
    the gap on both sides. Returns (x denominator, slash centre, ink width) or None."""
    hn, wn = dn.shape
    hd, wd = dd.shape
    num = np.zeros((h, w), bool)
    num[top:top + hn, :wn] = dn > INK
    gn = _grow(num, gap)
    for xd in range(1, w - wd + 1):
        den = np.zeros((h, w), bool)
        den[bot - hd:bot, xd:xd + wd] = dd > INK
        gd = _grow(den, gap)
        ok = []
        for k in range(0, 4 * w + 1):
            xm = k / 4.0
            s = slash(h, w, top, bot, xm, W)
            if s.sum() < W / math.cos(math.radians(22.5)) * (bot - top) * 0.99:       # clipped by the cell
                continue
            if _between(s, gn, gd):
                ok.append(xm)
        if ok:
            return xd, ok[len(ok) // 2], xd + wd
    return None


def render(M, code):
    """finished raster for a code in CODES or None (other scale / cell)."""
    N = M.N
    if code not in CODES or N not in CAP or M.g[code].shape[0] != 15:
        return None
    h, w = M.g[code].shape[0] * N, M.g[code].shape[1] * N
    dg, W = digits(N)
    top, bot = CAP[N]
    r = np.zeros((h, w))
    if code in SUP:
        d = dg[SUP[code]]
        if d.shape[1] > w:
            return None
        x = (w - d.shape[1]) // 2
        r[top:top + d.shape[0], x:x + d.shape[1]] = d
        return r
    dn, dd = dg[FRAC[code][0]], dg[FRAC[code][1]]
    res = None
    for gap in GAP:
        res = res or layout(h, w, top, bot, dn, dd, W, gap)
    if res is None:
        return None
    xd, xm, ink = res
    ox = max(0, (w - ink - N) // 2)                    # centred, keeping a right side bearing of one source px
    r[top:top + dn.shape[0], ox:ox + dn.shape[1]] = dn
    r[bot - dd.shape[0]:bot, ox + xd:ox + xd + dd.shape[1]] = dd
    return np.maximum(r, slash(h, w, top, bot, xm + ox, W))
