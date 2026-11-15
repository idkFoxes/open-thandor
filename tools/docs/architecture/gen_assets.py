"""Draws docs/architecture/assets/assets.svg, the block diagram of the assets area.

Run from the repository root: python tools/docs/architecture/gen_assets.py
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import archsvg as A  # noqa: E402

W, H = 1280, 850
d = A.Diagram(W, H, 'assets — PCK packages and catalogs',
              'src/assets/: the package layer reads (and for savegames writes) PCK archives; the catalogs register '
              'the loaded records by id for lookups. Click a box for its sources.')

SRC = '../../../src/assets/'


def mod(x, y, w, h, name, desc, chips, meta=''):
    free = d.box(x, y, w, h, name, desc, chips, href=SRC + name + '/', meta=meta, color='assets')
    assert free <= y + h, (name, free, y + h)


# --- top: files and platform
d.ext(60, 80, 560, 40, 'Game files: daten, modelle, graphik, sound, filme, level, patchNN .pck · loose files')
d.ext(660, 80, 580, 40, 'platform — CoreAssets_MountPackages at start-up, file I/O (g_FileSystem*)',
      href='../platform/README.md', color='platform')

mod(60, 150, 1180, 112, 'package',
    ['Mount table of 1024 slots: Package_Mount reads each archive directory; Package_FindEntryAcrossMounts searches '
     'the slots in mount order (earlier mounts win), a loose file next to the exe is the fallback.',
     'Package_DecodeEntryInto stages the packed bytes in the 8 MiB scratch buffer and decodes by method: '
     '0 Huffman/RLE, 1 stored, 2 field grid. Package_UpsertEntry / DeleteEntry rewrite save archives.'],
    ['runtime.cpp', 'codec.cpp', 'resource_loader.cpp', 'archive_write.cpp'], meta='4 files')

d.line([(340, 120), (340, 150)], 'read · write', 347, 140, both=True)
d.line([(950, 120), (950, 150)], 'Package_Mount · file I/O', 957, 140)

# --- level catalogs
d.band(452, 294, 'LEVEL CATALOGS')
RY, RH = 306, 150
mod(60, RY, 200, RH, 'army', ['ARM records (converter', '0x20008) by id; enabled', 'and placeable checks,',
                              'armour / energy sums'], ['catalog.cpp'])
mod(300, RY, 200, RH, 'model', ['MDL definitions: node', 'sprites, shot and effect', 'ids, terrain class values'],
    ['definitions.cpp'])
mod(540, RY, 200, RH, 'shot', ['SHT definitions: .spr', 'sprite, effect references,', 'terrain material check'],
    ['catalog.cpp'])
mod(780, RY, 200, RH, 'effect', ['EFF definitions (0xC0', 'bytes): sprite, linked', 'effect and shot ids'],
    ['catalog.cpp'])
mod(1020, RY, 220, RH, 'sprite', ['Shared registry of .spr', 'assets by id; relocates', 'their pointers when',
                                  'registered'], ['catalog.cpp'])

for x in (160, 400, 640, 880):
    d.line([(x, 262), (x, RY)])
d.text(887, 290, 'loaded asset buffers', 'l')
d.line([(500, 381), (540, 381)], 'ids', 520, 374, anchor='middle')
d.line([(740, 381), (780, 381)], 'refs', 760, 374, anchor='middle', both=True)
d.line([(980, 381), (1020, 381)], '.spr', 1000, 374, anchor='middle')

# --- consumers of the level catalogs
d.ext(60, 492, 680, 40, 'gameplay — NewLevel_LoadAssetList loads and prepares the four lists; army, model lookups',
      href='../gameplay/README.md', color='gameplay')
d.ext(780, 492, 460, 40, 'world — shot, effect and model definition lookups',
      href='../world/README.md', color='world')
d.line([(160, 492), (160, RY + RH)], '*_Prepare* · lookups', 167, 480)
d.line([(880, 492), (880, RY + RH)], 'lookups', 887, 486)
# gameplay <-> package: savegame archives, through the gap between army and model
d.line([(280, 492), (280, 262)], 'saves: Mount · UpsertEntry', 274, 385, both=True,
       anchor='middle', rotate=True)

# --- front-end catalogs and texts
d.band(60, 566, 'FRONT-END CATALOGS AND TEXTS')
FY, FH = 576, 140
mod(60, FY, 360, FH, 'rom', ['Menu rooms of engine\\zentrale.rom: records with', 'camera pose, lights, action tables and',
                             'sprite node trees; registry of 256 slots'], ['runtime.cpp'])
mod(460, FY, 380, FH, 'scenario', ['"Choose game" list: level.dat, campagne.dat', '(+ add-ons 00..99) and save\\*.sve headers;',
                                   'ScenarioCatalog_Rebuild via Resource_Load'], ['catalog.cpp'])
mod(880, FY, 360, FH, 'text', ['STR pages per locale (TextResourcePage_Load),', 'override table, TextResource_Resolve;',
                               'rich-text command streams'],
    ['resources.cpp', 'richtext.cpp', 'richtext_markup.cpp'], meta='3 files')

# rom registers its node sprites in the sprite registry
d.line([(380, FY), (380, 552), (760, 552), (760, 468), (1130, 468), (1130, RY + RH)], '.spr', 1010, 464)
# text pages come from the packages: right margin
d.line([(1240, 206), (1262, 206), (1262, 646), (1240, 646)], 'Package_LoadEntryWithSize', 1256, 420,
       anchor='middle', rotate=True)

# --- consumers of the front-end catalogs
BY = 776
d.ext(60, BY, 560, 44, 'ui — loads zentrale.rom, rebuilds the catalog; resolves texts',
      href='../ui/README.md', color='ui')
d.ext(660, BY, 180, 44, 'network', href='../network/README.md', color='network')
d.ext(880, BY, 360, 44, 'core · platform — load text pages at start-up',
      href='../core/README.md', color='core')
d.line([(240, BY), (240, FY + FH)], 'RomAsset_PrepareRecords', 247, 752)
d.line([(540, BY), (540, FY + FH)], 'ScenarioCatalog_Rebuild', 547, 752)
d.line([(750, FY + FH), (750, BY)], 'catalog to clients', 757, 752)
d.line([(1060, BY), (1060, FY + FH)], 'TextResourcePage_Load', 1067, 752)

d.save('docs/architecture/assets/assets.svg')

if __name__ == '__main__' and '--png' in sys.argv:
    A.render_png('docs/architecture/assets/assets.svg', sys.argv[sys.argv.index('--png') + 1], W, H)
