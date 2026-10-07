#!/usr/bin/env python3
"""Rewrite C-style struct/union typedefs as C++ declarations (step 13 W1, T1-T9).

For the files under the given path prefixes (relative to the repository root):
  typedef struct X X, *PX;        ->  struct X;            (PX recorded)
  typedef struct X X;             ->  struct X;
  typedef struct X {  ...  } X;   ->  struct X {  ...  };
  typedef struct X *Name;         ->  using Name = struct X *;   (a named pointer alias stays a name)
  typedef struct X Name;          ->  using Name = struct X;     (an alias with another name)
  (the same for union)
Then every recorded pointer alias token PX in src/ and include/ (outside comments, string and character
literals) is replaced by `X *`, and its now unused forward alias is gone with the typedef.

Typedefs to non-struct types are not touched. A PX use that the textual replacement would change in meaning is
reported and the run stops before writing anything:
  - `const PX` / `volatile PX` (the qualifier would move from the pointer to the pointee),
  - `PX a, b` declarations with more than one declarator (only the first would stay a pointer).

Usage: python tools/dev/typedef_to_struct.py [--apply] <path prefix> [<path prefix> ...]
Without --apply it only lists what it would change.
"""

import argparse
import os
import re
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
TOPS = ("src", "include")
EXTS = (".h", ".hpp", ".cpp", ".c", ".inc")

FORWARD_PTR = re.compile(r"^(\s*)typedef (struct|union) (\w+) \3, \*(\w+);(.*)$")
FORWARD = re.compile(r"^(\s*)typedef (struct|union) (\w+) \3;(.*)$")
DEFINITION = re.compile(r"^(\s*)typedef (struct|union) (\w+) \{(.*)$")
POINTER_ALIAS = re.compile(r"^(\s*)typedef (struct|union) (\w+) \*(\w+);(.*)$")
NAME_ALIAS = re.compile(r"^(\s*)typedef (struct|union) (\w+) (\w+);(.*)$")


def repo_files():
    for top in TOPS:
        for dirpath, _, names in os.walk(os.path.join(ROOT, top)):
            for name in sorted(names):
                if name.endswith(EXTS):
                    yield os.path.relpath(os.path.join(dirpath, name), ROOT).replace(os.sep, "/")


def read(rel):
    with open(os.path.join(ROOT, rel), encoding="utf-8", newline="") as f:
        return f.read()


def write(rel, text):
    with open(os.path.join(ROOT, rel), "w", encoding="utf-8", newline="") as f:
        f.write(text)


def split_lines(text):
    nl = "\r\n" if "\r\n" in text else "\n"
    return text.split(nl), nl


def convert_declarations(rel, text):
    """Returns (new text, {PX: X}, number of rewritten typedefs)."""
    lines, nl = split_lines(text)
    pointer_aliases = {}
    count = 0
    i = 0
    while i < len(lines):
        line = lines[i]
        m = FORWARD_PTR.match(line)
        if m:
            indent, kind, name, alias, rest = m.groups()
            lines[i] = "%s%s %s;%s" % (indent, kind, name, rest)
            pointer_aliases[alias] = name
            count += 1
            i += 1
            continue
        m = FORWARD.match(line)
        if m:
            indent, kind, name, rest = m.groups()
            lines[i] = "%s%s %s;%s" % (indent, kind, name, rest)
            count += 1
            i += 1
            continue
        m = DEFINITION.match(line)
        if m:
            indent, kind, name, rest = m.groups()
            close = re.compile(r"^%s\}\s*%s(\s*,\s*\*\s*(\w+))?\s*;(.*)$" % (re.escape(indent), re.escape(name)))
            for j in range(i + 1, len(lines)):
                c = close.match(lines[j])
                if c:
                    break
            else:
                raise SystemExit("%s:%d: no closing '} %s;' found" % (rel, i + 1, name))
            lines[i] = "%s%s %s {%s" % (indent, kind, name, rest)
            lines[j] = "%s};%s" % (indent, c.group(3))
            if c.group(2):
                pointer_aliases[c.group(2)] = name
            count += 1
            i += 1
            continue
        m = POINTER_ALIAS.match(line)
        if m:
            indent, kind, name, alias, rest = m.groups()
            lines[i] = "%susing %s = %s %s *;%s" % (indent, alias, kind, name, rest)
            count += 1
            i += 1
            continue
        m = NAME_ALIAS.match(line)
        if m and m.group(3) != m.group(4):
            indent, kind, name, alias, rest = m.groups()
            lines[i] = "%susing %s = %s %s;%s" % (indent, alias, kind, name, rest)
            count += 1
        i += 1
    return nl.join(lines), pointer_aliases, count


TOKEN = re.compile(r"//[^\n]*|/\*.*?\*/|\"(?:\\.|[^\"\\\n])*\"|'(?:\\.|[^'\\\n])*'|[A-Za-z_]\w*|\S", re.S)


def replace_aliases(rel, text, aliases, problems):
    """Replaces alias tokens outside comments and literals. Returns (new text, count)."""
    out = []
    pos = 0
    count = 0
    tokens = [(m.start(), m.end(), m.group(0)) for m in TOKEN.finditer(text)]
    code = [t for t in tokens if not t[2].startswith(("//", "/*", '"', "'"))]
    index_of = {t[0]: k for k, t in enumerate(code)}
    for start, end, tok in tokens:
        if tok in aliases:
            k = index_of[start]
            prev = code[k - 1][2] if k > 0 else ""
            line_no = text.count("\n", 0, start) + 1
            if prev in ("const", "volatile"):
                problems.append("%s:%d: '%s %s' (qualifier would move to the pointee)" % (rel, line_no, prev, tok))
            # a declaration `PX a, b`: identifier after PX, then a comma before the next ';' or ')' or '{'
            if k + 2 < len(code) and re.match(r"[A-Za-z_]\w*$", code[k + 1][2]) and prev in ("", ";", "{", "}", "(", ",") \
                    and code[k + 1][2] not in aliases:
                depth = 0
                for t in code[k + 2:]:
                    s = t[2]
                    if s in ("(", "[", "<"):
                        depth += 1
                    elif s in (")", "]", ">"):
                        if depth == 0:
                            break
                        depth -= 1
                    elif s in (";", "{"):
                        break
                    elif s == "," and depth == 0:
                        if prev in ("", ";", "{", "}"):
                            problems.append("%s:%d: '%s %s, ...' declares more than one name" %
                                            (rel, line_no, tok, code[k + 1][2]))
                        break
            out.append(text[pos:start])
            out.append(aliases[tok] + " *")
            pos = end
            count += 1
    out.append(text[pos:])
    return "".join(out), count


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("prefixes", nargs="+")
    args = ap.parse_args()
    prefixes = [p.rstrip("/") for p in args.prefixes]

    def selected(rel):
        return any(rel == p or rel.startswith(p + "/") for p in prefixes)

    files = list(repo_files())
    new_text = {}
    aliases = {}
    total_typedefs = 0
    for rel in files:
        if not selected(rel):
            continue
        text = read(rel)
        converted, found, n = convert_declarations(rel, text)
        if n:
            new_text[rel] = converted
            total_typedefs += n
            for a, x in found.items():
                if a in aliases and aliases[a] != x:
                    raise SystemExit("alias %s names two structs: %s and %s" % (a, aliases[a], x))
                aliases[a] = x
    problems = []
    total_uses = 0
    for rel in files:
        text = new_text.get(rel, None)
        if text is None:
            text = read(rel)
        replaced, n = replace_aliases(rel, text, aliases, problems)
        if n:
            new_text[rel] = replaced
            total_uses += n
    print("%d typedefs rewritten, %d pointer aliases, %d alias uses replaced, %d files" %
          (total_typedefs, len(aliases), total_uses, len(new_text)))
    if problems:
        print("\n".join(problems))
        raise SystemExit("stopped: %d uses need a hand edit first" % len(problems))
    if args.apply:
        for rel, text in new_text.items():
            write(rel, text)
    else:
        for rel in sorted(new_text):
            print("  " + rel)


if __name__ == "__main__":
    main()
