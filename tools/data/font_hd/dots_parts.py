"""Exactly aligned font dots (font.gfx, line height 15): ? ! (inverted: ¡ ¿), i j, : ; . , and the middle dot.

One dot for the whole font: square D x D with D = W (stroke width, output px), always fully inked (value 1).
Vertical positions (output rows, one value per scale):
  - baseline dot (. , : ; ! ?): bottom edge on the baseline row  yb = M.base_row (common baseline row)
  - upper ':' / ';' dot:         bottom edge on                   yu = round(snap_zone(9.0) * N)
  - i / j dot:                   bottom edge = stem top - G
  - gap between a stem end and its dot: G = N output px (1 original px, as in the original 'i')
Horizontal: the dot occupies exactly the columns of its stem (stem width = W = D) -> same centre axis, error 0.
  - ? ! i j: stem columns measured in the rendered glyph next to the dot
  - . , : ;: one shared dot column x0 for all four (mean ink centroid of the originals' dots)
  - , ;:     tail = right half of the dot, max(2, round(0.8 N)) rows below the baseline (as upscale4.g_comma)
  - ¡ = ! and ¿ = ? rotated by 180 deg inside their own ink box (dot/stem alignment carried over exactly),
    ink box centred where the original target has its ink, clamped to the cell.
Shapes of the ? bowl and of the stems are taken from the existing construction (upscale4 / upscale3); only the
stem end (gap G) and the dot are (re)placed.

Hook: upscale3.render calls render(M, code) first (M.dots = False disables it, used for the base renders).
"""
import numpy as np

CODES = {c: ch.encode('cp1252')[0] for c, ch in
         (('q', '?'), ('ex', '!'), ('iex', '¡'), ('iq', '¿'), ('i', 'i'), ('j', 'j'),
          ('colon', ':'), ('semi', ';'), ('dot', '.'), ('comma', ','), ('mid', '·'),
          ('ell', '…'), ('lq1', '‚'), ('lq2', '„'))}
# baseline glyphs made of several '.' / ',' : code -> (pitch in original px, count, with tail)
# (work_v3: the low quotes are built by pairs_parts - one source for all quotation marks - at the dot column
#  x_punct of this module and the same pitch, so they stay pixel-identical to ',' / two ','; only checked here)
MULTI = {CODES['ell']: (4, 3, False)}
CHECK_MULTI = {CODES['ell']: (4, 3, False), CODES['lq1']: (2, 1, True), CODES['lq2']: (2, 2, True)}
ALL = set(CODES.values()) - {CODES['lq1'], CODES['lq2']}


def applies(M):
    return M.g[ord('A')].shape[0] == 15 and getattr(M, 'dots', True)


def base_render(M, code):
    """the glyph as rendered without this module."""
    import upscale3 as S
    old = getattr(M, 'dots', True)
    M.dots = False
    try:
        return S.render(M, code)
    finally:
        M.dots = old


class Spec:
    def __init__(self, M):
        N = M.N
        self.N, self.D, self.G = N, M.W, N
        self.yb = M.base_row
        self.yu = int(round(M.snap_zone(9.0) * N))
        self.tail_w = max(1, (self.D + 1) // 2)
        self.tail_l = max(2, int(round(0.8 * N)))
        # shared column of . , : ; from the originals' dot centroids (lowest dot of each glyph)
        cs = []
        for k in ('dot', 'comma', 'colon', 'semi'):
            a = M.g[CODES[k]].copy()
            a[int(M.baseline + 0.5):] = 0           # without the comma tail
            rows = np.nonzero(a.max(axis=1) > 0.3)[0]
            a[:rows[-1] - 1] = 0                     # bottom dot only (2 rows)
            ys, xs = np.nonzero(a > 0)
            wgt = a[ys, xs]
            cs.append(float((wgt * (xs + 0.5)).sum() / wgt.sum()))
        cx = float(np.mean(cs)) * N
        Cx = np.floor(cx) + 0.5 if self.D % 2 else round(cx)
        self.x_punct = int(round(Cx - self.D / 2))


def _runs(r, thr=0.02):
    """row runs [y0, y1) with ink."""
    ink = r.max(axis=1) > thr
    out, y = [], 0
    while y < len(ink):
        if ink[y]:
            y0 = y
            while y < len(ink) and ink[y]:
                y += 1
            out.append((y0, y))
        else:
            y += 1
    return out


def stem_cols(r, rows):
    """columns [x0, x1) of the stem in the given rows (full ink in every row)."""
    sub = r[rows[0]:rows[1]]
    full = np.nonzero(sub.min(axis=0) > 0.5)[0]
    if len(full) == 0:
        return None
    # the run of full columns that is widest
    best, x = None, 0
    runs = np.split(full, np.nonzero(np.diff(full) > 1)[0] + 1)
    for run in runs:
        if best is None or len(run) > len(best):
            best = run
    return int(best[0]), int(best[-1]) + 1


def put_dot(r, ybot, x0, D):
    r[ybot - D:ybot, x0:x0 + D] = 1.0


def baseline_stem(M, sp, code):
    """? and !: stem from the construction, end trimmed / extended to dot top - G, dot under the stem."""
    r = base_render(M, code)
    runs = _runs(r)
    body = r.copy()
    body[runs[-1][0]:] = 0                          # drop the old dot
    y_end = _runs(body)[-1][1]                      # old stem end (exclusive)
    D, G = sp.D, sp.G
    xs = stem_cols(body, (y_end - 2, y_end))     # stem end (below the ? turn)
    assert xs is not None and xs[1] - xs[0] == D, (code, xs)
    dot_top = sp.yb - D
    new_end = dot_top - G
    if new_end < y_end:
        body[new_end:] = 0
    else:
        for y in range(y_end, new_end):
            body[y] = body[y_end - 1]
    put_dot(body, sp.yb, xs[0], D)
    return body


def dotted_stem(M, sp, code):
    """i / j: stem from the construction, dot D x D over the stem columns, bottom = stem top - G."""
    r = base_render(M, code)
    runs = _runs(r)
    body = r.copy()
    body[:runs[0][1]] = 0                           # drop the old dot (first run)
    top = _runs(body)[0][0]
    D = sp.D
    xs = stem_cols(body, (top, top + D))
    assert xs is not None and xs[1] - xs[0] == D, (code, xs)
    put_dot(body, top - sp.G, xs[0], D)
    return body


def punct(M, sp, code):
    a = M.g[code]
    N, D = sp.N, sp.D
    out = np.zeros((a.shape[0] * N, a.shape[1] * N))
    x0 = sp.x_punct
    put_dot(out, sp.yb, x0, D)
    if code in (CODES['colon'], CODES['semi']):
        put_dot(out, sp.yu, x0, D)
    if code in (CODES['comma'], CODES['semi']):
        out[sp.yb:min(out.shape[0], sp.yb + sp.tail_l), x0 + D - sp.tail_w:x0 + D] = 1.0
    return out


def multi(M, sp, code):
    """… (three '.'), ‚ (= ','), „ (two ','): the '.' / ',' at the shared column, repeated at the original pitch."""
    pitch, n, tail = MULTI[code]
    a = M.g[code]
    N, D = sp.N, sp.D
    out = np.zeros((a.shape[0] * N, a.shape[1] * N))
    for k in range(n):
        x0 = sp.x_punct + k * pitch * N
        put_dot(out, sp.yb, x0, D)
        if tail:
            out[sp.yb:min(out.shape[0], sp.yb + sp.tail_l), x0 + D - sp.tail_w:x0 + D] = 1.0
    return out


def middle_dot(M, sp, code):
    """the dot at the position of the existing construction (centre kept), exact D x D."""
    r = base_render(M, code)
    ys, xs = np.nonzero(r > 0.5)
    out = np.zeros_like(r)
    D = sp.D
    cy, cx = (ys.min() + ys.max() + 1) / 2.0, (xs.min() + xs.max() + 1) / 2.0
    y0, x0 = int(round(cy - D / 2)), int(round(cx - D / 2))
    out[y0:y0 + D, x0:x0 + D] = 1.0
    return out


def rotated(M, sp, code, src):
    """180 deg rotation of the finished source glyph inside its own ink box; rows as the source (same cap /
    baseline extent), columns centred on the original target's ink, clamped into the cell."""
    S_ = render(M, src)
    ys, xs = np.nonzero(S_ > 0.02)
    y0, y1, x0, x1 = ys.min(), ys.max() + 1, xs.min(), xs.max() + 1
    box = S_[y0:y1, x0:x1][::-1, ::-1]
    a = M.g[code]
    N = sp.N
    h, w = a.shape[0] * N, a.shape[1] * N
    cols = np.nonzero(a.max(axis=0) > 0.3)[0]
    wgt = a.max(axis=0)
    c_o = float((wgt * (np.arange(len(wgt)) + 0.5)).sum() / wgt.sum()) * N if len(cols) else w / 2
    bw = x1 - x0
    nx = int(round(c_o - bw / 2))
    nx = max(0, min(w - bw, nx))
    out = np.zeros((h, w))
    out[y0:y1, nx:nx + bw] = box
    return out


def render(M, code):
    """raster for a dot glyph, or None (not handled / other font)."""
    if code not in ALL or not applies(M):
        return None
    cache = M.__dict__.setdefault('_dots_cache', {})
    if code in cache:
        return cache[code].copy()
    sp = M.__dict__.get('_dots_spec') or Spec(M)
    M._dots_spec = sp
    if code in (CODES['q'], CODES['ex']):
        r = baseline_stem(M, sp, code)
    elif code in (CODES['i'], CODES['j']):
        r = dotted_stem(M, sp, code)
    elif code == CODES['iex']:
        r = rotated(M, sp, code, CODES['ex'])
    elif code == CODES['iq']:
        r = rotated(M, sp, code, CODES['q'])
    elif code == CODES['mid']:
        r = middle_dot(M, sp, code)
    elif code in MULTI:
        r = multi(M, sp, code)
    else:
        r = punct(M, sp, code)
    assert r.shape == (M.g[code].shape[0] * M.N, M.g[code].shape[1] * M.N)
    cache[code] = r
    return r.copy()


# ------------------------------------------------------------------ automatic check
def _dot_boxes(r, D):
    """exact D x D squares of value 1 that are separate components (row runs separated by empty rows)."""
    out = []
    for y0, y1 in _runs(r):
        if y1 - y0 != D:
            continue
        sub = r[y0:y1]
        allc = np.nonzero(sub.max(axis=0) > 0.02)[0]
        for cols in np.split(allc, np.nonzero(np.diff(allc) > 1)[0] + 1):
            if len(cols) == D and sub[:, cols].min() > 0.999:
                out.append((y0, y1, int(cols[0]), int(cols[-1]) + 1))
    return out


def check(M, rend=None):
    """list of (char, dot box, reference columns, axis deviation (output px), gap, notes)."""
    import upscale3 as S
    rend = rend or (lambda c: S.render(M, c))          # final pipeline (all hooks)
    sp = Spec(M)
    D, G = sp.D, sp.G
    res = []

    def row(ch, dot, ref, gap=None, note=''):
        dev = None if ref is None else (dot[2] + dot[3]) / 2.0 - (ref[0] + ref[1]) / 2.0
        res.append((ch, dot, ref, dev, gap, note))

    for ch, below in (('?', True), ('!', True), ('¡', False), ('¿', False), ('i', False), ('j', False)):
        r = rend(ch.encode('cp1252')[0])
        runs = _runs(r)
        dots = _dot_boxes(r, D)
        dot = [d for d in dots if (d[0], d[1]) == (runs[-1] if below else runs[0])]
        if not dot:
            res.append((ch, None, None, None, None, 'kein Punkt D x D gefunden'))
            continue
        dot = dot[0]
        if below:
            stem_end = runs[-2][1]
            ref = stem_cols(r, (stem_end - 2, stem_end))
            gap = dot[0] - stem_end
            note = 'Punkt unten an Grundlinie %s' % ('ja' if dot[1] == sp.yb else 'NEIN (%d != %d)' % (dot[1], sp.yb))
        else:
            stem_top = runs[1][0]
            ref = stem_cols(r, (stem_top, stem_top + 2))   # stem start (above the turn of ¿)
            gap = stem_top - dot[1]
            note = 'Punkt oben, Oberkante Zeile %d' % dot[0]
        row(ch, dot, ref, gap, note)
    base = None
    for ch in ('.', ',', ':', ';'):
        r = rend(ch.encode('cp1252')[0])
        dots = _dot_boxes(r[:sp.yb], D)
        low = [d for d in dots if d[1] == sp.yb]
        up = [d for d in dots if d[1] == sp.yu]
        if not low:
            res.append((ch, None, None, None, None, 'kein Grundlinien-Punkt'))
            continue
        if base is None:
            base = low[0]
        row(ch, low[0], (base[2], base[3]), None, 'unterer Punkt gegen "." ')
        if ch in ':;':
            if up:
                row(ch + ' oben', up[0], (low[0][2], low[0][3]), low[0][0] - up[0][1], 'oberer gegen unteren Punkt')
            else:
                res.append((ch + ' oben', None, None, None, None, 'kein oberer Punkt'))
        if ch in ',;':
            tail = r[sp.yb:]
            tc = np.nonzero(tail.max(axis=0) > 0.5)[0]
            res.append((ch + ' Schwanz', None, None, None, None,
                        'Spalten %d-%d (rechte Punkthaelfte %d-%d), %d Zeilen' %
                        (tc[0], tc[-1], low[0][3] - sp.tail_w, low[0][3] - 1, (tail.max(axis=1) > 0.5).sum())))
    for ch in ('…', '‚', '„'):
        r = rend(ch.encode('cp1252')[0])
        pitch, n, tail = CHECK_MULTI[ch.encode('cp1252')[0]]
        dots = sorted(d for d in _dot_boxes(r[:sp.yb], D) if d[1] == sp.yb)
        dots = sorted(dots, key=lambda d: d[2])
        if len(dots) != n:
            res.append((ch, None, None, None, None, '%d statt %d Punkte' % (len(dots), n)))
            continue
        for k, d in enumerate(dots):
            row(ch + ' %d' % (k + 1), d, (base[2] + k * pitch * sp.N, base[3] + k * pitch * sp.N), None,
                'gegen "." + %d x %d px' % (k, pitch * sp.N))
    r = rend(CODES['mid'])
    md = _dot_boxes(r, D)
    res.append(('·', md[0] if md else None, None, None, None,
                'Mittelpunkt %s' % ('D x D' if md else 'NICHT D x D')))
    return res, sp


def format_check(M, name):
    res, sp = check(M)
    lines = ['%s %dx: D = %d (= W), Luecke G = %d, Grundlinie yb = %d, ":"-oben yu = %d, x(. , : ;) = %d' %
             (name, M.N, sp.D, sp.G, sp.yb, sp.yu, sp.x_punct)]
    bad = 0
    for ch, dot, ref, dev, gap, note in res:
        ds = 'Zeilen %2d-%2d Spalten %2d-%2d' % (dot[0], dot[1] - 1, dot[2], dot[3] - 1) if dot else '-'
        rs = 'Bezug Spalten %2d-%2d' % (ref[0], ref[1] - 1) if ref else ''
        dv = 'Abweichung %+.1f' % dev if dev is not None else ''
        gs = 'Luecke %d' % gap if gap is not None else ''
        ok = (dot is not None or 'Schwanz' in ch) and (dev is None or dev == 0) and (gap is None or gap in (sp.G,)
                                                                                     or ch in (': oben', '; oben'))
        if ch in ('?', '!', '¡', '¿', 'i', 'j') and gap != sp.G:
            ok = False
        bad += not ok
        lines.append('  %-9s %-30s %-22s %-16s %-9s %s %s' % (ch, ds, rs, dv, gs, note, '' if ok else '  <-- FEHLER'))
    lines.append('  -> %s' % ('alle Abweichungen 0' if not bad else '%d Fehler' % bad))
    return lines, bad
