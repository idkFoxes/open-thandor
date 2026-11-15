"""Prototype upscalers for the original Thandor fonts (font.gfx / fontk.gfx).

Every method maps a glyph alpha array a (h, w) in [0, 1] to an array (h*N, w*N): exact original metric x N.
Coordinates: pixel j covers [j, j+1) in source px, its centre is j+0.5.
Only numpy; everything is written so it maps 1:1 onto small C++ loops.
"""
import os
import sys
import math
import numpy as np

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))  # tools/data
import gfx  # noqa: E402

W = os.path.dirname(os.path.abspath(__file__))
PAD = 3
# the game's font files: 'font.gfx' / 'fontk.gfx' -> the bytes of the .gfx asset; set by make_hd_font.py before the
# first load. Some glyphs of one font are built from the other (fontk's Yen and TM use font.gfx), so both are needed.
FONT_DATA = {}


def alpha_of(data, image):
    """The alpha channel (h, w) of one glyph in [0, 1]: the +0 ARGB alpha of its palette entries (direct colour:
    the texel alpha), the alpha gfx.to_pil gives, without Pillow."""
    w, h = image.size
    if image.palette == -1:
        raw = np.frombuffer(data, np.uint8, w * h * 4, image.offset).reshape(h, w, 4)
        return raw[:, :, 3].astype(np.float64) / 255.0
    base = gfx.HEADER + image.palette * gfx.BANK
    lut = np.frombuffer(data, np.uint8, 256 * 8, base).reshape(256, 8)[:, 3]
    idx = np.frombuffer(data, np.uint8, w * h, image.offset).reshape(h, w)
    return lut[idx].astype(np.float64) / 255.0


def load(name):
    key = os.path.basename(name)
    if key in FONT_DATA:
        d, imgs = gfx.parse(FONT_DATA[key], key)
    else:
        d, imgs = gfx.read(os.path.join(W, name))
    return [alpha_of(d, im) for im in imgs]


# ------------------------------------------------------------------ resampling (separable, matrix form)
def k_cubic(x, B=0.0, C=0.5):
    """Mitchell-Netravali family; B=0,C=0.5 = Catmull-Rom (Keys a=-0.5)."""
    x = np.abs(x)
    r = np.zeros_like(x)
    m1 = x < 1
    m2 = (x >= 1) & (x < 2)
    x1, x2 = x[m1], x[m2]
    r[m1] = ((12 - 9 * B - 6 * C) * x1 ** 3 + (-18 + 12 * B + 6 * C) * x1 ** 2 + (6 - 2 * B)) / 6
    r[m2] = ((-B - 6 * C) * x2 ** 3 + (6 * B + 30 * C) * x2 ** 2 + (-12 * B - 48 * C) * x2 + (8 * B + 24 * C)) / 6
    return r


def k_lanczos(x, a=3):
    x = np.abs(x)
    r = np.sinc(x) * np.sinc(x / a)
    r[x >= a] = 0
    return r


def k_linear(x):
    return np.clip(1 - np.abs(x), 0, None)


KERNELS = {'cubic': k_cubic, 'lanczos3': k_lanczos, 'lanczos2': lambda x: k_lanczos(x, 2), 'linear': k_linear,
           'mitchell': lambda x: k_cubic(x, 1 / 3, 1 / 3)}


def _rmat(n, f, kern):
    xs = (np.arange(n * f) + 0.5) / f - 0.5
    return kern(xs[:, None] - np.arange(n)[None, :])


def resample(a, f, kern='cubic'):
    k = KERNELS[kern]
    return _rmat(a.shape[0], f, k) @ a @ _rmat(a.shape[1], f, k).T


def box_down(a, s):
    h, w = a.shape
    return a.reshape(h // s, s, w // s, s).mean(axis=(1, 3))


def crop(a, N, h, w, pad=PAD):
    return a[pad * N:pad * N + h * N, pad * N:pad * N + w * N]


# ------------------------------------------------------------------ 0: nearest (today)
def nearest(a, N):
    return np.kron(a, np.ones((N, N)))


# ------------------------------------------------------------------ 1: coverage reconstruction
def coverage(a, N, kern='cubic', S=4, t=0.5, soft=0.0, gamma=1.0):
    """Alpha = area coverage. Reconstruct a continuous field (kernel), cut at t (edge = where coverage is t),
    re-render with S x S supersampling (box filter) -> AA width ~ one output pixel."""
    h, w = a.shape
    p = np.pad(a, PAD)
    if gamma != 1.0:
        p = p ** gamma
    hi = resample(p, N * S, kern)
    if soft > 0:
        m = np.clip((hi - t) / soft + 0.5, 0, 1)
    else:
        m = (hi > t).astype(np.float64)
    return crop(box_down(m, S), N, h, w)


# ------------------------------------------------------------------ 2: vectorisation
def marching_squares(f, level):
    """Closed iso-contours of grid f (values at grid points). Returns loops of (y, x) in grid index coords."""
    H, Wd = f.shape
    b = f > level
    case = (b[:-1, :-1] * 8 + b[:-1, 1:] * 4 + b[1:, 1:] * 2 + b[1:, :-1] * 1).astype(np.int8)
    cells = np.argwhere((case > 0) & (case < 15))
    pts = {}
    adj = {}

    def ekey(kind, i, j):
        return (i * Wd + j) * 2 + kind  # kind 0: horizontal edge (i,j)-(i,j+1); 1: vertical (i,j)-(i+1,j)

    def epoint(kind, i, j):
        k = ekey(kind, i, j)
        if k not in pts:
            v0 = f[i, j]
            v1 = f[i, j + 1] if kind == 0 else f[i + 1, j]
            t = (level - v0) / (v1 - v0)
            pts[k] = (i, j + t) if kind == 0 else (i + t, j)
        return k

    def link(k0, k1):
        adj.setdefault(k0, []).append(k1)
        adj.setdefault(k1, []).append(k0)

    for i, j in cells:
        c = case[i, j]
        tl, tr, br, bl = b[i, j], b[i, j + 1], b[i + 1, j + 1], b[i + 1, j]
        T = epoint(0, i, j) if tl != tr else None
        Bo = epoint(0, i + 1, j) if bl != br else None
        L = epoint(1, i, j) if tl != bl else None
        R = epoint(1, i, j + 1) if tr != br else None
        if c in (5, 10):
            centre = (f[i, j] + f[i, j + 1] + f[i + 1, j] + f[i + 1, j + 1]) / 4 > level
            if (c == 10) == centre:   # cut around tr and bl
                link(T, R)
                link(Bo, L)
            else:                     # cut around tl and br
                link(L, T)
                link(R, Bo)
        else:
            e = [k for k in (T, R, Bo, L) if k is not None]
            link(e[0], e[1])
    loops = []
    seen = set()
    for start in adj:
        if start in seen:
            continue
        loop = [start]
        seen.add(start)
        prev, cur = None, start
        while True:
            nb = adj[cur]
            nxt = nb[0] if nb[0] != prev else nb[1]
            if nxt == start or nxt in seen:
                break
            loop.append(nxt)
            seen.add(nxt)
            prev, cur = cur, nxt
        loops.append(np.array([pts[k] for k in loop], dtype=np.float64))
    return loops


def _dp(pts, eps):
    if len(pts) < 3:
        return [0, len(pts) - 1]
    a, b = pts[0], pts[-1]
    d = b - a
    n = math.hypot(d[0], d[1])
    if n < 1e-12:
        dist = np.hypot(*(pts - a).T)
    else:
        dist = np.abs(d[0] * (pts[:, 1] - a[1]) - d[1] * (pts[:, 0] - a[0])) / n
    i = int(np.argmax(dist))
    if dist[i] > eps:
        l = _dp(pts[:i + 1], eps)
        r = _dp(pts[i:], eps)
        return l[:-1] + [x + i for x in r]
    return [0, len(pts) - 1]


def dp_closed(loop, eps):
    c = loop.mean(axis=0)
    i0 = int(np.argmax(np.hypot(*(loop - c).T)))
    loop = np.roll(loop, -i0, axis=0)
    i1 = int(np.argmax(np.hypot(*(loop - loop[0]).T)))
    a = loop[:i1 + 1]
    b = np.vstack([loop[i1:], loop[:1]])
    ia = _dp(a, eps)
    ib = _dp(b, eps)
    out = [a[k] for k in ia[:-1]] + [b[k] for k in ib[:-1]]
    return np.array(out)


def snap_poly(poly, tol_deg=12.0, maxd=0.6, hint=None, N=1, hint_tol=0.3, minlen=0.0):
    """Snap segment directions to multiples of 45 deg (when within tol), merge collinear runs and rebuild
    the vertices as intersections of neighbouring lines. poly: (n, 2) in (y, x) source px.
    hint: snap horizontal / vertical lines onto the output pixel grid (1/N) when within hint_tol output px."""
    n = len(poly)
    if n < 3:
        return poly
    lines = []  # (point, dir, snapped_angle or None, length, end vertex)
    for k in range(n):
        p0, p1 = poly[k], poly[(k + 1) % n]
        d = p1 - p0
        L = math.hypot(d[0], d[1])
        if L < 1e-9:
            continue
        ang = math.degrees(math.atan2(d[0], d[1]))
        q = round(ang / 45.0) * 45.0
        if abs(ang - q) <= tol_deg:
            dv = np.array([math.sin(math.radians(q)), math.cos(math.radians(q))])
            sa = q % 360
        else:
            dv, sa = d / L, None
        lines.append([(p0 + p1) / 2, dv, sa, L, p1.copy()])
    # merge consecutive snapped lines with equal direction (incl. wrap)
    merged = []
    for ln in lines:
        if merged and ln[2] is not None and merged[-1][2] == ln[2]:
            m = merged[-1]
            w0, w1 = m[3], ln[3]
            m[0] = (m[0] * w0 + ln[0] * w1) / (w0 + w1)
            m[3] = w0 + w1
            m[4] = ln[4]
        else:
            merged.append(list(ln))
    if len(merged) > 1 and merged[0][2] is not None and merged[0][2] == merged[-1][2]:
        m, ln = merged[0], merged.pop()
        w0, w1 = m[3], ln[3]
        m[0] = (m[0] * w0 + ln[0] * w1) / (w0 + w1)
        m[3] = w0 + w1
    # drop very short unsnapped connector segments between two snapped lines (sharpens corners)
    if minlen > 0 and len(merged) > 4:
        keep = []
        for k, ln in enumerate(merged):
            pv, nx = merged[k - 1], merged[(k + 1) % len(merged)]
            if ln[3] < minlen and pv[2] is not None and nx[2] is not None and pv[2] != nx[2] \
                    and abs(pv[1][0] * nx[1][1] - pv[1][1] * nx[1][0]) > 0.5:
                pv[4] = ln[4]
                continue
            keep.append(ln)
        merged = keep
    if hint is not None:
        for ln in merged:
            if ln[2] is not None and ln[2] % 90 == 0:
                ax = 0 if ln[2] % 180 == 0 else 1  # horizontal line: fix y (index 0); vertical: fix x
                v = ln[0][ax] * N
                r = round(v)
                if abs(v - r) <= hint_tol:
                    ln[0][ax] = r / N
    out = []
    m = len(merged)
    if m < 3:
        return poly
    for k in range(m):
        a, b = merged[k], merged[(k + 1) % m]
        p, r = a[0], a[1]
        q, s = b[0], b[1]
        den = r[1] * s[0] - r[0] * s[1]
        vtx = a[4]
        if abs(den) > 1e-6:
            # p + t r = q + u s
            dq = q - p
            t = (dq[1] * s[0] - dq[0] * s[1]) / den
            x = p + t * r
            if math.hypot(*(x - vtx)) <= maxd:
                out.append(x)
                continue
        # fallback: project the original vertex on both lines
        out.append(p + np.dot(vtx - p, r) * r)
        out.append(q + np.dot(vtx - q, s) * s)
    return np.array(out)


def snap_runs(loop, R, tol_deg=15.0, win=0.5, minrun=0.5, eps=0.15, maxd=0.8, hint=None, N=1, hint_tol=0.3,
              minfree=1.0, maxdev=0.35):
    """Angle snapping on the dense contour: local tangent (window win px) -> snapped class (multiple of 45 deg
    within tol) -> runs of equal class -> one line per snapped run (fixed direction, through the run's mean),
    Douglas-Peucker lines for unsnapped runs -> vertices = intersections of neighbouring lines."""
    n = len(loop)
    if n < 8:
        return loop
    w = max(1, int(round(win * R)))
    d = np.roll(loop, -w, axis=0) - np.roll(loop, w, axis=0)
    ang = np.degrees(np.arctan2(d[:, 0], d[:, 1]))
    q = np.round(ang / 45.0) * 45.0
    cls = np.where(np.abs(ang - q) <= tol_deg, np.mod(q, 360), -1).astype(int)
    seglen = np.hypot(*(np.roll(loop, -1, axis=0) - loop).T)

    def runs_of(c):
        # circular runs: list of [class, [indices]]
        start = 0
        while start < n and c[start] == c[start - 1]:
            start += 1
        if start == n:
            return [[c[0], list(range(n))]]
        rs = []
        for k in range(n):
            i = (start + k) % n
            if rs and rs[-1][0] == c[i]:
                rs[-1][1].append(i)
            else:
                rs.append([c[i], [i]])
        return rs

    rs = runs_of(cls)
    # absorb short runs into the longer neighbour (repeat until stable)
    changed = True
    while changed and len(rs) > 1:
        changed = False
        lens = [seglen[r[1]].sum() for r in rs]
        lim = [minrun if r[0] >= 0 else minfree for r in rs]
        cand = [i for i in range(len(rs)) if lens[i] < lim[i]]
        if cand:
            # free (unsnapped) short runs first, shortest first
            k = min(cand, key=lambda i: (rs[i][0] >= 0, lens[i]))
            a, b = rs[k - 1], rs[(k + 1) % len(rs)]
            tgt = a if lens[k - 1] >= lens[(k + 1) % len(rs)] else b
            if rs[k][0] < 0 and a[0] >= 0 and b[0] >= 0:
                # corner between two snapped runs: split the free run between them -> sharp vertex
                idx = rs[k][1]
                h_ = len(idx) // 2
                cls[idx[:h_]] = a[0]
                cls[idx[h_:]] = b[0]
            else:
                cls[rs[k][1]] = tgt[0]
            rs = runs_of(cls)
            changed = True
    lines = []  # [point, dir, class, last raw point]
    for c, idx in rs:
        pts = loop[idx + [(idx[-1] + 1) % n]]
        if c >= 0:
            dv = np.array([math.sin(math.radians(c)), math.cos(math.radians(c))])
            mp = pts.mean(axis=0)
            dev = np.abs((pts[:, 0] - mp[0]) * dv[1] - (pts[:, 1] - mp[1]) * dv[0]).max()
            if dev <= maxdev:
                lines.append([mp, dv, c, pts[-1]])
                continue
            c = -1
        if c < 0:
            keep = _dp(pts, eps)
            for a_, b_ in zip(keep[:-1], keep[1:]):
                p0, p1 = pts[a_], pts[b_]
                L = math.hypot(*(p1 - p0))
                if L < 1e-9:
                    continue
                lines.append([(p0 + p1) / 2, (p1 - p0) / L, -1, p1])
    if hint is not None:
        for ln in lines:
            if ln[2] >= 0 and ln[2] % 90 == 0:
                ax = 0 if ln[2] % 180 == 0 else 1
                v = ln[0][ax] * N
                r = round(v)
                if abs(v - r) <= hint_tol:
                    ln[0][ax] = r / N
    m = len(lines)
    if m < 3:
        return snap_poly(dp_closed(loop, eps), tol_deg, maxd)
    out = []
    for k in range(m):
        a, b = lines[k], lines[(k + 1) % m]
        p, r, qq, s = a[0], a[1], b[0], b[1]
        vtx = a[3]
        den = r[1] * s[0] - r[0] * s[1]
        if abs(den) > 0.2:
            dq = qq - p
            t = (dq[1] * s[0] - dq[0] * s[1]) / den
            x = p + t * r
            if math.hypot(*(x - vtx)) <= maxd:
                out.append(x)
                continue
        out.append(p + np.dot(vtx - p, r) * r)
        out.append(qq + np.dot(vtx - qq, s) * s)
    return np.array(out)


def raster(polys, N, h, w, SS=8):
    """Even-odd fill, SS x SS samples per output pixel. polys in (y, x) source px (unpadded)."""
    ys = (np.arange(h * N * SS) + 0.5) / (N * SS)
    xs = (np.arange(w * N * SS) + 0.5) / (N * SS)
    E = []
    for P in polys:
        Q = np.roll(P, -1, axis=0)
        E.append(np.hstack([P, Q]))
    if not E:
        return np.zeros((h * N, w * N))
    E = np.vstack(E)  # y0 x0 y1 x1
    y0, x0, y1, x1 = E[:, 0], E[:, 1], E[:, 2], E[:, 3]
    Y = ys[:, None]
    cross = ((y0[None] <= Y) & (y1[None] > Y)) | ((y1[None] <= Y) & (y0[None] > Y))
    with np.errstate(divide='ignore', invalid='ignore'):
        xc = x0[None] + (Y - y0[None]) * (x1 - x0)[None] / (y1 - y0)[None]
    xc = np.where(cross, xc, np.inf)
    cnt = (xc[:, :, None] < xs[None, None, :]).sum(axis=1)
    inside = (cnt & 1).astype(np.float64)
    return box_down(inside, SS)


def vectorize(a, N, R=8, kern='cubic', level=0.5, eps=0.2, snap=True, tol_deg=12.0, maxd=0.5, hint=False,
              hint_tol=0.3, SS=8, minlen=0.0, return_polys=False, runs=True, win=1.0, minrun=1.0, minfree=1.0,
              maxdev=0.35):
    h, w = a.shape
    if a.max() <= 0:
        return np.zeros((h * N, w * N))
    p = np.pad(a, PAD)
    hi = resample(p, R, kern)
    loops = marching_squares(hi, level)
    polys = []
    for lp in loops:
        lp = (lp + 0.5) / R - PAD  # grid index -> source px
        if len(lp) < 3:
            continue
        if snap and runs:
            polys.append(snap_runs(lp, R, tol_deg, win, minrun, eps, maxd, hint=True if hint else None, N=N,
                                   hint_tol=hint_tol, minfree=minfree, maxdev=maxdev))
            continue
        sp = dp_closed(lp, eps) if eps > 0 else lp
        if snap:
            sp = snap_poly(sp, tol_deg, maxd, hint=True if hint else None, N=N, hint_tol=hint_tol, minlen=minlen)
        polys.append(sp)
    r = raster(polys, N, h, w, SS)
    return (r, polys) if return_polys else r


# ------------------------------------------------------------------ 3: pixel-art scalers
def _nb(p):
    """3x3 neighbourhood views of a padded (by >= 1) array; returns dict A..I for the inner region."""
    H, Wd = p.shape
    v = lambda dy, dx: p[1 + dy:H - 1 + dy, 1 + dx:Wd - 1 + dx]
    return dict(A=v(-1, -1), B=v(-1, 0), C=v(-1, 1), D=v(0, -1), E=v(0, 0), F=v(0, 1), G=v(1, -1), H=v(1, 0), I=v(1, 1))


def scale2x(b):
    n = _nb(np.pad(b, 1, mode='edge'))
    B, D, E, F, H = n['B'], n['D'], n['E'], n['F'], n['H']
    out = np.empty((b.shape[0] * 2, b.shape[1] * 2), dtype=b.dtype)
    out[0::2, 0::2] = np.where((D == B) & (B != F) & (D != H), D, E)
    out[0::2, 1::2] = np.where((B == F) & (B != D) & (F != H), F, E)
    out[1::2, 0::2] = np.where((D == H) & (D != B) & (H != F), D, E)
    out[1::2, 1::2] = np.where((H == F) & (D != H) & (B != F), F, E)
    return out


def scale3x(b):
    n = _nb(np.pad(b, 1, mode='edge'))
    A, B, C, D, E, F, G, H, I = (n[k] for k in 'ABCDEFGHI')
    c0 = (D == B) & (D != H) & (B != F)
    c2 = (B == F) & (B != D) & (F != H)
    c6 = (D == H) & (D != B) & (H != F)
    c8 = (H == F) & (D != H) & (B != F)
    out = np.empty((b.shape[0] * 3, b.shape[1] * 3), dtype=b.dtype)
    out[0::3, 0::3] = np.where(c0, D, E)
    out[0::3, 1::3] = np.where((c0 & (E != C)) | (c2 & (E != A)), B, E)
    out[0::3, 2::3] = np.where(c2, F, E)
    out[1::3, 0::3] = np.where((c0 & (E != G)) | (c6 & (E != A)), D, E)
    out[1::3, 1::3] = E
    out[1::3, 2::3] = np.where((c2 & (E != I)) | (c8 & (E != C)), F, E)
    out[2::3, 0::3] = np.where(c6, D, E)
    out[2::3, 1::3] = np.where((c6 & (E != I)) | (c8 & (E != G)), H, E)
    out[2::3, 2::3] = np.where(c8, F, E)
    return out


def scalex(a, N, t=0.5, aa=2):
    """Binarise at t, Scale2x / Scale3x to N, then light AA: Scale2x aa-1 more times and box back."""
    h, w = a.shape
    b = np.pad(a, PAD) > t
    b = scale2x(b) if N == 2 else scale3x(b)
    f = 1
    while f < aa:
        b = scale2x(b)
        f *= 2
    r = box_down(b.astype(np.float64), f) if f > 1 else b.astype(np.float64)
    return crop(r, N, h, w)


def _xbr_corner(p, tol):
    """xBR-lv2 rules for the bottom-right corner of every pixel of padded array p (pad >= 2).
    Returns (nc45, nc30, nc60, colour) for the inner region."""
    Hh, Ww = p.shape
    v = lambda dy, dx: p[2 + dy:Hh - 2 + dy, 2 + dx:Ww - 2 + dx]
    A, B, C, D, E, F, G, H, I = v(-1, -1), v(-1, 0), v(-1, 1), v(0, -1), v(0, 0), v(0, 1), v(1, -1), v(1, 0), v(1, 1)
    F4, I4, H5, I5 = v(0, 2), v(1, 2), v(2, 0), v(2, 1)
    df = lambda x, y: np.abs(x - y)
    eq = lambda x, y: df(x, y) < tol
    ne = lambda x, y: df(x, y) >= tol
    wd1 = df(E, C) + df(E, G) + df(I, H5) + df(I, F4) + 4 * df(H, F)
    wd2 = df(H, D) + df(H, I5) + df(F, I4) + df(F, B) + 4 * df(E, I)
    lv1 = ne(E, F) & ne(E, H) & ((~eq(F, B) & ~eq(H, D)) | (eq(E, I) & ~eq(F, I4) & ~eq(H, I5)) | eq(E, G) | eq(E, C))
    edr = (wd1 < wd2) & lv1
    edr_l = (2 * df(F, G) <= df(H, C)) & ne(E, G) & ne(D, G) & edr
    edr_u = (df(F, G) >= 2 * df(H, C)) & ne(E, C) & ne(B, C) & edr
    col = np.where(df(E, F) <= df(E, H), F, H)
    return edr, edr_l, edr_u, col


def xbr(a, N, t=0.5, binary=True, SS=4, tol=0.25, levels=1):
    """xBR-lv2 evaluated geometrically at N*SS sub-samples per source pixel, then box filtered to N.
    binary: run on the thresholded glyph; else on the alpha itself (eq tolerance tol).
    levels > 1 (binary only): threshold decomposition - run on `levels` level sets of the alpha and average."""
    h, w = a.shape
    if binary and levels > 1:
        ts = [(k + 0.5) / levels for k in range(levels)]
        return sum(xbr(a, N, tt, True, SS, tol, 1) for tt in ts) / levels
    p = np.pad(a, PAD)
    if binary:
        p = (p > t).astype(np.float64)
        tol = 0.5
    pp = np.pad(p, 2, mode='edge')
    rules = []
    for k in range(4):
        r = _xbr_corner(np.rot90(pp, k), tol)
        rules.append([np.rot90(x, -k) for x in r])
    M = N * SS
    hi = np.zeros((p.shape[0] * M, p.shape[1] * M))
    for my in range(M):
        for mx in range(M):
            fy, fx = (my + 0.5) / M, (mx + 0.5) / M
            val = p.copy()
            ly, lx = fy, fx
            for k in range(4):
                # local coordinates in the frame rotated k times by np.rot90
                if k > 0:
                    ly, lx = 1 - lx, ly
                edr, edr_l, edr_u, col = rules[k]
                f45 = ly + lx > 1.5
                f30 = ly + 0.5 * lx > 1.0
                f60 = ly + 2 * lx > 2.0
                nc = (edr & f45) | (edr_l & f30) | (edr_u & f60)
                val = np.where(nc, col, val)
            hi[my::M, mx::M] = val
    return crop(box_down(hi, SS), N, h, w)


# ------------------------------------------------------------------ helpers
def ink(glyphs):
    return float(sum(g.sum() for g in glyphs))


def run_font(glyphs, fn, N, **kw):
    return [fn(g, N, **kw) if g.size else np.zeros((g.shape[0] * N, g.shape[1] * N)) for g in glyphs]


# ------------------------------------------------------------------ diacritics: scale mark and body separately
def base_code(code):
    """cp1252 code of the base letter of an accented letter (prototype: unicodedata; C++: a ~60 entry table)."""
    import unicodedata
    if code < 0xC0:
        return None
    ch = bytes([code]).decode('cp1252', 'replace')
    dec = unicodedata.normalize('NFD', ch)
    if len(dec) < 2 or dec[0] == ch:
        return None
    try:
        return dec[0].encode('cp1252')[0]
    except UnicodeEncodeError:
        return None


def is_diaeresis(code):
    import unicodedata
    return '̈' in unicodedata.normalize('NFD', bytes([code]).decode('cp1252', 'replace'))


def split_bar(m, f=0.15):
    """Some 1x umlauts (Oe, Ue) are drawn as one bar: if the mark has no gap, cut the middle ~1.5 px."""
    cols = np.nonzero(m.max(axis=0) > 0.3)[0]
    if len(cols) < 3:
        return m
    prof = m[:, cols[0]:cols[-1] + 1].max(axis=0)
    if (prof < 0.45).any():
        return m  # already two dots
    c = (cols[0] + cols[-1] + 1) / 2.0
    xs = np.arange(m.shape[1]) + 0.5
    cut = np.abs(xs - c) < 0.75
    m = m.copy()
    m[:, cut] *= f
    return m


def deepen_valleys(m, d=0.2, f=0.3):
    """Pixels darker than both horizontal neighbours by > d (the partial-alpha gap between two umlaut dots)
    are scaled by f, so the reconstruction keeps the dots apart."""
    p = np.pad(m, ((0, 0), (1, 1)))
    l, r = p[:, :-2], p[:, 2:]
    v = (l > m + d) & (r > m + d)
    return np.where(v, m * f, m)


def dilate(m, r):
    out = m.copy()
    for dy in range(-r, r + 1):
        for dx in range(-r, r + 1):
            out |= np.roll(np.roll(m, dy, 0), dx, 1)
    return out


def upscale_glyph(glyphs, code, N, fn, gap=None, **kw):
    """Upscale glyph `code`; accented letters: mark (rows above the base letter's top) and body are scaled
    separately and the mark is kept `gap` output px away from the body (the 1x font fuses dots and body)."""
    a = glyphs[code]
    if a.size == 0:
        return np.zeros((a.shape[0] * N, a.shape[1] * N))
    b = base_code(code)
    if b is not None:
        rows = np.nonzero(glyphs[b].max(axis=1) > 0.1)[0]
        s = int(rows[0]) if len(rows) else 0
        if s > 0 and a[:s].max() > 0.1:
            mark, body = a.copy(), a.copy()
            mark[s:] = 0
            body[:s] = 0
            mark = deepen_valleys(mark)
            if is_diaeresis(code):
                mark = split_bar(mark)
            M, B = fn(mark, N, **kw), fn(body, N, **kw)
            g = gap if gap is not None else max(1, N // 2)
            keep = ~dilate(B > 0.3, g)
            return np.maximum(M * keep, B)
    return fn(a, N, **kw)
