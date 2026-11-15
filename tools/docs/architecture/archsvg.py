"""Shared drawing helpers for the block diagrams in docs/architecture/.

Every diagram script (gen_<page>.py next to this file) builds one SVG with
these helpers, so all pages share colours, fonts, box and arrow style.

    import archsvg as A
    d = A.Diagram(1280, 760, 'world', 'Entities, camera, terrain ...')
    d.box(60, 92, 400, 110, 'runtime', ['Entity registry ...'], ['core.cpp'],
          href='../../../src/world/runtime/', meta='2 files', color='world')
    d.ext(600, 92, 200, 40, 'gameplay', href='../gameplay/README.md')
    d.line([(460, 140), (600, 140)], 'tick', 470, 132)
    d.save('docs/architecture/world/world.svg')
    A.render_png('docs/architecture/world/world.svg', 'scratch/world.png', 1280, 760)

Run the scripts from the repository root: python tools/docs/architecture/gen_<page>.py
"""
import os
import subprocess
import tempfile
import time

# Area colours: (fill, stroke). Module boxes on an area page use their area colour;
# boxes for other areas use that area's colour with a dashed outline (ext()).
COLORS = {
    'ui': ('#e8f0fe', '#4a6fd0'), 'gameplay': ('#fdeee0', '#d07a2a'),
    'world': ('#fff6d6', '#c9a227'), 'graphics': ('#e6f6ec', '#2f9a57'),
    'audio': ('#e6f5f5', '#2a9a9a'), 'movie': ('#f3ecfb', '#8a5ac8'),
    'network': ('#fde8ef', '#c84a76'), 'assets': ('#f0f0f0', '#7a7a7a'),
    'core': ('#f0f0f0', '#7a7a7a'), 'platform': ('#e9edf2', '#4b5d73'),
    'outside': ('#fafafa', '#aaaaaa'),
}

STYLE = ('.t{font-size:17px;font-weight:700;fill:#1d1d1f}.d{font-size:12px;fill:#333}'
         '.m{font-size:11px;fill:#222}.f{font-size:11px;fill:#666}'
         '.l{font-size:11.5px;fill:#333;font-style:italic}'
         '.h{font-size:11px;font-weight:700;fill:#888;letter-spacing:1.5px}')


def esc(s):
    return str(s).replace('&', '&amp;').replace('<', '&lt;').replace('>', '&gt;')


class Diagram:
    def __init__(self, width, height, title, subtitle):
        self.w, self.h = width, height
        self.o = []
        a = self.o.append
        a(f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {width} {height}" '
          f'width="{width}" height="{height}" font-family="Segoe UI, Helvetica, Arial, sans-serif">')
        a('<defs><marker id="ar" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="7" '
          'markerHeight="7" orient="auto-start-reverse"><path d="M0,0 L10,5 L0,10 z" fill="#555"/>'
          f'</marker><style>{STYLE}</style></defs>')
        a(f'<rect width="{width}" height="{height}" fill="#fff"/>')
        a(f'<text x="20" y="32" style="font-size:22px;font-weight:700;fill:#111">{esc(title)}</text>')
        a(f'<text x="20" y="52" class="d">{esc(subtitle)}</text>')

    def band(self, x, y, text, anchor='start'):
        """Small grey capital label for a row or column."""
        self.o.append(f'<text x="{x}" y="{y}" class="h" text-anchor="{anchor}">{esc(text)}</text>')

    def box(self, x, y, w, h, title, desc, chips, href=None, meta='', color='core'):
        """Solid box: title, description lines, chips (modules or files)."""
        f, s = COLORS[color]
        a = self.o.append
        if href:
            a(f'<a href="{esc(href)}"><g>')
        a(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="10" fill="{f}" stroke="{s}" stroke-width="2"/>')
        a(f'<text x="{x+14}" y="{y+25}" class="t">{esc(title)}</text>')
        if meta:
            a(f'<text x="{x+w-12}" y="{y+24}" class="f" text-anchor="end">{esc(meta)}</text>')
        yy = y + 44
        for line in desc:
            a(f'<text x="{x+14}" y="{yy}" class="d">{esc(line)}</text>')
            yy += 15
        yy += 9
        cx = x + 14
        for m in chips:
            tw = len(m) * 6.3 + 14
            if cx + tw > x + w - 10:
                cx = x + 14
                yy += 24
            a(f'<rect x="{cx:.0f}" y="{yy-13}" width="{tw:.0f}" height="19" rx="9" fill="#fff" '
              f'stroke="{s}" stroke-opacity="0.6"/>')
            a(f'<text x="{cx+7:.0f}" y="{yy+1}" class="m">{esc(m)}</text>')
            cx += tw + 6
        if href:
            a('</g></a>')
        return yy + 10   # first free y below the chips, to check the box is tall enough

    def ext(self, x, y, w, h, text, href=None, color='outside'):
        """Dashed box for something outside this page: another area, a device, files."""
        f, s = COLORS[color]
        a = self.o.append
        if href:
            a(f'<a href="{esc(href)}"><g>')
        a(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="8" fill="{f}" stroke="{s}" '
          f'stroke-width="1.5" stroke-dasharray="5 4"/>')
        a(f'<text x="{x+w/2}" y="{y+h/2+4}" class="d" text-anchor="middle">{esc(text)}</text>')
        if href:
            a('</g></a>')

    def line(self, pts, label=None, lx=0, ly=0, both=False, anchor='start', rotate=False):
        """Arrow along a polyline. both=True puts heads on both ends."""
        d = 'M' + ' L'.join(f'{x},{y}' for x, y in pts)
        st = ' marker-start="url(#ar)"' if both else ''
        self.o.append(f'<path d="{d}" stroke="#555" stroke-width="1.8" fill="none" marker-end="url(#ar)"{st}/>')
        if label:
            tr = f' transform="rotate(-90 {lx} {ly})"' if rotate else ''
            self.o.append(f'<text x="{lx}" y="{ly}" class="l" text-anchor="{anchor}"{tr}>{esc(label)}</text>')

    def text(self, x, y, s, cls='d', anchor='start'):
        self.o.append(f'<text x="{x}" y="{y}" class="{cls}" text-anchor="{anchor}">{esc(s)}</text>')

    def save(self, path):
        os.makedirs(os.path.dirname(path) or '.', exist_ok=True)
        with open(path, 'w', encoding='utf-8', newline='\n') as f:
            f.write('\n'.join(self.o + ['</svg>']) + '\n')


EDGE = r'C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe'


def render_png(svg_path, png_path, width, height):
    """Render an SVG to PNG with headless Edge, for checking a diagram by eye."""
    svg = os.path.abspath(svg_path).replace('\\', '/')
    png = os.path.abspath(png_path)
    os.makedirs(os.path.dirname(png), exist_ok=True)
    # Edge does not write to 8.3 short paths (C:\Users\NAME~1\...), so expand the folder.
    png = os.path.join(os.path.realpath(os.path.dirname(png)), os.path.basename(png))
    svg = os.path.realpath(svg).replace('\\', '/')
    prof = tempfile.mkdtemp(prefix='archsvg_edge_')
    subprocess.run([EDGE, '--headless=new', '--disable-gpu', '--hide-scrollbars',
                    f'--user-data-dir={prof}', f'--window-size={width},{height}',
                    f'--screenshot={png}', 'file:///' + svg.replace(' ', '%20')],
                   check=False, timeout=120, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    # Edge can return before the screenshot is on disk; give it a moment.
    for _ in range(40):
        if os.path.exists(png):
            break
        time.sleep(0.5)
    return png
