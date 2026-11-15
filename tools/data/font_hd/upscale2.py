"""Method 2 'straight': orthogonal vectorisation for the very straight original Thandor font.

Iso contour (as method 2), then every contour piece is classified as horizontal/vertical (wide tolerance),
45 deg chamfer (only if long enough in the original) or free diagonal (only long, genuinely slanted runs).
Short arcs between two classified runs are split -> sharp 90 deg corners. Lines are intersected.
Hinting: horizontal/vertical edges are paired into strokes; stroke width rounded to whole output pixels
(font-wide standard width when close), position rounded to the output grid.
"""
import math
import numpy as np
import upscale as U

PAD = U.PAD


def _classify(ang, tol_hv, tol45):
    q90 = np.round(ang / 90.0) * 90.0
    q45 = np.round((ang - 45.0) / 90.0) * 90.0 + 45.0
    c = np.full(ang.shape, -1)
    m45 = np.abs(ang - q45) <= tol45
    mhv = np.abs(ang - q90) <= tol_hv
    c[m45] = np.mod(q45[m45], 360).astype(int)
    c[mhv] = np.mod(q90[mhv], 360).astype(int)
    return c


def _runs(c):
    n = len(c)
    start = 0
    while start < n and c[start] == c[start - 1]:
        start += 1
    if start == n:
        return [[int(c[0]), list(range(n))]]
    rs = []
    for k in range(n):
        i = (start + k) % n
        if rs and rs[-1][0] == c[i]:
            rs[-1][1].append(i)
        else:
            rs.append([int(c[i]), [i]])
    return rs


def loop_lines(loop, R, field, level, tol_hv=24.0, tol45=14.0, win=1.0, min_hv=0.3, min45=0.8, minfree=1.6,
               maxdev=0.3, eps=0.25):
    """Returns the line list of one closed contour: dicts p, d, cls (-1 free), end, lo, hi, inside(normal sign)."""
    n = len(loop)
    w = max(1, int(round(win * R / 2)))
    dd = np.roll(loop, -w, axis=0) - np.roll(loop, w, axis=0)
    ang = np.degrees(np.arctan2(dd[:, 0], dd[:, 1]))
    cls = _classify(ang, tol_hv, tol45)
    seglen = np.hypot(*(np.roll(loop, -1, axis=0) - loop).T)

    def lim(c):
        return minfree if c < 0 else (min45 if c % 90 else min_hv)

    for _ in range(4 * n):
        rs = _runs(cls)
        if len(rs) < 2:
            break
        lens = [seglen[r[1]].sum() for r in rs]
        cand = [i for i in range(len(rs)) if lens[i] < lim(rs[i][0])]
        if not cand:
            break
        # free first, then 45, then h/v; shortest first
        k = min(cand, key=lambda i: (0 if rs[i][0] < 0 else (1 if rs[i][0] % 90 else 2), lens[i]))
        a, b = rs[k - 1], rs[(k + 1) % len(rs)]
        idx = rs[k][1]
        if a[0] != b[0] and len(rs) > 2:
            h_ = len(idx) // 2
            cls[idx[:h_]] = a[0]
            cls[idx[h_:]] = b[0]
        else:
            cls[idx] = a[0] if lens[k - 1] >= lens[(k + 1) % len(rs)] else b[0]
    rs = _runs(cls)
    lines = []
    for c, idx in rs:
        pts = loop[idx + [(idx[-1] + 1) % n]]
        if c >= 0:
            dv = np.array([math.sin(math.radians(c)), math.cos(math.radians(c))])
            mp = pts.mean(axis=0)
            dev = np.abs((pts[:, 0] - mp[0]) * dv[1] - (pts[:, 1] - mp[1]) * dv[0]).max()
            if dev <= maxdev or (c % 90 == 0 and dev <= maxdev * 2):
                pr = (pts - mp) @ dv
                lines.append(dict(p=mp, d=dv, cls=c, end=pts[-1], lo=mp + pr.min() * dv, hi=mp + pr.max() * dv))
                continue
        keep = U._dp(pts, eps)
        for a_, b_ in zip(keep[:-1], keep[1:]):
            p0, p1 = pts[a_], pts[b_]
            L = math.hypot(*(p1 - p0))
            if L < 1e-9:
                continue
            lines.append(dict(p=(p0 + p1) / 2, d=(p1 - p0) / L, cls=-1, end=p1, lo=p0, hi=p1))
    # inside side of every line: probe the field 0.3 px along the normal
    H, W = field.shape
    def fv(pt):
        q = (pt + PAD) * R - 0.5
        iy, ix = int(round(q[0])), int(round(q[1]))
        return field[min(max(iy, 0), H - 1), min(max(ix, 0), W - 1)]

    for ln in lines:
        nrm = np.array([ln['d'][1], -ln['d'][0]])
        # compare both sides at three points along the line (robust against wobbly fringes)
        sp = [ln['lo'] * 0.75 + ln['hi'] * 0.25, ln['p'], ln['lo'] * 0.25 + ln['hi'] * 0.75]
        diff = sum(fv(x + 0.4 * nrm) - fv(x - 0.4 * nrm) for x in sp)
        ln['in'] = nrm if diff > 0 else -nrm
    return lines


def hint_lines(all_lines, N, stem=None, stem_tol=0.35, maxw=3.6):
    """Pair parallel h/v edges with opposite inside into strokes, round width (standard stem if close) and
    position to the output grid. Unpaired h/v edges are rounded to the grid."""
    hv = [ln for ln in all_lines if ln['cls'] >= 0 and ln['cls'] % 90 == 0]
    for ln in hv:
        ln['ax'] = 1 if ln['cls'] % 180 == 90 else 0      # vertical line: x is fixed (index 1)
        ln['v'] = ln['p'][ln['ax']]
        ln['s'] = 1 if ln['in'][ln['ax']] > 0 else -1     # +1: ink on the larger-coordinate side
        o = 1 - ln['ax']
        ln['r0'], ln['r1'] = sorted((ln['lo'][o], ln['hi'][o]))
        ln['set'] = None
    pairs = []
    for a in hv:
        if a['s'] != 1:
            continue
        best = None
        for b in hv:
            if b['ax'] != a['ax'] or b['s'] != -1:
                continue
            wdt = b['v'] - a['v']
            ov = min(a['r1'], b['r1']) - max(a['r0'], b['r0'])
            if 0.3 < wdt <= maxw and ov > 0.2:
                if best is None or wdt < best[0]:
                    best = (wdt, b)
        if best:
            pairs.append((best[0], a, best[1]))
    pairs.sort(key=lambda t: t[0])
    for wdt, a, b in pairs:
        wo = wdt * N
        if stem is not None and abs(wdt - stem[a['ax']]) <= stem_tol:
            wo = stem[a['ax']] * N
        wo = max(1, int(round(wo)))
        if a['set'] is not None and b['set'] is not None:
            continue
        if a['set'] is not None:
            b['set'] = a['set'] + wo
        elif b['set'] is not None:
            a['set'] = b['set'] - wo
        else:
            c = (a['v'] + b['v']) / 2 * N
            lo = int(math.floor(c - wo / 2 + 0.5))
            a['set'], b['set'] = lo, lo + wo
    for ln in hv:
        if ln['set'] is None:
            ln['set'] = int(round(ln['v'] * N))
        ln['p'] = ln['p'].copy()
        ln['p'][ln['ax']] = ln['set'] / N


def build_poly(lines, maxd=1.0):
    m = len(lines)
    out = []
    for k in range(m):
        a, b = lines[k], lines[(k + 1) % m]
        p, r, q, s = a['p'], a['d'], b['p'], b['d']
        vtx = a['end']
        den = r[1] * s[0] - r[0] * s[1]
        if abs(den) > 0.2:
            dq = q - p
            t = (dq[1] * s[0] - dq[0] * s[1]) / den
            x = p + t * r
            if math.hypot(*(x - vtx)) <= maxd:
                out.append(x)
                continue
        out.append(p + np.dot(vtx - p, r) * r)
        out.append(q + np.dot(vtx - q, s) * s)
    return np.array(out)


def _mirror(f, ax, K):
    """f mirrored along axis ax so that grid index i maps to K - i (zero outside)."""
    n = f.shape[ax]
    i = np.arange(n)
    j = K - i
    ok = (j >= 0) & (j < n)
    m = np.zeros_like(f)
    if ax == 1:
        m[:, ok] = f[:, j[ok]]
    else:
        m[ok, :] = f[j[ok], :]
    return m


def find_axis(f, ax, level, thr=0.015, er=1):
    """Mirror axis of the iso shape (f > level) along ax (ax 1: vertical axis / x mirrored). The XOR of shape
    and mirror image is eroded by `er` grid cells (ignores sub-pixel width differences, e.g. stems of 1.7 and
    1.9 px) and taken relative to the area. Returns (K, err); K = mirror index sum (i -> K - i) or None."""
    b = (f > level).astype(np.float64)
    if b.sum() == 0:
        return None, 1.0
    prof = b.sum(axis=1 - ax)
    idx = np.nonzero(prof > 0)[0]
    best = (9.0, None)
    for K in range(2 * idx[0], 2 * idx[-1] + 1):
        x = np.abs(b - _mirror(b, ax, K)) > 0.5
        e0 = x.sum() / b.sum()
        er_ = x.copy()
        for d in range(1, er + 1):
            er_ = er_ & np.roll(x, d, 0) & np.roll(x, -d, 0) & np.roll(x, d, 1) & np.roll(x, -d, 1)
        e = er_.sum() / b.sum() + 0.001 * e0
        if e < best[0]:
            best = (e, K)
    return (best[1] if best[0] <= thr else None), best[0]


def components(a, t=0.1):
    """8-connected components of a > t (list of boolean masks)."""
    m = a > t
    lab = np.zeros(a.shape, int)
    out = []
    for y, x in zip(*np.nonzero(m)):
        if lab[y, x]:
            continue
        k = len(out) + 1
        st = [(y, x)]
        lab[y, x] = k
        while st:
            cy, cx = st.pop()
            for dy in (-1, 0, 1):
                for dx in (-1, 0, 1):
                    yy, xx = cy + dy, cx + dx
                    if 0 <= yy < a.shape[0] and 0 <= xx < a.shape[1] and m[yy, xx] and not lab[yy, xx]:
                        lab[yy, xx] = k
                        st.append((yy, xx))
        out.append(lab == k)
    return out


def _k_to_src(K, R):
    return (K / 2.0 + 0.5) / R - PAD


def _groups(a, R, kern, level, sym, sym_thr):
    """Shape groups (field, axes, whole); axes = {ax: (own axis src, target axis src)}. The whole glyph if it
    is symmetric, else one group per connected component with its own symmetry; a small dot above a stem is
    centred on the stem (j)."""
    full = U.resample(np.pad(a, PAD), R, kern)
    if not sym:
        return [(full, {}, True)]
    axes = {}
    for ax in (1, 0):
        K, e = find_axis(full, ax, level, sym_thr)
        if K is not None:
            axes[ax] = (_k_to_src(K, R),) * 2
    comps = components(a)
    if axes or len(comps) < 2:
        return [(full, axes, True)]
    gs = []
    for ci, m in enumerate(comps):
        f = U.resample(np.pad(a * m, PAD), R, kern)
        ax_ = {}
        for ax in (1, 0):
            K, e = find_axis(f, ax, level, sym_thr)
            if K is not None:
                ax_[ax] = (_k_to_src(K, R),) * 2
        ys, xs = np.nonzero(m)
        if ys.max() - ys.min() <= 2 and xs.max() - xs.min() <= 2 and 1 in ax_:
            for cj, m2 in enumerate(comps):
                if cj == ci:
                    continue
                y2, x2 = np.nonzero(m2)
                if y2.min() > ys.max() and x2.min() <= xs.max() and x2.max() >= xs.min():
                    top = (m2 & (np.arange(a.shape[0])[:, None] < y2.min() + 3)) * a
                    cx = (top.sum(axis=0) * (np.arange(a.shape[1]) + 0.5)).sum() / top.sum()
                    if abs(cx - ax_[1][0]) < 0.6:
                        ax_[1] = (ax_[1][0], cx)
                    break
        gs.append((f, ax_, False))
    return gs


def straight(a, N, R=8, kern='cubic', level=0.485, SS=8, stem=None, hint=True, return_polys=False, chamfer_out=0.0,
             sym=True, sym_thr=0.015, axis_target=None, **kw):
    h, w = a.shape
    if a.max() <= 0:
        z = np.zeros((h * N, w * N))
        return (z, [], []) if return_polys else z
    L, G = [], []
    groups = _groups(a, R, kern, level, sym, sym_thr)
    if axis_target is not None and len(groups) == 1 and 1 in groups[0][1]             and abs(groups[0][1][1][0] - axis_target) < 0.75:
        groups[0][1][1] = (groups[0][1][1][0], axis_target)
    for gi, (f, axes, whole) in enumerate(groups):
        shift = np.zeros(2)
        tgt = {}
        for ax, (own, target) in axes.items():
            K = int(round(((own + PAD) * R - 0.5) * 2))
            f = 0.5 * (f + _mirror(f, ax, K))
            C_out = round(2 * target * N) / 2.0           # axis on the output half-pixel grid
            tgt[ax] = C_out
            shift[ax] = C_out / N - own                   # (y, x): ax 1 (vertical axis) shifts x
        G.append(dict(axes=tgt, whole=whole))
        for lp in U.marching_squares(f, level):
            lp = (lp + 0.5) / R - PAD
            if len(lp) < 8:
                continue
            ls = loop_lines(lp, R, f, level, **kw)
            if len(ls) >= 3:
                for ln in ls:
                    for key in ('p', 'end', 'lo', 'hi'):
                        ln[key] = ln[key] + shift
                    ln['g'] = gi
                L.append(ls)
    if chamfer_out:
        for ls in L:
            for ln in ls:
                if ln['cls'] >= 0 and ln['cls'] % 90:
                    ln['p'] = ln['p'] - chamfer_out * ln['in']
    if hint:
        allL = [x for ls in L for x in ls]
        hint_lines(allL, N, stem)
        for gi, g in enumerate(G):
            for ax, C_out in g['axes'].items():
                _mirror_hint([x for x in allL if x.get('g') == gi and 'ax' in x], ax, C_out, N)
    polys = [build_poly(ls) for ls in L]
    r = U.raster(polys, N, h, w, SS)
    if len(G) == 1 and G[0]['whole']:
        for ax, C_out in G[0]['axes'].items():
            K = int(round(2 * C_out)) - 1                  # output pixel j <-> K - j
            # copy the first half onto the second (exact symmetry without blurring small differences)
            cen = np.arange(r.shape[ax]) + 0.5
            sec = (cen > C_out)[None, :] if ax == 1 else (cen > C_out)[:, None]
            m = _mirror(r, ax, K)
            r = np.where(sec & (m >= 0), m, r)
    return (r, polys, G) if return_polys else r


def _mirror_hint(lines, ax, C_out, N):
    """Make hinted edges exactly mirror-symmetric about C_out (output px). ax 1: vertical axis (x mirrored)."""
    A = C_out / N
    for a_ in lines:
        if a_['ax'] == ax:
            if a_['v'] >= A - 0.05:
                continue
            for b in lines:
                if b is a_ or b['ax'] != ax or abs(b['v'] - (2 * A - a_['v'])) > 0.3:
                    continue
                if min(a_['r1'], b['r1']) - max(a_['r0'], b['r0']) > 0.2:
                    b['set'] = int(round(2 * C_out)) - a_['set']
                    b['p'] = b['p'].copy()
                    b['p'][ax] = b['set'] / N
        else:
            if (a_['r0'] + a_['r1']) / 2 >= A - 0.05:
                continue
            m0, m1 = 2 * A - a_['r1'], 2 * A - a_['r0']
            for b in lines:
                if b is a_ or b['ax'] != a_['ax'] or abs(b['v'] - a_['v']) > 0.3:
                    continue
                if min(m1, b['r1']) - max(m0, b['r0']) > 0.2:
                    b['set'] = a_['set']
                    b['p'] = b['p'].copy()
                    b['p'][b['ax']] = b['set'] / N


def measure_stem(glyphs, chars='lIHEFTnmhLiudb', **kw):
    """Median width of paired vertical / horizontal edges (source px) over some reference glyphs."""
    ws = {0: [], 1: []}
    for ch in chars:
        a = glyphs[ch.encode('cp1252')[0]]
        p = np.pad(a, PAD)
        hi = U.resample(p, 8, 'cubic')
        for lp in U.marching_squares(hi, 0.485):
            lp = (lp + 0.5) / 8 - PAD
            if len(lp) < 8:
                continue
            ls = loop_lines(lp, 8, hi, 0.485)
            hint_probe = [dict(x) for x in ls]
            for ln in hint_probe:
                ln['p'] = ln['p'].copy()
            # reuse the pairing logic without changing anything
            hint_lines(hint_probe, 1)
            # widths: from the paired 'set' values is lossy; recompute directly
            hv = [x for x in hint_probe if 'ax' in x]
            for x in hv:
                if x['s'] == 1:
                    cand = [y['v'] - x['v'] for y in hv if y['ax'] == x['ax'] and y['s'] == -1 and
                            0.3 < y['v'] - x['v'] <= 3.6 and min(x['r1'], y['r1']) - max(x['r0'], y['r0']) > 0.2]
                    if cand:
                        ws[x['ax']].append(min(cand))
    return {k: float(np.median(v)) if v else None for k, v in ws.items()}


def split_parts(glyphs, code):
    """(mark, body) for accented letters (same rule as upscale.upscale_glyph), else None."""
    a = glyphs[code]
    b = U.base_code(code)
    if b is None:
        return None
    rows = np.nonzero(glyphs[b].max(axis=1) > 0.1)[0]
    s = int(rows[0]) if len(rows) else 0
    if s <= 0 or a[:s].max() <= 0.1:
        return None
    mark, body = a.copy(), a.copy()
    mark[s:] = 0
    body[:s] = 0
    mark = U.deepen_valleys(mark)
    if U.is_diaeresis(code):
        mark = U.split_bar(mark)
    return mark, body


def straight_glyph(glyphs, code, N, gap=None, return_axes=False, **kw):
    """Straight variant for one glyph incl. accent handling: mark and body separately, the mark's vertical axis
    is put onto the body's axis (or the stem centre) so umlaut dots sit symmetric over the letter."""
    a = glyphs[code]
    if a.size == 0 or a.max() <= 0:
        z = np.zeros((a.shape[0] * N, a.shape[1] * N))
        return (z, []) if return_axes else z
    sp = split_parts(glyphs, code)
    if sp is None:
        r, _, G = straight(a, N, return_polys=True, **kw)
        return (r, G) if return_axes else r
    mark, body = sp
    B, _, Gb = straight(body, N, return_polys=True, **kw)
    target = None
    for g in Gb:
        if g['whole'] and 1 in g['axes']:
            target = g['axes'][1] / N
    M, _, Gm = straight(mark, N, return_polys=True, axis_target=target, **kw)
    g_ = gap if gap is not None else max(1, N // 2)
    keep = ~U.dilate(B > 0.3, g_)
    r = np.maximum(M * keep, B)
    return (r, Gb + Gm) if return_axes else r
