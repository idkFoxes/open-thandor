"""percent (0x25) of font.gfx = the per mille without its third zero: same small zero, same zero positions, same slash
(mille_parts). The original % and the first two zeros of the original per mille sit on the same pixels (0 top left,
0 bottom right, rows overlapping by one original row), so the layout comes from there.
fontk.gfx keeps its % (cell 8 x 9): two 12 px zeros fill the whole 24 px cell width, no room for the slash."""
import mille_parts
from mille_parts import LAYOUT, small_zero

CODE = 0x25
CELL = (15, 12)                                           # original cell of the font.gfx %


def render(M, code):
    N = M.N
    if code != CODE or N not in LAYOUT or M.g[code].shape != CELL or M.g[code].max() <= 0:
        return None
    L = LAYOUT[N]
    zh, zw = small_zero(N).shape
    r = mille_parts.compose(M, code, L, ((L['xs'][0], L['y0']), (L['xs'][1], L['y1'] - zh)))
    return r, '0x25 = 2 x fontk-0 (%dx%d) + 22.5-Grad-Strich %d px (percent_parts)' % (zw, zh, N)
