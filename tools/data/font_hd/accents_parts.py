"""Accented letters = finished base letter (pixel-identical, rendered at run time via upscale3.render) + one
constructed accent part per accent kind.

Rules (all sizes in output px of the current FontModel M):
  - Wa   accent stroke width, one value for all accents (ACC_W_SRC source px, at least 2 px)
  - gap  free rows between the top of the letter body and the accent band (N - 1, at least 1)
  - band one accent band per case: bottom = (highest body top of all base letters of that case) - gap,
         height H = min(free space above the capitals, ACC_H_SRC source px) -> same height for both cases
  - Akut  one straight stroke, ACUTE_DEG from the vertical, rising to the right, cut flat at band top/bottom
    Gravis = Akut mirrored (pixel-identical)
  - Zirkumflex two strokes at CIRC_DEG from the vertical with a sharp (mitred) apex, cut flat at the band bottom
    Hatschek = Zirkumflex flipped vertically (pixel-identical)
  - Tilde  one wave stroke (one sine period), height TILDE_FRAC * H, vertically centred in the band
  - Ring   circle, outer diameter = H, ring width Wa
  - Trema  two squares Du x Du (FontModel.Du), centre distance = original umlaut spacing per case (M.u_space),
           bottom on the band bottom
  - Cedille  stub (width Wa) from the body bottom, then a flat hook right / down / left below the baseline
  If a part is wider than the narrowest glyph cell that uses it, the part is rebuilt smaller (height - 1) until it
  fits -> still one part for all its glyphs. Placement: centred on the ink box of the body (x-height / cap part,
  without descender), the base letter keeps the left ink edge of the original accented glyph.
  Exceptions with their own form (not composed): Oe/oe slashed, AE/ae, sharp s, Eth/eth, Thorn/thorn.
"""
import math
import unicodedata as _ud
import numpy as np
import upscale as U
import upscale4 as U4

# --------------------------------------------------------------------------- parameters
ACC_W_SRC = 0.8          # accent stroke width (source px) -> 3x: 2, 2x: 2
ACC_H_SRC = 2.3          # max accent band height (source px; original accents are 2 rows) -> 3x: 7, 2x: 5
ACUTE_DEG = 33.75        # Akut / Gravis: angle to the vertical (same angle as X and 7)
CIRC_DEG = 45.0          # Zirkumflex / Hatschek arms: angle to the vertical
TILDE_FRAC = 0.75        # tilde height relative to the band height
SS = 8

ACC_NAMES = {'̀': 'Gravis', '́': 'Akut', '̂': 'Zirkumflex', '̃': 'Tilde', '̈': 'Trema',
             '̊': 'Ring', '̧': 'Cedille', '̌': 'Hatschek'}
EXCEPT = {0xD8, 0xF8, 0xC6, 0xE6, 0xDF, 0xD0, 0xF0, 0xDE, 0xFE}


def decomp(code):
    """(base code, combining accent) for a cp1252 code, or None."""
    if code < 0x80 or code in EXCEPT:
        return None
    ch = bytes([code]).decode('cp1252', 'replace')
    d = _ud.normalize('NFD', ch)
    if len(d) != 2 or d[1] not in ACC_NAMES:
        return None
    try:
        b = d[0].encode('cp1252')[0]
    except UnicodeEncodeError:
        return None
    return b, d[1]


COMPOSE_OK = {c for c in range(0x80, 0x100) if decomp(c) is not None}


def composable(M, code):
    """accented glyph present and drawn at the size of its base letter (fontk has some 15-row glyphs copied
    from font.gfx: S/s caron, Y diaeresis, Y/y acute -> those keep the normal path)."""
    d = decomp(code)
    if d is None or M.g[code].max() <= 0 or M.g[d[0]].max() <= 0:
        return False
    return abs(M.g[code].shape[0] - M.g[d[0]].shape[0]) <= 1


# --------------------------------------------------------------------------- base letters
def base_raster(M, b):
    """finished base letter (run-time call, so changes of the base construction carry over);
    i / j: dotless (everything above the x-height removed)."""
    import upscale3 as S
    r = S.render(M, b)
    if chr(b) in 'ij':
        xt = int(np.nonzero(S.render(M, ord('n')).max(axis=1) > 0.5)[0][0])
        r = r.copy()
        # cut at the last empty row between dot and stem (fallback: just above the x-height)
        ink = r.max(axis=1) > 0.02
        cut = xt - 1
        for y in range(xt, -1, -1):
            if not ink[y]:
                cut = y
                break
        r[:cut] = 0
    return r


def _ink(r, t=0.5):
    rows = np.nonzero(r.max(axis=1) > t)[0]
    cols = np.nonzero(r.max(axis=0) > t)[0]
    return rows, cols


ACC_BAND_ON_XH = False    # True: lower-case band on the hinted x-height row, gap exactly N - 1 (moves all lower-case accents at 3x)


def params(M):
    p = M.__dict__.get('_accp')
    if p is not None:
        return p
    N = M.N
    p = dict(Wa=max(2, int(round(ACC_W_SRC * N))), gap=max(1, N - 1))
    tops = {True: [], False: []}
    cells = {}
    for c in sorted(COMPOSE_OK):
        if not composable(M, c):
            continue
        b, acc = decomp(c)
        rows, _ = _ink(base_raster(M, b))
        if len(rows):
            tops[chr(b).isupper()].append(int(rows[0]))
        key = (acc, chr(b).isupper())
        cells[key] = min(cells.get(key, 10 ** 6), M.g[c].shape[1] * N)
    if tops[False] and getattr(M, 'xh_row', None) is not None and not ACC_BAND_ON_XH:
        # lower-case band stays where it was approved: x-height zone rounded (3x: one row above the hinted x-height
        # row, so 3 free rows there; v6 got this row from the unhinted u / y stem tops)
        tops[False].append(int(round(M.xh_zone * N)))
    p['top'] = {k: min(v) if v else None for k, v in tops.items()}
    p['base'] = {k: (v - p['gap'] if v is not None else None) for k, v in p['top'].items()}
    room = p['base'][True] if p['base'][True] is not None else p['base'][False]
    p['H'] = max(2, min(room, int(round(ACC_H_SRC * N))))
    p['cells'] = cells
    M.__dict__['_accp'] = p
    return p


# --------------------------------------------------------------------------- part construction
def _poly_raster(polys, h, w):
    """even-odd fill of polygons given in OUTPUT px (y, x)."""
    return U.raster([np.asarray(q, dtype=np.float64) for q in polys], 1, h, w, SS)


def _stroke(pts, Wa):
    return U4.offset_polygon([np.array(q, dtype=np.float64) for q in pts], Wa / 2.0)


def _crop(r):
    rows, cols = _ink(r, 0.05)
    if not len(rows):
        return None
    return r[rows[0]:rows[-1] + 1, cols[0]:cols[-1] + 1]


def make_acute(H, Wa):
    t = math.tan(math.radians(ACUTE_DEG))
    e = Wa
    w = int(math.ceil(H * t + 2 * Wa)) + 4
    x0 = Wa + 1.0                                   # centre line at the band bottom
    poly = _stroke([(H + e, x0 - e * t), (-e, x0 + (H + e) * t)], Wa)
    r = _poly_raster([poly], H, w)                  # rows outside [0, H) are cut flat
    return _crop(r)


def make_circ(H, Wa):
    """^ : two strokes, outer apex on the band top, sharp mitre."""
    t = math.tan(math.radians(CIRC_DEG))
    w = int(math.ceil(2 * H * t + 2 * Wa)) + 6
    cx = w / 2.0
    # centre-line apex below the band top so that the mitred outer apex touches row 0
    ay = Wa / 2.0 / math.sin(math.radians(CIRC_DEG))
    e = Wa
    L = (H + e - ay)
    poly = _stroke([(ay + L, cx - L * t), (ay, cx), (ay + L, cx + L * t)], Wa)
    r = _poly_raster([poly], H, w)
    return _crop(r)


def make_tilde(H, Wa):
    ht = max(Wa + 2, int(round(TILDE_FRAC * H)))
    amp = (ht - Wa) / 2.0
    width = 1.6 * H
    w = int(math.ceil(width)) + 2 * Wa + 4
    x0 = Wa + 1.0
    pts = []
    n = 40
    for i in range(n + 1):
        s = i / n
        x = x0 + s * width
        y = ht / 2.0 - amp * math.sin(2 * math.pi * s)   # starts in the middle, up, down, back to the middle
        pts.append((y, x))
    r = _poly_raster([_stroke(pts, Wa)], ht + 2, w)
    return _crop(r)


# v9: font.gfx 2x ring (H = 5): a closed round ring - full edges, soft outer corners, an open 3 x 3 hole
RING_5 = np.array([[.4, 1, 1, 1, .4],
                   [1, 0, 0, 0, 1],
                   [1, 0, 0, 0, 1],
                   [1, 0, 0, 0, 1],
                   [.4, 1, 1, 1, .4]])
RING_PLUS_HOLE = False   # v8: disc with a plus-shaped opening at H = 5


def make_ring(H, Wa, plus_hole=False):
    d = H
    R = d / 2.0
    c = (d / 2.0, d / 2.0)
    n = 64
    outer = [(c[0] + R * math.sin(2 * math.pi * i / n), c[1] + R * math.cos(2 * math.pi * i / n)) for i in range(n)]
    if plus_hole and d == 5 and not RING_PLUS_HOLE:
        return RING_5.copy()
    if plus_hole:                                   # H = 5: no room for a round hole (R - Wa = 0.5 is a grey pinhole):
        r = _poly_raster([outer], d, d)             # disc with a crisp plus-shaped opening, same form as the 3x hole
        m = d // 2
        r[m - 1:m + 2, m] = 0
        r[m, m - 1:m + 2] = 0
        return _crop(r)
    ri = max(0.5, R - Wa)
    inner = [(c[0] + ri * math.sin(2 * math.pi * i / n), c[1] + ri * math.cos(2 * math.pi * i / n)) for i in range(n)]
    return _crop(_poly_raster([outer, inner], d, d))


def make_dots(M, upper):
    N, D = M.N, M.Du
    sp = M.u_space.get(upper) or 2.25
    gapx = max(1, int(round(sp * N)) - D)
    part = np.zeros((D, 2 * D + gapx))
    part[:, :D] = 1
    part[:, D + gapx:] = 1
    return part


def make_cedilla(Wa, N):
    """stub down from the body bottom, hook right / down / left (centre line), sharp corners, butt ends.
    origin: row 0 = first row below the body, column centre = Wa/2 + k."""
    k = max(1.0, round(0.7 * N))                   # hook half width (centre line)
    s = Wa / 2.0 + max(0.0, N / 3.0 - 0.5)         # stub length below the body (centre line of the turn)
    d = Wa + max(1.0, N / 3.0)                     # hook height (centre line)
    cx = Wa / 2.0 + k + 1
    pts = [(-Wa, cx), (s, cx), (s, cx + k), (s + d, cx + k), (s + d, cx - k - Wa / 2.0)]
    h = int(math.ceil(s + d + Wa)) + 1
    w = int(math.ceil(cx + k + Wa)) + 2
    r = _poly_raster([_stroke(pts, Wa)], h, w)
    rows, cols = _ink(r, 0.05)
    # keep the column frame relative to the stub centre: return (raster, stub centre x)
    return r[:rows[-1] + 1, cols[0]:cols[-1] + 1], cx - cols[0]


def accent_part(M, acc, upper):
    """(raster, anchor_x) of the uniform accent part; anchor_x = column that goes onto the letter centre."""
    cache = M.__dict__.setdefault('_accparts', {})
    key = (acc, upper)
    if key in cache:
        return cache[key]
    p = params(M)
    Wa, H = p['Wa'], p['H']
    limit = p['cells'].get(key)
    if acc in ('̀', '́'):
        lim = min([v for (a, u), v in p['cells'].items() if a in ('̀', '́')] or [10 ** 6])
        h = H
        part = make_acute(h, Wa)
        while part.shape[1] > lim and h > Wa + 1:
            h -= 1
            part = make_acute(h, Wa)
        if acc == '̀':
            part = part[:, ::-1]
    elif acc in ('̂', '̌'):
        lim = min([v for (a, u), v in p['cells'].items() if a in ('̂', '̌')] or [10 ** 6])
        h = H
        part = make_circ(h, Wa)
        while part.shape[1] > lim and h > Wa + 1:
            h -= 1
            part = make_circ(h, Wa)
        if acc == '̌':
            part = part[::-1]
    elif acc == '̃':
        part = make_tilde(H, Wa)
    elif acc == '̊':
        part = make_ring(H, Wa, M.N == 2 and M.g[ord('A')].shape[0] == 15)   # font.gfx 2x only
    elif acc == '̈':
        part = make_dots(M, upper)
    elif acc == '̧':
        part, ax = make_cedilla(Wa, M.N)
        cache[key] = (part, ax)
        return cache[key]
    else:
        part = None
    res = (part, part.shape[1] / 2.0) if part is not None else (None, 0)
    cache[key] = res
    return res


# --------------------------------------------------------------------------- composition
def placed_base(M, code):
    """the finished base letter in the accented glyph's cell: left ink edge of the body as in the original."""
    b, acc = decomp(code)
    N = M.N
    a = M.g[code]
    h, w = a.shape[0] * N, a.shape[1] * N
    base = base_raster(M, b)
    rows_b = np.nonzero(M.g[b].max(axis=1) > 0.1)[0]
    s_row = rows_b[0] if chr(b) not in 'ij' else int(np.nonzero(M.g[ord('n')].max(axis=1) > 0.1)[0][0])
    cb = np.nonzero(M.g[b][s_row:].max(axis=0) > 0.3)[0]
    ca = np.nonzero(a[s_row:].max(axis=0) > 0.3)[0]
    dx = int(round((ca[0] - cb[0]) * N)) if len(ca) and len(cb) else 0
    ink = np.nonzero(base.max(axis=0) > 0.02)[0]
    if len(ink) and ink[0] + dx < 0:              # never cut the left stem at the cell edge (Ntilde: original has no margin)
        dx = -int(ink[0])
    out = np.zeros((h, w))
    hh = min(h, base.shape[0])                    # cells are top-aligned (fontk: Ccedil is one row taller)
    for x in range(w):
        xs = x - dx
        if 0 <= xs < base.shape[1]:
            out[:hh, x] = base[:hh, xs]
    return out


def compose(M, code):
    """(raster, message) or None if code is not an accented letter of the font."""
    if not composable(M, code):
        return None
    b, acc = decomp(code)
    N = M.N
    a = M.g[code]
    h, w = a.shape[0] * N, a.shape[1] * N
    upper = chr(b).isupper()
    p = params(M)
    out = placed_base(M, code)
    part, ax = accent_part(M, acc, upper)
    if part is None:
        return out, 'Akzent fehlt (%s)' % ACC_NAMES[acc]
    rows, cols = _ink(out)
    # centre: ink box of the body above the baseline (without descender)
    bl = int(round(M.baseline * N))
    body = out[:bl]
    _, cols = _ink(body)
    cx = (cols[0] + cols[-1] + 1) / 2.0
    ph, pw = part.shape
    if acc == '̧':
        yb = int(rows[rows < bl][-1]) + 1 if np.any(rows < bl) else bl
        y0 = yb
        x0 = int(math.floor(cx - ax + 0.5))
    else:
        bottom = p['base'][upper]
        if acc == '̃':
            y0 = bottom - p['H'] + (p['H'] - ph) // 2        # tilde: centred in the band
        else:
            y0 = bottom - ph                                  # everything else: on the band bottom
        x0 = int(math.floor(cx - pw / 2.0 + 0.5))
    if pw <= w:
        x0 = max(0, min(w - pw, x0))
    y0 = max(0, y0)
    ys0, ys1 = max(0, y0), min(h, y0 + ph)
    xs0, xs1 = max(0, x0), min(w, x0 + pw)
    out[ys0:ys1, xs0:xs1] = np.maximum(out[ys0:ys1, xs0:xs1], part[ys0 - y0:ys1 - y0, xs0 - x0:xs1 - x0])
    return out, '= %s + %s (Bauteile, accents_parts)' % (chr(b), ACC_NAMES[acc])
