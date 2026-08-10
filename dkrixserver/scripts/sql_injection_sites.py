#!/usr/bin/env python3
"""Count format-string SQL call sites in the server tree.

Used by check-sql-injection.sh. See that script's header for what the gate
means and what it deliberately does not measure.

What counts as a site: a call to one of

    executeQuery( executeQueryString( setStatement( Statement(

whose argument list contains a string literal holding a real printf
conversion specifier (%%s, %%d, %%lld, ...). `%%%%` is not a specifier.

How it avoids the failure modes of the grep it replaces:

  * comments are removed first (`//` and `/* */`), so the ~half of the
    legacy `executeQuery` occurrences that are dead SQL kept as reference
    comments do not count;
  * the argument list is found by a balanced-paren scan with string and
    char literals masked, so `VALUES('%%s')` and `MAX(Level)` are handled;
  * calls split across many lines are handled, because the scan is over
    the whole file, not line by line;
  * `PreparedStatement(` -- the safe form -- is excluded by an
    identifier-boundary lookbehind on the bare `Statement` alternative;
  * one call site counts once, however many lines or literals it spans.
"""

import argparse
import os
import re
import sys

TARGETS = ("executeQueryString", "executeQuery", "setStatement", "Statement")

# Identifier boundary on the left keeps `PreparedStatement(` and
# `setStatement(` from also matching the bare `Statement` alternative.
CALL_RE = re.compile(r"(?<![A-Za-z0-9_])(" + "|".join(TARGETS) + r")\s*\(")

# A printf conversion specifier, minus the `%` and minus `%%`.
CONV_RE = re.compile(
    r"%[-+ #0']*[0-9*]*(?:\.[0-9*]+)?(?:hh|h|ll|l|L|j|z|t|q|I64)?[diouxXeEfFgGaAcspn]"
)

SOURCE_EXTS = (".cpp", ".h", ".inl")

MASK = "\x01"  # stands in for one character of masked literal/comment text


def strip_comments_and_mask_literals(text):
    """Return (masked_code, literals).

    masked_code has the same length as text: comment bodies and the insides
    of string/char literals are replaced by filler, newlines preserved, so
    offsets and line numbers still line up. literals maps the offset of a
    string literal's opening quote to its raw body.
    """
    n = len(text)
    out = []
    literals = {}
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
            body = []
            while j < n:
                if text[j] == "\\" and j + 1 < n:
                    body.append(text[j])
                    body.append(text[j + 1])
                    j += 2
                    continue
                if text[j] == quote:
                    break
                if text[j] == "\n":  # unterminated on this line; give up on it
                    break
                body.append(text[j])
                j += 1
            end = j + 1 if j < n and text[j] == quote else j
            if quote == '"':
                literals[start] = "".join(body)
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
    return "".join(out), literals


def has_conversion(s):
    """True if s holds a printf conversion specifier. `%%` is skipped."""
    i = 0
    n = len(s)
    while i < n:
        if s[i] == "%":
            if i + 1 < n and s[i + 1] == "%":
                i += 2
                continue
            if CONV_RE.match(s, i):
                return True
            i += 1
        else:
            i += 1
    return False


def matching_paren(code, open_idx):
    """Index of the `)` closing the `(` at open_idx, or len(code)."""
    depth = 0
    k = open_idx
    n = len(code)
    while k < n:
        ch = code[k]
        if ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
            if depth == 0:
                return k
        k += 1
    return n


def scan_file(path):
    """Yield (line_no, callee, first_offending_literal) for each site."""
    with open(path, "r", encoding="utf-8", errors="replace") as fh:
        text = fh.read()
    if not any(name in text for name in TARGETS):
        return
    code, literals = strip_comments_and_mask_literals(text)
    lit_offsets = sorted(literals)
    line_starts = [0]
    for m in re.finditer("\n", code):
        line_starts.append(m.end())

    for m in CALL_RE.finditer(code):
        callee = m.group(1)
        open_idx = m.end() - 1
        close_idx = matching_paren(code, open_idx)
        hit = None
        for off in lit_offsets:
            if off < open_idx:
                continue
            if off > close_idx:
                break
            if has_conversion(literals[off]):
                hit = literals[off]
                break
        if hit is None:
            continue
        # bisect without importing: line_starts is sorted and small enough
        lo, hi = 0, len(line_starts) - 1
        while lo < hi:
            mid = (lo + hi + 1) // 2
            if line_starts[mid] <= m.start():
                lo = mid
            else:
                hi = mid - 1
        yield (lo + 1, callee, hit)


def excluded(path):
    base = os.path.basename(path)
    if base == "testdb.cpp":
        return True
    if ".backup" in base or base.endswith(".txt"):
        return True
    return False


def iter_sources(root):
    for dirpath, dirnames, filenames in os.walk(root):
        dirnames[:] = [d for d in dirnames if d not in (".git", "build", "bin", "lib")]
        for name in sorted(filenames):
            if not name.endswith(SOURCE_EXTS):
                continue
            path = os.path.join(dirpath, name)
            if excluded(path):
                continue
            yield path


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("root")
    ap.add_argument("--list", action="store_true", help="print each site")
    ap.add_argument("--by-file", action="store_true", help="print per-file totals")
    args = ap.parse_args()

    total = 0
    per_file = {}
    for path in iter_sources(args.root):
        rel = os.path.relpath(path, args.root).replace(os.sep, "/")
        for line_no, callee, lit in scan_file(path):
            total += 1
            per_file[rel] = per_file.get(rel, 0) + 1
            if args.list:
                snippet = lit.strip()
                if len(snippet) > 90:
                    snippet = snippet[:90] + "..."
                print("%s:%d: %s(  \"%s\"" % (rel, line_no, callee, snippet))
    if args.by_file:
        for rel, n in sorted(per_file.items(), key=lambda kv: (-kv[1], kv[0])):
            print("%4d  %s" % (n, rel))
    if not args.list and not args.by_file:
        print(total)
    return 0


if __name__ == "__main__":
    sys.exit(main())
