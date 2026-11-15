"""Method 2 'straight', third iteration: a consistent construction of the original font at N x.

Font-wide rules (FontModel, derived from the original glyphs):
  - one stroke width W (output px) for every vertical, horizontal and diagonal stroke
  - one chamfer size: outer 45 deg chamfer leg c_out, inner leg c_in (parallel, stroke width kept in the corner)
  - alignment zones: horizontal edges of all glyphs snapped onto common heights (baseline, x-height, cap height,
    bar heights, dot heights) -> same parts sit on the same output rows in every glyph
  - one dot: square W x W (., :, ;, !, ?, i, j), umlaut dots square Du x Du with one spacing per case
  - mirror symmetry (vertical axis on the output half-pixel grid, parity chosen to fit the stroke width)
Binary pixel-art symbols (0x01-0x1F, alpha only 0/255) use xBR.
"""
import math
import os
import numpy as np
import upscale as U
import upscale2 as V

PAD = U.PAD
R = 8
LEVEL = 0.485


# ------------------------------------------------------------------ geometry helpers
def annotate(ln):
    if ln['cls'] >= 0 and ln['cls'] % 90 == 0:
        ln['ax'] = 1 if ln['cls'] % 180 == 90 else 0      # vertical line: x fixed (index 1)
        ln['v'] = ln['p'][ln['ax']]
        ln['s'] = 1 if ln['in'][ln['ax']] > 0 else -1     # +1: ink on the larger-coordinate side
        o = 1 - ln['ax']
        ln['r0'], ln['r1'] = sorted((ln['lo'][o], ln['hi'][o]))
        ln.setdefault('v0', ln['v'])
        ln['set'] = None


def is_hv(ln):
    return 'ax' in ln


def is_45(ln):
    return ln['cls'] >= 0 and ln['cls'] % 90 != 0


def seg_len(ln):
    return float(np.hypot(*(ln['hi'] - ln['lo'])))


def find_pairs(hv, maxw):
    """stroke pairs (width, a, b): a has ink on its + side, b the nearest opposite edge with overlap."""
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
    return pairs


def corner_point(a, b):
    p, r, q, s = a['p'], a['d'], b['p'], b['d']
    den = r[1] * s[0] - r[0] * s[1]
    if abs(den) < 1e-6:
        return None
    dq = q - p
    t = (dq[1] * s[0] - dq[0] * s[1]) / den
    return p + t * r


def prep_loop(ls):
    """free lines within 16 deg of h/v become h/v (wobbly spines, e.g. E), equal consecutive h/v lines merge,
    45 / short h-v / 45 stairs become one 45 line, corner chamfers (45 between perpendicular h/v) are flagged."""
    for ln in ls:
        if ln['cls'] < 0 and seg_len(ln) >= 1.0:
            ang = math.degrees(math.atan2(ln['d'][0], ln['d'][1]))
            q = round(ang / 90.0) * 90.0
            lat = seg_len(ln) * abs(math.sin(math.radians(ang - q)))
            if abs(ang - q) <= 25 and lat <= 1.0:
                ln['cls'] = int(q % 360)
                ln['d'] = np.array([math.sin(math.radians(q)), math.cos(math.radians(q))])
                annotate(ln)
    k = 0
    while len(ls) > 3 and k < len(ls):
        a, b = ls[k], ls[(k + 1) % len(ls)]
        if a is not b and is_hv(a) and is_hv(b) and a['cls'] == b['cls']:
            la, lb = seg_len(a), seg_len(b)
            a['p'] = (a['p'] * la + b['p'] * lb) / (la + lb)
            a['hi'], a['end'] = b['hi'], b['end']
            a.pop('ax')
            a.pop('v0', None)
            annotate(a)
            ls.pop((k + 1) % len(ls))
            continue
        k += 1
    changed = True
    while changed and len(ls) > 4:
        changed = False
        m = len(ls)
        for k in range(m):
            a, b, c = ls[k], ls[(k + 1) % m], ls[(k + 2) % m]
            if is_45(a) and is_45(c) and a['cls'] == c['cls'] and is_hv(b) and seg_len(b) < 1.0:
                la, lb, lc = seg_len(a), seg_len(b), seg_len(c)
                p = (a['p'] * la + b['p'] * lb + c['p'] * lc) / (la + lb + lc)
                new = dict(a)
                new.update(p=p, lo=a['lo'], hi=c['hi'], end=c['end'])
                idx = sorted({k, (k + 1) % m, (k + 2) % m}, reverse=True)
                for i in idx:
                    ls.pop(i)
                ls.insert(min(k, len(ls)), new)
                changed = True
                break
    m = len(ls)
    for k, ln in enumerate(ls):
        a, b = ls[k - 1], ls[(k + 1) % m]
        ln['cham'] = bool(is_45(ln) and is_hv(a) and is_hv(b) and a['ax'] != b['ax'] and seg_len(ln) < 4.5)
    return ls


def _sample(f, y, x):
    gy, gx = (y + PAD) * R - 0.5, (x + PAD) * R - 0.5
    iy, ix = int(np.floor(gy)), int(np.floor(gx))
    if iy < 0 or ix < 0 or iy + 1 >= f.shape[0] or ix + 1 >= f.shape[1]:
        return 0.0
    fy, fx = gy - iy, gx - ix
    return (f[iy, ix] * (1 - fy) * (1 - fx) + f[iy + 1, ix] * fy * (1 - fx) + f[iy, ix + 1] * (1 - fy) * fx
            + f[iy + 1, ix + 1] * fy * fx)


CHAM_T = 0.6


def find_corners(L, fields, level):
    """90 deg corners between main h/v lines (optionally with a chamfer line between). Rounded or not is
    decided from the reconstructed original: distance t* along the bisector from the corner point to the
    iso line (sharp box corner: t* <= ~0.55 px, chamfer leg >= 0.8 px: t* >= ~0.65 px)."""
    out = []
    for li, ls in enumerate(L):
        m = len(ls)
        for k in range(m):
            a = ls[k]
            if not is_hv(a):
                continue
            b = ls[(k + 1) % m]
            mid = None
            if b.get('cham'):
                mid, b = b, ls[(k + 2) % m]
            if not is_hv(b) or b['ax'] == a['ax']:
                continue
            X = corner_point(a, b)
            if X is None:
                continue
            f, dx = fields[a['g']]
            convex = float(np.dot(a['in'], b['d'])) > 0
            ink = a['in'] + b['in']
            dv = ink / np.linalg.norm(ink)
            X0 = X - np.array([0.0, dx])
            t = 0.0
            if convex:      # walk into the ink until the shape starts
                while t < 3.0 and _sample(f, *(X0 + t * dv)) <= level:
                    t += 1 / 32
            else:           # walk out of the ink until the shape ends
                while t < 3.0 and _sample(f, *(X0 - t * dv)) > level:
                    t += 1 / 32
            out.append(dict(loop=li, a=a, b=b, m=mid, convex=convex, ink=ink, t=t, cham=t > CHAM_T,
                            orig=t > CHAM_T, g=a['g']))
    return out


def _main(ls, k, step):
    m = len(ls)
    j = (k + step) % m
    for _ in range(m):
        if not ls[j].get('cham'):
            return ls[j]
        j = (j + step) % m
    return ls[j]


def edge_len(ls, k):
    """length of line k between its corners with the previous / next main (non-chamfer) line."""
    a = ls[k]
    p0 = corner_point(_main(ls, k, -1), a)
    p1 = corner_point(a, _main(ls, k, 1))
    if p0 is None or p1 is None:
        return float(abs(np.dot(a['hi'] - a['lo'], a['d'])))
    return float(np.linalg.norm(p1 - p0))


# ------------------------------------------------------------------ font model
class FontModel:
    def __init__(self, glyphs, N, sym_thr=0.015, level=LEVEL, chamfer=None, dot=None, udot=None, W=None,
                 corner='arc', r_big=None, r_small=None, arc_seg=12, r_big_src=None):
        self.g, self.N, self.sym_thr, self.level = glyphs, N, sym_thr, level
        st = V.measure_stem(glyphs)
        self.W_src = float(np.mean([v for v in st.values() if v]))
        self.W = W if W else max(1, int(round(self.W_src * N)))
        self.maxw = 2.0 * self.W_src
        # pass 1: lines of all glyphs -> zones, chamfer size
        ys, xs, legs, ts = [], [], [], []
        for c in range(0x20, 0x100):
            a = glyphs[c]
            if a.max() <= 0:
                continue
            f = U.resample(np.pad(a, PAD), R, 'cubic')
            for lp in U.marching_squares(f, level):
                lp = (lp + 0.5) / R - PAD
                if len(lp) < 8:
                    continue
                ls = V.loop_lines(lp, R, f, level)
                ls2 = [dict(x) for x in ls]
                for x in ls2:
                    annotate(x)
                    x['g'] = 0
                ls2 = prep_loop(ls2)
                for cc in find_corners([ls2], [(f, 0.0)], level):
                    if cc['convex'] and cc['orig']:
                        ts.append(cc['t'])
                for k, ln in enumerate(ls):
                    if ln['cls'] >= 0 and ln['cls'] % 180 == 0:
                        ys.append((ln['p'][0], seg_len(ln)))
                    if ln['cls'] >= 0 and ln['cls'] % 180 == 90:
                        xs.append((ln['p'][1], seg_len(ln)))
                    if is_45(ln):
                        a_, b_ = ls[k - 1], ls[(k + 1) % len(ls)]
                        if a_['cls'] >= 0 and b_['cls'] >= 0 and a_['cls'] % 90 == 0 and b_['cls'] % 90 == 0 \
                                and a_['cls'] % 180 != b_['cls'] % 180 and np.dot(ln['in'], a_['d']) < 0:
                            legs.append(seg_len(ln) / math.sqrt(2))
        self.zones = self._cluster(ys)
        # common cap top edge (output row) for every glyph of the scale: the row the hinted top bar of E / P / O gets
        # (stroke pair centred on cap zone + W_src / 2); lone top edges in the cap zone (I ! l 1 stems) go there too
        cz = self.snap_zone(3.0)
        self.cap_zone = cz if cz != 3.0 else None
        self.cap_row = int(math.floor((cz + self.W_src / 2) * N - self.W / 2 + 0.5)) if self.cap_zone else None
        # common baseline edge (output row, exclusive), the counterpart of cap_row: the row the hinted bottom bar of
        # E / L / O gets (stroke pair centred on baseline zone - W_src / 2); lone bottom edges (I T F stems), the
        # flat leg foot and the baseline dots go there too. round(zone * N) can be one row lower (font.gfx 3x: 36 / 35)
        bz = self.snap_zone(12.0)
        self.base_zone = bz if bz != 12.0 else None
        self.base_row = int(math.floor((bz - self.W_src / 2) * N - self.W / 2 + 0.5)) + self.W
        # common x-height edge (output row): the row the hinted top bar of n / o / a gets (stroke pair zone + its lower
        # partner zone, centred). Lone stem tops of u / y lie just outside the zone's snap tolerance and would otherwise
        # round by themselves (font.gfx 3x: 15 next to 16). font.gfx only (fontk's x-height part is not at this height).
        self.xh_row = None
        if self.g[ord('A')].shape[0] == 15:
            xz = self.snap_zone(5.0)
            xb = self.snap_zone(xz + self.W_src)
            if xz != 5.0 and xb != xz + self.W_src:
                self.xh_zone = xz
                self.xh_row = int(math.floor((xz + xb) / 2 * N - self.W / 2 + 0.5))
        self.xzones = self._cluster(xs, min_w=12.0)
        self.leg_src = float(np.median(legs)) if legs else 1.0
        diff = int(round((2 - math.sqrt(2)) * self.W))
        self.c_out = chamfer if chamfer else max(diff + 1, int(round(self.leg_src * N)))
        self.c_in = max(1, self.c_out - diff)
        # rounded corners: radius from t* (distance corner -> iso line on the bisector = 0.414 r + 0.25 bias)
        self.corner, self.arc_seg = corner, arc_seg
        self.t_split = 1.25
        tb = [t for t in ts if t >= self.t_split]
        tsm = [t for t in ts if t < self.t_split]
        self.r_big_src = (float(np.median(tb)) - 0.25) / (math.sqrt(2) - 1) if tb else 2.0
        if r_big_src:
            self.r_big_src = r_big_src           # reference value (q.png approved by the user)
        self.r_small_src = (float(np.median(tsm)) - 0.25) / (math.sqrt(2) - 1) if tsm else 1.0
        self.r_in_min = max(2, int(math.ceil(self.W / 2)))
        self.r_big = r_big if r_big else max(self.W + self.r_in_min, int(round(self.r_big_src * N)))
        self.r_small = r_small if r_small else max(self.W + self.r_in_min, int(round(self.r_small_src * N)))
        if corner == 'chamfer':
            self.r_big = self.r_small = self.c_out
        # dots
        pb = dot_boxes(glyphs[ord('.')])
        self.baseline = self.snap_zone(pb[0][1]) if pb else glyphs[0x41].shape[0]
        self.D = dot if dot else self.W
        self.Du, self.u_space = self._umlaut_dots(udot)
        self.leg = leg_geometry(self) if glyphs[ord('R')].shape[0] >= 12 else None
        self.leg_ok = self.leg is not None

    @staticmethod
    def _cluster(ys, step=0.05, sigma=0.1, min_w=8.0):
        """peaks of the length-weighted histogram of horizontal edge heights (source px)."""
        if not ys:
            return []
        y = np.array([v for v, _ in ys])
        w = np.array([l for _, l in ys])
        grid = np.arange(y.min() - 1, y.max() + 1, step)
        dens = (w[None, :] * np.exp(-0.5 * ((grid[:, None] - y[None, :]) / sigma) ** 2)).sum(axis=1)
        mass = (w[None, :] * (np.abs(grid[:, None] - y[None, :]) <= 0.2)).sum(axis=1)
        peaks = [grid[i] for i in range(1, len(grid) - 1)
                 if dens[i] >= dens[i - 1] and dens[i] > dens[i + 1] and mass[i] >= min_w]
        return peaks

    def snap_zone(self, y, tol=0.25, zones=None):
        best = None
        for z in (self.zones if zones is None else zones):
            if abs(z - y) <= tol and (best is None or abs(z - y) < abs(best - y)):
                best = z
        return best if best is not None else y

    def _umlaut_dots(self, udot):
        sizes, space = [], {True: [], False: []}
        for c in range(0xC0, 0x100):
            if not U.is_diaeresis(c):
                continue
            sp = V.split_parts(self.g, c)
            if sp is None:
                continue
            dots = dot_boxes(sp[0], 3.2, aspect=False)
            if len(dots) == 2:
                for d in dots:
                    sizes.append(max(d[3] - d[2], d[1] - d[0]))
                xs = sorted((d[2] + d[3]) / 2 for d in dots)
                space[c < 0xE0].append(xs[1] - xs[0])
        Du = udot if udot else max(1, int(round(np.median(sizes) * self.N))) if sizes else self.D
        return Du, {k: float(np.median(v)) if v else None for k, v in space.items()}


def dot_boxes(a, maxs=2.8, aspect=True, baseline=None):
    """Small isolated components ('dots'): list of (y0, y1, x0, x1) in source px from the iso shape."""
    out = []
    for m in V.components(a, 0.2):
        ys, xs = np.nonzero(m)
        if ys.max() - ys.min() > 2 or xs.max() - xs.min() > 2:
            continue
        f = U.resample(np.pad(a * m, PAD), R, 'cubic') > LEVEL
        yy, xx = np.nonzero(f)
        if len(yy) == 0:
            continue
        y0, y1 = yy.min() / R - PAD, (yy.max() + 1) / R - PAD
        x0, x1 = xx.min() / R - PAD, (xx.max() + 1) / R - PAD
        if baseline is not None and y1 > baseline + 0.3:
            continue                                      # reaches below the baseline: comma, not a dot
        if y1 - y0 <= maxs and x1 - x0 <= maxs and (not aspect or 0.6 <= (y1 - y0) / max(x1 - x0, 1e-6) <= 1.6):
            out.append((y0, y1, x0, x1, m))
    return out


def square(y_bot_out, x_left_out, s, N):
    """axis-aligned square polygon (y, x) in source px from output px coordinates."""
    y1, y0 = y_bot_out / N, (y_bot_out - s) / N
    x0, x1 = x_left_out / N, (x_left_out + s) / N
    return np.array([[y0, x0], [y0, x1], [y1, x1], [y1, x0]])


# ------------------------------------------------------------------ glyph construction
BASE_DOT = {ord('!'), ord('?')}
XH_TOL = 0.35      # lower-case top edges this close to the x-height zone go to the common x-height row


def xh_edge(M, code, ln):
    return (M.xh_row is not None and code is not None and (0x61 <= code <= 0x7A or code == 0xE6)
            and abs(ln['v'] - M.xh_zone) <= XH_TOL)


def split_base_dot(M, a):
    """'!' and '?' have no gap in the 1x font (only an alpha dip): cut the rows of the '.' dot off as a dot."""
    p = M.g[ord('.')]
    rows = np.nonzero(p.max(axis=1) > 0.3)[0]
    if len(rows) == 0:
        return a, None
    r0 = rows[0]
    if a[r0:].max() < 0.3:
        return a, None
    body, dot = a.copy(), a.copy()
    body[r0:] = 0
    dot[:r0] = 0
    return body, dot


def build_shape(M, a, axis_target=None, dots_mode='dot', info=None, code=None, body_top=None):
    """Polygons (source px) of glyph alpha a. dots_mode: 'dot' -> small components become the font dot,
    'umlaut' -> umlaut dots (size Du, unified spacing)."""
    N, W = M.N, M.W
    info = info if info is not None else dict(conf=[], axes=[], whole=False)
    extra = None
    if code in BASE_DOT:
        a, extra = split_base_dot(M, a)
    if dots_mode == 'umlaut':
        dots = dot_boxes(a, 3.2, aspect=False)
    else:
        dots = dot_boxes(a, baseline=M.baseline) if dots_mode != 'none' else []
    if extra is not None:
        dots += dot_boxes(extra, 3.2, aspect=False)
    body = a.copy()
    for d in dots:
        body[d[4]] = 0
    polys = []
    stem_axis = None
    if body.max() > 0.1:
        stem_axis = shape_polys(M, body, polys, info, axis_target, code=code)
    info['n_body'] = len(polys)
    # dots
    if dots_mode == 'umlaut' and len(dots) == 2:
        dots = sorted(dots, key=lambda d: d[2])
        cx = axis_target if axis_target is not None else (dots[0][2] + dots[1][3]) / 2
        sp = M.u_space.get(info.get('upper', False)) or ((dots[1][2] + dots[1][3]) - (dots[0][2] + dots[0][3])) / 2
        C2 = int(round(2 * cx * N))                       # 2 * axis (output px), integer
        L = int(round((C2 - sp * N - M.Du) / 2))          # left dot left edge
        Rl = C2 - L - M.Du                                 # mirrored right dot left edge
        yb = int(round(M.snap_zone(max(d[1] for d in dots)) * N))
        if body_top is not None:
            yb = min(yb, body_top - max(1, N // 2) - 1 + 1)
        polys += [square(yb, L, M.Du, N), square(yb, Rl, M.Du, N)]
        info['axes'].append((1, C2 / 2.0))
        info['dots'] = 2
    else:
        D = M.D
        for d in dots:
            y0, y1, x0, x1, m = d
            cx = (x0 + x1) / 2
            # centre over / under a stem of the body if one overlaps in x
            if stem_axis is not None and abs(stem_axis - cx) < 0.9:
                cx = stem_axis
            elif axis_target is not None:
                cx = axis_target
            else:
                ys_, xs_ = np.nonzero(body > 0.5)
                if len(xs_):
                    # stem centre in the rows next to the dot
                    near = np.abs(ys_ + 0.5 - (y0 + y1) / 2) < 6
                    cols = xs_[near & (np.abs(xs_ + 0.5 - cx) < 1.6)]
                    if len(cols):
                        cx = (cols.min() + cols.max() + 1) / 2.0
            # x: parity so that the dot is centred exactly
            if D % 2:
                Cx = math.floor(cx * N) + 0.5
            else:
                Cx = round(cx * N)
            yb = int(round(M.snap_zone(y1) * N))
            polys.append(square(yb, int(round(Cx - D / 2)), D, N))
            info['dots'] = info.get('dots', 0) + 1
    return polys, info


def shape_polys(M, a, polys, info, axis_target=None, code=None):
    """Adds the polygons of the non-dot shape a; returns the x of the vertical axis (source px) if any."""
    N, W = M.N, M.W
    groups = V._groups(a, R, 'cubic', M.level, True, M.sym_thr)
    if axis_target is not None and len(groups) == 1 and 1 in groups[0][1] \
            and abs(groups[0][1][1][0] - axis_target) < 0.75:
        groups[0][1][1] = (groups[0][1][1][0], axis_target)
    L, G, fields = [], [], []
    for gi, (f, axes, whole) in enumerate(groups):
        dx = 0.0
        for ax, (own, target) in axes.items():
            K = int(round(((own + PAD) * R - 0.5) * 2))
            f = 0.5 * (f + V._mirror(f, ax, K))
        loops = []
        for lp in U.marching_squares(f, M.level):
            lp = (lp + 0.5) / R - PAD
            if len(lp) < 8:
                continue
            ls = V.loop_lines(lp, R, f, M.level)
            if len(ls) >= 3:
                loops.append(ls)
        g = dict(axes={}, whole=whole)
        if 1 in axes:
            own, target = axes[1]
            for ls in loops:
                for ln in ls:
                    annotate(ln)
            hv = [x for ls in loops for x in ls if is_hv(x) and x['ax'] == 1]
            centred = any(abs((a_['v'] + b_['v']) / 2 - own) < 0.3 for _, a_, b_ in find_pairs(hv, M.maxw))
            t = target * N
            if centred:
                C = math.floor(t) + 0.5 if W % 2 else float(round(t))
            else:
                C = round(2 * t) / 2.0
            dx = C / N - own
            for ls in loops:
                for ln in ls:
                    for key in ('p', 'end', 'lo', 'hi'):
                        ln[key] = ln[key] + np.array([0.0, dx])
            g['axes'][1] = C
        if 0 in axes:
            g['axes'][0] = None                             # decided after hinting
        for ls in loops:
            for ln in ls:
                ln.pop('ax', None)
                ln.pop('v0', None)
                annotate(ln)
                ln['g'] = gi
        L += loops
        G.append(g)
        fields.append((f, dx))
    L = [prep_loop(ls) for ls in L]
    corners = find_corners(L, fields, M.level)
    allL = [x for ls in L for x in ls]
    hv = [x for x in allL if is_hv(x)]
    # zones: horizontal edges onto the font-wide heights
    for ln in hv:
        z = M.snap_zone(ln['v']) if ln['ax'] == 0 else M.snap_zone(ln['v'], 0.35, M.xzones)
        ln['v'] = z
        ln['p'] = ln['p'].copy()
        ln['p'][ln['ax']] = z
    # uniform stroke width
    pairs = find_pairs(hv, M.maxw)
    xh_bar = None
    for wdt, a_, b_ in pairs:
        if abs(wdt - M.W_src) > 0.5 * M.W_src:
            info['conf'].append('Strich %.1f px im Original (%s)' % (wdt, 'senkrecht' if a_['ax'] == 1 else 'waagerecht'))
        if a_['set'] is not None and b_['set'] is not None:
            continue
        if a_['set'] is not None:
            b_['set'] = a_['set'] + W
        elif b_['set'] is not None:
            a_['set'] = b_['set'] - W
        else:
            c = (a_['v'] + b_['v']) / 2 * N
            lo = int(math.floor(c - W / 2 + 0.5))
            if a_['ax'] == 0 and M.cap_zone is not None and abs(a_['v'] - M.cap_zone) < 1e-9 and a_['s'] == 1:
                lo = M.cap_row                          # top bar on the cap height (also thin bars, e.g. @)
            elif a_['ax'] == 0 and M.base_zone is not None and abs(b_['v'] - M.base_zone) < 1e-9 and b_['s'] == -1:
                lo = min(lo, M.base_row - W)            # bottom bar on the baseline never below the common row (thin bars)
            elif a_['ax'] == 0 and a_['s'] == 1 and xh_edge(M, code, a_):
                if lo != M.xh_row:
                    xh_bar = (b_['v'], M.xh_row + W)    # bar moved onto the x-height (ae 2x): lone edges of its underside follow
                lo = M.xh_row
            a_['set'], b_['set'] = lo, lo + W
    for ln in hv:
        if ln['set'] is None:
            if ln['ax'] == 0 and ln['s'] == 1 and M.cap_zone is not None and abs(ln['v'] - M.cap_zone) < 1e-9:
                ln['set'] = M.cap_row                   # lone top edge on the cap height: common cap top row
            elif ln['ax'] == 0 and ln['s'] == -1 and M.base_zone is not None and abs(ln['v'] - M.base_zone) < 1e-9:
                ln['set'] = M.base_row                  # lone bottom edge on the baseline: common baseline row
            elif ln['ax'] == 0 and ln['s'] == -1 and xh_bar is not None and abs(ln['v'] - xh_bar[0]) < 1e-9:
                ln['set'] = xh_bar[1]
            elif ln['ax'] == 0 and ln['s'] == 1 and xh_edge(M, code, ln):
                ln['set'] = M.xh_row                    # lone top edge at the x-height (u y stems): common x-height row
            else:
                ln['set'] = int(round(ln['v'] * N))
        ln['p'] = ln['p'].copy()
        ln['p'][ln['ax']] = ln['set'] / N
    # symmetry of the hinted edges
    for gi, g in enumerate(G):
        gl = [x for x in hv if x['g'] == gi]
        if 1 in g['axes']:
            V._mirror_hint(gl, 1, g['axes'][1], N)
        if 0 in g['axes'] and gl:
            hz = [x for x in gl if x['ax'] == 0]
            if hz:
                top = min(x['set'] for x in hz)
                bot = max(x['set'] for x in hz)
                g['axes'][0] = (top + bot) / 2.0
                V._mirror_hint(gl, 0, g['axes'][0], N)
    # gaps that closed (conflicts)
    for a_ in hv:
        if a_['s'] != -1:
            continue
        for b_ in hv:
            if b_['ax'] != a_['ax'] or b_['s'] != 1 or b_['v0'] <= a_['v0']:
                continue
            g0 = b_['v0'] - a_['v0']
            if g0 > 2.5 or min(a_['r1'], b_['r1']) - max(a_['r0'], b_['r0']) <= 0.2:
                continue
            gap = b_['set'] - a_['set']
            if gap < 1 <= g0 * N or (gap < 2 and g0 * N >= 2.5):
                info['conf'].append('Luecke %.1f -> %d px (%s)' % (g0 * N, gap, 'x' if a_['ax'] == 1 else 'y'))
    # diagonal strokes: same width W measured perpendicular
    diag = [x for x in allL if not is_hv(x) and not x.get('cham')]
    done = set()
    for a_ in diag:
        if id(a_) in done:
            continue
        best = None
        for b_ in diag:
            if b_ is a_ or id(b_) in done or abs(a_['d'][0] * b_['d'][1] - a_['d'][1] * b_['d'][0]) > 0.17:
                continue
            if np.dot(a_['in'], b_['in']) > -0.9:
                continue
            dist = float(np.dot(b_['p'] - a_['p'], a_['in']))
            if not (0.3 < dist <= M.maxw * 1.2):
                continue
            pa = sorted((np.dot(a_['lo'], a_['d']), np.dot(a_['hi'], a_['d'])))
            pb = sorted((np.dot(b_['lo'], a_['d']), np.dot(b_['hi'], a_['d'])))
            if min(pa[1], pb[1]) - max(pa[0], pb[0]) < 0.3:
                continue
            if best is None or dist < best[0]:
                best = (dist, b_)
        if best:
            dist, b_ = best
            delta = (W / N - dist) / 2
            a_['p'] = a_['p'] - delta * a_['in']
            b_['p'] = b_['p'] - delta * b_['in']
            done.add(id(a_))
            done.add(id(b_))
    # corners: uniform 45 deg chamfers outside and inside (decision from the original alpha, see find_corners)
    for c in corners:
        ls = L[c['loop']]
        c['ia'] = next(i for i, x in enumerate(ls) if x is c['a'])
        c['ib'] = next(i for i, x in enumerate(ls) if x is c['b'])
        c['im'] = next(i for i, x in enumerate(ls) if x is c['m']) if c['m'] is not None else None
        c['X'] = corner_point(c['a'], c['b'])
        c['la'] = edge_len(ls, c['ia'])
        c['lb'] = edge_len(ls, c['ib'])
    # concave corners opposite a chamfered convex corner (and vice versa) are chamfered too
    lim = 2.4 * max(W / N, M.W_src)
    for c in corners:
        if c['X'] is None:
            continue
        for e in corners:
            if e is c or e['X'] is None or e['convex'] == c['convex'] or not (e['cham'] or c['cham'])                     or e['g'] != c['g']:
                continue
            if np.dot(c['ink'], e['ink']) >= 0:
                continue
            dv = e['X'] - c['X']
            if np.linalg.norm(dv) <= lim and np.dot(dv, c['ink']) > 0 and c.get('mate') is None and e.get('mate') is None:
                c['cham'] = e['cham'] = True
                c['mate'], e['mate'] = e, c
    # inner (concave) corners on the same vertical edge as a rounded inner corner are rounded too
    # (B / 8 counters at the waist, 3 at the middle tongue)
    for c in corners:
        if c['convex'] or c['cham'] or c['X'] is None:
            continue
        vert = c['a'] if is_hv(c['a']) and c['a']['ax'] == 1 else c['b']
        for e in corners:
            if e is not c and not e['convex'] and e['cham'] and (e['a'] is vert or e['b'] is vert):
                c['cham'] = True
                c['prop'] = True
                break
    inserts = {}
    removals = []
    for c in corners:
        if c['X'] is not None and not c['cham'] and c['m'] is not None:
            removals.append(c['m'])                        # detected 45 piece, but the original corner is sharp
            c['a']['end'] = c['X']
    # convex corners first (they define the arc centres), then the concave mates
    for c in sorted([c for c in corners if c['cham'] and c['X'] is not None], key=lambda c: not c['convex']):
        ls = L[c['loop']]
        a_, b_ = ls[c['ia']], ls[c['ib']]
        X = c['X']
        da, db = a_['d'], b_['d']
        # an edge may be used completely when its other end is a sharp corner (e.g. a stem with a square end)
        fa = 1.0 if not any(o is not c and o['cham'] and o['b'] is c['a'] for o in corners) else 0.5
        fb = 1.0 if not any(o is not c and o['cham'] and o['a'] is c['b'] for o in corners) else 0.5
        mx = max(1.0, math.floor(min(c['la'] * fa, c['lb'] * fb) * N + 0.5))
        big = (c['orig'] and c['t'] >= M.t_split) or (c.get('mate') is not None and c['mate']['t'] >= M.t_split)
        if c['convex']:
            r = M.r_big if big else M.r_small
            mt = c.get('mate')
            if mt is not None:
                # keep the stroke width in the corner: outer radius <= possible inner radius + W
                mi = max(0.0, math.floor(min(mt['la'], mt['lb']) * N / 2 + 0.5))
                r = min(r, mi + W)
        else:
            mt = c.get('mate')
            if mt is not None and 'centre' in mt:
                v = mt['centre'] - X
                r = float(np.mean(np.abs(v))) * N          # concentric with the outer arc
            else:
                r = (M.r_big if big else M.r_small) - M.W
            r = max(r, M.r_in_min)                          # always round inside
        if r > mx:
            if c['convex']:
                info['conf'].append('Rundung r %.0f->%.0f px (kurze Kante)' % (r, mx))
            r = mx
        if r < 0.5:
            continue
        rs = r / N
        c['centre'] = X + rs * (db - da)
        P1 = X - rs * da
        nseg = max(2, int(math.ceil(M.arc_seg * r / max(M.r_big, 1))))
        if M.corner == 'chamfer':
            nseg = 1
        pts = [P1]
        cen = c['centre']
        u0 = P1 - cen
        u1 = (X + rs * db) - cen
        for k in range(1, nseg + 1):
            t = k / nseg * math.pi / 2
            if M.corner == 'chamfer':
                pts.append(X + rs * db)
            else:
                pts.append(cen + u0 * math.cos(t) + u1 * math.sin(t))
        seg_lines = []
        for p0, p1 in zip(pts[:-1], pts[1:]):
            dv = (p1 - p0) / np.linalg.norm(p1 - p0)
            mid = (p0 + p1) / 2
            nin = np.array([dv[1], -dv[0]])
            if np.dot(nin, a_['in'] + b_['in']) < 0:
                nin = -nin
            seg_lines.append(dict(p=mid, d=dv, cls=45, end=p1, lo=p0, hi=p1, arc=True, **{'in': nin}))
        a_['end'] = P1
        if c['im'] is not None:
            ls[c['im']] = seg_lines
        else:
            inserts.setdefault(c['loop'], []).append((c['ia'], seg_lines))
    for li, ins in inserts.items():
        for ia, seg in sorted(ins, key=lambda t: -t[0]):
            L[li].insert(ia + 1, seg)
    # flatten (arcs are lists of lines)
    L = [[y for x in ls for y in (x if isinstance(x, list) else [x])] for ls in L]
    rm = {id(x) for x in removals}
    L = [[x for x in ls if isinstance(x, list) or id(x) not in rm] for ls in L]
    info.setdefault('corners', []).extend(
        dict(X=c['X'], convex=c['convex'], t=c['t'], orig=c['orig'], cham=c['cham'], ink=c['ink'])
        for c in corners if c['X'] is not None)
    for ls in L:
        polys.append(V.build_poly(ls, maxd=2.0))
    info['axes'] += [(ax, C) for g in G for ax, C in g['axes'].items() if C is not None and g['whole']]
    info['whole'] = len(G) == 1 and G[0]['whole']
    return (G[0]['axes'][1] / N) if (len(G) == 1 and 1 in G[0]['axes']) else None


def render_polys(M, polys, h, w, info, SS=8):
    nb = info.get('n_body', len(polys))
    r = U.raster(polys[:nb], M.N, h, w, SS)
    if info.get('whole'):
        for ax, C in info['axes']:
            K = int(round(2 * C)) - 1
            cen = np.arange(r.shape[ax]) + 0.5
            sec = (cen > C)[None, :] if ax == 1 else (cen > C)[:, None]
            r = np.where(sec, V._mirror(r, ax, K), r)
    if nb < len(polys):
        r = np.maximum(r, U.raster(polys[nb:], M.N, h, w, SS))
    return r


def render(M, code, return_info=False):
    a = M.g[code]
    N = M.N
    info = dict(conf=[], axes=[], whole=False)
    if a.size == 0 or a.max() <= 0:
        z = np.zeros((a.shape[0] * N, a.shape[1] * N))
        return (z, info) if return_info else z
    if code < 0x20 and np.all((a == 0) | (a == 1)):
        r = U.xbr(a, N, t=0.5)
        info['conf'].append('Symbol: xBR')
        return (r, info) if return_info else r
    # ---- part hooks (work_v3). Order:
    #  1 dots    first: owns ? ! i j . , : ; middle dot ...  and the rotated copies (inverted ! ?) -> must run before
    #            DERIVED; it re-renders ? ! i j itself with M.dots = False (stroke / contour body), then places the dot.
    #  2 pairs   brackets, < > guillemets, backslash, quotes: the partner is a mirror of the FINISHED source
    #            (backslash = mirror of the stroke-built slash, closing = mirror of the opening shape);
    #            must run before the strokes (its chevron replaced upscale4.g_less, removed) and before DERIVED.
    #  3 accents base letter rendered through this same function (finished, incl. dots / strokes / contour)
    #            + accent part; accented letters are not handled by 1, 2, 4.
    #  4 strokes upscale4 (+ diag_parts): stroke-built letters / digits.
    #  5 Q, DERIVED (fallback for fontk), contour construction.
    #  0 frac    (work_frac) 1/4 1/2 3/4 and superscripts: fontk's finished digits + slash; before everything else.
    import gat_parts                                      # work_gat: font.gfx @ = O arcs, uniform stroke, open form
    if code in gat_parts.CODES:
        r = gat_parts.render(M, code)
        if r is not None:
            info['conf'].append('font.gfx @ aus O-Boegen (gat_parts)')
            return (r, info) if return_info else r
    if getattr(M, 'compose', True):
        import gcaret_parts                               # work_gcaret: font.gfx ^ ae ordfeminine
        r = gcaret_parts.render(M, code)
        if r is not None:
            info['conf'].append('font.gfx ^ / ae / ordf (gcaret_parts)')
            return (r, info) if return_info else r
        import kacc_parts                                 # work_kacc: fontk a, y-acute, y-diaeresis, s-caron, S-caron, Y-diaeresis
        r = kacc_parts.render(M, code)
        if r is not None:
            info['conf'].append('fontk: Grundbuchstabe + Akzent (kacc_parts)')
            return (r, info) if return_info else r
        import ktm_parts                                  # work_ktm: fontk TM, f-hook = finished font.gfx glyph
        if code in ktm_parts.CODES:
            r = ktm_parts.render(M, code)
            if r is not None:
                info['conf'].append('= fertiges font.gfx-Glyph (gleiche Zelle, ktm_parts)')
                return (r, info) if return_info else r
        import gsym_parts                                 # work_gsym: font.gfx * TM pilcrow f-hook built from parts
        if code in gsym_parts.CODES:
            r = gsym_parts.render(M, code)
            if r is not None:
                info['conf'].append('Symbol aus Bauteilen (gsym_parts)')
                return (r, info) if return_info else r
        import kexcl_parts                                # work_kexcl: fontk ! inverted ! * = stem + dot, 6 arms
        r = kexcl_parts.render(M, code)
        if r is not None:
            info['conf'].append('fontk ! * aus Bauteilen (kexcl_parts)')
            return (r, info) if return_info else r
        import frac_parts
        if code in frac_parts.CODES:
            r = frac_parts.render(M, code)
            if r is not None:
                info['conf'].append('Bruch / Hochzahl = fontk-Ziffern + Schraegstrich (frac_parts)')
                return (r, info) if return_info else r
    if getattr(M, 'compose', True):
        import gmarks_parts                               # work_gmarks2: font.gfx free ` dieresis macron acute cedilla
        r = gmarks_parts.render(M, code)
        if r is not None:
            info['conf'].append('Freies Zeichen wie auf den fertigen Buchstaben (gmarks_parts)')
            return (r, info) if return_info else r
        import kmarks_parts                               # v9: fontk free ` ^ ~ dieresis macron acute cedilla
        r = kmarks_parts.render(M, code)
        if r is not None:
            info['conf'].append('fontk: freies Zeichen wie auf den fertigen Grossbuchstaben (kmarks_parts)')
            return (r, info) if return_info else r
    if getattr(M, 'dots', True):
        import dots_parts
        r = dots_parts.render(M, code)
        if r is not None:
            info['conf'].append('Punkt exakt ausgerichtet (dots_parts)')
            return (r, info) if return_info else r
    if code == 0x89 and getattr(M, 'compose', True):
        import mille_parts                                # per mille = 3 x fontk 0 + 22.5 deg slash
        res = mille_parts.render(M, code)
        if res is not None:
            r, msg = res
            info['conf'].append(msg)
            return (r, info) if return_info else r
    if code == 0x25 and getattr(M, 'compose', True):
        import percent_parts                              # % of font.gfx = per mille without its third zero
        res = percent_parts.render(M, code)
        if res is not None:
            r, msg = res
            info['conf'].append(msg)
            return (r, info) if return_info else r
    if getattr(M, 'compose', True):
        import regcopy_parts                              # work_regcopy: (R) (C) = O ring + reduced R / C
        r = regcopy_parts.render(M, code)
        if r is not None:
            info['conf'].append('= O-Ring + verkleinertes R / C (regcopy_parts)')
            return (r, info) if return_info else r
        import ksym_parts                                 # work_ksym: fontk O-slash, eth, degree, @ from its own letters
        r = ksym_parts.render(M, code)
        if r is not None:
            info['conf'].append('fontk-Symbol aus eigenen Buchstaben (ksym_parts)')
            return (r, info) if return_info else r
        import kbrace_parts                               # work_kbrace: fontk 0x7B..0x7E show the source pixels, not ASCII
        r = kbrace_parts.render(M, code)
        if r is not None:
            info['conf'].append('fontk: Quellpixel statt ASCII (kbrace_parts)')
            return (r, info) if return_info else r
        import pairs_parts
        res = pairs_parts.render(M, code)
        if res is not None:
            r, info['axes'], msg = res
            info['conf'].append(msg)
            return (r, info) if return_info else r
        import accents_parts
        res = accents_parts.compose(M, code) if code in accents_parts.COMPOSE_OK else None
        if res is not None:
            r, msg = res
            info['conf'].append(msg)
            return (r, info) if return_info else r
        if code == DOLLAR and M.g[code].max() > 0:
            r, msg = render_dollar(M)
            info['conf'].append(msg)
            return (r, info) if return_info else r
        if code == YEN and M.g[ord('Y')].max() > 0:
            res = render_yen(M) if M.g[ord('Y')].shape[0] == a.shape[0] else yen_from_font(M)
            if res is not None:
                r, msg = res
                info['conf'].append(msg)
                return (r, info) if return_info else r
        if code in SLASHED and M.g[SLASHED[code]].max() > 0 and M.g[code].shape[0] == M.g[SLASHED[code]].shape[0]:
            r = slashed(M, code, SLASHED[code])
            info['conf'].append('= %s + 45-Grad-Strich (Bauteil)' % chr(SLASHED[code]))
            return (r, info) if return_info else r
    if code == 0x23 and getattr(M, 'compose', True):
        import hash_parts                                 # work_hash: # = two / + the bars of =
        r = hash_parts.render(M, code, render(M, ord('=')))
        if r is not None:
            info['conf'].append('# = zwei / + Balken von = (hash_parts)')
            return (r, info) if return_info else r
    if code == 0xDF and getattr(M, 'compose', True):
        import sz_parts                                   # work_sz: sharp s = stem + top arc + B-style bowls
        r = sz_parts.render(M, code, render(M, ord('B')))
        if r is not None:
            info['conf'].append('sz = Stamm + Bogen + B-Bauche (sz_parts)')
            return (r, info) if return_info else r
    if code in (ord('6'), ord('9')) and getattr(M, 'compose', True):
        res = six_nine_from_8(M, code)
        if res is not None:
            r, msg = res
            info['conf'].append(msg)
            return (r, info) if return_info else r
    if code == 0x2F and getattr(M, 'strokes', True):
        import kslash_parts                               # work_kslash: fontk '/' = straight stroke like font.gfx '/'
        r = kslash_parts.render(M, code)
        if r is not None:
            info['conf'].append('fontk / = gerader 22,5-Grad-Strich (kslash_parts)')
            return (r, info) if return_info else r
    if code in (0x33, 0x37) and getattr(M, 'strokes', True):
        import k73_parts                                  # work_k73: fontk 7 = bar + straight diagonal, 3 = from the 8
        r = k73_parts.render(M, code)
        if r is not None:
            info['conf'].append('fontk 3 / 7 neu aufgebaut (k73_parts)')
            return (r, info) if return_info else r
    if code in (0x47, 0x67, 0x53, 0x73) and getattr(M, 'strokes', True):
        import kbdpq_parts                               # work_kbdpq: fontk G g S s = arcs of O / 0 (contour filled the counters)
        r = kbdpq_parts.render(M, code)
        if r is not None:
            info['conf'].append('fontk G / S = Boegen von O / 0 (kbdpq_parts)')
            return (r, info) if return_info else r
    if code in (0x32, 0x34) and getattr(M, 'strokes', True):
        import k4_parts                                  # work_k4: fontk 2 / 4 = strokes (contour closed the counters)
        r = k4_parts.render(M, code)
        if r is not None:
            info['conf'].append('fontk 2 / 4 = Striche (k4_parts)')
            return (r, info) if return_info else r
    if getattr(M, 'strokes', True):
        import kqvw_parts                                 # work_kqvw: fontk ? inverted ? V W v w ^ = strokes
        res = kqvw_parts.render(M, code)
        if res is not None:
            r, info['axes'] = res
            info['conf'].append('fontk: Strich-Konstruktion (kqvw_parts)')
            return (r, info) if return_info else r
    if getattr(M, 'strokes', True):
        import nm_parts                                   # work_n: N / M = stems + straight diagonals (both fonts)
        res = nm_parts.render(M, code)
        if res is not None:
            r, info['axes'] = res
            info['conf'].append('Staemme + gerade Diagonalen (nm_parts)')
            return (r, info) if return_info else r
        import upscale4
        res = upscale4.render(M, code)
        if res is not None:
            r, axes = res
            info['axes'] = axes
            info['conf'].append('Strich-Konstruktion')
            return (r, info) if return_info else r
    if code == ord('Q') and M.g[ord('O')].max() > 0:
        r = render_Q(M, a)
        info['conf'].append('Q = O-Form + 45-Grad-Schwanz (konstruiert)')
        return (r, info) if return_info else r
    if code in DERIVED and getattr(M, 'compose', True):
        src, how = DERIVED[code]
        if M.g[src].max() > 0 and derive_ok(M, code, src):
            r = derive(M, code, src, how)
            info['conf'].append('= %s %s (Bauteil)' % (bytes([src]).decode('cp1252'), how))
            return (r, info) if return_info else r
    sp = V.split_parts(M.g, code)
    if sp is None and code in LEG_CODES and M.leg_ok:
        r = render_leg_glyph(M, code)
        info['conf'].append('Bein = R-Bein (22,5 Grad, Bauteil)')
        return (r, info) if return_info else r
    if sp is None:
        polys, info = build_shape(M, a, info=info, code=code)
        r = render_polys(M, polys, a.shape[0], a.shape[1], info)
        if code == ord('e') and getattr(M, 'compose', True):
            import e_parts
            if e_parts.applies(M):
                re_ = e_parts.render_e(M, r, render(M, ord('o')))
                if re_ is not None:
                    r = re_
                    info['conf'].append('e = o-Ring (gleiche Boegen) + Querstrich (e_parts)')
        if code in PART_COPY:
            r, msg = copy_part(M, r, *PART_COPY[code])
            if msg:
                info['conf'].append(msg)
        if code == ord('?') and M.leg_ok:
            r = question_from_P(M, r)
            info['conf'].append('Bogen = P-Bogen (Bauteil)')
        if code in RECT_T:
            res = rect_t(M, r, code)
            if res is not None:
                r = res
                info['conf'].append('t: Stamm + Querbalken aus Rechtecken (90-Grad-Ecken)')
        if code in SHARED_X and M.leg_ok:
            r = shared_xheight(M, r, *SHARED_X[code])
            info['conf'].append('x-Hoehen-Teil = %s (%s)' % (SHARED_X[code][0], SHARED_X[code][1]))
        if code in RECT_CROSS:
            res = rect_cross(M, r, code)
            if res is not None:
                r = res
                info['conf'].append('%s: Stamm + Querbalken aus Rechtecken (90-Grad-Ecken)%s' % (RECT_CROSS[code], ', Bogen = y-Schweif-Bogen' if code == ord('f') else ''))
        if a.max() < 0.75 and r.sum() < 0.4 * a.sum() * N * N:
            # faint original (alpha max ~0.5, e.g. degree sign, small tilde): the iso line at LEVEL finds nothing
            # -> contour at a level relative to the glyph's own maximum
            r = U.vectorize(a, N, hint=True, level=LEVEL * a.max())
            info['conf'].append('schwache Glyphe: Iso-Level relativ zum Maximum')
        return (r, info) if return_info else r
    mark, body = sp
    ib = dict(conf=[], axes=[], whole=False)
    pb, ib = build_shape(M, body, info=ib)
    B = render_polys(M, pb, a.shape[0], a.shape[1], ib)
    target = None
    for ax, C in ib['axes']:
        if ax == 1:
            target = C / N
    if target is None:
        # body not symmetric (a, u): centre of the ink columns of the x-height part
        cols = np.nonzero(body.max(axis=0) > 0.5)[0]
        if len(cols):
            target = (cols.min() + cols.max() + 1) / 2.0
    im = dict(conf=[], axes=[], whole=False, upper=code < 0xE0)
    rows_b = np.nonzero(B.max(axis=1) > 0.3)[0]
    pm, im = build_shape(M, mark, axis_target=target, dots_mode='umlaut' if U.is_diaeresis(code) else 'none',
                         info=im, body_top=int(rows_b[0]) if len(rows_b) else None)
    Mk = U.raster(pm, N, a.shape[0], a.shape[1], 8)
    g_ = max(1, N // 2)
    keep = ~U.dilate(B > 0.3, g_)
    if (Mk * ~keep).sum() > 0.5:
        info['conf'].append('Akzent beruehrt Koerper (gekuerzt)')
    r = np.maximum(Mk * keep, B)
    info['conf'] += ib['conf'] + im['conf']
    info['axes'] = ib['axes'] + im['axes']
    return (r, info) if return_info else r


DOLLAR = ord('$')
DOLLAR_ODD = 1      # bar parity does not fit the S axis: +1 shift half a px right (as the original bar), 0 widen by 1


def render_dollar(M):
    """$ = finished S (same cell, else the $ body itself without bar) + straight vertical bar, width M.W,
    centred on the S axis (middle of its ink columns), sticking out above / below the S as far as in the
    original (rows of the original $ whose ink lies only in the bar columns)."""
    N, W, a = M.N, M.W, M.g[DOLLAR]
    src = ord('S') if M.g[ord('S')].shape == a.shape and M.g[ord('S')].max() > 0 else None
    if src is not None:
        body, what = render(M, src), 'S'
    else:
        old = getattr(M, "compose", True)
        M.compose = False                   # contour body of the $ itself (bar is too faint to survive)
        try:
            body, what = render(M, DOLLAR), '$-Koerper'
        finally:
            M.compose = old
    rows = np.nonzero(body.max(axis=1) > 0.5)[0]
    cols = np.nonzero(body.max(axis=0) > 0.5)[0]
    # protrusion in original px: ink rows of the $ above / below the S rows of the original
    so = M.g[src] if src is not None else a
    o_rows = np.nonzero(a.max(axis=1) > 0.25)[0]
    s_rows = np.nonzero(so.max(axis=1) > 0.5)[0]
    up = max(1, int(s_rows.min() - o_rows.min())) if src is not None else 1
    dn = max(1, int(o_rows.max() - s_rows.max())) if src is not None else 1
    c2 = int(cols.min() + cols.max() + 1)   # 2 x axis position (pixel-edge coordinates)
    bw = W
    if (c2 - bw) % 2:
        bw += 1 - DOLLAR_ODD
        c2 += DOLLAR_ODD
    x0 = (c2 - bw) // 2
    y0, y1 = max(0, rows.min() - up * N), min(body.shape[0], rows.max() + 1 + dn * N)
    r = body.copy()
    r[y0:y1, x0:x0 + bw] = 1.0
    return r, '$ = %s + senkrechter Strich %d px (x %d..%d, %d/%d px ueberstehend)' % (
        what, bw, x0, x0 + bw - 1, up * N, dn * N)


YEN = 0xA5
YEN_BAR = 0.5       # bar length relative to the original bars (user feedback: half as long)


def render_yen(M):
    """Yen = finished Y (pixel-identical outside the bars) + two straight horizontal bars, thickness M.W, symmetric
    around the Y's stem axis. Stacked on the plain stem below the fork, 1 px under it (the original bars reach up into
    the fork, so as high as possible); the rest of the free stem goes to the gap between the bars and the tail.
    Bar length: YEN_BAR x original (lowest row with ink outside the stem, edge columns weighted by coverage).
    Same cell height required (caller); a narrower cell moves the Y only as far as needed. None if it does not fit."""
    N, W, a = M.N, M.W, M.g[YEN]
    Y = render(M, ord('Y'))
    h, w = a.shape[0] * N, a.shape[1] * N
    cols = np.nonzero(Y.max(axis=0) > 0.02)[0]
    if cols[-1] - cols[0] + 1 > w:
        return None
    dx = min(0, w - 1 - cols[-1])
    r = np.zeros((h, w))
    x0, x1 = max(0, dx), min(w, Y.shape[1] + dx)
    r[:, x0:x1] = Y[:, x0 - dx:x1 - dx]
    ink = r > 0.02
    rows = np.nonzero(ink.any(axis=1))[0]
    yE = rows[-1]
    sc = np.nonzero(ink[yE - 1])[0]                  # stem columns (row above the last ink row)
    sL, sR = sc[0], sc[-1]
    yS = yE
    while yS > rows[0] and not ink[yS - 1, :sL].any() and not ink[yS - 1, sR + 1:].any():
        yS -= 1
    free = yE - yS + 1 - 2 * W
    if free < 3:                                     # gaps + tail need at least 1 px each
        return None
    gap1 = 1
    gap2 = (free - gap1 + 1) // 2
    # bar length: original, in output px, same parity as the stem so that it stays symmetric
    sw = sR - sL + 1
    ao = a[np.nonzero((a > 0.1).sum(axis=1) > 2)[0][-1]]
    L = np.clip(ao / np.median(ao[ao > 0.02]), 0, 1).sum() * N * YEN_BAR
    arm = min(int(round((L - sw) / 2.0)), sL, w - 1 - sR)
    b0, b1 = sL - arm, sR + arm + 1
    y1 = yS + gap1
    y2 = y1 + W + gap2
    r[y1:y1 + W, b0:b1] = 1.0
    r[y2:y2 + W, b0:b1] = 1.0
    return r, 'Yen = Y + 2 waagerechte Striche %d px (x %d..%d, Zeilen %d..%d und %d..%d, Abstand %d/%d, Rest %d)' % (
        W, b0, b1 - 1, y1, y1 + W - 1, y2, y2 + W - 1, gap1, gap2, yE - (y2 + W) + 1)


_YEN_SRC = {}
YEN_SRC_W = {2: 3, 3: 4}    # font.gfx stroke width per scale


def yen_from_font(M):
    """fontk: its Y is a smaller caps Y (9 rows) but its Yen cell holds the font.gfx artwork (15 rows), so the Yen is
    the finished font.gfx Yen at the same scale, pixel-identical. None if the cell differs or the scale is unknown."""
    if M.N not in YEN_SRC_W:
        return None
    if M.N not in _YEN_SRC:
        fn = 'font.gfx'
        _YEN_SRC[M.N] = FontModel(U.load(fn), M.N, W=YEN_SRC_W[M.N], r_big_src=3.17)
    src = _YEN_SRC[M.N]
    if src.g[YEN].shape != M.g[YEN].shape:
        return None
    return render(src, YEN), 'Yen = Yen aus font.gfx (gleiche Zelle, Bauteil)'


def six_nine_from_8(M, code):
    """9 = upper bowl of the 8 (rows down to the bottom of its middle bar, pixel-identical) + the 8's lower right
    quadrant (stem, inner and outer round corners) + the 8's bottom stroke continued left to its left ink edge
    (square end); the bowl's open bottom-left corner = the 8's outer bottom-left corner (last W rows, left W cols).
    6 = the same with top and bottom swapped: lower bowl of the 8 + upper left quadrant + top stroke continued right
    + the 8's outer top-right corner. If the 8 is symmetric in both axes the 6 is exactly the 9 rotated by 180 deg.
    Built in the 8's frame, placed horizontally centred in the target cell (whole output px, floor)."""
    N, W = M.N, M.W
    a = M.g[code]
    a8 = M.g[ord('8')]
    if a8.max() <= 0 or a.shape[0] != a8.shape[0]:
        return None
    E, inf = render(M, ord('8'), True)
    ax = [C for k, C in inf['axes'] if k == 1]
    if not ax:
        return None
    H, W8 = E.shape
    cut = int(math.floor(ax[0]))                     # first column of the right half (mirror axis)
    col = E[:, cut] > 0.5
    runs, y = [], 0
    while y < H:
        if col[y]:
            y0 = y
            while y < H and col[y]:
                y += 1
            runs.append((y0, y - 1))
        y += 1
    if len(runs) != 3:
        return None
    (yT, tb), (bt, bb), (b0, yB) = runs               # top stroke, middle bar, bottom stroke (rows)
    cols = np.nonzero(E.max(axis=0) > 0.5)[0]
    xL, xR = cols[0], cols[-1]
    B = np.zeros_like(E)
    if code == ord('9'):
        B[:bb + 1] = E[:bb + 1]                                       # upper bowl incl. the whole bar
        B[bb + 1:, cut:] = E[bb + 1:, cut:]                           # stem + lower right corners
        B[b0:yB + 1, :cut] = E[b0:yB + 1, cut:cut + 1]                # bottom stroke continued left
        B[bb + 1:, :xL] = 0
        dy = yB - bb
        for yy in range(max(0, bb - W + 1), bb + 1):                  # outer bottom-left corner of the bowl
            B[yy, :xL + W] = E[yy + dy, :xL + W]
    else:
        B[bt:] = E[bt:]                                               # lower bowl incl. the whole bar
        B[:bt, :cut] = E[:bt, :cut]                                   # left stroke + upper left corners
        B[yT:tb + 1, cut:] = E[yT:tb + 1, cut - 1:cut]                # top stroke continued right
        B[:bt, xR + 1:] = 0
        dy = yT - bt
        for yy in range(bt, min(H, bt + W)):                          # outer top-right corner of the bowl
            B[yy, xR + 1 - W:] = E[yy + dy, xR + 1 - W:]
    w = a.shape[1] * N
    dx = (w - W8) // 2
    out = np.zeros((H, w))
    for x in range(w):
        xs = x - dx
        if 0 <= xs < W8:
            out[:, x] = B[:, xs]
    return out, ('Bogen = oberer 8-Bogen' if code == ord('9') else 'Bogen = unterer 8-Bogen') + ' (Bauteil)'


def render_Q(M, a):
    """Q: the rendered O (same arcs, pixel-identical), placed at the best integer offset over Q's own bowl,
    plus a 45 deg tail of stroke width W through the bottom-right corner, cut at the baseline."""
    N, W = M.N, M.W
    h, w = a.shape[0] * N, a.shape[1] * N
    O = render(M, ord('O'))
    own = render_polys(M, build_shape(M, a, info=dict(conf=[], axes=[], whole=False))[0], a.shape[0], a.shape[1],
                       dict(conf=[], axes=[], whole=False))
    best = None
    for dx in range(-2 * N, 2 * N + 1):
        cv = np.zeros((h, w))
        x0, x1 = max(0, dx), min(w, O.shape[1] + dx)
        cv[:, x0:x1] = O[:h, x0 - dx:x1 - dx]
        e = np.abs(cv - own).sum()
        if best is None or e < best[0]:
            best = (e, dx, cv)
    _, dx, r = best
    rows = np.nonzero(r.max(axis=1) > 0.5)[0]
    cols = np.nonzero(r.max(axis=0) > 0.5)[0]
    yB, xR = rows[-1] + 1, cols[-1] + 1                     # outer bottom / right edge (output px)
    d = np.array([1.0, 1.0]) / math.sqrt(2)
    nrm = np.array([1.0, -1.0]) / math.sqrt(2)
    P0 = np.array([yB, xR]) - 2.4 * W * d
    P1 = np.array([yB, xR]) + 1.2 * W * d
    hw = W / 2.0
    poly = np.array([P0 + hw * nrm, P1 + hw * nrm, P1 - hw * nrm, P0 - hw * nrm]) / N
    t = U.raster([poly], N, a.shape[0], a.shape[1], 8)
    t[yB:, :] = 0                                           # tail ends on the baseline
    return np.maximum(r, t)


# shared parts: target code -> (source code, side). 'right': the right half of the source (from its mirror axis)
# replaces the target's columns from the same cut, aligned on the right ink edge.
PART_COPY = {ord('3'): (ord('8'), 'right')}
LEG_CODES = (ord('R'),)


def copy_part(M, r, src, side):
    S_, info = render(M, src, True)
    axes = [C for ax, C in info['axes'] if ax == 1]
    if not axes:
        return r, None
    C = axes[0]
    cs = np.nonzero(S_.max(axis=0) > 0.5)[0]
    ct = np.nonzero(r.max(axis=0) > 0.5)[0]
    if len(cs) == 0 or len(ct) == 0:
        return r, None
    dx = int(ct[0] - cs[0])                        # same left ink edge -> same ink width as the source
    cut = int(math.ceil(C)) + dx
    out = r.copy()
    w = r.shape[1]
    for x in range(max(0, cut), w):
        xs = x - dx
        out[:, x] = S_[:, xs] if 0 <= xs < S_.shape[1] else 0
    return out, 'rechte Haelfte = %s (Bauteil)' % bytes([src]).decode('cp1252')


TAN225 = math.tan(math.radians(22.5))


def leg_geometry(M):
    """R leg from the original: top = bottom edge of the middle bar, foot centre on the baseline row,
    angle forced to 22.5 deg from the vertical (original R: ~26 deg)."""
    a = M.g[ord('R')]
    bar_rows = [y for y in range(a.shape[0]) if a[y, 2:5].min() > 0.5]
    if not bar_rows:
        return None
    y_top = M.snap_zone(bar_rows[-1] + 1.0)
    rows = np.nonzero(a.max(axis=1) > 0.3)[0]
    yb = rows[-1]
    w = a[yb, 3:]
    x_f = 3 + (w * (np.arange(len(w)) + 0.5)).sum() / max(w.sum(), 1e-6)
    return y_top, M.baseline, x_f


def leg_raster(M, a_shape, y_top, base, x_foot):
    N, W = M.N, M.W
    d = np.array([1.0, TAN225]) / math.hypot(1.0, TAN225)          # (y, x) downwards-right
    nrm = np.array([d[1], -d[0]])
    L = (base - y_top)
    P_bot = np.array([base, x_foot]) * N
    P_top = P_bot - (L + 0.5 * W / N) / d[0] * N * d
    P_bot2 = P_bot + 2.0 * W * d
    hw = W / 2.0
    poly = np.array([P_top + hw * nrm, P_bot2 + hw * nrm, P_bot2 - hw * nrm, P_top - hw * nrm]) / N
    t = U.raster([poly], N, a_shape[0], a_shape[1], 8)
    t[M.base_row:, :] = 0                                             # foot flat on the common baseline row
    return t


def render_leg_glyph(M, code):
    """R and K: glyph without its lower right leg + the common 22.5 deg leg (pixel-identical part; for K it is
    shifted by whole output pixels so that its top meets the arm junction)."""
    N = M.N
    a = M.g[code].copy()
    y_top, base, x_f = M.leg
    cut = int(round(y_top))
    junction = None
    if code == ord('k'):
        cut = 9                                                       # k: junction row 8, leg rows 9-11
    if code in (ord('K'), ord('k')):
        row = a[cut - 1 if code == ord('k') else cut, 2:]
        junction = 2 + (row * (np.arange(len(row)) + 0.5)).sum() / max(row.sum(), 1e-6)
    a[cut:, 2:] = 0
    if code == ord('R') and M.g[ord('P')].shape == M.g[code].shape:
        r = render(M, ord('P'))                                       # R = P (pixel-identical) + leg
    else:
        polys, info = build_shape(M, a, info=dict(conf=[], axes=[], whole=False))
        r = render_polys(M, polys, a.shape[0], a.shape[1], info)
    if code == ord('k'):
        # lower-case proportion: same angle and width, shorter (from the k junction to the baseline)
        yk = M.snap_zone(float(cut))
        top_k = junction
        leg = leg_raster(M, a.shape, yk, base, top_k + (base - yk) * TAN225)
        return np.maximum(r, leg)
    leg = leg_raster(M, a.shape, y_top, base, x_f)
    if code == ord('R'):
        # the leg starts at the bottom edge of the middle bar: nothing of it reaches into the counter
        import counters_parts
        leg = counters_parts.clip_leg(r, leg, N)
    if junction is not None:
        top_x_R = x_f - (base - y_top) * TAN225                       # leg centre at the top (R)
        dx = int(round((junction - top_x_R) * N))
        leg = np.roll(leg, dx, axis=1)
        if dx < 0:
            leg[:, dx:] = 0
        elif dx > 0:
            leg[:, :dx] = 0
    return np.maximum(r, leg)


def question_from_P(M, r):
    """'?': the bowl (rows above the bottom of P's middle bar, right of P's stem) is P's rendered bowl, shifted by
    whole source pixels so that the right edges match the original '?'."""
    N = M.N
    P = render(M, ord('P'))
    p1, q1 = M.g[ord('P')], M.g[ord('?')]
    cp = np.nonzero(p1.max(axis=0) > 0.5)[0][-1]
    cq = np.nonzero(q1.max(axis=0) > 0.5)[0][-1]
    dx = int(cq - cp) * N
    y_cut = int(round(M.leg[0] * N))                                 # bottom of the middle bar
    stem_r = int(math.ceil((np.nonzero(p1[6] > 0.5)[0][0] + M.W_src) * N)) if (p1[6] > 0.5).any() else 2 * N
    out = r.copy()
    x0 = max(0, stem_r + dx)
    for x in range(x0, r.shape[1]):
        xs = x - dx
        out[:y_cut, x] = P[:y_cut, xs] if 0 <= xs < P.shape[1] else 0
    return out


# f, dagger, pound: the contour construction chamfers the joints between crossbar and stem (45 deg slant / pyramid
# slope / 1 px fill). Rule: straight strokes are rectangles of width M.W, joints have 90 deg corners.
#   f      stem = columns of the finished render's bottom row, below the crossbar; crossbar = M.W rows at the
#          original's left-arm row centroid, left end from the original coverage, right end = finished render's end;
#          the hook above the crossbar is kept as rendered, the 45 deg slant left of its top-row edge is removed.
#   dagger stem = M.W columns centred on the original stem, vertical extent of the finished render; crossbar = M.W
#          rows at the original's left-arm row centroid, arms symmetric, length from the original coverage.
#   pound  only the fill right of the stem in the N rows directly above the middle crossbar is removed.
RECT_CROSS = {ord('f'): 'f', 0x86: 'dagger', 0xA3: 'pound'}


def _cov_edges(c, thr=0.3):
    k = np.nonzero(c > thr)[0]
    return k[0] + 1 - c[k[0]], k[-1] + c[k[-1]]


F_BAR_FLUSH = True     # crossbar right end flush with the f top (as in the original); False: v4 crossbar end


def y_tail_arc(M):
    """arc of the finished y's tail, rotated 180 deg so that it opens to the lower left -> f top:
    square block, stem side left (edge = stem left), run side right (W full rows on top).
    Block = from the first non-full px of the tail's bottom row to the stem's right edge."""
    W = M.W
    y = render(M, ord('y'))
    rows = np.nonzero(y.max(axis=1) > 0.5)[0]
    if not len(rows):
        return None
    b = rows[-1]
    sr = np.nonzero(y[rows[0]] > 0.5)[0][-1] + 1               # first ink row: right stem only at its right end
    ink = np.nonzero(y[b] > 0.02)[0]
    soft = ink[y[b][ink] < 0.999]
    if not len(soft):
        return None
    A = sr - soft[0]
    if A <= W or b - A + 1 < 0:
        return None
    arc = y[b - A + 1:b + 1, soft[0]:sr][::-1, ::-1].copy()
    if arc[:W, -1].min() < 0.5 or arc[W:, -1].max() > 0.5:     # run side = exactly W ink rows
        return None
    return arc


def rect_cross(M, r, code):
    N, W = M.N, M.W
    a = M.g[code]
    kind = RECT_CROSS[code]
    out = r.copy()
    rows = np.nonzero(r.max(axis=1) > 0.5)[0]
    if len(rows) == 0:
        return None
    if kind == 'pound':
        cnt = (r > 0.5).sum(axis=1)
        yb = rows[-1]
        while yb > rows[0] and cnt[yb] >= cnt[rows[-1]] - 1:
            yb -= 1                                            # yb = stem row directly above the bottom bar
        if cnt[yb] > W + 1:
            return None
        ys = yb
        while ys > rows[0] and cnt[ys] <= W + 1:
            ys -= 1                                            # ys = bottom row of the middle crossbar
        y0 = ys
        while y0 - 1 > rows[0] and cnt[y0 - 1] > W + 1 and cnt[y0 - 1] >= cnt[ys] - 1:
            y0 -= 1                                            # y0 = top row of the middle crossbar
        sr = np.nonzero(r[yb] > 0.5)[0][-1] + 1
        out[max(0, y0 - N):y0, sr:] = 0
        return out
    # original: crossbar row centre from the leftmost ink column (left arm only), stem edges from rows below it
    col = np.nonzero(a.max(axis=0) > 0.3)[0][0]
    ca = a[:, col]
    xr = [y for y in range(a.shape[0]) if a[y, col] > 0.5]
    if not xr:
        return None
    ry = np.arange(max(0, xr[0] - 1), min(a.shape[0], xr[-1] + 2))   # arm rows +- 1 (antialias neighbours)
    cy = (ca[ry] * ry).sum() / ca[ry].sum() + 0.5
    y0 = int(math.floor(N * cy - W / 2 + 0.5))
    y1 = y0 + W                                                # crossbar rows y0..y1-1
    st = [y for y in range(max(xr) + 1, a.shape[0]) if a[y].max() > 0.5 and a[y, col] < 0.3]
    if not st:
        return None
    xl, _ = _cov_edges(a[xr].mean(axis=0))
    stl, str_ = _cov_edges(a[st].mean(axis=0))
    el = int(math.floor(N * (stl - xl) + 0.5))
    if kind == 'f':
        sc = np.nonzero(r[rows[-1]] > 0.5)[0]                  # bottom row = pure stem
        sl, sr = sc[0], sc[-1] + 1
        if sr - sl != W:
            return None
        if rows[0] >= y0:
            return None
        xe = np.nonzero(r[y0:y1].max(axis=0) > 0.5)[0][-1] + 1  # crossbar right end as rendered
        arc = y_tail_arc(M)
        if arc is None:
            return None
        A, T = arc.shape[0], rows[0]
        # top reach of the original: rows above the crossbar arm
        top = [y for y in range(xr[0]) if a[y].max() > 0.5]
        if not top:
            return None
        _, he = _cov_edges(a[top].mean(axis=0))
        he = int(math.floor(N * he + 0.5))
        if T + A > y0 or sl + A > he or he > r.shape[1]:
            return None
        out[:y0] = 0                                           # everything above the crossbar is rebuilt
        out[T:y0, sl:sr] = 1                                   # stem runs straight up into the arc
        out[T:T + A, sl:sl + A] = arc                          # = y tail arc, rotated 180 deg
        out[T:T + W, sl + A:he] = 1                            # short run, rectangular end
        out[y1:, :sl] = 0                                      # chamfer residue below the crossbar
        out[y1:, sr:] = 0
        out[y0:y1] = 0
        out[y0:y1, max(0, sl - el):max(F_BAR_FLUSH and he or xe, sr)] = 1
        out[y1:rows[-1] + 1, sl:sr] = 1
        return out
    # dagger
    cx = N * (stl + str_) / 2
    sl = int(math.floor(cx - W / 2 + 0.5))
    sr = sl + W
    el = int(math.floor(cx - N * xl - W / 2 + 0.5))           # arm = original half crossbar minus half stem
    out[:] = 0
    out[rows[0]:rows[-1] + 1, sl:sr] = 1
    out[y0:y1, max(0, sl - el):min(r.shape[1], sr + el)] = 1
    return out


# x-height parts shared between lower-case letters: target -> (source char, transform)
SHARED_X = {ord('h'): ('n', 'same'), ord('u'): ('n', 'rot180'), ord('d'): ('b', 'mirror'),
            ord('p'): ('b', 'vflip'), ord('q'): ('b', 'rot180')}


def shared_xheight(M, r, src, how):
    """rows from the x-height to the baseline are taken from the source glyph (pixel-identical part),
    transformed, aligned on the left ink edge of the target's own x-height part."""
    N = M.N
    n = render(M, ord('n'))
    rows = np.nonzero(n.max(axis=1) > 0.5)[0]
    y0, y1 = rows[0], rows[-1] + 1
    full = render(M, ord(src))
    if src == 'b':
        # whole glyph: d = b mirrored, p = b flipped about the middle of the x-height part, q = b rotated
        H = full.shape[0]
        if how in ('vflip', 'rot180'):
            k = y0 + y1 - 1
            fl = np.zeros_like(full)
            for y in range(H):
                if 0 <= k - y < H:
                    fl[y] = full[k - y]
            full = fl
        if how in ('mirror', 'rot180'):
            full = full[:, ::-1]
        y0, y1 = 0, H
        S_ = full
    else:
        S_ = full[y0:y1]
        if how == 'mirror':
            S_ = S_[:, ::-1]
        elif how == 'rot180':
            S_ = S_[::-1, ::-1]
    cs = np.nonzero(S_.max(axis=0) > 0.5)[0]
    ct = np.nonzero(r[y0:y1].max(axis=0) > 0.5)[0]
    if len(cs) == 0 or len(ct) == 0:
        return r
    dx = int(ct[0] - cs[0])
    out = r.copy()
    out[y0:y1] = 0
    for x in range(r.shape[1]):
        xs = x - dx
        if 0 <= xs < S_.shape[1]:
            out[y0:y1, x] = S_[:, xs]
    return out


# lowercase t: the contour construction chamfers the corners between crossbar and stem (45 deg fill on the
# left, small fill below the crossbar on the right). Upper part rebuilt from two rectangles; the foot is kept.
RECT_T = (ord('t'),)


def rect_t(M, r, code):
    """stem (columns / top row of the finished render) + crossbar (its rows of the finished render, length per
    side from the original coverage: edge = col + coverage, extension = edge distance to the stem edge, x N,
    rounded half up). Everything above the last pure-stem row (scanning up from the foot) is replaced."""
    N = M.N
    a = M.g[code]
    out = r.copy()
    rows = np.nonzero(r.max(axis=1) > 0.5)[0]
    if len(rows) == 0:
        return None
    sc = np.nonzero(r[rows[0]] > 0.5)[0]                      # top row = pure stem
    sl, sr = sc[0], sc[-1] + 1
    cnt = (r > 0.5).sum(axis=1)
    wide = [y for y in rows if cnt[y] >= (sr - sl) + 2 * N]
    if not wide:
        return None
    y0 = y1 = wide[0]
    while y1 + 1 < r.shape[0] and cnt[y1 + 1] >= (sr - sl) + 2 * N:
        y1 += 1
    yp = None                                                 # last pure stem row above the foot
    for y in range(rows[-1], y1, -1):
        ink = np.nonzero(r[y] > 0.02)[0]
        if len(ink) and ink[0] >= sl and ink[-1] < sr:
            yp = y
            break
    if yp is None:
        return None
    # original: crossbar rows = widest ink rows, stem rows = ink rows below them with the stem's ink count
    ac = (a > 0.3).sum(axis=1)
    xr = np.nonzero(ac == ac.max())[0]
    st = [y for y in range(xr[-1] + 1, a.shape[0]) if 0 < ac[y] < ac.max()]
    if not st:
        return None
    cx, cs = a[xr].mean(axis=0), a[st[:2]].mean(axis=0)
    def edges(c):
        k = np.nonzero(c > 0.3)[0]
        return k[0] + 1 - c[k[0]], k[-1] + c[k[-1]]
    (xl, xrr), (stl, str_) = edges(cx), edges(cs)
    el = int(math.floor(N * (stl - xl) + 0.5))
    er = int(math.floor(N * (xrr - str_) + 0.5))
    # user decision: the crossbar's right end is flush with the foot's right edge (at 2x it already is)
    foot = np.nonzero(r[yp + 1:rows[-1] + 1].max(axis=0) > 0.5)[0]
    if len(foot):
        er = int(foot[-1]) + 1 - sr
    out[:yp + 1] = 0
    out[rows[0]:yp + 1, sl:sr] = 1
    out[y0:y1 + 1, max(0, sl - el):min(r.shape[1], sr + er)] = 1
    return out


# mirror / rotation pairs: target -> (source, transform). work_v3: in font.gfx all of them come from pairs_parts /
# dots_parts (hooks above); this is the fallback for fontk (glyphs pairs_parts does not handle)
def _c(ch):
    return ch.encode('cp1252')[0]


DERIVED = {_c(')'): (_c('('), 'gespiegelt'), _c(']'): (_c('['), 'gespiegelt'), _c('}'): (_c('{'), 'gespiegelt'),
           _c('>'): (_c('<'), 'gespiegelt'), _c('\u00bb'): (_c('\u00ab'), 'gespiegelt'),
           _c('\u203a'): (_c('\u2039'), 'gespiegelt'), _c(chr(92)): (_c('/'), 'gespiegelt'),
           _c('\u00a1'): (_c('!'), 'gedreht'), _c('\u00bf'): (_c('?'), 'gedreht')}


SLASHED = {0xD8: ord('O'), 0xF8: ord('o')}


def slashed(M, code, src):
    """Oslash / oslash = the finished O / o (pixel-identical, left ink edge as in the original) + one straight
    45 deg stroke of width W through the centre of its ink box, rising to the right, cut flat at the body's top
    and bottom edge (the contour path turned the crossing into a blob)."""
    import upscale4 as U4
    N = M.N
    a = M.g[code]
    h, w = a.shape[0] * N, a.shape[1] * N
    base = render(M, src)
    cb = np.nonzero(M.g[src].max(axis=0) > 0.3)[0]
    ca = np.nonzero(a.max(axis=0) > 0.3)[0]
    dx = int(round((ca[0] - cb[0]) * N)) if len(ca) and len(cb) else 0
    out = np.zeros((h, w))
    for x in range(w):
        xs = x - dx
        if 0 <= xs < base.shape[1]:
            out[:, x] = base[:h, xs]
    rows = np.nonzero(out.max(axis=1) > 0.5)[0]
    cols = np.nonzero(out.max(axis=0) > 0.5)[0]
    y0, y1 = rows[0] / N, (rows[-1] + 1) / N
    cy, cx = (y0 + y1) / 2, (cols[0] + cols[-1] + 1) / 2 / N
    e = (y1 - y0) / 2 + 2 * M.W / N
    st = U4.union(U4.stroke_raster(M, a.shape, [(cy + e, cx - e), (cy - e, cx + e)], clip=(y0, y1)))
    return np.maximum(out, st[:h, :w])


def derive_ok(M, code, src):
    """only a real partner: ink boxes of the originals of the same size (+-1 px); fontk '}' is another symbol."""
    def box(a):
        rr = np.nonzero(a.max(axis=1) > 0.3)[0]
        cc = np.nonzero(a.max(axis=0) > 0.3)[0]
        return (rr[-1] - rr[0], cc[-1] - cc[0]) if len(rr) and len(cc) else None
    bt, bs = box(M.g[code]), box(M.g[src])
    return bt is not None and bs is not None and abs(bt[0] - bs[0]) <= 1 and abs(bt[1] - bs[1]) <= 1


def derive(M, code, src, how):
    """pixel-identical mirrored / rotated copy of the source glyph, placed so that its ink box sits where the
    original target glyph has its ink (left edge and top edge, rounded to whole output pixels)."""
    N = M.N
    S_ = render(M, src)
    S_ = S_[:, ::-1] if how == 'gespiegelt' else S_[::-1, ::-1]
    a = M.g[code]
    h, w = a.shape[0] * N, a.shape[1] * N
    rows_o = np.nonzero(a.max(axis=1) > 0.3)[0]
    cols_o = np.nonzero(a.max(axis=0) > 0.3)[0]
    rs = np.nonzero(S_.max(axis=1) > 0.5)[0]
    cs = np.nonzero(S_.max(axis=0) > 0.5)[0]
    if len(rows_o) == 0 or len(rs) == 0:
        return np.zeros((h, w))
    if how == 'gespiegelt':
        dy = 0                                            # same height / baseline as the source
        # mirror the source's placement: right bearing of the source = left bearing of the target
        dx = int(round(cols_o[0] * N)) - cs[0]          # left ink edge as in the original target
    else:
        dy = int(round(rows_o[0] * N)) - rs[0]
        dx = int(round(cols_o[0] * N)) - cs[0]
    out = np.zeros((h, w))
    for y in range(h):
        ys = y - dy
        if not 0 <= ys < S_.shape[0]:
            continue
        xs = np.arange(w) - dx
        ok = (xs >= 0) & (xs < S_.shape[1])
        out[y, ok] = S_[ys, xs[ok]]
    return out
