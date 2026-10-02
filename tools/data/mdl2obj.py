"""Exports a Thandor unit (army asset) as Wavefront OBJ + MTL + PNG textures, e.g. for Blender.

usage:
  python tools/data/mdl2obj.py GAME_DIR UNIT_NAME_OR_ID OUT_DIR [options]

  UNIT_NAME_OR_ID  an army asset id (unit.arm registryId, e.g. 1) or a (part of a) model name as shown in game,
                   e.g. "Wiesel" (matched against the help.str model names of every locale; the first army
                   asset whose root model is that definition wins, its id is printed)
  --variant N      upgrade stage: index into each tree node's linked definition ids (0 = base, 1 = "A", ...);
                   a missing stage falls back to the base definition
  --color N        faction colour / texture set gfx\\mdl\\army<N>.gfx (default 1; 0 is the neutral set)
  --gfx-base NAME  texture set base name (default "army"; the levels can use "sarmy")
  --lod N          level of detail (mesh group) to export, 0 = most detailed (default)
  --scale F        world units -> OBJ units (default 10: one engine unit (Q12 4096) = 10 OBJ units,
                   which makes the Jeep Wiesel ~4.5 long, i.e. roughly metres)
  --name NAME      base name of the written files (default: the unit name)
  --list           only list the army assets and their model trees

Data path (all documented in the open-thandor sources):
  DATEN.PCK  arm\\unit.arm   ArmyAssetRecord (src/assets/army/catalog.c): +0x08 registryId, +0x0C root
             ArmyModelTreeNode (include/thandor/assets/army/catalog.h): +0x08 childCount, +0x0C children[5]
             (offsets from the asset start), +0x20 linkedDefinitionIds[8] (upgrade stages).
  DATEN.PCK  mdl\\*.mdl      model definitions (src/assets/model/definitions.c): record +0x04 nameTextIndex
             (help.str string 0x4F + index), +0x08 definitionId, +0x64 root MdlSerializedNodeHeader:
             +0x04 nodeFlags (low nibble 0 = has a sprite, else an attachment point for the next linked
             child model of the army tree), +0x08..+0x10 local rotation angles 0..2 (65536 = full turn),
             +0x14 childCount, +0x18 child offsets[6], +0x38 UTF-16 sprite path (without .spr).
  MODELLE.PCK spr\\...\\*.spr geometry (src/assets/sprite/catalog.c, ModelResource in types.h):
             +0xB0 mesh group (LOD) count, +0xE4/+0xE8 attachment/point table (0x10-byte records: kind in
             bits 0..3, child slot above, then x, y, z in Q12), mesh groups from +0x200 (0x20 header:
             byte size, mesh count, -, flags), each mesh a 0x20 header (size, mask, vertex count,
             triangle count) + 0x40-byte vertices (x, y, z, -, normal xyz in Q12) + 0x40-byte triangles
             (three {vertex offset from the asset start, u, v (Q12 texels, 256 = one texture width)},
             plane normal, texture subresource index, render flags).
  Placement (src/world/model/hierarchy.c ModelNodeRuntime_CreateHierarchyRecursive, runtime.c
             ModelRuntimePool_RepairDeferredChild): a child node sits at the translation of the first point
             record of kind 0 or 1 naming its slot in the parent's sprite, rotated by its local angles
             (FixedTransform_BuildRotationBasis, src/core/math/fixed.c); a linked child model takes the angles
             of the attachment-point node it replaces. World = parent world x child local.
  GRAPHIK.PCK gfx\\mdl\\army<N>.gfx textures (src/graphics/resources/texture.c): +0xB0 subresource count,
             +0xB4 palette count, +0xB8 subresource table (0x20 bytes: logical w, h, palette index (<0 =
             direct ARGB8888), data offset, origin x, y, pixel w, h); palettes at +0x200 + i*0x800,
             256 x {ARGB8888, framebuffer pixel}.

Coordinates: the engine is Z-up (x = forward, z = up). The OBJ is written Y-up (x, z, -y), which is what
Blender's OBJ importer expects by default (Forward -Z, Up Y), so the model stands upright after import.
Triangles are written counter-clockwise around their stored (outward) plane normal.
"""
import argparse
import math
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pck  # noqa: E402

Q12 = 4096.0
ANGLE = 2.0 * math.pi / 65536.0


def u32(d, o):
    return struct.unpack_from("<I", d, o)[0]


def i32(d, o):
    return struct.unpack_from("<i", d, o)[0]


class Packages:
    """Lazy reader of the PCK packages; later packages in the list override earlier entries of the same path."""

    def __init__(self, game_dir, names):
        self.index = {}
        self.data = {}
        for name in names:
            path = os.path.join(game_dir, name)
            if not os.path.exists(path):
                continue
            try:
                data, items = pck.entries(path)
            except Exception:  # e.g. PATCH01.PCK is a RAR archive, not a package
                continue
            self.data[path] = data
            for entry in items:
                self.index[entry[0].lower()] = (path, entry)

    def get(self, name):
        path, entry = self.index[name.lower()]
        return pck.read_entry(self.data[path], entry)

    def names(self, prefix):
        return [n for n in self.index if n.startswith(prefix.lower())]


# ---------------------------------------------------------------- text, definitions, army assets

def read_text_page(raw):
    """str asset: locale blocks after 0x200 bytes ({size, string count, country code, -} + offsets)."""
    pages = {}
    offset = 0x200
    for _ in range(u32(raw, 0xB0)):
        size, count, country = struct.unpack_from("<III", raw, offset)
        strings = []
        for i in range(count):
            start = offset + u32(raw, offset + 16 + 4 * i)
            end = start
            while raw[end:end + 2] != b"\0\0":
                end += 2
            text = raw[start:end].decode("utf-16-le", "replace")
            strings.append("".join(c for c in text if " " <= c < "" and ord(c) < 0x8000))
        pages[country] = strings
        offset += size
    return pages


def load_definitions(packages):
    defs = {}
    for name in sorted(packages.names("mdl\\")):
        raw = packages.get(name)
        offset = 0x200
        for _ in range(u32(raw, 0xB0)):
            size, name_index, def_id = struct.unpack_from("<III", raw, offset)
            defs[def_id] = {"raw": raw, "offset": offset, "nameIndex": name_index, "file": name,
                            "root": u32(raw, offset + 0x64)}
            offset += size
    return defs


def definition_name(defs, texts, def_id, country=49):
    d = defs.get(def_id)
    if d is None:
        return "?"
    strings = texts.get(country) or next(iter(texts.values()))
    index = 0x4F + d["nameIndex"]
    return strings[index] if index < len(strings) else "?"


def load_army_assets(packages):
    raw = packages.get("arm\\unit.arm")
    armies = []
    offset = 0x200
    for _ in range(u32(raw, 0xB0)):
        size, _variant, army_id, root = struct.unpack_from("<IIII", raw, offset)
        armies.append({"id": army_id, "root": root, "flags": u32(raw, offset + 0x14)})
        offset += size
    return raw, armies


def army_tree(raw, offset):
    """ArmyModelTreeNode -> {'ids': [...8 linked definition ids], 'children': [...]}"""
    count = u32(raw, offset + 8)
    return {"ids": list(struct.unpack_from("<8I", raw, offset + 0x20)),
            "children": [army_tree(raw, u32(raw, offset + 0x0C + 4 * i)) for i in range(count)]}


KNOWN_DEFINITIONS = set()


def pick_id(ids, variant):
    """Upgrade stage `variant` of a tree node; missing or unknown stages fall back to the base definition."""
    if variant < len(ids) and ids[variant] and (not KNOWN_DEFINITIONS or ids[variant] in KNOWN_DEFINITIONS):
        return ids[variant]
    return ids[0]


# ---------------------------------------------------------------- math (port of FixedTransform_*)

def rotation_basis(roll, elevation, azimuth):
    """FixedTransform_BuildRotationBasis in floating point; returns rows r0, r1, r2."""
    el, az, ro = elevation * ANGLE, azimuth * ANGLE, roll * ANGLE
    ce, se = math.cos(el), math.sin(el)
    m = [[0.0] * 3 for _ in range(3)]
    m[0][2], m[1][2], m[2][2] = ce * math.cos(az), ce * math.sin(az), se
    m[2][1] = ce * math.sin(ro - az)
    m[2][0] = -ce * math.cos(ro - az)
    a = (math.cos(ro) - math.cos(ro - 2 * az)) / 2
    b = (math.cos(ro) + math.cos(ro - 2 * az)) / 2
    c = (math.sin(ro) + math.sin(ro - 2 * az)) / 2
    dd = (math.sin(ro - 2 * az) - math.sin(ro)) / 2
    m[0][0] = a + b * se
    m[1][1] = a * se + b
    m[1][0] = c - dd * se
    m[0][1] = dd - c * se
    return m


def compose(outer, inner):
    """(R, t) pairs: outer * inner (FixedTransform_Compose)."""
    ro, to = outer
    ri, ti = inner
    r = [[sum(ro[i][k] * ri[k][j] for k in range(3)) for j in range(3)] for i in range(3)]
    t = [sum(ro[i][k] * ti[k] for k in range(3)) + to[i] for i in range(3)]
    return r, t


def apply(xf, p, translate=True):
    r, t = xf
    return [sum(r[i][k] * p[k] for k in range(3)) + (t[i] if translate else 0.0) for i in range(3)]


# ---------------------------------------------------------------- sprites

class Sprite:
    def __init__(self, raw):
        if raw[:4] != b"spr\0":
            raise ValueError("not an spr asset")
        self.raw = raw
        self.points = []
        table, count = u32(raw, 0xE4), u32(raw, 0xE8)
        for i in range(count):
            key, x, y, z = struct.unpack_from("<Iiii", raw, table + 16 * i)
            self.points.append((key & 15, key >> 4, (x / Q12, y / Q12, z / Q12)))

    def attachment(self, slot):
        for kind, sel, pos in self.points:
            if kind in (0, 1) and sel == slot:
                return pos
        return None

    def meshes(self, lod):
        raw = self.raw
        groups = u32(raw, 0xB0)
        group = 0x200
        for _ in range(min(lod, groups - 1)):
            group += u32(raw, group)
        result = []
        mesh = group + 0x20
        for mi in range(u32(raw, group + 4)):
            size, mask, vcount, tcount = struct.unpack_from("<4I", raw, mesh)
            vstart = mesh + 0x20
            verts = []
            for v in range(vcount):
                x, y, z, _, nx, ny, nz = struct.unpack_from("<7i", raw, vstart + 0x40 * v)
                verts.append(((x / Q12, y / Q12, z / Q12), (nx / Q12, ny / Q12, nz / Q12)))
            tris = []
            tstart = vstart + 0x40 * vcount
            for t in range(tcount):
                r = struct.unpack_from("<IiiIiiIiiiiiII", raw, tstart + 0x40 * t)
                idx = [(r[0] - vstart) // 0x40, (r[3] - vstart) // 0x40, (r[6] - vstart) // 0x40]
                uv = [(r[1] / Q12, r[2] / Q12), (r[4] / Q12, r[5] / Q12), (r[7] / Q12, r[8] / Q12)]
                tris.append({"idx": idx, "uv": uv, "normal": (r[9] / Q12, r[10] / Q12, r[11] / Q12),
                             "sub": r[12], "flags": r[13]})
            result.append({"mask": mask, "verts": verts, "tris": tris, "index": mi})
            mesh += size
        return result


# ---------------------------------------------------------------- hierarchy

def build_parts(packages, defs, army_raw, tree, variant, lod):
    """Walks the army tree and the MDL node trees; returns [{name, sprite, xform, meshes}]."""
    parts = []
    sprites = {}

    def sprite(path):
        key = path.lower() + ".spr"
        if key not in sprites:
            sprites[key] = Sprite(packages.get(key))
        return sprites[key]

    def walk_model(def_id, army_node, xform, model_label):
        d = defs[def_id]
        raw = d["raw"]
        linked = list(army_node["children"])  # linked child models fill attachment points in order

        def walk_node(offset, xf, depth, override_angles=None):
            flags = u32(raw, offset + 4)
            angles = struct.unpack_from("<3I", raw, offset + 8)
            if override_angles is not None:
                angles = override_angles
            count = u32(raw, offset + 0x14)
            children = struct.unpack_from("<6I", raw, offset + 0x18)
            path = raw[offset + 0x38:offset + 0x38 + 256].decode("utf-16-le").split("\0")[0]
            spr = sprite(path)
            parts.append({"name": "%s_%s" % (model_label, os.path.basename(path)), "sprite": spr, "xform": xf,
                          "meshes": spr.meshes(lod), "def": def_id})
            for slot in range(count):
                pos = spr.attachment(slot)
                if pos is None:
                    continue
                child = children[slot]
                child_flags = u32(raw, child + 4)
                child_angles = struct.unpack_from("<3I", raw, child + 8)
                local = (rotation_basis(child_angles[2], child_angles[1], child_angles[0]), list(pos))
                child_xf = compose(xf, local)
                if child_flags & 0xF == 0:
                    walk_node(child, child_xf, depth + 1)
                elif linked:
                    sub = linked.pop(0)
                    sub_id = pick_id(sub["ids"], variant)
                    walk_model(sub_id, sub, child_xf, "%s%d" % (model_label, slot + 1))

        root = d["root"]
        walk_node(root, xform, 0)

    root_id = pick_id(tree["ids"], variant)
    d = defs[root_id]
    angles = struct.unpack_from("<3I", d["raw"], d["root"] + 8)
    walk_model(root_id, tree, (rotation_basis(angles[2], angles[1], angles[0]), [0.0, 0.0, 0.0]), "p")
    return parts


# ---------------------------------------------------------------- textures

def export_texture(gfx, index, path):
    from PIL import Image
    count, _banks, table = struct.unpack_from("<3I", gfx, 0xB0)
    if index >= count:
        return False
    _lw, _lh, palette, data, _ox, _oy, w, h = struct.unpack_from("<IIiIiiII", gfx, table + 0x20 * index)
    img = Image.new("RGBA", (w, h))
    px = []
    if palette < 0:
        for i in range(w * h):
            argb = u32(gfx, data + 4 * i)
            px.append(((argb >> 16) & 255, (argb >> 8) & 255, argb & 255, argb >> 24))
    else:
        pal_base = 0x200 + palette * 0x800
        pal = []
        for i in range(256):
            argb = u32(gfx, pal_base + 8 * i)
            pal.append(((argb >> 16) & 255, (argb >> 8) & 255, argb & 255, argb >> 24))
        px = [pal[b] for b in gfx[data:data + w * h]]
    if all(p[3] == 0 for p in px):  # alpha unused: make it opaque
        px = [(r, g, b, 255) for r, g, b, _ in px]
    img.putdata(px)
    img.save(path)
    return True


# ---------------------------------------------------------------- main

def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("game_dir")
    ap.add_argument("unit", nargs="?", default="")
    ap.add_argument("out_dir", nargs="?", default=".")
    ap.add_argument("--variant", type=int, default=0)
    ap.add_argument("--color", type=int, default=1)
    ap.add_argument("--gfx-base", default="army")
    ap.add_argument("--lod", type=int, default=0)
    ap.add_argument("--scale", type=float, default=10.0)
    ap.add_argument("--name")
    ap.add_argument("--list", action="store_true")
    args = ap.parse_args()

    packages = Packages(args.game_dir, ["ENGINE.PCK", "DATEN.PCK", "MODELLE.PCK", "GRAPHIK.PCK", "PATCH00.PCK"])
    texts = read_text_page(packages.get("texte\\help.str"))
    defs = load_definitions(packages)
    KNOWN_DEFINITIONS.update(defs)
    army_raw, armies = load_army_assets(packages)
    trees = {a["id"]: army_tree(army_raw, a["root"]) for a in armies if a["root"]}

    def describe(node, depth=0):
        names = ", ".join("%d %s" % (i, definition_name(defs, texts, i)) for i in node["ids"] if i)
        lines = ["%s[%s]" % ("  " * depth, names)]
        for c in node["children"]:
            lines += describe(c, depth + 1)
        return lines

    if args.list:
        for a in armies:
            if a["id"] in trees:
                print("army %d (flags 0x%x)" % (a["id"], a["flags"]))
                print("\n".join("  " + l for l in describe(trees[a["id"]])))
        return

    # resolve the unit
    army_id = None
    if args.unit.isdigit():
        army_id = int(args.unit)
    else:
        wanted = args.unit.lower()
        matches = [i for i in defs if any(i_name and wanted in i_name.lower() for i_name in
                   (definition_name(defs, texts, i, c) for c in texts))]
        for a in armies:
            if a["id"] in trees and trees[a["id"]]["ids"][0] in matches:
                army_id = a["id"]
                break
        if army_id is None:
            for a in armies:  # any node of the tree
                if a["id"] in trees and any(i in matches for i in trees[a["id"]]["ids"]):
                    army_id = a["id"]
                    break
    if army_id not in trees:
        sys.exit("unit %r not found (try --list)" % args.unit)
    tree = trees[army_id]
    unit_name = definition_name(defs, texts, pick_id(tree["ids"], args.variant))
    print("army asset %d: %s" % (army_id, unit_name))
    print("\n".join("  " + l for l in describe(tree)))

    parts = build_parts(packages, defs, army_raw, tree, args.variant, args.lod)
    base = args.name or "".join(c if c.isalnum() else "_" for c in unit_name).strip("_").lower() or "unit"
    os.makedirs(args.out_dir, exist_ok=True)
    gfx_name = "gfx\\mdl\\%s%d.gfx" % (args.gfx_base, args.color)
    gfx = packages.get(gfx_name)
    sub_count = u32(gfx, 0xB0)

    def to_obj(p):
        x, y, z = p
        return (x * args.scale, z * args.scale, -y * args.scale)

    obj = ["# Thandor army asset %d (%s), exported by open-thandor tools/data/mdl2obj.py" % (army_id, unit_name),
           "# variant %d, LOD %d, textures %s, scale %g, Y-up (engine Z-up: x fwd, z up)" %
           (args.variant, args.lod, gfx_name, args.scale),
           "mtllib %s.mtl" % base]
    used_subs = set()
    vcount = ncount = tcount = 0
    stats = []
    for part in parts:
        r, _ = part["xform"]
        obj.append("o %s" % part["name"])
        nverts = ntris = 0
        for mesh in part["meshes"]:
            if not mesh["tris"]:
                continue
            obj.append("g %s_mesh%d" % (part["name"], mesh["index"]))
            base_v = vcount
            base_n = ncount
            for pos, nrm in mesh["verts"]:
                wp = apply(part["xform"], pos)
                obj.append("v %.6f %.6f %.6f" % to_obj(wp))
                wn = apply(part["xform"], nrm, translate=False)
                ln = math.sqrt(sum(c * c for c in wn)) or 1.0
                obj.append("vn %.5f %.5f %.5f" % (wn[0] / ln, wn[2] / ln, -wn[1] / ln))
            vcount += len(mesh["verts"])
            ncount += len(mesh["verts"])
            current = None
            for tri in mesh["tris"]:
                sub = tri["sub"]
                mat = ("tex%03d" % sub) if sub < sub_count else ("col%03d" % (tri["flags"] & 0x1FF))
                if tri["flags"] & 0x400:
                    mat += "_2s"
                used_subs.add((mat, sub, tri["flags"] & 0x1FF))
                if mat != current:
                    obj.append("usemtl %s" % mat)
                    current = mat
                idx = list(tri["idx"])
                uv = list(tri["uv"])
                # winding: counter-clockwise around the stored plane normal (outward)
                p = [mesh["verts"][i][0] for i in idx]
                e1 = [p[1][k] - p[0][k] for k in range(3)]
                e2 = [p[2][k] - p[0][k] for k in range(3)]
                cr = (e1[1] * e2[2] - e1[2] * e2[1], e1[2] * e2[0] - e1[0] * e2[2], e1[0] * e2[1] - e1[1] * e2[0])
                if sum(cr[k] * tri["normal"][k] for k in range(3)) < 0:
                    idx = [idx[0], idx[2], idx[1]]
                    uv = [uv[0], uv[2], uv[1]]
                vt = []
                for u, v in uv:
                    obj.append("vt %.6f %.6f" % (u / 256.0, 1.0 - v / 256.0))
                    tcount += 1
                    vt.append(tcount)
                obj.append("f " + " ".join("%d/%d/%d" % (base_v + i + 1, t, base_n + i + 1) for i, t in zip(idx, vt)))
                ntris += 1
            nverts += len(mesh["verts"])
        stats.append((part["name"], nverts, ntris))
    with open(os.path.join(args.out_dir, base + ".obj"), "w") as f:
        f.write("\n".join(obj) + "\n")

    mtl = ["# materials of %s.obj; textures from %s" % (base, gfx_name)]
    pal = None
    try:
        pal = packages.get("gfx\\mdl\\%s%d.pal" % (args.gfx_base, args.color))
    except KeyError:
        pass
    written = set()
    for name, sub, bank in sorted(used_subs):
        if name in written:
            continue
        written.add(name)
        if sub < sub_count:
            png = "%s_%s%d_%03d.png" % (base, args.gfx_base, args.color, sub)
            export_texture(gfx, sub, os.path.join(args.out_dir, png))
            mtl += ["newmtl %s" % name, "Ka 1 1 1", "Kd 1 1 1", "Ks 0 0 0", "d 1", "illum 1", "map_Kd %s" % png, ""]
        else:  # untextured: material colour from the palette asset (never happens for the stock units)
            argb = u32(pal, 0x200 + 8 * bank) if pal and bank < u32(pal, 0xB0) else 0xFFFFFFFF
            mtl += ["newmtl %s" % name,
                    "Kd %.3f %.3f %.3f" % (((argb >> 16) & 255) / 255, ((argb >> 8) & 255) / 255, (argb & 255) / 255),
                    "illum 1", ""]
    with open(os.path.join(args.out_dir, base + ".mtl"), "w") as f:
        f.write("\n".join(mtl) + "\n")

    for name, nv, nt in stats:
        print("  part %-24s %4d vertices %4d triangles" % (name, nv, nt))
    print("total %d vertices, %d triangles, %d textures -> %s" %
          (vcount, sum(s[2] for s in stats), len({s for _, s, _ in used_subs if s < sub_count}), os.path.join(args.out_dir, base + ".obj")))


if __name__ == "__main__":
    main()
