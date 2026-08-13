#!/usr/bin/env python3
"""Count `reserve()`-for-`resize()` sites in the server tree.

Used by check-reserve-sizing.sh. See that script's header for what the
gate means and why it exists.

--------------------------------------------------------------------------
THE BUG SHAPE
--------------------------------------------------------------------------

    m_Things.reserve(N);            // capacity N, size() still 0
    for (int i = 0; i < N; i++)
        m_Things[i] = NULL;         // every write is out of bounds

`reserve()` changes capacity, not size. Writing through `operator[]`,
`at()`, `front()` or an iterator walk afterwards is undefined behaviour on
a vector whose `size()` is 0 — and it usually *appears* to work, because
the storage is allocated, which is exactly why this family kept being
written. The visible symptoms are the second-order ones: `begin()`…`end()`
iterates nothing, so `clear()` frees nothing and the container leaks every
object it ever held; and `reserve()` does not value-initialise, so a NULL
check on a "reserved" slot tests indeterminate memory and passes.

Six instances of this had to be found by hand in this tree (18-J, 18-P x3,
18-Q, 18-AL, 18-AR). Five of them were found *after* the pattern was named.

--------------------------------------------------------------------------
WHAT COUNTS AS A SITE
--------------------------------------------------------------------------

One `.reserve(` / `->reserve(` call is a site when, within its *file
group*, the reserved container is

  * never given a size — no `resize`, `assign`, `swap`, `push_back`,
    `emplace_back`, `insert`, `emplace`, `push_front`, `emplace_front`,
    `append`, `+=` anywhere in the group — AND
  * used as though it had one — a subscript `c[i]`, an `at()`, a
    `front()`/`back()`, an iterator walk (`begin`/`end`/`rbegin`/`rend`/
    `cbegin`/`cend`), or a range-for over it.

`reserve()` followed by `push_back()` is the correct, intended idiom and
is never a site. That is what keeps GCPetStashList and PlayerCreature —
which reserve and then fill with `push_back` — out of the count.

A *file group* is every source file sharing a directory and a basename
stem: `OptionInfo.cpp` + `OptionInfo.h` are one group. This tree is
one-class-per-file-pair, and the two halves of an instance genuinely do
land in different halves of a pair — `OptionInfoManager`'s `reserve()` is
in the .cpp and the indexed write is an inline method in the .h.

The container is keyed by the last top-level identifier of the receiver
expression, so `m_UpgradeRatio[0].reserve(grade)` keys on
`m_UpgradeRatio` and matches the later `m_UpgradeRatio[s][grade]` read.
Identifiers inside subscripts do not become the key, so
`m_Ratio[i].reserve(N)` keys on `m_Ratio`, not on `i`.

--------------------------------------------------------------------------
WHAT IT CANNOT SEE
--------------------------------------------------------------------------

  * A container reserved in one file group and sized or indexed in
    another. Both directions: a cross-group `resize()` is a false
    positive, a cross-group subscript is a false negative. Nothing in the
    tree does either today [measured 2026-08-13].
  * Two different classes in one group that happen to share a member
    name. One class's `push_back` would suppress the other's site.
  * Whether the site is reachable. `PacketValidator`'s loop is bounded by
    a constant that is 0 in all three server binaries; it is counted
    anyway, because the gate measures the shape, not the reachability.
    Deciding reachability is a human's job and it is what the audit is
    for. That division of labour is what the gate's first catch showed:
    it reported `UserGateway.cpp:32` knowing nothing about whether the
    file was built, and the humans then found the file could not compile
    at all and deleted it.
  * Anything a `#define` hides, and anything outside the scanned tree.
  * `map`/`set`-family containers are excluded by declared type:
    `reserve()` then `operator[]` on an `unordered_map` is correct code.
    The exclusion is lexical, so a container reached only through a
    typedef whose name does not say `map`/`set` is not excluded — but a
    typedef of a *vector* must not be excluded, which is why the test is
    an exclusion of map/set rather than a requirement of `vector<`. The
    case that forced this shape was `UserGateway`'s `USER_INFO`, a
    `typedef vector<int>` that a `vector<`-requiring test would have
    missed entirely — the file has since been deleted, but the rule it
    justified is why the next such typedef will still be caught.
"""

import argparse
import os
import re
import sys

SOURCE_EXTS = (".cpp", ".h", ".inl")

MASK = "\x01"  # stands in for one character of masked literal/comment text

# Calls that give a container a size. `reserve()` + `push_back()` is the
# correct idiom, so the growth operations belong here too.
SIZING_METHODS = (
    "resize",
    "assign",
    "swap",
    "push_back",
    "emplace_back",
    "insert",
    "emplace",
    "push_front",
    "emplace_front",
    "append",
)

# Uses that require elements to exist, not merely capacity.
ELEMENT_METHODS = (
    "at",
    "begin",
    "end",
    "rbegin",
    "rend",
    "cbegin",
    "cend",
    "front",
    "back",
)

# Declared container types for which `reserve()` + `operator[]` is
# correct code rather than the bug.
ASSOCIATIVE_RE = re.compile(r"(?<![A-Za-z0-9_])(?:unordered_|hash_)?(?:multi)?(?:map|set)\s*<")

RESERVE_RE = re.compile(r"(?:\.|->)\s*reserve\s*\(")

IDENT_RE = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")


def strip_comments_and_mask_literals(text):
    """Return code of the same length as text, comments and literal bodies blanked.

    Newlines are preserved so offsets and line numbers still line up. Same
    lexer as sql_injection_sites.py; comments matter here for the same
    reason they matter there — LuckInfo.cpp's whole manager, reserve calls
    included, sits inside one `/* */` block.
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


def receiver_start(code, op_idx):
    """Index where the receiver expression ending at op_idx begins.

    op_idx is the index of the `.` (or of the `-` of `->`) that the call
    hangs off. Walks backwards over identifiers, `.`/`->`/`::`, and
    balanced `[...]` / `(...)`.
    """
    k = op_idx
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

    `m_UpgradeRatio[0]`     -> m_UpgradeRatio   (not `0`)
    `m_Ratio[i]`            -> m_Ratio          (not `i`)
    `this->m_Buffer`        -> m_Buffer
    `pInfo->m_Levels`       -> m_Levels
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


def find_reserves(code):
    """Yield (offset_of_call, receiver_start, receiver_end, base) per reserve()."""
    for m in RESERVE_RE.finditer(code):
        op_idx = m.start()
        start = receiver_start(code, op_idx)
        receiver = code[start:op_idx]
        base = base_name(receiver)
        if base is None:
            continue
        yield (m.start(), start, op_idx, base)


def uses_of(code, base):
    """(sized, element_use) for one container name in one blob of code.

    An optional leading `[...]` absorbs the array-of-vectors shape
    (`m_UpgradeRatio[0].resize(n)`, `m_UpgradeRatio[s][g]`).
    """
    name = re.escape(base)
    sub = r"\s*(?:\[[^\[\]]*\])?\s*"
    sized = re.compile(
        r"(?<![A-Za-z0-9_])" + name + sub + r"(?:\.|->)\s*(?:" + "|".join(SIZING_METHODS) + r")\s*\("
    )
    # `s += "x"` grows a string, so it sizes. `v[i] += x` does not — it is an
    # element use, and allowing a subscript here is what made an early
    # revision of this checker miss ZoneGroupManager's `groups` (18-P):
    # `groups[newGroupID] += pInfo->load` read as "sized" and cancelled the
    # site out. No subscript is allowed between the name and the `+=`.
    appended = re.compile(r"(?<![A-Za-z0-9_])" + name + r"\s*\+=")
    element = re.compile(
        r"(?<![A-Za-z0-9_])" + name + sub + r"(?:\.|->)\s*(?:" + "|".join(ELEMENT_METHODS) + r")\s*\("
    )
    subscript = re.compile(r"(?<![A-Za-z0-9_])" + name + sub + r"\[")
    rangefor = re.compile(r":\s*" + name + r"\s*\)")

    is_sized = bool(sized.search(code)) or bool(appended.search(code))
    is_element = (
        bool(element.search(code)) or bool(subscript.search(code)) or bool(rangefor.search(code))
    )
    return is_sized, is_element


def declared_associative(code, base):
    """True if `base` is declared with a map/set-family type in this group."""
    decl = re.compile(
        r"(?<![A-Za-z0-9_])((?:const\s+)?[A-Za-z_][A-Za-z0-9_:]*\s*<[^;{}]*?>[\s*&]+)"
        + re.escape(base)
        + r"\s*(?:\[[^\[\]]*\])?\s*[;=,)]"
    )
    for m in decl.finditer(code):
        if ASSOCIATIVE_RE.search(m.group(1)):
            return True
    return False


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
    if ".backup" in base or base.endswith(".txt"):
        return True
    return False


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
    """Return (sites, groups_scanned).

    sites is a list of (rel_path, line, base, snippet).
    """
    base_dir = os.path.dirname(root) if os.path.isfile(root) else root
    groups = {}
    for path in iter_sources(root):
        rel = os.path.relpath(path, base_dir).replace(os.sep, "/")
        with open(path, "r", encoding="utf-8", errors="replace") as fh:
            text = fh.read()
        groups.setdefault(group_key(path), []).append((rel, text))

    sites = []
    for _key, members in sorted(groups.items()):
        if not any("reserve" in text for _rel, text in members):
            continue
        masked = [(rel, strip_comments_and_mask_literals(text)) for rel, text in members]

        # Blank out each reserve() receiver so `m_X[0].reserve(n)` does not
        # count as an indexed use of m_X.
        reserves = []
        blanked = []
        for rel, code in masked:
            spans = []
            for call_off, rstart, rend, base in find_reserves(code):
                reserves.append((rel, code, call_off, base))
                spans.append((rstart, rend))
            chars = list(code)
            for rstart, rend in spans:
                for k in range(rstart, rend):
                    if chars[k] != "\n":
                        chars[k] = MASK
            blanked.append("".join(chars))
        group_code = "\n".join(blanked)

        for rel, code, call_off, base in reserves:
            if declared_associative(group_code, base):
                continue
            is_sized, is_element = uses_of(group_code, base)
            if is_sized or not is_element:
                continue
            line_no = line_of(code, call_off)
            snippet = code[max(0, call_off - 60) : call_off]
            snippet = snippet.rsplit("\n", 1)[-1].strip()
            end = code.find(")", call_off)
            snippet += code[call_off : end + 1 if end != -1 else call_off]
            sites.append((rel, line_no, base, " ".join(snippet.split())))

    sites.sort()
    return sites


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("root")
    ap.add_argument("--list", action="store_true", help="print each site")
    args = ap.parse_args()

    sites = collect(args.root)

    if args.list:
        for rel, line_no, base, snippet in sites:
            print("%s:%d: %s -- %s" % (rel, line_no, base, snippet))
    else:
        print(len(sites))
    return 0


if __name__ == "__main__":
    sys.exit(main())
