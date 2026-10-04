"""A small C/C++ source index for the documentation generators (no compiler needed).

Reads every file under src/ and include/thandor/ and finds, per file:
  - the file comment (a prose comment near the top that is not attached to a declaration),
  - the functions defined at file scope (name, line, static or not, header comment above the definition, body span),
  - the variables defined at file scope (name, line),
  - the #include lines,
  - the identifiers used inside each function body and inside variable initialisers (for the caller scan).

The scan is lexical: comments and string literals are blanked out, preprocessor lines are skipped, and file-scope
declarations are split at `;` and at the closing brace of function bodies. Names are unique in this code base, so
a reference to a name is taken as a reference to its (single) definition."""
import pathlib
import re

ROOT = pathlib.Path(__file__).resolve().parent.parent.parent
SOURCE_SUFFIXES = (".cpp", ".c", ".h", ".hpp")

IDENT = re.compile(r"[A-Za-z_]\w*")
QUALIFIER_TAIL = re.compile(r"\)\s*(?:const|noexcept|override|final|\s)*$")
DECLSPEC = re.compile(r"(?:__declspec|THANDOR_ALIGN)\s*\((?:[^()]|\([^()]*\))*\)|alignas\s*\((?:[^()]|\([^()]*\))*\)|"
                      r"__attribute__\s*\(\((?:[^()]|\([^()]*\))*\)\)")
KEYWORDS = {
    "if", "while", "for", "switch", "return", "sizeof", "static", "const", "extern", "inline", "volatile",
    "struct", "union", "enum", "class", "typedef", "unsigned", "signed", "int", "char", "short", "long", "void",
    "float", "double", "bool", "auto", "constexpr", "static_assert", "template", "typename", "using", "namespace",
    "operator", "else", "do", "case", "default", "break", "continue", "goto", "true", "false", "nullptr", "new",
    "delete", "this", "register", "thread_local", "noexcept", "decltype", "alignof", "__forceinline", "__cdecl",
    "__stdcall", "__fastcall", "THANDOR_SLOT", "UI_SLOT", "THANDOR_PTR",
}
BOILERPLATE_COMMENT = re.compile(r"^(Module data\.|Implementation ownership:|Submodule:|Module:)", re.I)


class Function:
    def __init__(self, name, line, is_static, comment, body_start, body_end):
        self.name = name
        self.line = line
        self.is_static = is_static
        self.comment = comment
        self.body = (body_start, body_end)
        self.uses = set()


class Variable:
    def __init__(self, name, line, is_static, comment):
        self.name = name
        self.line = line
        self.is_static = is_static
        self.comment = comment
        self.uses = set()


class SourceFile:
    def __init__(self, path):
        self.path = path
        self.rel = path.relative_to(ROOT).as_posix()
        self.is_header = path.suffix in (".h", ".hpp")
        self.text = path.read_text(encoding="utf-8", errors="replace")
        self.functions = []
        self.variables = []
        self.includes = []
        self.comment = ""
        self.comments = []  # (start, end, text)
        self.anonymous_depth = 0
        self._parse()

    # ---- lexing -------------------------------------------------------------------------------------------------
    def _blank(self):
        """Comments and literal contents replaced by spaces (newlines kept), preprocessor lines blanked."""
        text = self.text
        out = list(text)
        i, n = 0, len(text)
        line_start = True
        while i < n:
            c = text[i]
            if line_start and c == "#":
                # preprocessor line incl. continuations
                j = i
                while j < n:
                    if text.startswith("/*", j):  # a comment may run past the end of the directive line
                        k = text.find("*/", j + 2)
                        k = n if k < 0 else k + 2
                        self.comments.append((j, k, text[j:k]))
                        j = k
                        continue
                    if text.startswith("//", j):
                        k = text.find("\n", j)
                        k = n if k < 0 else k
                        self.comments.append((j, k, text[j:k]))
                        j = k
                        continue
                    if text[j] == "\n" and text[j - 1] != "\\":
                        break
                    j += 1
                directive = text[i:j]
                m = re.match(r"#\s*include\s*[<\"]([^>\"]+)[>\"]", directive)
                if m:
                    self.includes.append(m.group(1))
                for k in range(i, j):
                    if out[k] != "\n":
                        out[k] = " "
                i = j
                continue
            if c in " \t\r":
                i += 1
                continue
            if c == "\n":
                line_start = True
                i += 1
                continue
            line_start = False
            if text.startswith("/*", i):
                j = text.find("*/", i + 2)
                j = n if j < 0 else j + 2
                self.comments.append((i, j, text[i:j]))
                for k in range(i, j):
                    if out[k] != "\n":
                        out[k] = " "
                i = j
                continue
            if text.startswith("//", i):
                j = text.find("\n", i)
                j = n if j < 0 else j
                self.comments.append((i, j, text[i:j]))
                for k in range(i, j):
                    out[k] = " "
                i = j
                continue
            if c in "\"'":
                # raw strings are not used in this code base
                j = i + 1
                while j < n and text[j] != c:
                    if text[j] == "\\":
                        j += 1
                    elif text[j] == "\n":
                        break
                    j += 1
                for k in range(i + 1, min(j, n)):
                    if out[k] != "\n":
                        out[k] = " "
                i = j + 1
                continue
            i += 1
        return "".join(out)

    def line_of(self, offset):
        return self.text.count("\n", 0, offset) + 1

    def _comment_before(self, offset):
        """The comment that ends right before offset (only whitespace in between), or ''."""
        best = None
        for start, end, body in self.comments:
            line_start = self.text.rfind("\n", 0, start) + 1
            if self.text[line_start:start].strip():
                continue  # trailing comment of a code line ("}  // namespace")
            if end <= offset and self.text[end:offset].strip() == "":
                best = (start, end, body)
            elif start >= offset:
                break
        return clean_comment(best[2]) if best else ""

    # ---- parsing ------------------------------------------------------------------------------------------------
    def _parse(self):
        code = self._blank()
        self.code = code
        n = len(code)
        i = 0
        chunk_start = 0
        transparent = []  # brace depth markers for namespace / extern "C" blocks
        depth = 0
        while i < n:
            c = code[i]
            if c == "{":
                head = code[chunk_start:i]
                m = re.search(r"(?:\bnamespace\b([\w\s:]*)|\bextern\s*\"\s*C?\s*\"\s*)$", head)
                if m:
                    # names in an anonymous namespace are file-local like statics
                    anonymous = m.group(0).startswith("namespace") and not m.group(1).strip()
                    transparent.append(anonymous)
                    self.anonymous_depth = sum(transparent)
                    i += 1
                    chunk_start = i
                    continue
                close = match_brace(code, i)
                stripped = head.rstrip()
                if QUALIFIER_TAIL.search(stripped) and not re.search(r"=\s*$", stripped):
                    self._add_function(head, chunk_start, i, close)
                    i = close + 1
                    chunk_start = i
                    continue
                # struct definition or brace initialiser: the declaration ends at the next ';'
                i = close + 1
                continue
            if c == "}":
                if transparent:
                    transparent.pop()
                    self.anonymous_depth = sum(transparent)
                i += 1
                chunk_start = i
                continue
            if c == ";":
                self._add_declaration(code[chunk_start:i], chunk_start, i)
                i += 1
                chunk_start = i
                continue
            i += 1
        self.comment = self._file_comment()

    def _add_function(self, head, head_start, brace, close):
        stripped = DECLSPEC.sub(lambda m: " " * len(m.group(0)), head).rstrip()
        stripped = re.sub(r"(?:\s|const|noexcept|override|final)*$", "", stripped)
        if not stripped.endswith(")"):
            return
        open_paren = match_paren_back(stripped, len(stripped) - 1)
        if open_paren < 0:
            return
        m = re.search(r"([A-Za-z_][\w:]*)\s*$", stripped[:open_paren])
        if not m:
            # a function returning a function pointer: T (*Name(args))(params)
            m = re.search(r"\(\s*\*\s*([A-Za-z_]\w*)\s*\(", stripped)
            if not m:
                return
        name = m.group(1).split("::")[-1]
        if name in KEYWORDS:
            return
        lead = len(head) - len(head.lstrip())
        decl_start = head_start + lead
        words = set(IDENT.findall(stripped[:m.start()]))
        # a static (inline) function of a header is shared by every file that includes it
        is_static = ("static" in words or self.anonymous_depth > 0) and not self.is_header
        fn = Function(name, self.line_of(head_start + m.start()), is_static,
                      self._comment_before(decl_start), brace, close)
        fn.uses = set(IDENT.findall(self.code[brace:close]))
        self.functions.append(fn)

    def _add_declaration(self, chunk, chunk_start, end):
        text = DECLSPEC.sub(lambda m: " " * len(m.group(0)), chunk)
        body = text.strip()
        if not body:
            return
        first = IDENT.match(body)
        if not first or first.group(0) in ("typedef", "using", "static_assert", "template", "friend", "return"):
            return
        if re.match(r"extern\b(?!\s*\"\s*C)", body):
            return
        words_all = body
        # split the initialiser off: everything after the first top-level '='
        decl, init = split_top(words_all, "=")
        if first.group(0) in ("struct", "union", "enum", "class") and "{" in decl:
            # a type definition; only declarators after its closing brace define variables
            decl = decl[decl.rfind("}") + 1:]
            if not IDENT.search(decl):
                return
            decl = "T " + decl
        # remove brace groups from the declarator part (struct bodies)
        decl_flat = remove_groups(decl, "{", "}")
        if "(" in decl_flat:
            fp = re.search(r"\(\s*\*\s*([A-Za-z_]\w*)\s*\)", decl_flat)
            if not fp:
                return  # prototype or macro invocation
            names = [fp.group(1)]
        else:
            names = []
            for part in split_all_top(decl_flat, ","):
                part = remove_groups(part, "[", "]")
                idents = [w for w in IDENT.findall(part) if w not in ("const", "volatile", "static", "struct",
                                                                      "union", "enum", "class", "extern", "C",
                                                                      "inline", "constexpr",
                                                                      "thread_local")]
                if not idents:
                    continue
                tokens = IDENT.findall(part)
                if tokens and tokens[0] in ("struct", "union", "enum", "class") and len(idents) <= 1 and init is None:
                    continue  # bare type definition / forward declaration
                if len(idents) < 2 and not names:
                    continue  # a lone type name or macro: no declarator
                names.append(idents[-1])
        if not names:
            return
        lead = len(chunk) - len(chunk.lstrip())
        decl_start = chunk_start + lead
        is_static = re.match(r"(?:\s|const\b|constexpr\b)*static\b", body) is not None and not self.is_header
        is_static = is_static or self.anonymous_depth > 0
        comment = self._comment_before(decl_start)
        uses = set(IDENT.findall(init)) if init else set()
        for name in names:
            off = chunk.find(name)
            var = Variable(name, self.line_of(chunk_start + max(off, 0)), is_static, comment)
            var.uses = uses
            self.variables.append(var)

    def _file_comment(self):
        """First prose comment that is not the licence block, not boilerplate and not attached to a declaration."""
        m = re.search(r"\S", self.code)
        first_code = m.start() if m else len(self.text)
        for start, end, body in self.comments:
            if start > first_code:
                break
            text = clean_comment(body)
            if BOILERPLATE_COMMENT.match(text):
                break  # what follows belongs to declarations
            if not text or "Open Thandor" in text:
                continue
            line_start = self.text.rfind("\n", 0, start) + 1
            if self.text[line_start:start].strip():
                continue  # trailing comment of a code line
            after = self.text[end:end + 400]
            # attached comments are followed directly (no blank line) by code or a directive that is not an
            # #include; a comment followed after a blank line by a function belongs to that function
            m = re.match(r"[ \t]*\r?\n([ \t]*\r?\n)?\s*(\S.*)?", after)
            if m and m.group(1) is None and m.group(2) and not m.group(2).startswith(("#", "/*", "//")):
                continue
            if m and m.group(2) and not m.group(2).startswith(("#", "/*", "//", "typedef", "struct", "enum",
                                                                 "union", "class")) \
                    and "(" in re.split(r"[;{=]", after[m.start(2):], maxsplit=1)[0]:
                continue
            if m and m.group(1) is None and m.group(2) and m.group(2).startswith("#") \
                    and not m.group(2).startswith("#include"):
                continue
            if len(text) < 30:
                continue
            return text
        return ""


def clean_comment(body):
    if body.startswith("/*"):
        body = body[2:-2] if body.endswith("*/") else body[2:]
        lines = [re.sub(r"^\s*\*(?!/)\s?", "", ln).strip() for ln in body.splitlines()]
    else:
        lines = [re.sub(r"^\s*//+\s?", "", ln).strip() for ln in body.splitlines()]
    return " ".join(ln for ln in lines if ln).strip()


def first_sentences(text, count, limit):
    """The first count sentences of a comment (up to about limit characters)."""
    out = ""
    rest = text
    for _ in range(count):
        if not rest:
            break
        sentence = first_sentence(rest, limit)
        if out and len(out) + len(sentence) > limit:
            break
        out = (out + " " + sentence).strip()
        if sentence.endswith(" ..."):
            break
        rest = rest[len(sentence):].strip()
    return out


def first_sentence(text, limit=220):
    """The first sentence of a comment (up to limit characters)."""
    protected = re.sub(r"\b(i\.e|e\.g|etc|cf|vs|approx|no)\.", lambda m: m.group(0).replace(".", "\u0001"), text)
    m = re.match(r"(.+?[.!?])(?:\s|$)", protected)
    s = (m.group(1) if m else protected).replace("\u0001", ".")
    text = protected.replace("\u0001", ".")
    # do not cut inside a parenthesis that opens before the period
    if s.count("(") > s.count(")"):
        m2 = re.match(r"(.+?\)[^.]*?[.!?])(?:\s|$)", text)
        if m2:
            s = m2.group(1)
    if len(s) > limit:
        s = s[:limit].rsplit(" ", 1)[0] + " ..."
    return s


def match_brace(code, i):
    depth = 0
    n = len(code)
    while i < n:
        c = code[i]
        if c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                return i
        i += 1
    return n - 1


def match_paren_back(text, i):
    depth = 0
    while i >= 0:
        c = text[i]
        if c == ")":
            depth += 1
        elif c == "(":
            depth -= 1
            if depth == 0:
                return i
        i -= 1
    return -1


def split_top(text, sep):
    depth = 0
    for i, c in enumerate(text):
        if c in "([{":
            depth += 1
        elif c in ")]}":
            depth -= 1
        elif c == sep and depth == 0:
            if sep == "=" and i + 1 < len(text) and text[i + 1] == "=":
                continue
            return text[:i], text[i + 1:]
    return text, None


def split_all_top(text, sep):
    parts, depth, last = [], 0, 0
    for i, c in enumerate(text):
        if c in "([{<":
            depth += 1
        elif c in ")]}>":
            depth -= 1
        elif c == sep and depth == 0:
            parts.append(text[last:i])
            last = i + 1
    parts.append(text[last:])
    return parts


def remove_groups(text, open_c, close_c):
    out, depth = [], 0
    for c in text:
        if c == open_c:
            depth += 1
            continue
        if c == close_c:
            depth -= 1
            continue
        if depth == 0:
            out.append(c)
    return "".join(out)


def load_tree():
    """All source and header files of src/ and include/thandor/, parsed."""
    files = []
    for base in (ROOT / "src", ROOT / "include" / "thandor"):
        for path in sorted(base.rglob("*")):
            if path.suffix in SOURCE_SUFFIXES and path.is_file():
                files.append(SourceFile(path))
    return files


def definitions(files):
    """name -> (file, Function|Variable); source files win over headers, non-static over static."""
    table = {}
    for f in files:
        for d in f.functions + f.variables:
            prev = table.get(d.name)
            if prev is None:
                table[d.name] = (f, d)
                continue
            pf, pd = prev
            if pf.rel.startswith("include/") and f.rel.startswith("src/"):
                table[d.name] = (f, d)
            elif pd.is_static and not d.is_static:
                table[d.name] = (f, d)
    return table
