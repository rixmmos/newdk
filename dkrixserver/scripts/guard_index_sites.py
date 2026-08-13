#!/usr/bin/env python3
"""Count guards that index a container in order to test it.

Used by check-guard-index.sh. See that script's header for what the gate
means and why it exists.

--------------------------------------------------------------------------
THE BUG SHAPE
--------------------------------------------------------------------------

    Assert(m_pItemInfos[itemType] != NULL);
    return m_pItemInfos[itemType];

The `Assert` reads as a safety check. It is not one: it subscripts the
array *in order to* test the slot, so if `itemType` is out of range the
guard is itself the out-of-bounds read. It faults rather than catches, and
under `NDEBUG` it does not even fault — it vanishes and the caller reads
whatever was there.

The `if` form is the same defect wearing a different hat:

    if (m_Advances[questLevel] != NULL)
        throw DuplicatedException(...);
    m_Advances[questLevel] = pAdvance;

The audit calls this section 4 pattern 2. Instances were found by hand in
18-U, 18-AH, 18-AL and again in 18-AW, each time in a file the previous
wave had already read.

--------------------------------------------------------------------------
WHAT COUNTS AS A SITE
--------------------------------------------------------------------------

A *guard* is either

  * `Assert( COND )`, or
  * `if ( COND )` whose entire body is a single `return` or `throw`.

Both are "this condition decides whether the code may proceed". A guard is
a site when COND contains a container access

      X[i]        X.at(i)        X->at(i)

that is

  1. an *existence* test — the access is immediately compared against
     `NULL` / `nullptr` / `0`, or immediately negated with `!`. That is the
     shape the audit names: the guard is asking "is this slot populated?",
     which is a question you cannot ask without already being in bounds.
     A guard that tests the *value* of an element it has legitimately
     reached (`if (m_STR[attrType] > maxSlayerAttr) throw`) is a range
     check on the value, not on the index, and is not counted — see the
     blind-spot list; and
  2. not bounded (below); and
  3. not on a `map`/`set`-family container, where `operator[]` inserts
     rather than reads out of bounds, and not a compile-time-constant
     index into a container declared with an explicit array extent, where
     the compiler has already checked it.

--------------------------------------------------------------------------
WHERE THE "PROVABLY BOUNDED" LINE IS DRAWN
--------------------------------------------------------------------------

A lexical tool cannot decide whether an index is in range — that needs the
container's runtime length, which in this tree usually comes from a DB
row count. So the checker does not try. It answers a narrower, decidable
question: *did anybody write a bound for this index that survives
`NDEBUG`, anywhere the guard can see?* An access is treated as bounded,
and dropped, when any of these holds:

  * an enclosing `for`/`while`/`if` header compares an identifier from the
    index expression against something (`for (i = 0; i < n; i++)`,
    `if (index < m_Size) { ... }`). The unbraced single-statement body
    form is handled too — `for (...) if (a[i] == NULL) return i;` is one
    statement, not a block;
  * a real `if` earlier in the same function tests an identifier from the
    index expression and its body transfers control — `return`, `throw`,
    `continue`, `break`, `goto`, `exit`. This is exactly the chokepoint
    idiom 18-U introduced (`if (!verifyIndex(index)) throw ...;` placed
    *before* the surviving `Assert`), so a converted accessor stops being
    a site;
  * the same condition bounds the index to the left of the access:
    `Assert(i < m_Size && m_pThings[i] != NULL)`.

Two consequences worth stating plainly, because they are where the gate is
wrong rather than merely silent:

  * **A bound is accepted without being checked for correctness.** 18-AH's
    `SkillDomainInfoManager` destructor walked `j <= 150` over arrays that
    hold 101 — a written bound, and wrong. A wrong constant guard is
    audit pattern 3, and finding it needs the DB row counts; this gate
    would call it bounded. It measures whether a runtime bound was
    *written*, not whether it is *right*.
  * **Another `Assert` never counts as a bound.** `Assert(verifyIndex(i))`
    followed by `Assert(m_ppItem[i] == NULL)` leaves the second one a
    site, which is correct: under `NDEBUG` both disappear together.

--------------------------------------------------------------------------
WHAT IT CANNOT SEE
--------------------------------------------------------------------------

  * A bound established in a caller, or in another function in the same
    file. Only the enclosing function body is examined.
  * Value guards. `if (m_STR[attrType] > maxSlayerAttr) throw` has the
    same lexical shape and is deliberately excluded by the
    existence-test rule; there are nine of them in the `PC*Info2.h`
    headers and none is a bounds check.
  * Whether the site is reachable, or whether the index can carry
    attacker-controlled data. A wire-reachable `Assert` and a
    startup-only one count the same. Deciding that is the audit's job —
    §1 row 10 records that 18-U's conversions were deliberately scoped to
    wire-reachable checks and that the server-side-index Asserts in
    `ItemInfoManager` and friends were kept on purpose. Those are in the
    baseline, not bugs to fix.
  * Anything a `#define` hides, except the brace-opening macros
    `__BEGIN_TRY`, `__BEGIN_DEBUG`, `__BEGIN_DEBUG_EX` and
    `__ENTER_CRITICAL_SECTION`, which are normalised to real braces
    before scanning. Without that, brace matching is off by one from the
    first `__BEGIN_TRY` onwards and every enclosing-scope test in the
    file silently returns the wrong answer.
  * A container whose declaration lives outside its own file group: the
    map/set and fixed-extent-array tests both search only the group.
"""

import argparse
import os
import re
import sys

SOURCE_EXTS = (".cpp", ".h", ".inl")

MASK = "\x01"  # stands in for one character of masked literal/comment text

IDENT_RE = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")

ASSERT_RE = re.compile(r"(?<![A-Za-z0-9_])Assert\s*\(")
IF_RE = re.compile(r"(?<![A-Za-z0-9_])if\s*\(")
LOOP_RE = re.compile(r"(?<![A-Za-z0-9_])(for|while)\s*\(")
AT_RE = re.compile(r"(?:\.|->)\s*at\s*\(")

CONTROL_KEYWORDS = ("if", "for", "while", "switch", "catch", "do")

# What makes an `if` body a guard rather than ordinary work.
GUARD_BODY_RE = re.compile(r"^(?:return|throw)(?![A-Za-z0-9_])")

# What makes an earlier `if` a bound rather than ordinary work.
TRANSFER_RE = re.compile(
    r"(?<![A-Za-z0-9_])(?:return|throw|continue|break|goto|exit|abort)(?![A-Za-z0-9_])"
)

# The access is compared against nothing but emptiness.
EXISTENCE_AFTER_RE = re.compile(r"\s*(?:==|!=)\s*(?:NULL|nullptr|0)(?![A-Za-z0-9_.])")

# Declared container types where `operator[]` inserts instead of reading out
# of bounds, so indexing to test is correct code.
ASSOCIATIVE_RE = re.compile(r"(?<![A-Za-z0-9_])(?:unordered_|hash_)?(?:multi)?(?:map|set)\s*<")

# Words that can precede `name[` without declaring anything.
NOT_A_TYPE = frozenset(
    (
        "return",
        "delete",
        "new",
        "if",
        "else",
        "while",
        "for",
        "switch",
        "case",
        "throw",
        "sizeof",
        "goto",
        "do",
        "break",
        "continue",
        "Assert",
    )
)

# `__BEGIN_TRY` is `try {` and `__END_CATCH` is `} catch (...) {...}`. The
# braces are inside the macro, so raw brace matching over this tree is wrong
# from the first one onwards. Same for the critical-section pair.
OPEN_SCOPE_MACROS = (
    "__BEGIN_TRY",
    "__BEGIN_DEBUG_EX",
    "__BEGIN_DEBUG",
    "__ENTER_CRITICAL_SECTION",
)
CLOSE_SCOPE_MACROS = (
    "__END_CATCH_NO_RETHROW",
    "__END_CATCH",
    "__END_DEBUG_EX",
    "__END_DEBUG",
    "__LEAVE_CRITICAL_SECTION",
)
SCOPE_MACRO_RE = re.compile(
    r"(?<![A-Za-z0-9_])(" + "|".join(OPEN_SCOPE_MACROS + CLOSE_SCOPE_MACROS) + r")(?![A-Za-z0-9_])"
)


def strip_comments_and_mask_literals(text):
    """Return code of the same length as text, comments and literal bodies blanked.

    Newlines are preserved so offsets and line numbers still line up. Same
    lexer as sql_injection_sites.py and reserve_sizing_sites.py; comments
    matter here because a commented-out bounds check is exactly what 18-AL
    found in `InfoClassManager::getItemInfo` — the only `if` in the file was
    inside a comment, and a raw grep would have counted the guard as real.
    """
    n = len(text)
    out = []
    i = 0
    while i < n:
        c = text[i]
        if c == "/" and i + 1 < n and text[i + 1] == "/":
            while i < n and text[i] != "\n":
                out.append(" ")
                i += 1
        elif c == "/" and i + 1 < n and text[i + 1] == "*":
            out.append(" ")
            out.append(" ")
            i += 2
            while i < n and not (text[i] == "*" and i + 1 < n and text[i + 1] == "/"):
                out.append("\n" if text[i] == "\n" else " ")
                i += 1
            if i < n:
                out.append(" ")
                out.append(" ")
                i += 2
        elif c == '"' or c == "'":
            quote = c
            start = i
            j = i + 1
            while j < n:
                if text[j] == "\\" and j + 1 < n:
                    j += 2
                    continue
                if text[j] == quote or text[j] == "\n":
                    break
                j += 1
            end = j + 1 if j < n and text[j] == quote else j
            for k in range(start, end):
                if text[k] == "\n":
                    out.append("\n")
                elif k == start or k == end - 1:
                    out.append(quote)
                else:
                    out.append(MASK)
            i = end
        else:
            out.append(c)
            i += 1
    return "".join(out)


def normalize_scope_macros(code):
    """Rewrite the brace-carrying macros to literal braces, preserving length."""

    def sub(m):
        brace = "{" if m.group(1) in OPEN_SCOPE_MACROS else "}"
        return brace + " " * (len(m.group(1)) - 1)

    return SCOPE_MACRO_RE.sub(sub, code)


def matching(code, open_idx, opener="(", closer=")"):
    """Index of the bracket closing the one at open_idx, or -1."""
    depth = 0
    k = open_idx
    n = len(code)
    while k < n:
        ch = code[k]
        if ch == opener:
            depth += 1
        elif ch == closer:
            depth -= 1
            if depth == 0:
                return k
        k += 1
    return -1


def receiver_start(code, end_idx):
    """Index where the receiver expression ending at end_idx begins.

    Walks backwards over identifiers, `.`/`->`/`::`, and balanced
    `[...]` / `(...)`, so `m_DomainInfoLists[DomainType]` is the receiver of
    the second subscript in `m_DomainInfoLists[DomainType][Level]`.
    """
    k = end_idx
    while k > 0:
        c = code[k - 1]
        if c.isalnum() or c == "_":
            k -= 1
        elif c == "]" or c == ")":
            close, opener = (c, "[" if c == "]" else "(")
            depth = 0
            j = k
            while j > 0:
                ch = code[j - 1]
                if ch == close:
                    depth += 1
                elif ch == opener:
                    depth -= 1
                    if depth == 0:
                        j -= 1
                        break
                j -= 1
            if depth != 0:
                break
            k = j
        elif c == ".":
            k -= 1
        elif c == ">" and k >= 2 and code[k - 2] == "-":
            k -= 2
        elif c == ":" and k >= 2 and code[k - 2] == ":":
            k -= 2
        else:
            break
    return k


def base_name(receiver):
    """The container's key: last identifier of the receiver at bracket depth 0.

    `m_DomainInfoLists[DomainType]` -> m_DomainInfoLists  (not `DomainType`)
    `this->m_pItemInfos`            -> m_pItemInfos
    """
    depth = 0
    name = None
    i = 0
    n = len(receiver)
    while i < n:
        ch = receiver[i]
        if ch in "([":
            depth += 1
            i += 1
        elif ch in ")]":
            depth -= 1
            i += 1
        elif depth == 0 and (ch.isalpha() or ch == "_"):
            m = IDENT_RE.match(receiver, i)
            token = m.group(0)
            if token != "this":
                name = token
            i = m.end()
        else:
            i += 1
    return name


def word_ending_at(code, k):
    """The identifier whose last character is code[k], or None."""
    if k < 0 or not (code[k].isalnum() or code[k] == "_"):
        return None
    j = k
    while j > 0 and (code[j - 1].isalnum() or code[j - 1] == "_"):
        j -= 1
    return code[j : k + 1]


def enclosing_block(code, offset):
    """Index of the `{` opening the innermost block containing offset, or -1."""
    depth = 0
    k = offset
    while k > 0:
        ch = code[k - 1]
        if ch == "}":
            depth += 1
        elif ch == "{":
            if depth == 0:
                return k - 1
            depth -= 1
        k -= 1
    return -1


def block_header(code, brace_idx):
    """(keyword, condition_text) for the block opened at brace_idx.

    condition_text is None when the `{` is not preceded by a `(...)`: a bare
    block, an `else`, a `try`, a class or namespace body.
    """
    k = brace_idx - 1
    while k > 0 and code[k].isspace():
        k -= 1
    while True:  # step over `void f() const {`
        w = word_ending_at(code, k)
        if w in ("const", "noexcept", "override", "final"):
            k -= len(w)
            while k > 0 and code[k].isspace():
                k -= 1
        else:
            break
    if k <= 0:
        return (None, None)
    if code[k] != ")":
        return (word_ending_at(code, k), None)
    close = k
    depth = 0
    j = close
    while j >= 0:
        if code[j] == ")":
            depth += 1
        elif code[j] == "(":
            depth -= 1
            if depth == 0:
                break
        j -= 1
    if j < 0:
        return (None, None)
    cond = code[j + 1 : close]
    p = j - 1
    while p > 0 and code[p].isspace():
        p -= 1
    return (word_ending_at(code, p), cond)


def function_scope(code, offset):
    """(function_body_start, enclosing (keyword, condition) pairs) for offset.

    Walks outward through enclosing blocks. A `{` headed by a control
    keyword plus `(...)` contributes its condition; one headed by anything
    else with a `(...)` is the function body and ends the walk; one with no
    `(...)` at all — `else`, `try`, a bare block, a class body — is stepped
    over silently. A constructor's `: m_A(0) {` ends the walk, because the
    token before the `(` is a member name and not a control keyword.
    """
    conds = []
    k = offset
    while True:
        b = enclosing_block(code, k)
        if b < 0:
            return (0, conds)
        kw, cond = block_header(code, b)
        if cond is None:
            k = b
            continue
        if kw in CONTROL_KEYWORDS:
            conds.append((kw, cond))
            k = b
            continue
        return (b + 1, conds)


def unbraced_loop_headers(code, fn_start, guard_start):
    """for/while headers whose *unbraced* single-statement body holds the guard.

    `for (int i = 0; i < MAX; i++) if (Table[i] == token) return i;` has no
    braces at all, so enclosing_block() walks straight past the loop to the
    function body and the bound is invisible without this.
    """
    out = []
    for m in LOOP_RE.finditer(code, fn_start, guard_start):
        op = m.end() - 1
        cl = matching(code, op)
        if cl < 0 or cl >= guard_start:
            continue
        if any(ch in code[cl + 1 : guard_start] for ch in "{};"):
            continue
        out.append((m.group(1), code[op + 1 : cl]))
    return out


def find_accesses(cond):
    """Yield (receiver, index_expr, recv_start, close_idx) for each access in cond.

    Both `X[i]` and `X.at(i)` / `X->at(i)`. Offsets are relative to cond.
    """
    i = 0
    n = len(cond)
    while i < n:
        if cond[i] == "[":
            close = matching(cond, i, "[", "]")
            if close < 0:
                break
            rs = receiver_start(cond, i)
            receiver = cond[rs:i].strip()
            if receiver:
                yield (receiver, cond[i + 1 : close].strip(), rs, close)
            i = close + 1
        else:
            i += 1
    for m in AT_RE.finditer(cond):
        op = m.end() - 1
        close = matching(cond, op)
        if close < 0:
            continue
        rs = receiver_start(cond, m.start())
        receiver = cond[rs : m.start()].strip()
        if receiver:
            yield (receiver, cond[op + 1 : close].strip(), rs, close)


def is_existence_test(cond, recv_start, close_idx):
    """True if this access is compared against emptiness rather than a value."""
    if EXISTENCE_AFTER_RE.match(cond, close_idx + 1):
        return True
    before = cond[:recv_start].rstrip()
    return before.endswith("!") and not before.endswith("!=")


def is_constant_index(expr):
    """An integer literal or an ALL_CAPS constant — checkable by the compiler."""
    e = expr.strip()
    return bool(re.fullmatch(r"[0-9]+[uUlL]*", e) or re.fullmatch(r"[A-Z][A-Z0-9_]*", e))


def bounded_by_control(conds, expr):
    """An enclosing for/while/if compares an identifier of expr against something."""
    for t in set(IDENT_RE.findall(expr)):
        esc = re.escape(t)
        for kw, cond in conds:
            if kw not in ("for", "while", "if"):
                continue
            if re.search(r"(?<![A-Za-z0-9_])" + esc + r"\s*(?:<|<=|!=|>|>=)", cond) or re.search(
                r"(?:<|<=|!=|>|>=)\s*" + esc + r"(?![A-Za-z0-9_])", cond
            ):
                return True
    return False


def bounded_by_preceding_if(code, fn_start, guard_start, expr):
    """A real (non-Assert) `if` earlier in the function tests the index and bails."""
    toks = set(IDENT_RE.findall(expr))
    if not toks:
        return False
    region = code[fn_start:guard_start]
    for m in IF_RE.finditer(region):
        op = m.end() - 1
        cl = matching(region, op)
        if cl < 0:
            continue
        cond = region[op + 1 : cl]
        if not any(
            re.search(r"(?<![A-Za-z0-9_])" + re.escape(t) + r"(?![A-Za-z0-9_])", cond)
            for t in toks
        ):
            continue
        rest = region[cl + 1 :]
        j = 0
        while j < len(rest) and rest[j].isspace():
            j += 1
        if j < len(rest) and rest[j] == "{":
            end = matching(rest, j, "{", "}")
            body = rest[j : end + 1] if end > 0 else rest[j : j + 400]
        else:
            end = rest.find(";", j)
            body = rest[j : end + 1] if end > 0 else rest[j : j + 200]
        if TRANSFER_RE.search(body):
            return True
    return False


def bounded_in_same_condition(cond, open_idx, expr):
    """`Assert(i < m_Size && m_pThings[i] != NULL)` — a bound left of the access."""
    left = cond[:open_idx]
    if not re.search(r"&&|\|\|", left):
        return False
    for t in set(IDENT_RE.findall(expr)):
        esc = re.escape(t)
        if re.search(r"(?<![A-Za-z0-9_])" + esc + r"\s*(?:<|<=|>|>=|!=)", left) or re.search(
            r"(?:<|<=|>|>=|!=)\s*" + esc + r"(?![A-Za-z0-9_])", left
        ):
            return True
    return False


def declared_associative(code, base):
    """True if `base` is declared with a map/set-family type in this file group."""
    decl = re.compile(
        r"(?<![A-Za-z0-9_])((?:const\s+)?[A-Za-z_][A-Za-z0-9_:]*\s*<[^;{}]*?>[\s*&]+)"
        + re.escape(base)
        + r"\s*(?:\[[^\[\]]*\])?\s*[;=,)]"
    )
    for m in decl.finditer(code):
        if ASSOCIATIVE_RE.search(m.group(1)):
            return True
    return False


def declared_fixed_array(code, base):
    """True if `base` is declared with an explicit array extent in this group.

    `DarkLightInfo* m_DarkLightInfos[nDarkLightInfos];` yes;
    `ItemInfo** m_pItemInfos;` no — which is the whole point, because 18-AW's
    `Assert(m_pItemInfos[0] != NULL)` is a constant index into a heap array
    whose length is a runtime field.

    Matched by looking for `<type> <base>[...]` followed by `;`, `=`, `,`,
    `)` or another `[`. The preceding word must not be a keyword: `delete
    m_pItemInfos[i];` otherwise reads as a declaration and silently turns
    every constant-index site in the file into a false negative.
    """
    for m in re.finditer(r"(?<![A-Za-z0-9_])" + re.escape(base) + r"\s*\[[^\[\]]*\]", code):
        j = m.end()
        while j < len(code) and code[j].isspace():
            j += 1
        if j >= len(code) or code[j] not in ";=,)[":
            continue
        k = m.start() - 1
        while k >= 0 and code[k].isspace():
            k -= 1
        while k >= 0 and code[k] in "*&":
            k -= 1
            while k >= 0 and code[k].isspace():
                k -= 1
        if k >= 0 and code[k] == ">":  # vector<T> m_V[3];
            depth = 0
            while k >= 0:
                if code[k] == ">":
                    depth += 1
                elif code[k] == "<":
                    depth -= 1
                    if depth == 0:
                        break
                k -= 1
            k -= 1
            while k >= 0 and code[k].isspace():
                k -= 1
        w = word_ending_at(code, k)
        if w is None or w in NOT_A_TYPE:
            continue
        return True
    return False


def guards(code):
    """Yield (kind, cond_start, cond_end, guard_start) for every guard."""
    for m in ASSERT_RE.finditer(code):
        op = m.end() - 1
        cl = matching(code, op)
        if cl < 0:
            continue
        yield ("Assert", op + 1, cl, m.start())
    for m in IF_RE.finditer(code):
        op = m.end() - 1
        cl = matching(code, op)
        if cl < 0:
            continue
        rest = code[cl + 1 :]
        j = 0
        while j < len(rest) and rest[j].isspace():
            j += 1
        if j < len(rest) and rest[j] == "{":
            end = matching(rest, j, "{", "}")
            body = rest[j + 1 : end] if end > 0 else ""
        else:
            end = rest.find(";", j)
            body = rest[j:end] if end > 0 else ""
        s = body.strip()
        if not s or s.count(";") > 1 or not GUARD_BODY_RE.match(s):
            continue
        yield ("if", op + 1, cl, m.start())


def line_of(code, offset):
    return code.count("\n", 0, offset) + 1


def group_key(path):
    """(directory, basename stem) — the translation-unit family."""
    dirname = os.path.dirname(path)
    stem = os.path.basename(path)
    for ext in SOURCE_EXTS:
        if stem.endswith(ext):
            stem = stem[: -len(ext)]
            break
    return (dirname, stem)


def excluded(path):
    base = os.path.basename(path)
    return ".backup" in base or base.endswith(".txt")


def iter_sources(root):
    if os.path.isfile(root):
        yield root
        return
    for dirpath, dirnames, filenames in os.walk(root):
        dirnames[:] = [d for d in dirnames if d not in (".git", "build", "bin", "lib")]
        for name in sorted(filenames):
            if not name.endswith(SOURCE_EXTS):
                continue
            path = os.path.join(dirpath, name)
            if excluded(path):
                continue
            yield path


def collect(root):
    """Return the sites as a sorted list of (rel_path, line, base, kind, snippet)."""
    base_dir = os.path.dirname(root) if os.path.isfile(root) else root
    groups = {}
    for path in iter_sources(root):
        rel = os.path.relpath(path, base_dir).replace(os.sep, "/")
        with open(path, "r", encoding="utf-8", errors="replace") as fh:
            text = fh.read()
        groups.setdefault(group_key(path), []).append((rel, text))

    sites = []
    for _key, members in sorted(groups.items()):
        masked = [
            (rel, normalize_scope_macros(strip_comments_and_mask_literals(text)))
            for rel, text in members
        ]
        group_code = "\n".join(code for _rel, code in masked)
        assoc = {}
        fixed = {}
        for rel, code in masked:
            for kind, cs, ce, gstart in guards(code):
                cond = code[cs:ce]
                if "[" not in cond and "at(" not in cond:
                    continue
                scope = None
                for receiver, expr, rstart, close in find_accesses(cond):
                    if not expr:
                        continue
                    if not is_existence_test(cond, rstart, close):
                        continue
                    base = base_name(receiver)
                    if base is None:
                        continue
                    if base not in assoc:
                        assoc[base] = declared_associative(group_code, base)
                    if assoc[base]:
                        continue
                    if is_constant_index(expr):
                        if base not in fixed:
                            fixed[base] = declared_fixed_array(group_code, base)
                        if fixed[base]:
                            continue
                    open_idx = cond.find("[", rstart)
                    if open_idx == -1:
                        open_idx = rstart
                    if bounded_in_same_condition(cond, open_idx, expr):
                        continue
                    if scope is None:
                        scope = function_scope(code, gstart)
                    fn_start, conds = scope
                    allconds = conds + unbraced_loop_headers(code, fn_start, gstart)
                    if bounded_by_control(allconds, expr):
                        continue
                    if bounded_by_preceding_if(code, fn_start, gstart, expr):
                        continue
                    sites.append(
                        (rel, line_of(code, gstart), base, kind, " ".join(cond.split())[:110])
                    )
                    break
    sites.sort()
    return sites


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("root")
    ap.add_argument("--list", action="store_true", help="print each site")
    args = ap.parse_args()

    sites = collect(args.root)

    if args.list:
        for rel, line_no, base, kind, snippet in sites:
            print("%s:%d: %s [%s] -- %s" % (rel, line_no, base, kind, snippet))
    else:
        print(len(sites))
    return 0


if __name__ == "__main__":
    sys.exit(main())
