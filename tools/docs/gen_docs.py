"""Regenerates the source documentation from the tree.

usage: python tools/docs/gen_docs.py [--check]

Writes
  docs/MODULE_TREE.md          every module (src/<area>/<module>/) with its .cpp and .h files
  docs/SOURCE_FILE_GUIDE.md    index: every module and file with its purpose (the file comment of the .cpp, else
                               of the .h)
  docs/source_guide/<area>.md  per source/header pair: purpose, the main functions with the first sentence of their
                               header comment, the data it defines, which other files call into it and which files
                               it depends on
  docs/original_addresses.txt  the file column (where each original function/variable is defined now)

Everything is read from src/ and include/thandor/ with the lexical index in source_index.py: file comments,
function header comments, #include lines and a caller scan by name (names are unique; a file-local static wins
inside its own file). --check only reports whether the documents are up to date."""
import collections
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import source_index as si  # noqa: E402

ROOT = si.ROOT
DOCS = ROOT / "docs"
AREA_ORDER = ["assets", "audio", "core", "gameplay", "graphics", "movie", "network", "platform", "ui", "world",
              "generated"]
AREA_TITLES = {
    "assets": "asset loading, catalogs and resource formats",
    "audio": "sound backend slots, sample codec and positioned sound",
    "core": "error handling, fixed-point maths, memory, settings and strings",
    "gameplay": "armies, AI, factions, selection, input commands and the in-game session",
    "graphics": "renderer backends, framebuffer, textures, palettes and the render pipeline",
    "movie": "FLM movie playback and encoding",
    "network": "network transport and the session/transfer protocol",
    "platform": "start-up, SDL3 platform layer, file system, system services and developer tools",
    "ui": "control framework, dialogs, front end menus and the in-game interface",
    "world": "world runtime: camera, terrain, pathing, models, effects and shots",
    "generated": "the remaining Windows import declarations",
}
# changelog sections written for a module that has been renamed since
CHANGELOG_RENAMED = {"world/motion": "world/camera"}
MAX_FUNCTIONS = 10
MAX_CALLER_NAMES = 2
MAX_CALLER_FILES = 10
# included by (nearly) every file; not listed under Includes
COMMON_INCLUDES = {"thandor/thandor.h", "thandor/core/types.h", "thandor/core/contracts.h"}


def module_of(rel):
    """(area, module, stem) of a file; module '' for area-level files."""
    parts = rel.split("/")
    if parts[0] == "src":
        inner = parts[1:]
    else:  # include/thandor/...
        inner = parts[2:]
    stem = inner[-1].rsplit(".", 1)[0]
    if len(inner) == 1:
        return ("", "", stem)
    if len(inner) == 2:
        return (inner[0], "", stem)
    return (inner[0], inner[1], stem)


class Unit:
    """A source/header pair (or a lone source or header) of one module."""

    def __init__(self, area, module, stem):
        self.area, self.module, self.stem = area, module, stem
        self.source = None
        self.header = None
        self.private_header = None

    @property
    def files(self):
        return [f for f in (self.source, self.header, self.private_header) if f]

    @property
    def key(self):
        return "/".join(p for p in (self.area, self.module, self.stem) if p)

    @property
    def anchor(self):
        return "file-" + self.key.replace("/", "-").replace("_", "-")

    @property
    def title(self):
        names = [f.path.name for f in self.files]
        return " / ".join(names)

    def functions(self):
        return [fn for f in self.files for fn in f.functions]

    def variables(self):
        return [v for f in self.files for v in f.variables]


def link(rel, line=None):
    return "../" + rel + (f"#L{line}" if line else "")


def md_escape(text):
    return text.replace("|", "\\|").replace("<", "&lt;").replace(">", "&gt;")


def build(files):
    by_rel = {f.rel: f for f in files}
    units = {}
    module_umbrellas = {}
    area_headers = collections.defaultdict(list)
    top_headers = []
    module_dirs = {(p.parent.name, p.name) for p in (ROOT / "src").glob("*/*") if p.is_dir()}
    module_dirs |= {(p.parent.name, p.name) for p in (ROOT / "include" / "thandor").glob("*/*") if p.is_dir()}
    for f in files:
        area, module, stem = module_of(f.rel)
        if not area:
            top_headers.append(f)  # thandor.h and the area umbrella headers
            continue
        if not module and f.rel.startswith("include/") and (area, stem) in module_dirs:
            module_umbrellas[(area, stem)] = f
            continue
        unit = units.setdefault((area, module, stem), Unit(area, module, stem))
        if f.rel.endswith((".cpp", ".c")):
            unit.source = f
        elif f.rel.startswith("include/"):
            unit.header = f
        else:
            unit.private_header = f
    return units, module_umbrellas, top_headers


def resolve_tables(units):
    """name -> unit for the non-static definitions, and per unit its file-local statics."""
    global_table = {}
    local = collections.defaultdict(dict)
    for unit in units.values():
        for d in unit.functions() + unit.variables():
            if d.is_static:
                local[id(unit)][d.name] = d
            else:
                prev = global_table.get(d.name)
                # a header declaration-only match never happens (only definitions are indexed); prefer sources
                if prev is None or (prev[1].source is None and unit.source is not None):
                    global_table[d.name] = (d, unit)
    return global_table, local


def cross_references(units):
    """callers[unit] = {caller_unit: set(caller names)}, uses[unit] = {callee_unit: set(callee names)}."""
    global_table, local = resolve_tables(units)
    callers = collections.defaultdict(lambda: collections.defaultdict(set))
    callee_names = collections.defaultdict(lambda: collections.defaultdict(set))
    uses = collections.defaultdict(lambda: collections.defaultdict(set))
    for unit in units.values():
        mine = local[id(unit)]
        for d in unit.functions() + unit.variables():
            for name in d.uses:
                if name in mine or name == d.name:
                    continue
                hit = global_table.get(name)
                if not hit or hit[1] is unit:
                    continue
                target_def, target = hit
                callers[target.key][unit.key].add(d.name)
                callee_names[target.key][unit.key].add(name)
                uses[unit.key][target.key].add(name)
    return callers, callee_names, uses, global_table


def changelog_anchors():
    anchors = {}
    for doc in ("CHANGELOG.md", "CHANGELOG_FULL.md"):
        path = ROOT / doc
        if not path.exists():
            continue
        for m in re.finditer(r'id="module-([^"]+)"', path.read_text(encoding="utf-8")):
            anchors.setdefault(m.group(1), set()).add(doc)
    return anchors


def module_changelog_links(area, module, anchors, prefix):
    names = [f"{area}/{module}"] + [old for old, new in CHANGELOG_RENAMED.items() if new == f"{area}/{module}"]
    out = []
    for anchor in sorted(anchors):
        for name in names:
            dashed = name.replace("/", "-")
            if anchor.startswith(dashed + "-"):
                label = anchor[len(dashed) + 1:]
                if name != f"{area}/{module}":
                    label = f"{name}/{label}"
                docs = anchors[anchor]
                parts = []
                if "CHANGELOG.md" in docs:
                    parts.append(f"[dev]({prefix}CHANGELOG.md#module-{anchor})")
                if "CHANGELOG_FULL.md" in docs:
                    parts.append(f"[full]({prefix}CHANGELOG_FULL.md#module-{anchor})")
                out.append(f"`{label}` " + " · ".join(parts))
    return out


def family_summary(functions):
    families = collections.Counter(fn.name.split("_")[0] for fn in functions if "_" in fn.name)
    return ", ".join(f"`{name}_*` ({count})" for name, count in families.most_common(4))


# files whose first comment is not a description of the file
PURPOSE_OVERRIDES = {
    "core/types": "The common types: Bool8, the fixed-point scalars, angles, vectors, ids and the other small types "
                  "used all over the program, in the original's 32-bit layouts.",
    "generated/imports": "Declarations of the Windows API functions the remaining code calls (the original's import "
                         "table, KERNEL32 and friends).",
}


def purpose(unit):
    if unit.key in PURPOSE_OVERRIDES:
        return PURPOSE_OVERRIDES[unit.key]
    if unit.stem == "types" and unit.module:
        return ("The types of the module (structs, unions, enums and scalar typedefs in the original's 32-bit "
                "layouts, pointer fields as Ptr32): the ones only it uses and the shared ones it owns.")
    for f in (unit.source, unit.header, unit.private_header):
        if f and f.comment:
            return si.first_sentences(f.comment, 2, 420)
    return ""


def sorted_units(units):
    def order(item):
        (area, module, stem), _ = item
        area_index = AREA_ORDER.index(area) if area in AREA_ORDER else len(AREA_ORDER)
        return (area_index, module, stem)
    return [u for _, u in sorted(units.items(), key=order)]


def modules_of(units):
    grouped = collections.OrderedDict()
    for unit in sorted_units(units):
        grouped.setdefault((unit.area, unit.module), []).append(unit)
    return grouped


def module_label(area, module):
    return f"{area}/{module}" if module else f"{area}"


def module_anchor(area, module):
    return "module-" + (f"{area}-{module}" if module else area).replace("_", "-")


# ---- MODULE_TREE.md ---------------------------------------------------------------------------------------------
def write_module_tree(units, umbrellas, top_headers, anchors):
    lines = [
        "# Module tree",
        "",
        "Browse the source by ownership: every module (`src/<area>/<module>/`) with its implementation files and "
        "public headers (`include/thandor/<area>/<module>/`). Generated by "
        "[`tools/docs/gen_docs.py`](../tools/docs/gen_docs.py); do not edit by hand.",
        "",
        "[Source file guide](SOURCE_FILE_GUIDE.md) · [Developer changelog](../CHANGELOG.md) · "
        "[Full recovery changelog](../CHANGELOG_FULL.md) · [Umbrella header](../include/thandor/thandor.h)",
        "",
        "The changelog sections describe the files as they were before step 5 split them; they are listed under the "
        "module that holds the code now.",
        "",
    ]
    grouped = modules_of(units)
    current_area = None
    total_files = sum(len(u.files) for u in units.values())
    total_functions = sum(len(u.functions()) for u in units.values())
    lines[2] += f" {len(grouped)} modules, {total_files} files, {total_functions} function definitions."
    for (area, module), members in grouped.items():
        if area != current_area:
            current_area = area
            lines += ["", f"## {area.capitalize()}", ""]
            if area in AREA_TITLES:
                lines += [AREA_TITLES[area][0].upper() + AREA_TITLES[area][1:] + ".", ""]
            area_header = f"include/thandor/{area}.h"
            if (ROOT / area_header).exists():
                lines += [f"Area header: [`{area}.h`]({link(area_header)})", ""]
            lines += ["| Module | Functions | Sources | Headers | Changes |", "| --- | ---: | --- | --- | --- |"]
        sources = [u.source for u in members if u.source] + [u.private_header for u in members if u.private_header]
        headers = []
        if (area, module) in umbrellas:
            headers.append(umbrellas[(area, module)])
        headers += [u.header for u in members if u.header]
        source_cell = ", ".join(f"[`{f.path.name}`]({link(f.rel)})" for f in sources) or "-"
        header_cell = ", ".join(f"[`{f.path.name}`]({link(f.rel)})" for f in headers) or "-"
        functions = sum(len(u.functions()) for u in members)
        changes = "<br>".join(module_changelog_links(area, module, anchors, "../")) if module else ""
        lines.append(f"| [`{module_label(area, module)}`](SOURCE_FILE_GUIDE.md#{module_anchor(area, module)}) | "
                     f"{functions} | {source_cell} | {header_cell} | {changes} |")
    return "\n".join(lines) + "\n"


# ---- SOURCE_FILE_GUIDE.md and docs/source_guide/<area>.md ------------------------------------------------------
GUIDE_DIR = "source_guide"


def unit_ref(unit, from_area):
    """Link target of a unit's section, seen from the page of from_area (None: the index page)."""
    if from_area is None:
        return f"{GUIDE_DIR}/{unit.area}.md#{unit.anchor}"
    if unit.area == from_area:
        return f"#{unit.anchor}"
    return f"{unit.area}.md#{unit.anchor}"


def area_sentence(area):
    return AREA_TITLES[area][0].upper() + AREA_TITLES[area][1:] + "." if area in AREA_TITLES else ""


def write_guide(units, umbrellas, anchors):
    """The index page and one detail page per area: {path relative to docs/: text}."""
    callers, callee_names, uses, _ = cross_references(units)
    by_key = {u.key: u for u in units.values()}
    grouped = modules_of(units)
    areas = list(dict.fromkeys(area for area, _ in grouped))
    index = [
        "# Source file guide",
        "",
        "What each source/header pair owns, who calls into it and what it depends on. Generated by "
        "[`tools/docs/gen_docs.py`](../tools/docs/gen_docs.py) from the file comments, the function header comments, "
        "the `#include` lines and a scan of the function bodies for names defined elsewhere; do not edit by hand.",
        "",
        "[Module tree](MODULE_TREE.md) · [Developer changelog](../CHANGELOG.md) · "
        "[Full recovery changelog](../CHANGELOG_FULL.md) · [Umbrella header](../include/thandor/thandor.h)",
        "",
        "This page lists every file with its purpose (the first sentence of its file comment; files without one show "
        "their function families). The area pages under `source_guide/` hold the details per file: the public "
        "functions with the first sentence of their comment, the data it defines, **Called from** (other files whose "
        "functions or data initialisers name one of its functions or variables) and **Depends on** (the files that "
        "define what it uses).",
        "",
        "Areas: " + " · ".join(f"[{area}]({GUIDE_DIR}/{area}.md)" for area in areas),
        "",
    ]
    area_pages = collections.OrderedDict()
    for (area, module), members in grouped.items():
        page = area_pages.get(area)
        if page is None:
            page = area_pages[area] = [
                f"# Source file guide: {area}",
                "",
                area_sentence(area) + " Generated by [`tools/docs/gen_docs.py`](../../tools/docs/gen_docs.py); do "
                "not edit by hand. Every implementation file includes `thandor/thandor.h` (and with it "
                "the module type headers and `core/contracts.h`); **Includes** lists only the other headers the source "
                "pulls in.",
                "",
                "[Source file guide](../SOURCE_FILE_GUIDE.md) · [Module tree](../MODULE_TREE.md) · Areas: "
                + " · ".join(f"[{a}]({a}.md)" for a in areas),
                "",
            ]
            index += [f"## [{area.capitalize()}]({GUIDE_DIR}/{area}.md)", ""]
            if area in AREA_TITLES:
                index += [area_sentence(area), ""]
        title = f"### `{module_label(area, module)}`" + ("" if module else " (area-level files)")
        anchor = f'<a id="{module_anchor(area, module)}"></a>'
        meta = []
        if (area, module) in umbrellas:
            umbrella = umbrellas[(area, module)]
            meta.append(f"Module header: [`{umbrella.path.name}`]({link(umbrella.rel)})")
        if module:
            changes = module_changelog_links(area, module, anchors, "../")
            if changes:
                meta.append("Changelog: " + "; ".join(changes))
        index += [anchor, title, ""]
        if meta:
            index += [" · ".join(meta), ""]
        for unit in members:
            text = purpose(unit)
            if text:
                summary = si.first_sentence(text, 200)
            elif unit.functions():
                summary = "no file comment; main functions " + ", ".join(
                    f"`{fn.name}`" for fn in ranked_functions(unit, callee_names[unit.key])[:3])
            else:
                summary = ""
            index.append(f"- [`{unit.title}`]({unit_ref(unit, None)})"
                         + (f" - {md_escape(summary)}" if summary else ""))
        index.append("")
        page_meta = [m.replace("](../", "](../../") for m in meta]
        page_meta.append("Files: " + ", ".join(f"[`{u.stem}`](#{u.anchor})" for u in members))
        page += [anchor, title, "", " · ".join(page_meta), ""]
        for unit in members:
            page += unit_section(unit, by_key, callers[unit.key], callee_names[unit.key], uses[unit.key])
    pages = {"SOURCE_FILE_GUIDE.md": "\n".join(index).rstrip() + "\n"}
    for area, page in area_pages.items():
        pages[f"{GUIDE_DIR}/{area}.md"] = "\n".join(page).rstrip() + "\n"
    return pages


def ranked_functions(unit, unit_callee_names):
    """The public functions (file-local ones if there are none), most widely used from other files first."""
    functions = [fn for fn in unit.functions() if not fn.is_static] or unit.functions()
    return sorted(functions, key=lambda fn: (-sum(1 for names in unit_callee_names.values() if fn.name in names),
                                             fn.line))


def unit_section(unit, by_key, unit_callers, unit_callee_names, unit_uses):
    deep = "../"  # the area pages live one directory below docs/
    out = [f'<a id="{unit.anchor}"></a>', f"#### `{unit.title}`", ""]
    refs = []
    if unit.source:
        refs.append(f"[Source]({deep}{link(unit.source.rel)})")
    if unit.private_header:
        refs.append(f"[Private header]({deep}{link(unit.private_header.rel)})")
    if unit.header:
        refs.append(f"[Header]({deep}{link(unit.header.rel)})")
    out += [" · ".join(refs), ""]
    functions = unit.functions()
    text = purpose(unit)
    if text:
        out += [md_escape(text), ""]
    elif functions:
        out += [f"No file comment; function families: {family_summary(functions) or '-'}.", ""]
    public = [fn for fn in functions if not fn.is_static]
    local = [fn for fn in functions if fn.is_static]
    if functions:
        head = f"**Functions** ({len(public)} public"
        head += f", {len(local)} file-local)" if local else ")"
        if public:
            out += [head + ":", ""]
            ranked = ranked_functions(unit, unit_callee_names)
            shown = sorted(ranked[:MAX_FUNCTIONS], key=lambda fn: fn.line)
            for fn in shown:
                file = next(f for f in unit.files if fn in f.functions)
                summary = si.first_sentence(fn.comment, 200) if fn.comment else ""
                entry = f"- [`{fn.name}`]({deep}{link(file.rel, fn.line)})"
                if summary:
                    entry += " - " + md_escape(summary)
                out.append(entry)
            rest = [fn for fn in public if fn not in shown]
            if rest:
                out.append(f"- {len(rest)} more: "
                           + ", ".join(f"`{fn.name}`" for fn in sorted(rest, key=lambda f: f.line)))
            out.append("")
        else:
            out += [head + ".", ""]
    variables = unit.variables()
    if variables:
        public_vars = [v for v in variables if not v.is_static]
        names = ", ".join(f"`{v.name}`" for v in (public_vars or variables)[:8])
        more = len(public_vars or variables) - 8
        out += [f"**Data** ({len(public_vars)} shared, {len(variables) - len(public_vars)} file-local): {names}"
                + (f" and {more} more" if more > 0 else "") + ".", ""]
    if unit_callers:
        entries = []
        ordered = sorted(unit_callers, key=lambda k: (-len(unit_callers[k]), k))
        for key in ordered[:MAX_CALLER_FILES]:
            names = sorted(unit_callers[key])
            shown = ", ".join(f"`{n}`" for n in names[:MAX_CALLER_NAMES])
            if len(names) > MAX_CALLER_NAMES:
                shown += f" +{len(names) - MAX_CALLER_NAMES}"
            entries.append(f"[`{key}`]({unit_ref(by_key[key], unit.area)}) ({shown})")
        if len(ordered) > MAX_CALLER_FILES:
            entries.append(f"{len(ordered) - MAX_CALLER_FILES} more: " + ", ".join(
                f"[`{key}`]({unit_ref(by_key[key], unit.area)})" for key in ordered[MAX_CALLER_FILES:]))
        out += [f"**Called from** ({len(unit_callers)} files): " + "; ".join(entries) + ".", ""]
    elif functions or variables:
        out += ["**Called from:** no other file (entry points, slots filled at run time or file-local use).", ""]
    if unit_uses:
        ordered = sorted(unit_uses, key=lambda k: (-len(unit_uses[k]), k))
        entries = [f"[`{key}`]({unit_ref(by_key[key], unit.area)}) ({len(unit_uses[key])})" for key in ordered]
        out += [f"**Depends on** ({len(ordered)} files, names used): " + ", ".join(entries) + ".", ""]
    includes = []
    own = {f"thandor/{unit.key}.h"} | COMMON_INCLUDES
    for f in [unit.source or unit.header or unit.private_header]:
        for inc in f.includes:
            if inc not in own and inc not in includes:
                includes.append(inc)
    if includes:
        out += ["**Includes:** " + ", ".join(f"`{inc}`" for inc in includes) + ".", ""]
    return out


# ---- original_addresses.txt -------------------------------------------------------------------------------------
ADDRESS_LINE = re.compile(r"^(0x[0-9A-Fa-f]{8}\s+\S+\s+(\S+)\s+)(\S.*?)\s*$")


def update_addresses(files):
    table = si.definitions(files)
    path = DOCS / "original_addresses.txt"
    out = []
    missing = []
    for line in path.read_text(encoding="utf-8").splitlines():
        m = ADDRESS_LINE.match(line)
        if not m:
            out.append(line)
            continue
        prefix, name, old = m.groups()
        hit = table.get(name)
        if hit:
            new = hit[0].rel
        elif old.startswith("(removed"):
            new = old
        else:
            new = "(removed)"
            missing.append(name)
        out.append(prefix + new)
    text = "\n".join(out) + "\n"
    text = re.sub(
        r"# Written once from the address comments in src/ when they were removed \(step 5c\); the code no longer\n"
        r"# mentions original addresses\. Columns: address, kind, C name, file at the time of writing\.\n",
        "# Written once from the address comments in src/ when they were removed (step 5c); the code no longer\n"
        "# mentions original addresses. Columns: address, kind, C name, file that defines it now (updated by\n"
        "# tools/docs/gen_docs.py). \"(removed)\": deleted with the Win32/DirectX backend that SDL3 replaced (step 5f);\n"
        "# \"(removed, unused)\": dropped earlier because nothing used it.\n",
        text)
    return text, missing


def main():
    check = "--check" in sys.argv
    files = si.load_tree()
    units, umbrellas, top_headers = build(files)
    anchors = changelog_anchors()
    outputs = {DOCS / "MODULE_TREE.md": write_module_tree(units, umbrellas, top_headers, anchors)}
    for rel, text in write_guide(units, umbrellas, anchors).items():
        outputs[DOCS / rel] = text
    # area pages of areas that no longer exist
    stale_pages = [page for page in (DOCS / GUIDE_DIR).glob("*.md") if page not in outputs]
    addresses, missing = update_addresses(files)
    outputs[DOCS / "original_addresses.txt"] = addresses
    stale = [page.relative_to(ROOT).as_posix() for page in stale_pages]
    if not check:
        for page in stale_pages:
            page.unlink()
    for path, text in outputs.items():
        current = path.read_text(encoding="utf-8").replace("\r\n", "\n") if path.exists() else None
        if current != text:
            stale.append(path.relative_to(ROOT).as_posix())
            if not check:
                path.parent.mkdir(parents=True, exist_ok=True)
                with open(path, "w", encoding="utf-8", newline="\r\n") as handle:
                    handle.write(text)
    if missing:
        print(f"{len(missing)} original symbols not defined any more, marked (removed): " + ", ".join(missing[:10])
              + (" ..." if len(missing) > 10 else ""))
    if check:
        print("out of date: " + ", ".join(stale) if stale else "docs are up to date")
        return 1 if stale else 0
    print("written: " + (", ".join(stale) if stale else "nothing changed"))
    return 0


if __name__ == "__main__":
    sys.exit(main())
