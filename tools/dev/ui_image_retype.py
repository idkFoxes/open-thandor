#!/usr/bin/env python3
"""Retype the nodes of a UI template image struct (step 13, item 2, D1 (a)).

The template images (``InGameUiImage``, ``FrontendUiImage``, ``DisplaySettingsUiImage``,
``FourValueDialogUiImage``, ``FatalErrorUiImage``) declare every node as a generic pair

    UiNodeBase okButton; /* +00B4 g_UiFramedTextButtonControlVtable: ... */
    uint32_t okButton_fields[4];          (or a named "template fields" struct)

This tool rewrites such a pair into one member of the node's control type (chosen by the vtable named
in the member comment) and rewrites the matching initialiser of the image variable into designated
initialisers of that type, with exactly the same bytes:

    UiListOffsetControl errorMessageText; /* +0058 g_UiListOffsetControlVtable: ... */

    { /* +0058 errorMessageText g_UiListOffsetControlVtable */
        .base = {
            .nextSibling = ..., .vtable = THANDOR_PTR(&g_UiListOffsetControlVtable), ...},
        .labelFlags = 0x00000015},

Usage (from the repository root; without --write it only prints the report):

    python tools/dev/ui_image_retype.py --image FatalErrorUiImage \\
        --header include/thandor/ui/dialogs/types.h --source src/ui/dialogs/fatal_error.cpp [--write]
    python tools/dev/ui_image_retype.py --image DisplaySettingsUiImage \\
        --header include/thandor/ui/dialogs/types.h --source src/ui/dialogs/display_settings.cpp \\
        --node applyButton=UiDisplaySettingsApplyButton --node colorBiasValueText=UiDisplaySettingsValueReadout
    ... --asserts      also print static_asserts (node offsets, member sizes) for src/core/layout_checks.cpp
    ... --only a,b     retype only these nodes (the others are reported and left alone)

What it does per node (the report prints one line per node with the outcome):

- The node's extent is the bytes from the node to the next member of the image (the ``UiNodeBase`` plus its
  ``_fields`` member). Its type is ``--node name=Type`` if given, else ``VTABLE_TYPES[vtable]``. A class template
  whose last member is an array sized by its parameter (``UiLayoutContainerControl<N>``) gets the N that fills
  the extent.
- type size == extent: the pair becomes ``Type name;``.
- type size < extent: ``Type name;`` plus ``uint32_t name_trailing[k];`` for the dwords behind the control
  (in the in-game image mostly the next node's tooltip text id). They keep their values.
- type size > extent ("truncated": the original image ends the node before the class's last fields, e.g. a
  text button's activationSound overlaps the next node): the node stays ``UiNodeBase``; if
  ``FIELDS_FALLBACK[vtable]`` names a fields struct of exactly the extent, an untyped ``uint32_t name_fields[N]``
  becomes that struct (named initialisers), else the pair is left as it is.
- unknown vtable, a member that is no ``UiNodeBase``, or a value the tool cannot place (see limits): left as
  it is and reported.

How the bytes are kept: the old initialiser of the pair (the ``UiNodeBase`` block, kept as text, and the
fields block: positional dwords or designated fields of the old struct) is turned into (offset, size, value)
items; each item is placed on the field of the new type at the same offset. Literals are split or joined when
the new fields are narrower or wider (1/2-byte fields from a dword, little-endian), written signed for signed
fields (0xFFFFFFFF into int32_t becomes -1) and unsigned for unsigned ones; zero literals are dropped (the
field is zero either way). A value going into a ``Ptr32<UiNodeBase>`` field becomes ``UI_TEMPLATE_LINK(x)`` /
``UI_TEMPLATE_NO_LINK``, into another ``Ptr32<T>`` ``Thandor_U32ToPointer<T>(x)`` (e.g. a text resource id in
``UiSingleLineTextControl::text``), into an ``enum class`` field ``static_cast<E>(x)``. The check that the bytes
did not change is the ``uitemplate`` self-test (golden_cmp.ps1 -Tests uitemplate) plus the sizeof/offsetof
asserts: run both after every use.

Limits:
- Type sizes come from a small parser of the headers below include/thandor (structs, ``typedef struct``,
  one-parameter class templates, enums with a fixed underlying type, ``using``/``typedef`` aliases,
  ``#pragma pack(push, 1)`` regions). Anonymous nested unions/structs and bit-fields are not supported; a node
  whose type needs them is reported as an error. The computed node offsets are checked against the ``+XXXX``
  in the member comments and must match.
- A non-literal value (a macro, an expression) must land on a field of the same offset and size; it is not
  split or joined.
- Comments inside an old fields block move to their own lines in front of the new element (repeats once); the
  comments in the UiNodeBase block and between elements stay where they are. ``Thandor_PointerToU32(x)`` in a
  dword that becomes a ``Ptr32<T>`` field is written ``THANDOR_PTR(x)`` (same 32-bit value).
- The users of the image (``*_UI(root, node)`` and direct member accesses such as ``image.node.rightOffset``)
  are not rewritten: a node that became typed needs ``.base.`` (``.root.base.``, ``.selectable.base.``) on its
  UiNodeBase fields, and casts to the new type become unnecessary. The compiler finds the first kind.
- ``--node`` overrides must be layout-compatible views of the node (e.g. UiDisplaySettingsApplyButton for a
  framed text button with extra tail fields); the tool checks only their size. Every type it writes into the
  image header must be declared before the image struct (a view type from a module header has to move into the
  types header, as UiDisplaySettingsApplyButton did in U2).
- A class ending in a variable-length array (``UiConditionalActionControl::textLines[1]``) is sized with that
  one slot, so the other slots end up in ``name_trailing``; give such nodes a ``--node`` type with the real slot
  count if the slots should be named. Check the report's "trailing" lines for such cases.
- Dry runs at the time of U2: InGameUiImage 407 typed, 26 trailing, 12 fields, 4 truncated, 3 without a type
  (g_FrontendResultsTableVtable); FrontendUiImage 235 typed, 16 trailing, 1 truncated. No placement errors.
"""

import argparse
import os
import re
import sys

# Control type per template vtable (step 13 U1 named the missing ones). Extend for U3/U4 as needed.
VTABLE_TYPES = {
    "g_UiPanelControlVtable": "UiPanelControl",
    "g_UiResizableWindowControlVtable": "UiResizableWindowControl",
    "g_UiTitledWindowControlVtable": "UiTitledWindowControl",
    "g_UiTextButtonControlVtable": "UiTextButtonControl",
    "g_UiFramedTextButtonControlVtable": "UiFramedTextButtonControl",
    "g_UiGraphicsAdapterTextButtonVtable": "UiTextButtonControl",
    "g_UiNumericPairTextButtonVtable": "UiNumericPairTextButton",
    "g_UiPayloadPairTextButtonVtable": "UiPayloadPairTextButton",
    "g_UiFocusProxyControlVtable": "UiFocusProxyControl",
    "g_UiListOffsetControlVtable": "UiListOffsetControl",
    "g_UiCommandVisibilitySingleLineTextVtable": "UiCommandVisibilitySingleLineText",
    "g_UiCommandVisibilityWrappedTextVtable": "UiCommandVisibilityWrappedText",
    "g_UiRangeSliderControlVtable": "UiRangeSliderControl",
    "g_UiLayoutContainerControlVtable": "UiLayoutContainerControl",
    "g_UiSpriteButtonControlVtable": "UiSpriteButtonControl",
    "g_UiCommandSpriteButtonControlVtable": "UiCommandSpriteButtonControl",
    "g_UiCommandSpriteButtonWithDetailsVtable": "UiCommandSpriteButtonWithDetails",
    "g_UiImageControlVtable": "UiImageControl",
    "g_UiImageActionControlVtable": "UiImageActionControl",
    "g_UiConditionalActionControlVtable": "UiConditionalActionControl",
    "g_UiRequiredTextEditControlVtable": "UiRequiredTextEditControl",
    "g_UiSelectionGeometryControlVtable": "UiSelectionGeometryControl",
    "g_UiTransferProgressGaugeVtable": "UiHorizontalGaugeControl",
    "g_UiCatalogEntryControlVtable": "UiCatalogEntryControl",
    "g_UiImagePanelControlVtable": "UiImagePanelControl",
    "g_UiArmyMetricsPanelVtable": "UiArmyMetricsPanel",
    "g_UiScrollableControlVtable": "UiScrollableControl",
    "g_UiNineSlicePanelControlVtable": "UiNineSlicePanelControl",
    "g_UiFillPanelControlVtable": "UiFillPanelControl",
    "g_UiFormattedContainerVtable": "UiFormattedContainer",
    "g_UiListControlVtable": "UiListControl",
    "g_UiTextListControlVtable": "UiTextListControl",
    "g_UiSoftwareTexturePreviewControlVtable": "UiSoftwareTexturePreviewControl",
    "g_FrontendModelPointerContextVtable": "FrontendModelPointerContext",
}

# Fields struct for a truncated node (class longer than the template extent) of these vtables.
FIELDS_FALLBACK = {
    "g_UiTextButtonControlVtable": "UiTextButtonTemplateFields",
    "g_UiFramedTextButtonControlVtable": "UiTextButtonTemplateFields",
    "g_UiGraphicsAdapterTextButtonVtable": "UiTextButtonTemplateFields",
    "g_UiRangeSliderControlVtable": "UiRangeSliderTemplateFields",
}

NODE_BASE = "UiNodeBase"
NODE_BASE_SIZE = 0x4C

SCALARS = {
    "char": (1, True), "int8_t": (1, True), "uint8_t": (1, False), "bool": (1, False), "Bool8": (1, False),
    "int16_t": (2, True), "uint16_t": (2, False), "wchar_t": (2, False), "char16_t": (2, False),
    "int": (4, True), "int32_t": (4, True), "unsigned": (4, False), "uint32_t": (4, False), "float": (4, True),
    "long": (4, True), "UPtr32": (4, False), "int64_t": (8, True), "uint64_t": (8, False), "double": (8, True),
    "unsigned int": (4, False), "signed int": (4, True), "short": (2, True), "unsigned short": (2, False),
    "unsigned char": (1, False), "signed char": (1, True), "unsigned long": (4, False), "long long": (8, True),
    "unsigned long long": (8, False),
}


class GenError(Exception):
    pass


# ---------------------------------------------------------------------------------------------------------------
# Types


class Type:
    """kind: scalar | enum | ptr | struct. Struct fields are (name, type expression text, dims list)."""

    def __init__(self, name, kind, size=0, align=4, signed=False, scoped=False, target=None, fields=None,
                 packed=False, tparam=None):
        self.name, self.kind, self.size, self.align = name, kind, size, align
        self.signed, self.scoped, self.target = signed, scoped, target
        self.fields, self.packed, self.tparam = fields or [], packed, tparam


def strip_comments(text):
    text = re.sub(r"/\*.*?\*/", lambda m: " " * 0 + "\n" * m.group(0).count("\n"), text, flags=re.S)
    return re.sub(r"//[^\n]*", "", text)


class TypeDb:
    def __init__(self, root):
        self.aliases = {}  # name -> type expression text
        self.enums = {}  # name -> Type
        self.structs = {}  # name -> (body text, packed, template param or None)
        self.consts = {}  # numeric #define / constexpr
        self.cache = {}
        inc = os.path.join(root, "include", "thandor")
        for dirpath, _, files in os.walk(inc):
            for f in files:
                if f.endswith(".h"):
                    with open(os.path.join(dirpath, f), encoding="utf-8", errors="replace") as fh:
                        self._scan(fh.read())

    def _scan(self, raw):
        for m in re.finditer(r"^\s*#\s*define\s+(\w+)\s+\(?(-?(?:0x[0-9A-Fa-f]+|\d+))[uUlL]*\)?\s*(?:/[*/].*)?$",
                             raw, re.M):
            self.consts[m.group(1)] = int(m.group(2), 0)
        text = strip_comments(raw)
        # pack regions: offsets of "#pragma pack(push, 1)" / "pop"
        packs = [(m.start(), "push" in m.group(0)) for m in re.finditer(r"#\s*pragma\s+pack\s*\(\s*(push\s*,\s*1|pop)",
                                                                       text)]

        def packed_at(pos):
            state = False
            for p, push in packs:
                if p < pos:
                    state = push
            return state

        for m in re.finditer(r"\busing\s+(\w+)\s*=\s*([^;]+);", text):
            self.aliases.setdefault(m.group(1), m.group(2).strip())
        for m in re.finditer(r"\btypedef\s+(?!struct\b|union\b|enum\b)([\w:<>\s\*,()]+?)\s+(\w+)\s*;", text):
            if "*" not in m.group(1) and "(" not in m.group(1):
                self.aliases.setdefault(m.group(2), m.group(1).strip())
        for m in re.finditer(r"\benum\s+(class\s+|struct\s+)?(\w+)\s*(?::\s*([\w:]+))?\s*\{", text):
            base = m.group(3) or "int"
            size, signed = SCALARS.get(base, (4, True))
            self.enums[m.group(2)] = Type(m.group(2), "enum", size, min(size, 4), signed, bool(m.group(1)))
        for m in re.finditer(r"(template\s*<\s*[\w:]+\s+(\w+)\s*>\s*)?(?:typedef\s+)?\bstruct\s+(\w+)\s*\{", text):
            start = m.end()
            depth, i = 1, start
            while depth and i < len(text):
                depth += {"{": 1, "}": -1}.get(text[i], 0)
                i += 1
            name = m.group(3)
            if name not in self.structs:
                self.structs[name] = (text[start:i - 1], packed_at(m.start()), m.group(2))

    def eval_dim(self, text, env):
        text = text.strip()
        if text in env:
            return env[text]
        if text in self.consts:
            return self.consts[text]
        try:
            return int(text, 0)
        except ValueError:
            raise GenError("array size %r not understood" % text)

    def get(self, expr, env=None):
        """Type for a type expression ("uint32_t", "struct UiNodeBase", "Ptr32<uint16_t>", "Foo<3>")."""
        expr = re.sub(r"\b(struct|union|enum|const|volatile)\s+", "", expr.strip()).strip()
        key = (expr, tuple(sorted((env or {}).items())))
        if key in self.cache:
            return self.cache[key]
        t = self._resolve(expr, env or {})
        self.cache[key] = t
        return t

    def _resolve(self, expr, env):
        m = re.match(r"^Ptr32\s*<(.*)>$", expr)
        if m:
            return Type(expr, "ptr", 4, 4, target=re.sub(r"\bstruct\s+", "", m.group(1)).strip())
        if expr in SCALARS:
            size, signed = SCALARS[expr]
            return Type(expr, "scalar", size, min(size, 4), signed)
        if expr in self.enums:
            return self.enums[expr]
        m = re.match(r"^(\w+)\s*<\s*([^>]+)\s*>$", expr)
        if m and m.group(1) in self.structs:
            body, packed, tparam = self.structs[m.group(1)]
            return self._struct(expr, body, packed, {tparam: self.eval_dim(m.group(2), env)} if tparam else {})
        if expr in self.structs:
            body, packed, tparam = self.structs[expr]
            if tparam:
                t = Type(expr, "template", tparam=tparam)
                return t
            return self._struct(expr, body, packed, {})
        if expr in self.aliases and self.aliases[expr] != expr:
            t = self.get(self.aliases[expr], env)
            return t
        raise GenError("type %r not found" % expr)

    def _struct(self, name, body, packed, env):
        if "{" in body or ":" in re.sub(r"::", "", body):
            raise GenError("struct %s has nested types or bit-fields (not supported)" % name)
        fields, off, align = [], 0, 1
        for decl in body.split(";"):
            decl = " ".join(decl.split())
            if not decl:
                continue
            m = re.match(r"^((?:struct\s+|union\s+|enum\s+)?(?:(?:unsigned|signed|long|short)\s+)*[\w:]+(?:\s*<[^;]*>)?)\s+(\w+)((?:\s*\[[^\]]+\])*)$", decl)
            if not m:
                raise GenError("struct %s: field %r not understood" % (name, decl))
            ftype = self.get(m.group(1), env)
            dims = [self.eval_dim(d, env) for d in re.findall(r"\[([^\]]+)\]", m.group(3))]
            count = 1
            for d in dims:
                count *= d
            falign = 1 if packed else ftype.align
            off = (off + falign - 1) // falign * falign
            fields.append((m.group(2), ftype, dims, off))
            off += ftype.size * count
            align = max(align, falign)
        size = (off + align - 1) // align * align
        return Type(name, "struct", size, align, fields=fields, packed=packed)


# ---------------------------------------------------------------------------------------------------------------
# Initialiser parsing


def skip_ws_comments(text, i):
    while i < len(text):
        if text[i].isspace():
            i += 1
        elif text.startswith("/*", i):
            i = text.index("*/", i) + 2
        elif text.startswith("//", i):
            i = text.index("\n", i)
        else:
            break
    return i


def split_top(text):
    """Split the inside of a braced list at top-level commas; returns [(start, end)] spans (raw, untrimmed)."""
    spans, depth, i, start = [], 0, 0, 0
    while i < len(text):
        c = text[i]
        if text.startswith("/*", i):
            i = text.index("*/", i) + 2
            continue
        if text.startswith("//", i):
            i = text.index("\n", i)
            continue
        if c in "\"'":
            j = i + 1
            while text[j] != c:
                j += 2 if text[j] == "\\" else 1
            i = j + 1
            continue
        if c in "({[":
            depth += 1
        elif c in ")}]":
            depth -= 1
        elif c == "," and depth == 0:
            spans.append((start, i))
            start = i + 1
        i += 1
    if text[start:].strip() and skip_ws_comments(text, start) < len(text):
        spans.append((start, len(text)))
    return spans


def match_brace(text, i):
    depth = 0
    while i < len(text):
        if text.startswith("/*", i):
            i = text.index("*/", i) + 2
            continue
        if text.startswith("//", i):
            i = text.index("\n", i)
            continue
        if text[i] == "{":
            depth += 1
        elif text[i] == "}":
            depth -= 1
            if depth == 0:
                return i
        i += 1
    raise GenError("unbalanced braces")


class Elem:
    """One element of a braced list: designator (or None), value text (braced or expression), spans."""

    def __init__(self, text, start, end):
        self.raw_start, self.raw_end = start, end
        i = skip_ws_comments(text, start)
        self.start = i
        self.designator = None
        m = re.match(r"\.(\w+)\s*=\s*", text[i:end])
        if m:
            self.designator = m.group(1)
            i += m.end()
        self.value_start = i
        self.value = text[i:end].rstrip()
        self.end = i + len(self.value)
        self.braced = self.value.startswith("{")


def parse_list(text):
    """text is "{...}"; returns the elements of the list."""
    assert text.startswith("{")
    end = match_brace(text, 0)
    inner = text[1:end]
    return [Elem(inner, s, e) for s, e in split_top(inner)]


def block_comments(text):
    """The comments inside an initialiser block, in order, without repeats."""
    seen = []
    for c in re.findall(r"/\*.*?\*/|//[^\n]*", text, re.S):
        c = " ".join(c.split())
        if c not in seen:
            seen.append(c)
    return seen


class Item:
    def __init__(self, off, size, text, typ):
        self.off, self.size, self.text, self.typ = off, size, text.strip(), typ
        self.value = literal_value(self.text, size)
        self.used = 0  # bytes placed


def literal_value(text, size):
    t = re.sub(r"[uUlL]+$", "", text.strip())
    neg = t.startswith("-")
    t = t[1:].strip() if neg else t
    if not re.fullmatch(r"0x[0-9A-Fa-f]+|\d+", t):
        return None
    v = int(t, 0)
    if neg:
        v = -v
    return v & ((1 << (8 * size)) - 1)


def collect_items(db, typ, text, off, items, warnings, where):
    """Items of an old initialiser (text: braced list or expression) of type typ at byte offset off."""
    text = text.strip()
    if typ.kind == "struct":
        if not text.startswith("{"):
            raise GenError("%s: struct value %r without braces" % (where, text))
        pos = 0
        names = [f[0] for f in typ.fields]
        for e in parse_list(text):
            if e.designator:
                if e.designator not in names:
                    raise GenError("%s: %s has no field %s" % (where, typ.name, e.designator))
                pos = names.index(e.designator)
            if pos >= len(typ.fields):
                raise GenError("%s: too many values for %s" % (where, typ.name))
            fname, ftype, dims, foff = typ.fields[pos]
            collect_field(db, ftype, dims, e.value, off + foff, items, warnings, where + "." + fname)
            pos += 1
        return
    if text.startswith("{"):
        elems = parse_list(text)
        if len(elems) != 1:
            raise GenError("%s: scalar with braced list %r" % (where, text))
        text = elems[0].value
    items.append(Item(off, typ.size, text, typ))


def collect_field(db, ftype, dims, text, off, items, warnings, where):
    if not dims:
        collect_items(db, ftype, text, off, items, warnings, where)
        return
    count = 1
    for d in dims[1:]:
        count *= d
    if not text.strip().startswith("{"):
        raise GenError("%s: array without braces" % where)
    stride = ftype.size * count
    for k, e in enumerate(parse_list(text.strip())):
        if e.designator:
            raise GenError("%s: designator in an array" % where)
        collect_field(db, ftype, dims[1:], e.value, off + k * stride, items, warnings, "%s[%d]" % (where, k))


# ---------------------------------------------------------------------------------------------------------------
# Emission


def fmt_unsigned(v):
    return "0x%X" % v if v > 9 else str(v)


def fmt_literal(v, typ):
    bits = 8 * typ.size
    if typ.signed and v >= 1 << (bits - 1):
        return str(v - (1 << bits))
    return fmt_unsigned(v)


class Placer:
    """Places items onto the leaves of a new type."""

    def __init__(self, items, where):
        self.items, self.where = items, where

    def value_for(self, off, typ):
        """Initialiser text for the leaf at off of type typ, or None when it stays zero."""
        size = typ.size
        covering = [it for it in self.items if it.off < off + size and off < it.off + it.size]
        if not covering:
            return None
        if len(covering) == 1 and covering[0].off == off and covering[0].size == size:
            it = covering[0]
            it.used += size
            return self.convert(it.text, it.value, it.typ, typ)
        # split or join literals
        v = 0
        for it in covering:
            if it.value is None:
                raise GenError("%s: value %r (offset 0x%X, %d bytes) does not line up with the new field at 0x%X "
                               "(%d bytes)" % (self.where, it.text, it.off, it.size, off, size))
            lo, hi = max(off, it.off), min(off + size, it.off + it.size)
            part = (it.value >> (8 * (lo - it.off))) & ((1 << (8 * (hi - lo))) - 1)
            v |= part << (8 * (lo - off))
            it.used += hi - lo
        return self.convert(None, v, None, typ)

    def convert(self, text, value, oldtyp, typ):
        if typ.kind == "ptr":
            target = typ.target
            if oldtyp is not None and oldtyp.kind == "ptr":
                return text
            if value == 0:
                return None
            unwrapped = re.match(r"^Thandor_PointerToU32\((.*)\)$", text or "", re.S)
            if unwrapped and value is None:
                return "THANDOR_PTR(%s)" % unwrapped.group(1).strip()
            if target == NODE_BASE:
                if value is not None:
                    return "UI_TEMPLATE_NO_LINK" if value == 0xFFFFFFFF else "UI_TEMPLATE_LINK(0x%X)" % value
                if re.match(r"^\w*LINK\b", text):
                    return text
            if value is None or (text is not None and not text.startswith("-")):
                arg = text
            else:
                arg = fmt_unsigned(value)
            return "Thandor_U32ToPointer<%s>(%s)" % (target, arg)
        if value is None:
            if typ.kind == "enum" and typ.scoped and (oldtyp is None or oldtyp.name != typ.name):
                return "static_cast<%s>(%s)" % (typ.name, text)
            return text
        if value == 0:
            return None
        if typ.kind == "enum" and typ.scoped:
            return "static_cast<%s>(%s)" % (typ.name, fmt_literal(value, typ))
        if text is not None:
            # keep the original spelling when it means the same value for the new field
            sign_ok = not (typ.signed and value >= 1 << (8 * typ.size - 1)) or text.startswith("-")
            unsigned_ok = typ.signed or not text.startswith("-")
            if sign_ok and unsigned_ok:
                return text
        return fmt_literal(value, typ)

    def check_all_used(self):
        for it in self.items:
            if it.used < it.size and it.value != 0:
                raise GenError("%s: value %r at offset 0x%X has no field in the new type" % (self.where, it.text,
                                                                                           it.off))


def emit_struct(typ, off, placer, node_text, indent):
    """Entries ([lines]) of a designated initialiser of struct typ at byte offset off."""
    entries = []
    for fname, ftype, dims, foff in typ.fields:
        o = off + foff
        if ftype.kind == "struct" and ftype.name == NODE_BASE and node_text is not None and o == 0 and not dims:
            entries.append([".%s = {" % fname] + node_text)
            continue
        if dims:
            val = emit_array(ftype, dims, o, placer, node_text, indent + 4)
        elif ftype.kind == "struct":
            sub = emit_struct(ftype, o, placer, node_text, indent + 4)
            val = join_entries(sub, indent + 4) if sub else None
        else:
            v = placer.value_for(o, ftype)
            val = [v] if v is not None else None
        if val:
            entries.append([".%s = %s" % (fname, val[0])] + val[1:])
    return entries


def emit_array(ftype, dims, off, placer, node_text, indent):
    count = 1
    for d in dims[1:]:
        count *= d
    stride = ftype.size * count
    elems = []
    for k in range(dims[0]):
        o = off + k * stride
        if len(dims) > 1:
            val = emit_array(ftype, dims[1:], o, placer, node_text, indent + 4)
        elif ftype.kind == "struct":
            sub = emit_struct(ftype, o, placer, node_text, indent + 4)
            val = join_entries(sub, indent + 4) if sub else None
        else:
            v = placer.value_for(o, ftype)
            val = [v] if v is not None else None
        elems.append(val)
    while elems and elems[-1] is None:
        elems.pop()
    if not elems:
        return None
    parts = [e if e is not None else ["{}" if ftype.kind == "struct" or len(dims) > 1 else "0"] for e in elems]
    return join_entries(parts, indent)


def join_entries(entries, indent, width=118):
    """A braced list "{a, b, ...}" from entries; single-line entries share lines, multi-line ones get their own."""
    flat = all(len(e) == 1 for e in entries)
    one = "{" + ", ".join(e[0] for e in entries) + "}"
    if flat and len(one) + indent < 100:
        return [one]
    pad = " " * indent
    lines, row = [], ""
    for e in entries:
        if len(e) == 1:
            if row and len(pad) + len(row) + 2 + len(e[0]) > width:
                lines.append(pad + row + ",")
                row = ""
            row = row + ", " + e[0] if row else e[0]
        else:
            if row:
                lines.append(pad + row + ",")
                row = ""
            lines.append(pad + e[0])
            lines.extend(e[1:-1])
            lines.append(e[-1] + ",")
    if row:
        lines.append(pad + row + ",")
    lines[-1] = lines[-1][:-1] + "}"
    return ["{"] + lines


# ---------------------------------------------------------------------------------------------------------------
# The image


class Member:
    def __init__(self, line_no, line, indent, typ_text, name, dims_text, comment):
        self.line_no, self.line, self.indent = line_no, line, indent
        self.typ_text, self.name, self.dims_text, self.comment = typ_text, name, dims_text, comment
        self.off = None
        self.size = None


def parse_image(db, header_text, image):
    m = re.search(r"typedef struct %s \{\n(.*?)\n\} %s;" % (image, image), header_text, re.S)
    if not m:
        m = re.search(r"struct %s \{\n(.*?)\n\};" % image, header_text, re.S)
    if not m:
        raise GenError("struct %s not found in the header" % image)
    first_line = header_text[:m.start(1)].count("\n")
    members = []
    in_comment = False
    for k, line in enumerate(m.group(1).split("\n")):
        stripped = line.strip()
        if in_comment or stripped.startswith("/*") or stripped.startswith("//"):
            # a comment line or block between members (kept as it is)
            in_comment = (in_comment or stripped.startswith("/*")) and "*/" not in stripped
            continue
        mm = re.match(r"^(\s*)((?:struct\s+)?[\w:]+(?:<[^;]*>)?)\s+(\w+)((?:\[[^\]]+\])*);\s*(/\*.*\*/)?\s*$", line)
        if not mm:
            if line.strip():
                raise GenError("image member line not understood: %r" % line)
            continue
        members.append(Member(first_line + k, line, mm.group(1), mm.group(2), mm.group(3), mm.group(4),
                              mm.group(5) or ""))
    off = 0
    for mem in members:
        t = db.get(mem.typ_text)
        count = 1
        for d in re.findall(r"\[([^\]]+)\]", mem.dims_text):
            count *= db.eval_dim(d, {})
        mem.off, mem.size, mem.type = off, t.size * count, t
        cm = re.match(r"/\* \+([0-9A-Fa-f]+)\b", mem.comment)
        if cm and int(cm.group(1), 16) != off:
            raise GenError("%s: comment says +%s, computed +%04X" % (mem.name, cm.group(1), off))
        off += mem.size
    return members, off


def node_vtable(mem):
    m = re.match(r"/\* \+[0-9A-Fa-f]+ (g_\w+)", mem.comment)
    return m.group(1) if m else None


def find_initialiser(source_text, image):
    m = re.search(r"\b%s\s+\w+\s*=\s*\{" % image, source_text)
    if not m:
        raise GenError("no initialiser of %s in the source" % image)
    start = m.end() - 1
    return start, match_brace(source_text, start)


def node_block_lines(value_text, indent):
    """The UiNodeBase block as ([header comment], [body lines]) re-indented for a nesting at indent."""
    assert value_text.startswith("{") and value_text.endswith("}")
    inner = value_text[1:-1]
    first, _, rest = inner.partition("\n")
    header = first.strip()
    body = [l.strip() for l in rest.split("\n") if l.strip()]
    if not rest.strip():  # all on one line
        body, header = [header], ""
    return header, [" " * indent + l for l in body]


def plan_node(db, mem, fields_mem, ext, override, warnings):
    """Returns (kind, new type text or None, Type or None, note)."""
    vt = node_vtable(mem)
    tname = override or VTABLE_TYPES.get(vt)
    if not tname:
        return "skip", None, None, "no type for vtable %s" % vt
    base = db.get(tname)
    if base.kind == "template":
        body, packed, tparam = db.structs[base.name]
        t1 = db.get("%s<1>" % base.name)
        t2 = db.get("%s<2>" % base.name)
        step = t2.size - t1.size
        n = 1 + (ext - t1.size) // step if step else 1
        if n < 1 or (ext - t1.size) % step:
            # largest N that fits, rest trailing
            n = max(1, 1 + (ext - t1.size) // step)
        tname = "%s<%d>" % (base.name, n)
        base = db.get(tname)
    if base.size == ext:
        return "typed", tname, base, ""
    if base.size < ext:
        if (ext - base.size) % 4:
            return "skip", None, None, "%s is 0x%X bytes, extent 0x%X (not a dword multiple)" % (tname, base.size,
                                                                                              ext)
        return "trailing", tname, base, "%d trailing dword(s)" % ((ext - base.size) // 4)
    fb = FIELDS_FALLBACK.get(vt)
    note = "truncated: %s is 0x%X bytes, the template keeps 0x%X" % (tname, base.size, ext)
    if fb and fields_mem is not None and db.get(fb).size == ext - NODE_BASE_SIZE:
        if fields_mem.typ_text == fb:
            return "keep", None, None, note + "; fields already " + fb
        return "fields", fb, db.get(fb), note + "; fields become " + fb
    return "keep", None, None, note


def run(args):
    root = args.root
    db = TypeDb(root)
    hpath, spath = os.path.join(root, args.header), os.path.join(root, args.source)
    header_text = open(hpath, encoding="utf-8").read()
    source_text = open(spath, encoding="utf-8").read()
    members, total = parse_image(db, header_text, args.image)
    overrides = dict(o.split("=", 1) for o in args.node)
    only = set(args.only.split(",")) if args.only else None
    init_open, init_close = find_initialiser(source_text, args.image)
    init_inner = source_text[init_open + 1:init_close]
    spans = split_top(init_inner)
    elems = [Elem(init_inner, s, e) for s, e in spans]
    designated = any(e.designator for e in elems)
    if designated and not all(e.designator for e in elems):
        raise GenError("mixed designated and positional initialiser")
    if designated:
        by_name = {e.designator: e for e in elems}
    else:
        if len(elems) > len(members):
            raise GenError("more initialiser elements than members")
        by_name = {mem.name: e for mem, e in zip(members, elems)}

    warnings, report, asserts = [], [], []
    header_edits = {}  # line_no -> replacement lines (None = delete)
    source_edits = []  # (start, end, text) in init_inner
    for i, mem in enumerate(members):
        if mem.typ_text != NODE_BASE or not node_vtable(mem):
            continue
        fields_mem = members[i + 1] if i + 1 < len(members) and members[i + 1].name == mem.name + "_fields" else None
        ext = mem.size + (fields_mem.size if fields_mem else 0)
        try:
            kind, tname, ntype, note = plan_node(db, mem, fields_mem, ext, overrides.get(mem.name), warnings)
        except GenError as exc:
            kind, tname, ntype, note = "skip", None, None, "ERROR: %s" % exc
        if only is not None and mem.name not in only and kind != "keep":
            kind, note = "skip", "not in --only"
        line = "%-34s +%04X  0x%-4X %-40s %-9s %s" % (mem.name, mem.off, ext, node_vtable(mem), kind, tname or "")
        report.append(line + (("  (" + note + ")") if note else ""))
        if kind in ("skip", "keep"):
            continue
        node_e = by_name.get(mem.name)
        fields_e = by_name.get(fields_mem.name) if fields_mem else None
        where = "%s.%s" % (args.image, mem.name)
        try:
            items = []
            if fields_e is not None:
                if fields_mem.dims_text:
                    ft = fields_mem.type
                    dims = [db.eval_dim(d, {}) for d in re.findall(r"\[([^\]]+)\]", fields_mem.dims_text)]
                    collect_field(db, ft, dims, fields_e.value, NODE_BASE_SIZE, items, warnings, where + "_fields")
                else:
                    collect_items(db, fields_mem.type, fields_e.value, NODE_BASE_SIZE, items, warnings,
                                  where + "_fields")
            if node_e is None or not node_e.braced:
                raise GenError("%s: no braced UiNodeBase initialiser" % where)
            elem_indent = len(init_inner[:node_e.start]) - len(init_inner[:node_e.start].rstrip(" "))
            # comments of the old fields block go on their own lines in front of the new element
            comments = block_comments(fields_e.value) if fields_e is not None else []
            prefix = "".join(c + "\n" + " " * elem_indent for c in comments)
            if kind == "fields":
                placer = Placer(items, where)
                entries = emit_struct(ntype, NODE_BASE_SIZE, placer, None, elem_indent + 4)
                placer.check_all_used()
                val = join_entries(entries, elem_indent + 4) if entries else ["{}"]
                new_fields = prefix + (".%s = " % fields_mem.name if designated else "") + val[0] + \
                    ("\n" + "\n".join(val[1:]) if len(val) > 1 else "")
                source_edits.append((fields_e.start, fields_e.end, new_fields))
                header_edits[fields_mem.line_no] = [fields_mem.indent + "%s %s;%s" % (
                    tname, fields_mem.name, (" " + fields_mem.comment) if fields_mem.comment else "")]
                asserts.append("static_assert(offsetof(%s, %s) == 0x%X && sizeof(%s) == 0x%X,\n              "
                               "\"%s.%s is a %s\");" % (args.image, fields_mem.name, fields_mem.off, tname,
                                                        ntype.size, args.image, fields_mem.name, tname))
                continue
            header, body = node_block_lines(node_e.value, 0)
            placer = Placer([it for it in items if it.off < ntype.size], where)
            trailing = [it for it in items if it.off >= ntype.size]
            if any(it.off < ntype.size < it.off + it.size for it in items):
                raise GenError("%s: a value crosses the end of %s" % (where, tname))
            node_lines = body
            entries = emit_struct_with_node(ntype, placer, node_lines, elem_indent + 4)
            placer.check_all_used()
            text_lines = render_entries(entries, elem_indent + 4)
            head = (".%s = " % mem.name if designated else "") + "{" + ((" " + header) if header else "")
            new_text = prefix + head + "\n" + "\n".join(text_lines)
            if kind == "trailing":
                k = (ext - ntype.size) // 4
                tvals = []
                for d in range(k):
                    off = ntype.size + 4 * d
                    hit = [it for it in trailing if it.off == off and it.size == 4]
                    part = [it for it in trailing if it.off < off + 4 and off < it.off + it.size]
                    if hit:
                        tvals.append(hit[0].text)
                    elif part:
                        if any(it.value is None for it in part):
                            raise GenError("%s: trailing value not dword aligned" % where)
                        v = 0
                        for it in part:
                            lo, hi = max(off, it.off), min(off + 4, it.off + it.size)
                            v |= ((it.value >> (8 * (lo - it.off))) & ((1 << (8 * (hi - lo))) - 1)) << (8 * (lo - off))
                        tvals.append("0x%08X" % v)
                    else:
                        tvals.append("0x00000000")
                while tvals and literal_value(tvals[-1], 4) == 0:
                    tvals.pop()
                tname_trail = mem.name + "_trailing"
                new_text += ",\n" + " " * elem_indent + (".%s = " % tname_trail if designated else "") + \
                    "{" + ", ".join(tvals) + "}"
            end = fields_e.end if fields_e is not None else node_e.end
            source_edits.append((node_e.start, end, new_text))
            new_header = [mem.indent + "%s %s;%s" % (tname, mem.name, (" " + mem.comment) if mem.comment else "")]
            if kind == "trailing":
                k = (ext - ntype.size) // 4
                new_header.append(mem.indent + "uint32_t %s_trailing[%d]; /* +%04X: template dwords behind the "
                                  "control */" % (mem.name, k, mem.off + ntype.size))
            header_edits[mem.line_no] = new_header
            if fields_mem:
                header_edits[fields_mem.line_no] = None
            asserts.append("static_assert(offsetof(%s, %s) == 0x%X && sizeof(%s) == 0x%X,\n              "
                           "\"%s.%s is a %s\");" % (args.image, mem.name, mem.off, tname, ntype.size, args.image,
                                                    mem.name, tname))
            if kind == "trailing":
                asserts.append("static_assert(offsetof(%s, %s_trailing) == 0x%X,\n              "
                               "\"%s.%s_trailing follows the control\");" % (
                                   args.image, mem.name, mem.off + ntype.size, args.image, mem.name))
        except GenError as exc:
            report[-1] += "  ERROR: %s" % exc
            warnings.append(str(exc))
            continue

    print("%s: %d members, 0x%X bytes" % (args.image, len(members), total))
    print("%-34s %-6s %-6s %-40s %-9s %s" % ("node", "offset", "extent", "vtable", "result", "type"))
    for line in report:
        print(line)
    for w in warnings:
        print("warning:", w)
    if args.asserts:
        print()
        print("\n".join(asserts))
    if not args.write:
        return 1 if any("ERROR" in r for r in report) else 0
    # apply
    new_inner = init_inner
    for start, end, text in sorted(source_edits, reverse=True):
        new_inner = new_inner[:start] + text + new_inner[end:]
    source_text = source_text[:init_open + 1] + new_inner + source_text[init_close:]
    lines = header_text.split("\n")
    for line_no in sorted(header_edits, reverse=True):
        rep = header_edits[line_no]
        lines[line_no:line_no + 1] = rep if rep is not None else []
    header_text = "\n".join(lines)
    with open(hpath, "w", encoding="utf-8", newline="\n") as fh:
        fh.write(header_text)
    with open(spath, "w", encoding="utf-8", newline="\n") as fh:
        fh.write(source_text)
    print("written: %s, %s" % (args.header, args.source))
    return 1 if any("ERROR" in r for r in report) else 0


def emit_struct_with_node(ntype, placer, node_lines, indent):
    return emit_struct_nested(ntype, 0, placer, node_lines, indent)


def emit_struct_nested(typ, off, placer, node_lines, indent):
    """Entries of struct typ at off; the UiNodeBase at offset 0 takes node_lines (its designated body)."""
    entries = []
    for fname, ftype, dims, foff in typ.fields:
        o = off + foff
        if not dims and ftype.kind == "struct" and ftype.name == NODE_BASE and o == 0:
            pad = " " * (indent + 4)
            body = [pad + l.strip() for l in node_lines]
            entries.append([".%s = {" % fname] + body[:-1] + [body[-1] + "}"])
            continue
        if not dims and ftype.kind == "struct" and o == 0:
            sub = emit_struct_nested(ftype, o, placer, node_lines, indent + 4)
            lines = render_entries(sub, indent + 4)
            entries.append([".%s = {" % fname] + lines)
            continue
        if dims:
            val = emit_array(ftype, dims, o, placer, None, indent + 4)
        elif ftype.kind == "struct":
            sub = emit_struct(ftype, o, placer, None, indent + 4)
            val = join_entries(sub, indent + 4) if sub else None
        else:
            v = placer.value_for(o, ftype)
            val = [v] if v is not None else None
        if val:
            entries.append([".%s = %s" % (fname, val[0])] + val[1:])
    return entries


def render_entries(entries, indent, width=118):
    """Lines of entries at indent, the last one closed with "}"."""
    pad = " " * indent
    lines, row = [], ""
    for e in entries:
        if len(e) == 1:
            if row and len(pad) + len(row) + 2 + len(e[0]) > width:
                lines.append(pad + row + ",")
                row = ""
            row = row + ", " + e[0] if row else e[0]
        else:
            if row:
                lines.append(pad + row + ",")
                row = ""
            lines.append(pad + e[0])
            lines.extend(e[1:-1])
            lines.append(e[-1] + ",")
    if row:
        lines.append(pad + row + ",")
    lines[-1] = lines[-1][:-1] + "}"
    return lines


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--root", default=os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..")))
    ap.add_argument("--image", required=True, help="image struct name, e.g. FatalErrorUiImage")
    ap.add_argument("--header", required=True, help="header with the image struct (relative to --root)")
    ap.add_argument("--source", required=True, help="source with the image's initialiser (relative to --root)")
    ap.add_argument("--node", action="append", default=[], metavar="NAME=TYPE", help="type for one node")
    ap.add_argument("--only", help="comma-separated nodes to retype (default: all)")
    ap.add_argument("--asserts", action="store_true", help="print layout_checks.cpp asserts for the retyped nodes")
    ap.add_argument("--write", action="store_true", help="rewrite header and source (default: report only)")
    args = ap.parse_args()
    try:
        return run(args)
    except GenError as exc:
        print("error:", exc, file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
