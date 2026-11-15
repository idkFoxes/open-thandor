"""fontk.gfx TM 0x99 and f-hook 0x83 (work_ktm), 3x and 2x only: the fontk cells have the size of the font.gfx cells
and hold the same artwork (unlike its letters: fontk T is 9 rows, font.gfx T 15), so the glyph is the finished
font.gfx glyph at the same scale, pixel-identical (route of yen_from_font). font.gfx itself is not touched."""
import os

CODES = (0x99, 0x83)
SRC_W = {2: 3, 3: 4}                     # font.gfx stroke (dumpcur.WDEF)
_src = {}


def render(M, code):
    if code not in CODES or M.N not in SRC_W or M.g[ord('A')].shape[0] == 15:    # 15 rows = font.gfx
        return None
    import upscale as U, upscale3 as S
    if M.N not in _src:
        fn = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'font.gfx')
        _src[M.N] = S.FontModel(U.load(fn), M.N, W=SRC_W[M.N], r_big_src=3.17)
    src = _src[M.N]
    if src.g[code].shape != M.g[code].shape:
        return None
    return S.render(src, code)
