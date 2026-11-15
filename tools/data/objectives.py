"""Lists the victory and defeat conditions and the briefing texts of the stock levels (objective overlay, plan
docs/plans/objective_overlay.md, package O1).

usage:
  python tools/data/objectives.py [GAME_DIR] [--out FILE] [--summary]

  GAME_DIR   game directory with the PCK packages (default: the soak copy, DEFAULT_GAME_DIR); read only
  --out FILE writes the full per mission + faction dump (markdown, docs/plans/objective_overlay_levels.md)
  --summary  prints the summary tables (condition kinds, coverage of the generated fallback text, plan 3b)

Levels: the campaigns of level\\campagne.dat (their .cgn records in campaign order) and the single / multiplayer
maps of level\\level.dat. Each level is parsed with tools/data/lev.py. Per playable faction (1..assignable faction
count that has an end trigger): the end triggers with movieVariant 1 (win) and 0 (loss), their condition with
BOOLEAN_POSTFIX_EXPRESSION trees resolved (InGameScheduledCondition_EvaluatePostfixExpression: 0xFC end, 0xFD NOT,
0xFE AND, 0xFF OR, other bytes push the satisfied bit of that condition), the operands as the level script reads
them (InGameScheduledCondition_Holds, src/gameplay/session/level_script.cpp).

Texts (PATCH00.PCK is mounted before ENGINE.PCK, so its texte\\*.str win, as in CoreAssets_MountPackages):
  level title    TEXT_ID_LEVEL_TITLE_BASE 0x2230 + title index: compact id, page 0x22 texte\\level.str, string 0x30 + i
  mission help   TEXT_ID_MISSION_HELP_BASE 0x230017 + title index * TEXT_ID_LEVEL_DESCRIPTION_STRIDE 0x10 + faction
                 (InGameMissionHelpPage_Toggle): extended id, page 0x23 = texte\\inhalt.str (GAME_TEXT_PAGE_INHALT),
                 string 0x17 + 0x10 * i + faction. The frontend briefing (TEXT_ID_LEVEL_BRIEFING_BASE) is the same id.
  German = locale block with country code 49, English = 44. Rich-text commands (code units with bit 15) are
  dropped, RICHTEXT_OP_LINE_BREAK becomes a line break (include/thandor/assets/text/richtext.h).
Army asset names come from the enumerator comments of PckArmyAssetIdCatalog (include/thandor/core/types.h).

Coverage classes (how well plan table 3b can describe a faction's win condition without a hand-written text):
  good     one win trigger whose condition is one plain kind, or an AND of "faction g has no army" /
           "faction g inactive or allied with me" leaves (= "Alle Gegner vernichten" when they cover every other
           active faction, else "Fraktionen 2, 3 und 4 ausschalten")
  partial  a flat AND / OR of plain kinds ("Teilziele mit und/oder"), a NOT of one plain kind, or several win
           triggers (first + "(+n weitere)")
  multi    anything else: only "Mehrere Ziele (F1)"
  none     no win trigger for this faction
"""
import argparse
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import cgn  # noqa: E402
import lev  # noqa: E402
import mdl2obj  # noqa: E402  (Packages)

DEFAULT_GAME_DIR = r"C:\Users\Gerrit Bluemel\Documents\Projekte\ot-soak"
REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
PACKAGES = ["ENGINE.PCK", "DATEN.PCK", "LEVEL.PCK", "PATCH00.PCK"]
BS = "\\"

COUNTRY_GERMAN = 49
COUNTRY_ENGLISH = 44
LEVEL_TITLE_STRING_BASE = 0x30  # TEXT_ID_LEVEL_TITLE_BASE 0x2230 & 0xFF
MISSION_HELP_STRING_BASE = 0x17  # TEXT_ID_MISSION_HELP_BASE 0x230017 & 0xFFFF
LEVEL_DESCRIPTION_STRIDE = 0x10

TOKEN_END, TOKEN_NOT, TOKEN_AND, TOKEN_OR = 0xFC, 0xFD, 0xFE, 0xFF
KIND_POSTFIX = 26
SIMULATION_TICKS_PER_SECOND = 20  # the level script runs every 20 ticks and counts COUNTDOWN_ELAPSED down by one

# rich-text records (include/thandor/assets/text/richtext.h): opcode -> record length in code units
RICHTEXT_RECORD_UNITS = {0x06: 9, 0x14: 3, 0x15: 3, 0x16: 3, 0x18: 5, 0x19: 5, 0x1A: 5}
RICHTEXT_FIXED_SPACE, RICHTEXT_SOFT_HYPHEN, RICHTEXT_LINE_BREAK = 0x10, 0x11, 0x12

PLAN_KIND_NAMES = {
    2: 'FACTION_HAS_NO_ARMY', 4: 'FACTION_HAS_NO_COMMAND_GROUP_A_ARMY', 6: 'FACTION_HAS_NO_ARMY_OF_ASSET',
    8: 'FACTION_INACTIVE_OR_RELATION_AT_LEAST_8', 10: 'XENITE_AT_LEAST', 12: 'TRITIUM_AT_LEAST',
    14: 'TRITIUM_EXTRACTION_RATE_AT_LEAST', 16: 'ARMY_OF_ASSET_COUNT_AT_LEAST',
    18: 'FACTION_TERRAIN_OCCUPANCY_MASK_F9_PERCENT_AT_LEAST', 20: 'COUNTDOWN_ELAPSED',
    22: 'XENITE_STORAGE_LIMIT_AT_MOST_0FA0', 24: 'NO_ARMY_OF_CLASS_OUTSIDE_COMMAND_GROUP_A',
    26: 'BOOLEAN_POSTFIX_EXPRESSION',
}


# ---------------------------------------------------------------- texts

def decode_richtext(units):
    """Plain text of a rich-text stream (list of UTF-16 code units, without terminator)."""
    out = []
    i = 0
    while i < len(units):
        unit = units[i]
        if unit & 0x8000:
            op = unit & 0x1F
            if op == RICHTEXT_LINE_BREAK:
                out.append('\n')
            elif op == RICHTEXT_FIXED_SPACE:
                out.append(' ')
            i += RICHTEXT_RECORD_UNITS.get(op, 1)
            continue
        out.append(chr(unit))
        i += 1
    return ''.join(out)


def read_str(raw):
    """{country code: [plain strings]} of a 'str' asset (TextResourceAssetHeader: locale block count at +0xB0,
    blocks from +0x200: {blockSizeBytes, stringCount, countryCode, -} + block-relative string offsets)."""
    pages = {}
    offset = 0x200
    for _ in range(struct.unpack_from('<I', raw, 0xB0)[0]):
        size, count, country = struct.unpack_from('<III', raw, offset)
        strings = []
        for i in range(count):
            pos = offset + struct.unpack_from('<I', raw, offset + 16 + 4 * i)[0]
            units = []
            while pos + 2 <= len(raw):
                unit = struct.unpack_from('<H', raw, pos)[0]
                if unit == 0:
                    break
                units.append(unit)
                pos += 2
            strings.append(decode_richtext(units))
        pages.setdefault(country, strings)
        offset += size
    return pages


def text(page, country, index):
    strings = page.get(country, [])
    return strings[index].strip() if 0 <= index < len(strings) else None


_ASSET_NAMES = None


def asset_name(asset_id):
    """Readable name of an army asset id: the enumerator comment in include/thandor/core/types.h up to the colon
    (e.g. 300 -> "Hauptquartier"; after the colon when the model has no name), else the enumerator name."""
    global _ASSET_NAMES
    if _ASSET_NAMES is None:
        _ASSET_NAMES = {}
        path = os.path.join(REPO, 'include', 'thandor', 'core', 'types.h')
        if os.path.exists(path):
            for m in re.finditer(r'\b(ARM_\w+)=(\d+),\s*/\*\s*([^*]*?)\s*\*/', open(path, encoding='utf-8').read()):
                label, _, weapon = (part.strip() for part in m.group(3).partition(':'))
                if not label or label.startswith('('):
                    label = weapon or m.group(1)  # "(no name): Laserspule": the weapon names the building
                _ASSET_NAMES[int(m.group(2))] = label
    return _ASSET_NAMES.get(asset_id, lev.army_name(asset_id))


# ---------------------------------------------------------------- condition trees

def resolve(conditions, index, depth=0):
    """Expression tree of condition index: ('leaf', condition) | ('not', node) | ('and'|'or', [nodes]).
    NOT NOT x becomes x, nested ANDs / ORs are flattened."""
    condition = conditions[index]
    if condition['kind'] != KIND_POSTFIX:
        return ('leaf', condition)
    if depth > 16:
        return ('leaf', condition)  # cyclic reference; never in the stock data
    stack = []
    for token in condition['expression']:
        if token == TOKEN_END:
            break
        if token == TOKEN_NOT:
            node = stack.pop() if stack else ('leaf', None)
            stack.append(node[1] if node[0] == 'not' else ('not', node))
        elif token in (TOKEN_AND, TOKEN_OR):
            op = 'and' if token == TOKEN_AND else 'or'
            right = stack.pop() if stack else ('leaf', None)
            left = stack.pop() if stack else ('leaf', None)
            children = []
            for child in (left, right):
                children.extend(child[1] if child[0] == op else [child])
            stack.append((op, children))
        else:
            stack.append(resolve(conditions, token, depth + 1) if token < len(conditions) else ('leaf', None))
    return stack[-1] if stack else ('leaf', None)


def leaves(node):
    if node[0] == 'leaf':
        return [node[1]]
    if node[0] == 'not':
        return leaves(node[1])
    return [leaf for child in node[1] for leaf in leaves(child)]


def describe_leaf(c, local):
    """German words for one plain condition (local = the faction whose trigger it is)."""
    if c is None:
        return 'ungültige Bedingung'
    kind = c['kind']
    ops = c.get('operands', [0, 0, 0])

    def who(f):
        return 'Fraktion %d (eigene)' % f if f == local else 'Fraktion %d' % f
    if kind == 0:
        return 'leere Bedingung (nie erfüllt)'
    if kind == 2:
        return '%s hat keine Einheiten mehr' % who(ops[0])
    if kind == 4:
        return '%s hat keine Einheiten der Kommandogruppe A mehr' % who(ops[0])
    if kind == 6:
        return '%s hat kein(e) %s (ARM %d) mehr' % (who(ops[0]), asset_name(ops[2]), ops[2])
    if kind == 8:
        return 'Fraktion %d oder %d inaktiv, oder %d mit %d verbündet' % (ops[1], ops[0], ops[1], ops[0])
    if kind == 10:
        return '%s hat mindestens %d Xenit' % (who(ops[0]), ops[1] >> 4)
    if kind == 12:
        return '%s hat mindestens %d Tritium' % (who(ops[0]), ops[1] >> 4)
    if kind == 14:
        return 'Tritium-Förderrate von %s mindestens %s (Q4 %d pro Tick)' % (who(ops[0]), q4(ops[1]), ops[1])
    if kind == 16:
        return '%s hat mindestens %d x %s (ARM %d)' % (who(ops[0]), ops[1], asset_name(ops[2]), ops[2])
    if kind == 18:
        return 'Gebietsanteil (Belegungsmaske, Zellen-Offset %d) mindestens %d %%' % (ops[0], ops[1])
    if kind == 20:
        seconds = ops[1]
        return 'Countdown abgelaufen (%d s = %d:%02d)' % (seconds, seconds // 60, seconds % 60)
    if kind == 22:
        return 'Xenit-Lager von %s höchstens 250' % who(ops[0])
    if kind == 24:
        return 'keine Einheit der Laufzeitklasse %d außerhalb der Kommandogruppe A' % ops[0]
    return 'unbekannte Bedingungsart %d %s' % (kind, ops)


def q4(value):
    return ('%d' % (value >> 4)) if value % 16 == 0 else ('%.4g' % (value / 16.0))


def describe(node, local, top=True):
    if node[0] == 'leaf':
        words = describe_leaf(node[1], local)
        return words if top or ' oder ' not in words else '(%s)' % words
    if node[0] == 'not':
        return 'NICHT (%s)' % describe(node[1], local, False)
    joined = (' UND ' if node[0] == 'and' else ' ODER ').join(describe(child, local, False) for child in node[1])
    return joined if top else '(%s)' % joined


# ---------------------------------------------------------------- semantics and coverage

def opponents(node, local):
    """The other factions an AND (or a single leaf) of FACTION_HAS_NO_ARMY / FACTION_INACTIVE_OR_ALLIED leaves
    about other factions names, with whether an alliance also counts; None for any other shape."""
    parts = node[1] if node[0] == 'and' else [node]
    covered = set()
    allied = False
    for part in parts:
        if part[0] != 'leaf' or part[1] is None:
            return None
        c = part[1]
        if c['kind'] == 2 and c['operands'][0] != local:
            covered.add(c['operands'][0])
        elif c['kind'] == 8 and local in c['operands'][:2]:
            pair = c['operands'][:2]
            covered.add(pair[0] if pair[1] == local else pair[1])
            allied = True
        else:
            return None
    return covered, allied


def faction_list(factions):
    names = ['%d' % f for f in sorted(factions)]
    return names[0] if len(names) == 1 else ', '.join(names[:-1]) + ' und ' + names[-1]


def plain(node):
    return node[0] == 'leaf' and node[1] is not None and node[1]['kind'] not in (0, KIND_POSTFIX)


def classify_tree(node, local, active):
    """'good', 'partial' or 'multi' for one trigger's condition tree, and the generated line-1 summary."""
    found = opponents(node, local)
    if found is not None and (node[0] == 'and' or found[1]):
        covered, allied = found
        verb = 'ausschalten' if allied else 'vernichten'
        others = set(range(1, active + 1)) - {local}
        if others and others <= covered:
            # factions above activeFactionCount never have units: the stock scripts list up to 4 or 5 anyway
            return 'good', 'Alle Gegner %s' % verb
        return 'good', 'Fraktion%s %s %s' % ('en' if len(covered) > 1 else '', faction_list(covered), verb)
    if plain(node):
        return 'good', short_leaf(node[1], local)
    if node[0] == 'not' and plain(node[1]):
        c = node[1][1]
        if c['kind'] == 14 and c['operands'][0] != local and c['operands'][1] <= 16:
            return 'partial', 'Tritium-Förderung von Fraktion %d stoppen' % c['operands'][0]
        return 'partial', 'NICHT ' + short_leaf(c, local)
    if node[0] in ('and', 'or') and all(plain(child) for child in node[1]):
        word = ' und ' if node[0] == 'and' else ' oder '
        return 'partial', word.join(short_leaf(child[1], local) for child in node[1])
    return 'multi', 'Mehrere Ziele (F1)'


def short_leaf(c, local):
    """The 3b line-1 wording of one plain condition."""
    kind, ops = c['kind'], c.get('operands', [0, 0, 0])
    if kind == 2:
        return 'Fraktion %d vernichten' % ops[0] if ops[0] != local else 'eigene Einheiten verlieren'
    if kind == 4:
        return 'Alle feindlichen Gebäude zerstören'
    if kind == 6:
        return '%s zerstören' % asset_name(ops[2]) if ops[0] != local else '%s verlieren' % asset_name(ops[2])
    if kind == 8:
        other = ops[0] if ops[1] == local else ops[1]
        return 'Fraktion %d ausschalten' % other
    if kind == 10:
        return 'Xenit sammeln (%d)' % (ops[1] >> 4)
    if kind == 12:
        return 'Tritium sammeln (%d)' % (ops[1] >> 4)
    if kind == 14:
        return 'Tritium-Förderung steigern (%s)' % q4(ops[1]) if ops[0] == local else \
            'Tritium-Förderung von Fraktion %d >= %s' % (ops[0], q4(ops[1]))
    if kind == 16:
        return '%s bauen (%d)' % (asset_name(ops[2]), ops[1])
    if kind == 18:
        return 'Gebiet kontrollieren (%d %%)' % ops[1]
    if kind == 20:
        return 'Durchhalten (%d:%02d)' % (ops[1] // 60, ops[1] % 60)
    if kind == 22:
        return 'Xenit-Lager klein halten'
    if kind == 24:
        return 'Einheiten zusammenhalten'
    return '?'


CLASS_ORDER = ('good', 'partial', 'multi', 'none')
CLASS_RANK = {name: i for i, name in enumerate(CLASS_ORDER)}


def analyse_faction(level, faction):
    conditions = level['conditions']
    active = level['world']['activeFactionCount']
    result = {'faction': faction, 'wins': [], 'losses': []}
    for t in level['triggers']:
        if not t['stateFlags'] or t['faction'] != faction:
            continue
        tree = resolve(conditions, t['condition'])
        entry = {'trigger': t, 'tree': tree, 'words': describe(tree, faction),
                 'kinds': sorted({leaf['kind'] for leaf in leaves(tree) if leaf is not None}),
                 'topKind': conditions[t['condition']]['kind']}
        entry['class'], entry['generated'] = classify_tree(tree, faction, active)
        (result['wins'] if t['movieVariant'] == 1 else result['losses']).append(entry)
    if not result['wins']:
        result['class'], result['generated'] = 'none', '-'
    else:
        first = result['wins'][0]
        result['class'] = first['class'] if len(result['wins']) == 1 else \
            max(first['class'], 'partial', key=CLASS_RANK.get)
        result['generated'] = first['generated'] + \
            (' (+%d weitere)' % (len(result['wins']) - 1) if len(result['wins']) > 1 else '')
    return result


PLACEHOLDER = re.compile(r'^\s*((Lang|Kurz)text für die|(Long|Short) text for the)', re.I)


def playable_factions(level, briefing):
    """Factions 1..assignableFactionCount that have an end trigger or a real mission help text (not the
    "Langtext für die n. Partei." placeholder); briefing(faction) returns the German text."""
    assignable = level['world']['assignableFactionCount']
    with_trigger = {t['faction'] for t in level['triggers'] if t['stateFlags']}
    factions = [f for f in range(1, assignable + 1)
                if f in with_trigger or (briefing(f) and not PLACEHOLDER.match(briefing(f)))]
    return factions or [1]


# ---------------------------------------------------------------- level discovery

class Data:
    def __init__(self, game_dir):
        self.packages = mdl2obj.Packages(game_dir, PACKAGES)
        self.titles = read_str(self.packages.get('texte' + BS + 'level.str'))
        self.help = read_str(self.packages.get('texte' + BS + 'inhalt.str'))

    def level(self, file_name):
        """Parsed level of level\\<file_name>.lev, or None when the package lacks it."""
        key = 'level' + BS + file_name.lower() + '.lev'
        if key not in self.packages.index:
            return None
        return lev.parse(self.packages.get(key))


def catalog_names(raw):
    return [cgn.parse_catalog_record(raw, offset)['name'].rstrip('.')
            for offset in range(0, len(raw) - cgn.CATALOG_RECORD + 1, cgn.CATALOG_RECORD)
            if cgn.parse_catalog_record(raw, offset)['name']]


def missions(data):
    """[(group, order label, level file name)] in campaign order, then the maps of level.dat."""
    out = []
    campaigns = catalog_names(data.packages.get('level' + BS + 'campagne.dat'))
    order = ['tutorial', 'hansolo', 'nimm2', 'luke']
    campaigns.sort(key=lambda name: order.index(name) if name in order else len(order))
    for name in campaigns:
        campaign = cgn.parse(data.packages.get('level' + BS + name + '.cgn'))
        for record in campaign['levels']:
            out.append((name, 'id %d' % record['levelId'], record['levelFileName']))
    for name in catalog_names(data.packages.get('level' + BS + 'level.dat')):
        out.append(('level.dat', '-', name))
    return out


def collect(data):
    rows = []
    for group, label, file_name in missions(data):
        level = data.level(file_name)
        row = {'group': group, 'label': label, 'file': file_name, 'level': level, 'factions': []}
        if level is not None:
            title = level['titleTextIndex']
            row['titleDe'] = text(data.titles, COUNTRY_GERMAN, LEVEL_TITLE_STRING_BASE + title)
            row['titleEn'] = text(data.titles, COUNTRY_ENGLISH, LEVEL_TITLE_STRING_BASE + title)
            def help_index(faction):
                return MISSION_HELP_STRING_BASE + title * LEVEL_DESCRIPTION_STRIDE + faction
            for faction in playable_factions(level, lambda f: text(data.help, COUNTRY_GERMAN, help_index(f))):
                analysis = analyse_faction(level, faction)
                index = help_index(faction)
                analysis['helpId'] = 0x230000 + index
                analysis['briefingDe'] = text(data.help, COUNTRY_GERMAN, index)
                analysis['briefingEn'] = text(data.help, COUNTRY_ENGLISH, index)
                row['factions'].append(analysis)
        rows.append(row)
    return rows


# ---------------------------------------------------------------- output

GROUP_TITLES = {'tutorial': 'Tutorial (tutorial.cgn)', 'hansolo': 'Die Invasion (hansolo.cgn)',
                'nimm2': 'Kyrrdon (nimm2.cgn)', 'luke': 'Piraten (luke.cgn)',
                'level.dat': 'Einzel- und Mehrspielerkarten (level.dat)'}


def quote(body):
    if not body:
        return '> (kein Text)'
    return '\n'.join('> ' + line if line.strip() else '>' for line in body.replace('\r', '').split('\n'))


def trigger_label(entry):
    t = entry['trigger']
    return 'Trigger %d (Endauswahl %d, Bedingung %d)' % (t['index'], t['endMovieSelection'], t['condition'])


def write_levels(rows, out):
    lines = ['# Ziel-Overlay: Siegbedingungen und Briefings der Stock-Level', '',
             'Erzeugt von `python tools/data/objectives.py --out docs/plans/objective_overlay_levels.md` aus den '
             'Spieldaten (LEVEL.PCK, texte\\level.str / texte\\inhalt.str, PATCH00 vor ENGINE). Eingabe für die '
             'Kurzziel-Texte (Plan O2). Pro Mission und spielbarer Fraktion: Sieg-Trigger (Variante 1) und '
             'Niederlage-Trigger (Variante 0) in Worten, die Abdeckung durch den automatischen Rückfall (Plan 3b) '
             'mit dem erzeugten Text und die Missionshilfe (F1) auf Deutsch und Englisch.', '',
             'Abdeckung: **gut** = eine Bedingungsart bzw. „alle Gegner“, **teilweise** = und/oder-Teilziele, '
             'NICHT oder mehrere Sieg-Trigger,**nur Mehrere Ziele** = verschachtelt, **kein Sieg** = kein Sieg-Trigger.', '']
    class_words = {'good': 'gut', 'partial': 'teilweise', 'multi': 'nur Mehrere Ziele', 'none': 'kein Sieg'}
    group = None
    for row in rows:
        if row['group'] != group:
            group = row['group']
            lines += ['## ' + GROUP_TITLES.get(group, group), '']
        head = '### `%s`' % row['file']
        if row['level'] is None:
            lines += [head + ' (%s)' % row['label'], '', 'Level-Datei fehlt in LEVEL.PCK (im .cgn eingetragen).', '']
            continue
        lines += [head + ' – %s / %s' % (row['titleDe'], row['titleEn']), '',
                  '%s, Titelindex %d, Fraktionen zuweisbar %d / aktiv %d.' % (
                      row['label'] if row['label'] != '-' else 'Karte', row['level']['titleTextIndex'],
                      row['level']['world']['assignableFactionCount'], row['level']['world']['activeFactionCount']), '']
        for f in row['factions']:
            lines += ['#### Fraktion %d' % f['faction'], '']
            if f['wins']:
                for entry in f['wins']:
                    lines.append('- Sieg, %s: %s' % (trigger_label(entry), entry['words']))
            else:
                lines.append('- Sieg: kein Sieg-Trigger')
            for entry in f['losses']:
                lines.append('- Niederlage, %s: %s' % (trigger_label(entry), entry['words']))
            lines.append('- Abdeckung 3b: **%s** – erzeugt: „%s“' % (class_words[f['class']], f['generated']))
            lines += ['', 'Missionshilfe DE (0x%06X):' % f['helpId'], '', quote(f['briefingDe']), '',
                      'Missionshilfe EN:', '', quote(f['briefingEn']), '']
    with open(out, 'w', encoding='utf-8', newline='\n') as handle:
        handle.write('\n'.join(lines).rstrip('\n') + '\n')


def summary(rows):
    """Markdown summary tables (printed)."""
    present = [r for r in rows if r['level'] is not None]
    missing = [r for r in rows if r['level'] is None]
    pairs = [(r, f) for r in present for f in r['factions']]
    groups = []
    for r in rows:
        if r['group'] not in groups:
            groups.append(r['group'])
    out = []
    out.append('Level: %d eingetragen, %d vorhanden (fehlend: %s); %d Mission+Fraktion-Paare.' % (
        len(rows), len(present), ', '.join('`%s`' % r['file'] for r in missing) or '-', len(pairs)))
    out.append('')
    # condition kinds: in how many win / loss triggers (top kind) and as a leaf
    win_top, loss_top, win_leaf, loss_leaf = {}, {}, {}, {}
    for _, f in pairs:
        for entry in f['wins']:
            win_top[entry['topKind']] = win_top.get(entry['topKind'], 0) + 1
            for kind in entry['kinds']:
                win_leaf[kind] = win_leaf.get(kind, 0) + 1
        for entry in f['losses']:
            loss_top[entry['topKind']] = loss_top.get(entry['topKind'], 0) + 1
            for kind in entry['kinds']:
                loss_leaf[kind] = loss_leaf.get(kind, 0) + 1
    out.append('| Bedingungsart | Sieg-Trigger (oberste Art) | Sieg (als Teilbedingung) | '
               'Niederlage-Trigger (oberste Art) | Niederlage (als Teilbedingung) |')
    out.append('|---|---|---|---|---|')
    for kind in sorted(set(win_top) | set(loss_top) | set(win_leaf) | set(loss_leaf)):
        out.append('| %d %s | %d | %d | %d | %d |' % (kind, PLAN_KIND_NAMES.get(kind, '?'), win_top.get(kind, 0),
                                                     win_leaf.get(kind, 0), loss_top.get(kind, 0),
                                                     loss_leaf.get(kind, 0)))
    out.append('')
    out.append('| Gruppe | Paare | gut | teilweise | nur „Mehrere Ziele“ | kein Sieg |')
    out.append('|---|---|---|---|---|---|')
    for group in groups + ['gesamt']:
        selected = [f for r, f in pairs if group in ('gesamt', r['group'])]
        counts = [sum(1 for f in selected if f['class'] == c) for c in CLASS_ORDER]
        out.append('| %s | %d | %s |' % (group, len(selected), ' | '.join(str(n) for n in counts)))
    out.append('')
    out.append('| Level | Fraktion | Klasse | erzeugter Text (3b) |')
    out.append('|---|---|---|---|')
    for r, f in pairs:
        if f['class'] != 'good' or len(f['wins']) != 1:
            out.append('| `%s` | %d | %s | %s |' % (r['file'], f['faction'], f['class'], f['generated']))
    return '\n'.join(out)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('game_dir', nargs='?', default=DEFAULT_GAME_DIR)
    ap.add_argument('--out')
    ap.add_argument('--summary', action='store_true')
    args = ap.parse_args()
    rows = collect(Data(args.game_dir))
    if args.out:
        write_levels(rows, args.out)
    if args.summary or not args.out:
        sys.stdout.reconfigure(encoding='utf-8')
        print(summary(rows))


if __name__ == '__main__':
    main()
