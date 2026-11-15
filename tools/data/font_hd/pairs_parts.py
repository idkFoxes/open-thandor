"""Mirror pairs and quotation marks (font.gfx and the 15-row punctuation of fontk.gfx).

Rule: ONE shape is constructed (opening bracket, left guillemet, '<', '/', the comma-shaped quote part), the
partner is its pixel-identical horizontal mirror (function mirror_partner, never a separate construction).

Shapes (stroke construction of upscale4: font-wide width W, arcs with the font radii, flat / butt ends):
  (  stem + 22.5 deg end strokes (angle of the font's diagonals), turn radius r_big, ends cut flat at the
     top / bottom line, vertically symmetric (mirror_y).
  [  stem + two bars, sharp corners (as the original).
  {  stem, top / bottom hook with r_small, 45 deg point in the middle, vertically symmetric.
  <  same chevron function, parameters of upscale4.g_less (22.5 deg to the horizontal), ends cut
     vertically (g_less left a stray column at the tip).   / : whatever S.render gives (diagonal agent); here only \\ = mirror(/).
  <  (single guillemet) chevron: two strokes at 33.75 deg to the vertical (as X / 7), blunt tip, cut flat at top / bottom, symmetric;
  << = two of the same chevron part, nested (pitch W*sqrt2 + gap).
Heights: brackets from the cap top edge (TOPe, as E/H) to the original bottom (13.5, includes the descender);
guillemets centred on the original guillemet centre.

Placement ("symmetric to the partner, in its own original advance"): the source keeps its original left
ink edge (rounded to output px); the partner's inner bearing (left) = the source's inner bearing (right);
if the partner's advance is smaller (only << 8 / >> 7), the source is moved right to at least half of the
partner's free space, and the partner's inner bearing shrinks so that both outer bearings stay equal.

Quotes: one part Q = the comma part (dot D x D + tail of width ceil(D/2) on the right, as upscale4.g_comma).
  , (0x82) = Q low (dot bottom on the baseline, as ','),  ,, (0x84) = 2 x Q low,
  ' (0x92) = Q high (top on the cap top edge), '' (0x94) = 2 x Q high,
  ` (0x91) = Q high rotated 180 deg (6-shape), `` (0x93) = 2 x rotated,
  ' and " (straight) = a tick of width D and height 3 source px, " = 2 ticks.
  Double marks: pitch D + max(1, round(0.67 N)) for all of them.
"""
import math
import numpy as np
import upscale3 as S
import upscale4 as U4


def _c(ch):
    return ch.encode('cp1252')[0]


OX = 3                                    # canvas margin (source px) left of the construction


# ------------------------------------------------------------------ helpers
def ink_cols(r, t=0.02):
    c = np.nonzero(r.max(axis=0) > t)[0]
    return (int(c[0]), int(c[-1]) + 1) if len(c) else None


def place(part, h, w, x_left):
    """part: raster with the full output height h; its ink box goes to column x_left (output px)."""
    out = np.zeros((h, w))
    ic = ink_cols(part)
    if ic is None:
        return out
    a, b = ic
    src = part[:h, a:b]
    x0, x1 = x_left, x_left + (b - a)
    s0, s1 = max(0, -x0), (b - a) - max(0, x1 - w)
    if s1 > s0:
        out[:src.shape[0], max(0, x0):max(0, x0) + (s1 - s0)] = src[:, s0:s1]
    return out


def orig_lb(M, code):
    """original left ink edge (coverage-weighted), output px."""
    a = M.g[code]
    cols = np.nonzero(a.max(axis=0) > 0.3)[0]
    if len(cols) == 0:
        return 0
    lb = cols[0] + 1 - a[:, cols[0]].max()
    return int(round(lb * M.N))


def canvas(M, code):
    a = M.g[code]
    return (a.shape[0], a.shape[1] + 2 * OX)


def mirror_partner(M, code, src):
    """partner = pixel-identical horizontal mirror of the source glyph raster (whatever builds the source)."""
    N = M.N
    s = S.render(M, src)
    h, w = M.g[code].shape[0] * N, M.g[code].shape[1] * N
    ic = ink_cols(s)
    if ic is None:
        return np.zeros((h, w))
    lb_s, rb_s = ic[0], s.shape[1] - ic[1]
    wid = ic[1] - ic[0]
    lb_t = max(0, min(rb_s, w - wid - lb_s))
    return place(s[:, ::-1], h, w, lb_t)


def mirror_pair_ok(M, a, b):
    """test helper: ink of b == ink of a mirrored (pixel-identical)."""
    ra, rb = S.render(M, a), S.render(M, b)
    ia, ib = ink_cols(ra), ink_cols(rb)
    A = ra[:, ia[0]:ia[1]][:, ::-1]
    B = rb[:, ib[0]:ib[1]]
    return A.shape == B.shape and float(np.abs(A - B).max()) == 0.0


# ------------------------------------------------------------------ vertical frame of the brackets
def frame(M, g):
    T = g.TOPe                                           # cap top edge (as E / H)
    B = g.se(13.5)                                       # original bracket bottom (with descender)
    yc = round((T + B) * M.N / 2 * 2) / 2 / M.N          # vertical mirror axis on the half-pixel grid
    return T, B, yc


# ------------------------------------------------------------------ opening shapes
def g_paren(M, g, code):
    shape = canvas(M, code)
    T, B, yc = frame(M, g)
    xs = OX + g.w / 2                                    # stem centre (left ink edge at OX)
    dxe = 1.25                                           # end centre offset at the top line (original '(' 2.1 - 0.85)
    t = U4.T225
    xe = xs + dxe
    y1 = T + dxe / t                                     # start of the straight stem
    ext = 2 * g.w
    pts = [(T - ext, xe + ext * t), (y1, xs), (yc + g.w, xs)]
    r = U4.union(U4.stroke_raster(M, shape, pts, rad=['big'], clip=(T, B)))
    r = U4.mirror_y(r, yc * M.N)
    return r


def g_bracket(M, g, code):
    shape = canvas(M, code)
    T, B, yc = frame(M, g)
    xs = OX + g.w / 2
    xr = OX + g.se(2.8)                                  # original '[' ink width 2.8
    pts = [(T + g.w / 2, xr), (T + g.w / 2, xs), (B - g.w / 2, xs), (B - g.w / 2, xr)]
    return U4.union(U4.stroke_raster(M, shape, pts))


def g_brace(M, g, code):
    shape = canvas(M, code)
    T, B, yc = frame(M, g)
    xp = OX + g.w / 2                                    # point centre (left ink edge ~ OX)
    k = 1.25                                             # 45 deg run of the point (original: stem col 1-2, point col 0)
    xs = xp + k                                          # stem centre
    xr = OX + g.se(4.0)                                  # hook end (original '{' ink 0.2 .. 4.3)
    ext = g.w
    pts = [(T + g.w / 2, xr), (T + g.w / 2, xs), (yc - k, xs), (yc + ext, xp - ext)]
    r = U4.union(U4.stroke_raster(M, shape, pts, rad=['small', 'small']))
    r[int(round(yc * M.N)):] = 0
    r = U4.mirror_y(r, yc * M.N)
    # blunt point: cut flat at the point centre's left stroke edge
    r[:, :int(round((xp - g.w / 2) * M.N))] = 0
    return r


def chevron(M, g, shape, xt, yc, a, slope, xcut=None):
    """'<'-type part: tip centre (yc, xt), two arms with dx/dy = slope (1 = 45 deg), centre-line half height a,
    ends cut flat at the top / bottom line (xcut given: cut vertically at x = xcut instead), sharp inner corner, blunt tip (cut at the tip centre - W/2),
    vertically symmetric (mirror_y about yc)."""
    e = 2 * g.w
    pts = [(yc - a - e, xt + (a + e) * slope), (yc, xt), (yc + a + e, xt + (a + e) * slope)]
    r = U4.union(U4.stroke_raster(M, shape, pts))
    if xcut is None:
        yt, yb = yc - a - g.w / 2, yc + a + g.w / 2
        r[:int(round(yt * M.N))] = 0
        r[int(round(yb * M.N)):] = 0
    else:
        r[:, int(round(xcut * M.N)):] = 0
    r[:, :int(round((xt - g.w / 2) * M.N))] = 0
    return U4.mirror_y(r, yc * M.N)


GUIL_A = 1.5                                             # guillemet centre-line half height (source px)
GUIL_SLOPE = U4.T3375                                    # arms 33.75 deg to the vertical (angle of X / 7)


def guil_yc(M, g):
    return round(8.8 * M.N * 2) / 2 / M.N                # original guillemet centre (cy 8.8), half-pixel grid


def guil_part(M, g, code):
    return chevron(M, g, canvas(M, code), OX + g.w / 2, guil_yc(M, g), GUIL_A, GUIL_SLOPE)


def g_guil1(M, g, code):
    return guil_part(M, g, code)


def g_guil2(M, g, code):
    """<< = two single guillemet parts (pixel-identical), nested like the original: pitch = horizontal width
    of an arm (W / cos 33.75) + a gap of max(1, round(0.67 N)) px between the parallel arms."""
    one = guil_part(M, g, code)
    pitch = int(round(M.W / math.cos(math.atan(GUIL_SLOPE)))) + max(1, int(round(0.67 * M.N)))
    two = one.copy()
    two[:, pitch:] = np.maximum(two[:, pitch:], one[:, :-pitch])
    return two


def g_less(M, g, code):
    """< : same chevron construction, parameters of upscale4.g_less (arms 22.5 deg to the horizontal,
    tip height 7.8, tip 0.7 + W/2, right end 7.2); replaces g_less (its tip left a stray column)."""
    slope = math.tan(math.radians(67.5))
    yc = g.sc(7.8)
    xt = OX + 0.7 + g.w / 2
    xr = OX + g.se(7.2)
    a = (xr - xt) / slope
    return chevron(M, g, canvas(M, code), xt, yc, a + g.w, slope, xcut=xr)


OPEN = {_c('<'): g_less, _c('('): g_paren, _c('['): g_bracket, _c('{'): g_brace, _c('‹'): g_guil1, _c('«'): g_guil2}
PARTNER = {_c(')'): _c('('), _c(']'): _c('['), _c('}'): _c('{'), _c('›'): _c('‹'),
           _c('»'): _c('«'), _c('>'): _c('<'), _c('\\'): _c('/')}


# ------------------------------------------------------------------ quotation marks
def comma_part(M):
    """dot D x D + tail (width ceil(D/2), right half, length max(2, round(0.8 N))) - same as upscale4.g_comma."""
    D, N = M.D, M.N
    tw = max(1, (D + 1) // 2)
    tl = max(2, int(round(0.8 * N)))
    p = np.zeros((D + tl, D))
    p[:D] = 1
    p[D:, D - tw:] = 1
    return p


def quote_pitch(M):
    return M.D + max(1, int(round(0.67 * M.N)))


QUOTES = {_c('‚'): ('low', 1, False), _c('„'): ('low', 2, False),
          _c('’'): ('high', 1, False), _c('”'): ('high', 2, False),
          _c('‘'): ('high', 1, True), _c('“'): ('high', 2, True),
          _c("'"): ('tick', 1, False), _c('"'): ('tick', 2, False)}


def g_quote(M, g, code):
    N = M.N
    kind, n, rot = QUOTES[code]
    a = M.g[code]
    h, w = a.shape[0] * N, a.shape[1] * N
    if kind == 'tick':
        p = np.ones((int(round(3.0 * N)), M.D))
    else:
        p = comma_part(M)
        if rot:
            p = p[::-1, ::-1]
    ph, pw = p.shape
    pitch = quote_pitch(M)
    tw = pw + (n - 1) * pitch
    part = np.zeros((ph, tw))
    for i in range(n):
        part[:, i * pitch:i * pitch + pw] = np.maximum(part[:, i * pitch:i * pitch + pw], p)
    if kind == 'low':
        y0 = M.base_row - M.D        # dot top, dot bottom on the baseline (as ',')
    else:
        y0 = int(round(g.TOPe * N))
    x0 = min(orig_lb(M, code), max(0, w - tw))
    if kind == 'low':
        # one optics with ',' (dots_parts): same dot column and the same pitch (2 original px) -> the low quotes
        # are ',' resp. two ',' pixel-identically
        import dots_parts
        if dots_parts.applies(M):
            sp = M.__dict__.get('_dots_spec') or dots_parts.Spec(M)
            M._dots_spec = sp
            x0 = sp.x_punct
            if n == 2:
                pitch = 2 * N
                part = np.zeros((ph, pw + pitch))
                for i in range(n):
                    part[:, i * pitch:i * pitch + pw] = np.maximum(part[:, i * pitch:i * pitch + pw], p)
                tw = part.shape[1]
    out = np.zeros((h, w))
    out[y0:y0 + ph, x0:x0 + tw] = part[:max(0, min(ph, h - y0)), :min(tw, w - x0)]
    return out


def source_lb(M, code, r):
    """left ink edge of a constructed source: the original one; if the partner's advance is smaller
    (<< 8 / >> 7), at least half of the partner's free space, so that the mirrored partner keeps the same
    outer bearing (otherwise >> would end on its advance)."""
    lb = orig_lb(M, code)
    ic = ink_cols(r)
    partner = [t for t, s_ in PARTNER.items() if s_ == code]
    if ic is not None and partner:
        wp = M.g[partner[0]].shape[1] * M.N
        if wp < M.g[code].shape[1] * M.N:
            lb = max(lb, (wp - (ic[1] - ic[0])) // 2)
    return lb


# ------------------------------------------------------------------ entry
def applies(M, code):
    if M.g[code].shape[0] != 15:
        return False
    if code == _c('}') and M.g[code].shape[1] != M.g[_c('{')].shape[1]:
        return False                                     # fontk.gfx '}' is not a brace (other glyph): not handled here
        # (upscale3.DERIVED still mirrors '{' onto it, as in the base - open point)
    return code in OPEN or code in PARTNER or code in QUOTES


def render(M, code):
    """returns (raster, axes, message) or None."""
    if not getattr(M, 'pairs', True) or not applies(M, code):
        return None
    N = M.N
    g = U4.G(M)
    h, w = M.g[code].shape[0] * N, M.g[code].shape[1] * N
    if code in PARTNER:
        src = PARTNER[code]
        if M.g[src].max() <= 0:
            return None
        return mirror_partner(M, code, src), [], '= %s gespiegelt (Spiegelpaar, pixelgleich)' % \
            bytes([src]).decode('cp1252')
    if code in OPEN:
        r = OPEN[code](M, g, code)
        return place(r, h, w, source_lb(M, code, r)), [], 'Spiegelpaar-Form (konstruiert)'
    if code in QUOTES:
        return g_quote(M, g, code), [], 'Anfuehrungszeichen (Komma-Teil / Strich, einheitlich)'
    return None
