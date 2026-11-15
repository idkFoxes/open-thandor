"""Draws docs/architecture/network/network.svg, the block diagram of the network area.

Run from the repository root: python tools/docs/architecture/gen_network.py
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import archsvg as A  # noqa: E402

W, H = 1280, 650
d = A.Diagram(W, H, 'network — modules',
              'src/network/: the session and transfer protocol on top of a pluggable UDP backend. '
              'Arrows are real calls or data flows. Click a box for its source folder or area page.')

d.band(1220, 74, 'OTHER AREAS', anchor='end')
d.ext(60, 80, 300, 44, 'ui (frontend pages · ui/core receive ring)', href='../ui/README.md', color='ui')
d.ext(400, 80, 230, 44, 'gameplay (session tick · scenario load)', href='../gameplay/README.md', color='gameplay')
d.ext(670, 80, 150, 44, 'world (terrain edit)', href='../world/README.md', color='world')
d.ext(860, 80, 150, 44, 'assets (PCK codec)', href='../assets/README.md', color='assets')
d.ext(1050, 80, 170, 44, 'core (random seeds)', href='../core/README.md', color='core')

d.band(1220, 204, 'NETWORK', anchor='end')
y = d.box(60, 212, 1160, 126, 'protocol',
          ['Lobby (discovery, session list, join), session-start snapshot exchange, in-game lockstep command batches,',
           'command queues and dispatch tables, chunked mailbox transfer (maps, catalog), packet cipher, receive timer.'],
          ['lobby.cpp', 'frontend_session.cpp', 'command_exchange.cpp', 'lockstep.cpp', 'commands.cpp',
           'mailbox.cpp', 'cipher.cpp', 'scenario_transfer.cpp', 'packet_bytes.h', 'types.h'],
          href='../../../src/network/protocol/', meta='8 files · 4.0k lines', color='network')
assert y <= 212 + 126, y
y = d.box(60, 420, 560, 110, 'backend',
          ['WinSock 1.1 (wsock32.dll) bound at start-up; eight backend slots:',
           'select, cleanup, open/bind, close, receive, send, parse, format address.'],
          ['runtime.cpp', 'fallback_udp.cpp', 'types.h'],
          href='../../../src/network/backend/', meta='2 files', color='network')
assert y <= 420 + 110, y

d.ext(760, 450, 260, 44, 'platform (bootstrap · Game_Run)', href='../platform/README.md', color='platform')
d.ext(200, 590, 280, 40, 'LAN peers')

# ui <-> protocol
d.line([(110, 124), (110, 212)], 'lobby ticks · join', 117, 150)
d.line([(270, 212), (270, 124)], 'packet ring', 277, 186)
d.text(277, 200, 'lobby command handlers', cls='l')
# gameplay <-> protocol
d.line([(440, 124), (440, 212)], 'lockstep step', 447, 150)
d.text(447, 164, 'map send buffer', cls='l')
d.line([(580, 212), (580, 124)], 'game commands', 587, 186)
# protocol -> world, assets, core
d.line([(745, 212), (745, 124)], 'terrain edits', 752, 186)
d.line([(935, 212), (935, 124)], 'decode maps', 942, 186)
d.line([(1135, 212), (1135, 124)], 'seed sync', 1142, 186)
# protocol -> backend
d.line([(340, 338), (340, 420)], 'send · receive datagram', 347, 372)
d.text(347, 386, 'close on host loss', cls='l')
# ui -> backend (left gutter)
d.line([(60, 102), (34, 102), (34, 470), (60, 470)], 'open port 929 · addresses', 28, 300,
       anchor='middle', rotate=True)
# platform -> backend
d.line([(760, 472), (620, 472)], 'Network_Init', 690, 464, anchor='middle')
d.text(690, 490, 'close at exit', cls='l', anchor='middle')
# backend <-> LAN peers
d.line([(340, 530), (340, 590)], 'UDP', 347, 565, both=True)

d.save('docs/architecture/network/network.svg')

if __name__ == '__main__' and '--png' in sys.argv:
    A.render_png('docs/architecture/network/network.svg', sys.argv[sys.argv.index('--png') + 1], W, H)
