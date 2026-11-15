"""Draws docs/architecture/core/core.svg, the block diagram of the core area.

Run from the repository root: python tools/docs/architecture/gen_core.py
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import archsvg as A  # noqa: E402

W, H = 1280, 820
d = A.Diagram(W, H, 'core — settings, errors, memory, maths, strings',
              'src/core/: services every area uses. Arrows show the main calls; memory, text and the base headers '
              'are used nearly everywhere and have no arrows. Click a box for its sources.')

SRC = '../../../src/core/'


def mod(x, y, w, h, name, desc, chips, meta='', href=None):
    free = d.box(x, y, w, h, name, desc, chips, href=href or SRC + name + '/', meta=meta, color='core')
    assert free <= y + h, (name, free, y + h)


# --- top: files and the areas that start core up
d.ext(60, 80, 300, 40, 'thandor.ini · thandor.dat (read only)')
d.ext(400, 80, 440, 40, 'platform — bootstrap starts core up; file system',
      href='../platform/README.md', color='platform')
d.ext(880, 80, 360, 40, 'assets — text pages, last error path',
      href='../assets/README.md', color='assets')

# --- settings and error
mod(60, 160, 560, 200, 'settings',
    ['256-byte settings image: the 200 bytes of the original thandor.dat',
     'plus open-thandor dwords; PersistentSettings_Read returns the caller\'s',
     'default for a dword not present. Load parses thandor.ini (current dir,',
     'then exe dir), else migrates thandor.dat. Key table with a range per',
     'key; other modules register own sections ([keys]); unknown keys and',
     'sections are kept. Flush writes the ini. original_mode.cpp: the',
     '"Original Thandor" switch; OtFeature_Active gates the additions.'],
    ['persistent.cpp', 'original_mode.cpp', 'settings/persistent.h', 'settings/original_mode.h', 'settings/types.h'],
    meta='2 files')
mod(660, 160, 580, 200, 'error',
    ['ErrorSystem_Init points both handlers at FatalError_Exit and loads',
     'texte\\error.str as text page 0. FatalError_Exit(value, failed) passes',
     'the value through, or FatalError_ShowAndExit builds the message (error',
     'code or rich-text stream, last path, three details), shuts the game',
     'down and exits. Entered at most once.'],
    ['runtime.cpp', 'error/runtime.h', 'error/types.h'], meta='1 file')

d.line([(210, 120), (210, 160)], 'read · write (.tmp, then replace)', 217, 143, both=True)
d.line([(500, 120), (500, 160)], 'PersistentSettings_Load · file I/O', 507, 143, both=True)
d.line([(760, 120), (760, 160)], 'ErrorSystem_Init', 767, 143)
d.line([(1060, 160), (1060, 120)], 'error.str · TextResource_Resolve', 1067, 143)

# --- math, memory, text
RY, RH = 410, 160
mod(60, RY, 420, RH, 'math',
    ['Q12 / Q28 fixed point (fixed_point.h); sine table built at',
     'start-up, atan2, vectors, 3x4 transforms, triangle solver,',
     'cubic splines (float). Random: two LCG streams, the',
     'session draws from the secondary one.'],
    ['fixed_trig.cpp', 'fixed_vector.cpp', 'fixed_transform.cpp', 'geometry.cpp', 'spline.cpp', 'random.cpp'],
    meta='6 files')
mod(520, RY, 340, RH, 'memory',
    ['96 MiB arena from a private heap (ArenaHeap_Init),',
     'first fit in 32-byte steps; g_MemoryApi alloc /',
     'free is used by every area. Runtime_Shutdown',
     'flushes the settings and shuts the subsystems down.'],
    ['allocator.cpp', 'synchronization.cpp'], meta='2 files')
mod(900, RY, 340, RH, 'text',
    ['UTF-16 paths: WidePath_SetExtensionCode,',
     'SplitParentAndLeaf, CombineDirectoryAndLeaf;',
     'number formatting, case-insensitive compare,',
     'narrow to UTF-16 copies.'],
    ['path.cpp', 'string.cpp'], meta='2 files')

# --- consumers
CY = 610
for x, w, key in ((60, 95, 'gameplay'), (165, 95, 'world'), (270, 100, 'graphics'), (380, 100, 'network')):
    d.ext(x, CY, w, 40, key, href=f'../{key}/README.md', color=key)
    d.line([(x + w / 2, RY + RH), (x + w / 2, CY)])
d.text(66, CY + 60, 'math users: fixed-point maths and random draws (audio, movie, platform, ui too)', 'l')
d.ext(492, CY, 400, 40, 'ui — settings menus, Original Thandor switch, FatalError_*', href='../ui/README.md', color='ui')
d.line([(500, 360), (500, CY)], 'PersistentSettings_Read / Write', 494, 485, anchor='middle', rotate=True)
d.line([(880, 360), (880, CY)], 'FatalError_*', 874, 485, anchor='middle', rotate=True)

# --- base headers
mod(60, 690, 1180, 106, 'core (base headers)',
    ['Header-only basics: Ptr32<T> 32-bit pointer fields of the original layouts, flag enums (THANDOR_FLAG_ENUM), '
     'THANDOR_SLOT function slots, byte views, MMX colour lanes, x86 helpers.',
     'layout_checks.cpp: compile-time static_asserts that the type-header structs keep their 32-bit sizes and '
     'pointer offsets.'],
    ['ptr32.h', 'flags.h', 'slot.h', 'contracts.h', 'bytes.h', 'color_lanes.h', 'x86_emulation.h', 'types.h',
     'layout_checks.cpp'], href='../../../include/thandor/core/')

d.save('docs/architecture/core/core.svg')

if __name__ == '__main__' and '--png' in sys.argv:
    A.render_png('docs/architecture/core/core.svg', sys.argv[sys.argv.index('--png') + 1], W, H)
