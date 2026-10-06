"""Writes the army reference table docs/reference/army_units.md from the game data (read only).

usage:
  python tools/data/army_table.py [GAME_DIR] [--out FILE] [--enum-comments HEADER]

  GAME_DIR          game directory with the PCK packages (default: the soak copy, see DEFAULT_GAME_DIR)
  --out FILE        markdown output (default docs/reference/army_units.md next to this script's repository)
  --enum-comments H rewrites the trailing comments of the ARM_* enumerators in header H
                    (include/thandor/core/types.h) with the readable unit / building name; nothing else changes
  --check           only print the consistency findings (tree child count vs. the model's attachment points)

Data path (readers shared with tools/data/mdl2obj.py):
  DATEN.PCK arm\\unit.arm, arm\\building.arm   ArmyAssetRecord (+0x08 registryId, +0x0C root ArmyModelTreeNode,
            +0x14 flags); ArmyModelTreeNode (include/thandor/assets/army/catalog.h): +0x08 childCount,
            +0x0C children[5], +0x20 linkedDefinitionIds[8] (the upgrade stages; ModelRuntime creation picks the
            last stage whose technology the faction has, ModelDefinition_SelectFactionUnlockedLinkedId).
  DATEN.PCK mdl\\*.mdl  ModelDefinition: +0x04 nameTextIndex, +0x08 definitionId, +0x1C0 requiredTechnologyBit
            (the TEC id whose bit unlocks the definition, ModelDefinition_IsFactionTechnologyLocked).
  texte\\help.str     page 0x18: model names at 0x4F + nameTextIndex (TEXT_ID_MODEL_NAME_BASE 0x18004F),
                     "no weapon" at 0x4E (TEXT_ID_SELECTION_DETAIL_NO_WEAPON).
  texte\\techno.str   page 0x30: technology names at 2 * TEC id (TECHNOLOGY_TEXT_ID_BASE 0x300000).
  PATCH00.PCK is mounted before DATEN.PCK, so its entries win (as in CoreAssets_MountPackages).
Names are the German texts (country code 49) the game shows with the default language.
"""
import argparse
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mdl2obj  # noqa: E402  (Packages, read_text_page, load_definitions, army_tree, u32)

DEFAULT_GAME_DIR = r"C:\Users\Gerrit Bluemel\Documents\Projekte\ot-soak"
REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
COUNTRY_GERMAN = 49
MODEL_NAME_BASE = 0x4F
NO_WEAPON_TEXT = 0x4E
REQUIRED_TECHNOLOGY_OFFSET = 0x1C0
BS = "\\"

# What the weapon slots (ArmyModelTreeNode.children[i], ModelRuntimeSlot.attachments[i]) hold in the stock data;
# see the "Slots" section of the output.
SLOT_NAMES = ["primary", "secondary", "third"]


def load_records(packages, name):
    """[{id, root, flags, tree}] of one .arm asset (records with a root node only)."""
    raw = packages.get("arm" + BS + name)
    records = []
    offset = 0x200
    for _ in range(mdl2obj.u32(raw, 0xB0)):
        size, _variant, army_id, root = struct.unpack_from("<IIII", raw, offset)
        if root:
            records.append({"id": army_id, "flags": mdl2obj.u32(raw, offset + 0x14),
                            "tree": mdl2obj.army_tree(raw, root), "file": name})
        offset += size
    return records


def load_tec_enum():
    """TEC id -> enumerator name from include/thandor/gameplay/ai/types.h."""
    path = os.path.join(REPO, "include", "thandor", "gameplay", "ai", "types.h")
    names = {}
    if os.path.exists(path):
        for m in re.finditer(r"\b(TEC_\w+)=(\d+)", open(path, encoding="utf-8").read()):
            names[int(m.group(2))] = m.group(1)
    return names


class Data:
    def __init__(self, game_dir):
        packages = mdl2obj.Packages(game_dir, ["ENGINE.PCK", "DATEN.PCK", "MODELLE.PCK", "PATCH00.PCK"])
        self.packages = packages
        help_pages = mdl2obj.read_text_page(packages.get("texte" + BS + "help.str"))
        techno_pages = mdl2obj.read_text_page(packages.get("texte" + BS + "techno.str"))
        self.help = help_pages[COUNTRY_GERMAN]
        self.techno = techno_pages[COUNTRY_GERMAN]
        self.defs = mdl2obj.load_definitions(packages)
        self.tec_enum = load_tec_enum()
        self.sprites = {}

    def name(self, def_id):
        d = self.defs.get(def_id)
        if d is None:
            return None
        index = MODEL_NAME_BASE + d["nameIndex"]
        text = " ".join(self.help[index].split()) if index < len(self.help) else ""
        return text or "(no name)"

    def tec(self, def_id):
        d = self.defs.get(def_id)
        return None if d is None else mdl2obj.u32(d["raw"], d["offset"] + REQUIRED_TECHNOLOGY_OFFSET)

    def stages(self, ids):
        """[(stage, id, name, tec)] of the non-zero linked ids; name None = no such definition."""
        return [(i, d, self.name(d), self.tec(d)) for i, d in enumerate(ids) if d]

    def attachment_points(self, def_id):
        """Number of attachment descriptors ModelNodeRuntime_CreateHierarchyRecursive records for the definition:
        child nodes that the parent's sprite has an attachment point for (kind 0/1, mdl2obj.Sprite.attachment)
        and that are attachment points themselves (nodeFlags low nibble != 0). None when unknown."""
        d = self.defs.get(def_id)
        if d is None:
            return None
        raw = d["raw"]

        def walk(node):
            count = 0
            path = raw[node + 0x38:node + 0x38 + 256].decode("utf-16-le").split("\0")[0]
            key = path.lower() + ".spr"
            if key not in self.packages.index:
                return 0
            sprite = self.sprites.get(key) or self.sprites.setdefault(key, mdl2obj.Sprite(self.packages.get(key)))
            for slot in range(min(mdl2obj.u32(raw, node + 0x14), 6)):
                if sprite.attachment(slot) is None:
                    continue
                child = mdl2obj.u32(raw, node + 0x18 + 4 * slot)
                count += 1 if mdl2obj.u32(raw, child + 4) & 15 else walk(child)
            return count
        return walk(d["root"])


def stage_chain(data, ids):
    """'id name (TEC n)' of every upgrade stage, separated by ' / '."""
    parts = []
    for _stage, def_id, name, tec in data.stages(ids):
        if name is None:
            parts.append("%d (not defined, never selected)" % def_id)
        else:
            parts.append("%d %s (TEC %d)" % (def_id, name, tec))
    return " / ".join(parts)


def weapon_cell(data, node):
    """Short slot cell: base model id + name; nested children (turret on turret) in brackets."""
    ids = node["ids"]
    text = "%d %s" % (ids[0], data.name(ids[0]) or "?")
    if node["children"]:
        text += " [" + "; ".join(weapon_cell(data, c) for c in node["children"]) + "]"
    return text


def collect_models(data, records):
    """Every child model (weapon / turret) reached from the records' trees, keyed by its base id."""
    models = {}

    def walk(node):
        for child in node["children"]:
            models.setdefault(child["ids"][0], child["ids"])
            walk(child)
    for record in records:
        walk(record["tree"])
    return models


def write_markdown(data, units, buildings, out):
    lines = []
    w = lines.append
    w("# Army units and buildings")
    w("")
    w("Generated by `python tools/data/army_table.py` from the game data (`DATEN.PCK`/`PATCH00.PCK`: `arm\\unit.arm`,")
    w("`arm\\building.arm`, `mdl\\*.mdl`, `texte\\help.str`, `texte\\techno.str`); do not edit by hand. Names are")
    w("the German model names the in-game selection panel shows (`TEXT_ID_MODEL_NAME_BASE` + the definition's")
    w("`nameTextIndex`). The ARM ids are the `ARM_*` enumerators of `include/thandor/core/types.h`.")
    w("")
    w("## How to read")
    w("")
    w("- An army record (ARM id) is a model tree (`ArmyModelTreeNode`): the root is the chassis, its children")
    w("  are the weapon / turret slots (`children[i]`, created as `ModelRuntimeSlot.attachments[i]`).")
    w("- Every node lists up to eight model definition ids, the upgrade stages: stage 0 is the default, the")
    w("  game takes the last stage whose technology the faction has (`ModelDefinition_SelectFactionUnlockedLinkedId`).")
    w("  The TEC after each stage is the definition's `requiredTechnologyBit`; a stage id without a definition")
    w("  is never selected.")
    w("- The unit and building tables name each slot's model by its stage-0 id; the stages of every weapon /")
    w("  turret model and the TEC each one needs are in [Weapon and turret models](#weapon-and-turret-models).")
    w("- **bare** marks a record whose chassis has no weapon slot (the selection panel shows \"%s\")." %
      data.help[NO_WEAPON_TEXT].strip())
    w("- Slot 0 holds the primary weapon, slot 1 the secondary one (two-weapon walkers: slot 0 the heavier")
    w("  weapon, slot 1 the lighter or a second copy). In code: `ARMY_WEAPON_SLOT_PRIMARY` / `_SECONDARY`.")
    w("")

    # units: one section per chassis (base id of the root)
    w("## Units (`arm\\unit.arm`)")
    w("")
    groups = []
    for record in units:
        root = record["tree"]["ids"][0]
        if not groups or groups[-1][0] != root:
            groups.append((root, []))
        groups[-1][1].append(record)
    max_slots = max(len(r["tree"]["children"]) for r in units)
    for root, records in groups:
        tree = records[0]["tree"]
        w("### %s (MDL %d)" % (data.name(root) or "?", root))
        w("")
        w("Chassis stages: %s" % stage_chain(data, tree["ids"]))
        w("")
        header = "| ARM | " + " | ".join("Slot %d (%s)" % (i, SLOT_NAMES[i]) for i in range(max_slots)) + " |"
        w(header)
        w("|" + "---|" * (max_slots + 1))
        for record in records:
            children = record["tree"]["children"]
            cells = [weapon_cell(data, c) for c in children]
            if not children:
                cells = ["**bare**"]
            cells += [""] * (max_slots - len(cells))
            mark = ""
            if record["tree"]["ids"] != tree["ids"]:
                mark = " (chassis stages: %s)" % stage_chain(data, record["tree"]["ids"])
            w("| %d%s | %s |" % (record["id"], mark, " | ".join(cells)))
        w("")

    # buildings
    w("## Buildings (`arm\\building.arm`)")
    w("")
    w("| ARM | Building (stages) | Slots |")
    w("|---|---|---|")
    for record in buildings:
        tree = record["tree"]
        slots = "; ".join("%d: %s" % (i, weapon_cell(data, c)) for i, c in enumerate(tree["children"])) or "-"
        w("| %d | %s | %s |" % (record["id"], stage_chain(data, tree["ids"]), slots))
    w("")

    # weapon / turret stages
    w("## Weapon and turret models")
    w("")
    w("Upgrade stages of every model that sits in a slot above, with the technology each stage needs.")
    w("")
    w("| Model | Stages (id name, required TEC) |")
    w("|---|---|")
    for base, ids in sorted(collect_models(data, units + buildings).items()):
        w("| %d %s | %s |" % (base, data.name(base) or "?", stage_chain(data, ids)))
    w("")

    # technologies used above
    used = set()
    for record in units + buildings:
        stack = [record["tree"]]
        while stack:
            node = stack.pop()
            stack += node["children"]
            used |= {data.tec(d) for d in node["ids"] if d and data.tec(d) is not None}
    w("## Technologies referenced above")
    w("")
    w("| TEC | Enumerator | Name in game |")
    w("|---|---|---|")
    for tec in sorted(used):
        text = " ".join(data.techno[2 * tec].split()) if 2 * tec < len(data.techno) else ""
        w("| %d | `%s` | %s |" % (tec, data.tec_enum.get(tec, "?"), text))
    w("")
    os.makedirs(os.path.dirname(out), exist_ok=True)
    with open(out, "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(lines))
    print("wrote", out, "(%d units, %d buildings)" % (len(units), len(buildings)))


ASCII = {"ä": "ae", "ö": "oe", "ü": "ue", "Ä": "Ae", "Ö": "Oe", "Ü": "Ue",
         "ß": "ss"}


def enum_label(data, record):
    """'Chassis: weapon, weapon' (repeats as '2x weapon'), ASCII only (the sources are ASCII)."""
    tree = record["tree"]
    text = data.name(tree["ids"][0]) or "?"
    if tree["children"]:
        names = [weapon_cell(data, c).split(" ", 1)[1] for c in tree["children"]]
        parts = []
        for name in names:
            if parts and parts[-1][1] == name:
                parts[-1][0] += 1
            else:
                parts.append([1, name])
        text += ": " + ", ".join(name if n == 1 else "%dx %s" % (n, name) for n, name in parts)
    elif record["file"] == "unit.arm":
        text += " (bare)"
    return "".join(ASCII.get(c, c) for c in text)


def rewrite_enum_comments(data, records, header):
    labels = {r["id"]: enum_label(data, r) for r in records}
    pattern = re.compile(r"^(    ARM_(\d+)_(?:UNIT|BUILDING)_MDL\d+=\d+,?)(?: /\*.*\*/)?$")
    out = []
    changed = 0
    for line in open(header, encoding="utf-8").read().split("\n"):
        m = pattern.match(line)
        if m and int(m.group(2)) in labels:
            label = labels[int(m.group(2))].replace("*/", "* /")
            new = "%s /* %s */" % (m.group(1), label)
            changed += new != line
            line = new
        out.append(line)
    with open(header, "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(out))
    print("rewrote", changed, "enumerator comments in", header)


def check(data, records):
    """Prints the records whose tree has more children than the root model has attachment points: the extra
    children are not created (ModelRuntimePool_RepairDeferredChild's out-of-range branch)."""
    problems = 0
    for record in records:
        stack = [record["tree"]]
        while stack:
            node = stack.pop()
            stack += node["children"]
            if not node["children"]:
                continue
            for def_id in node["ids"]:
                points = data.attachment_points(def_id) if def_id else None
                if points is not None and points < len(node["children"]):
                    problems += 1
                    print("ARM %d: model %d has %d attachment points, tree node has %d children" %
                          (record["id"], def_id, points, len(node["children"])))
    print("%d mismatches" % problems)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("game_dir", nargs="?", default=DEFAULT_GAME_DIR)
    ap.add_argument("--out", default=os.path.join(REPO, "docs", "reference", "army_units.md"))
    ap.add_argument("--enum-comments")
    ap.add_argument("--check", action="store_true")
    args = ap.parse_args()
    data = Data(args.game_dir)
    units = load_records(data.packages, "unit.arm")
    buildings = load_records(data.packages, "building.arm")
    if args.check:
        check(data, units + buildings)
        return
    write_markdown(data, units, buildings, args.out)
    if args.enum_comments:
        rewrite_enum_comments(data, units + buildings, args.enum_comments)


if __name__ == "__main__":
    main()
