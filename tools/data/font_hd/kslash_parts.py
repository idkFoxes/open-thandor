"""fontk.gfx '/' (0x2F): one continuous straight stroke, built like the font.gfx '/' (upscale4.g_slash), which the
stroke path only reaches for font.gfx. 22.5 deg from vertical, uniform width M.W (3x 3 px, 2x 2 px), horizontal cut ends.
The fontk original has nearly the same pixels as the font.gfx one (same 15-row cell, ink rows 3..13), so the output
rows and the centre (2.7 source px) are those of the font.gfx '/'. '\' follows via pairs_parts.mirror_partner."""
import upscale4 as U4

CODE = 0x2F
ROWS = {3: (10, 40), 2: (6, 27)}      # output rows [y0, y1) of the font.gfx '/'
XM = 2.7                              # x of the stroke centre at mid height (source px), as g_slash


def applies(M, code):
    return code == CODE and M.g[ord('A')].shape[0] != 15 and M.g[code].shape[0] == 15 and M.N in ROWS


def render(M, code):
    if not applies(M, code):
        return None
    N = M.N
    y0, y1 = ROWS[N][0] / N, ROWS[N][1] / N
    ym = (y0 + y1) / 2
    e = 2.0                                                # run past the clip lines: the ends are cut flat
    ya, yb = y0 - e, y1 + e
    pts = [(ya, XM + (ym - ya) * U4.T225), (yb, XM - (yb - ym) * U4.T225)]
    return U4.lo(U4.stroke_raster(M, M.g[code].shape, pts, clip=(y0, y1)))
