#!/usr/bin/env python3
"""Count runtime-assembled SQL sites in the server tree.

Used by check-sql-injection.sh. See that script's header for what the gate
means and what it deliberately does not measure.

--------------------------------------------------------------------------
WHAT COUNTS AS A SITE
--------------------------------------------------------------------------

A *SQL sink* is one of

    pStmt->executeQuery( SQL , ... )        variadic, vsprintf'd
    pStmt->executeQueryString( SQL )        pass-through
    pStmt->setStatement( SQL , ... )        variadic, vsprintf'd
    Statement  name( pConn, SQL , ... )     stack construction, variadic
    Statement( pConn, SQL , ... )           call form
    PreparedStatement name( pConn, SQL )    parameterised

For each sink the *SQL argument* is isolated (argument 0 for the method
forms, argument 1 for the constructor forms — the connection is argument
0, and folding it into the SQL is exactly how a naive scan invents the
Bug 18-A signature `DARKEDENSELECT ...`). The argument expression is then
classified with its string literals masked out:

  format  the argument is entirely string literals AND one of them carries
          a printf conversion specifier, AND the sink is a variadic one.
          The caller's values are vsprintf'd into the SQL text with no
          escaping: the classic injection shape.

  stream  the argument is not a pure literal and mentions `.toString()`.
          This is the `StringStream sql; sql << "..." << value; ...
          executeQueryString(sql.toString())` shape — SQL assembled by
          operator<< over many lines, the dominant legacy construction in
          this tree, and completely invisible to every earlier revision of
          this gate.

  splice  the argument is not a pure literal and does not mention
          toString(). This is `"UPDATE " + getObjectTableName() + " SET
          ..."` and friends: a C++ expression concatenated into the SQL
          text. Includes the ~20 deliberate identifier splices that cannot
          be parameterised (a placeholder cannot bind a table or column
          name) and the handful of sinks whose SQL arrives through a local
          variable the checker cannot see through.

`format` + `stream` + `splice` = every site where the SQL text handed to
MySQL is not a compile-time constant. That total is the injection
*surface*. Whether a given site is exploitable depends on whether the
interpolated value can carry player-controlled text, which this gate does
not and cannot decide.

Sites whose SQL argument is a pure string literal with no conversion
specifier are *static* and are not counted at all: MySQL receives exactly
what is in the source.

--------------------------------------------------------------------------
HOW IT AVOIDS THE FAILURE MODES OF ITS PREDECESSORS
--------------------------------------------------------------------------

  * comments are removed first (`//` and `/* */`), so the large body of
    dead pre-migration SQL kept as reference comments does not count;
  * the argument list is found by a balanced-paren scan with string and
    char literals masked, so `VALUES('%s')` and `MAX(Level)` are handled;
  * calls split across any number of lines are handled, because the scan
    is over the whole file rather than line by line — this is what makes
    the multi-line StringStream and multi-line PreparedStatement shapes
    visible;
  * the argument list is split on *top-level* commas, so the connection
    argument is excluded from the SQL and nested calls stay intact;
  * `PreparedStatement(` is excluded from the bare `Statement`
    alternative by an identifier-boundary lookbehind, and is matched
    separately in its own declaration form;
  * one call site counts once, however many lines or literals it spans;
  * `%%` is not a conversion specifier;
  * a conversion specifier inside a *PreparedStatement* literal is NOT a
    format site — that API never vsprintfs. `DATE_FORMAT(t,'%y%m%d')` in
    GuildUnion.cpp:609 is the one such literal in the tree and counting it
    would be a false positive.
"""

import argparse
import os
import re
import sys

# Sinks taking the SQL as argument 0.
METHOD_SINKS = ("executeQueryString", "executeQuery", "setStatement")
# Sinks taking the connection as argument 0 and the SQL as argument 1.
CTOR_SINKS = ("PreparedStatement", "Statement")

# Variadic sinks: these vsprintf the SQL argument. A conversion specifier
# in one of their literals is a format-string SQL site.
VARIADIC = ("executeQuery", "setStatement", "Statement")

METHOD_RE = re.compile(r"(?<![A-Za-z0-9_])(" + "|".join(METHOD_SINKS) + r")\s*\(")
# `PreparedStatement stmt(` / `Statement stmt(` — a stack construction with
# the type name separated from the paren by the variable name.
CTOR_DECL_RE = re.compile(
    r"(?<![A-Za-z0-9_])(" + "|".join(CTOR_SINKS) + r")\s+([A-Za-z_][A-Za-z0-9_]*)\s*\("
)
# `Statement(` / `new Statement(` call form. The lookbehind keeps
# `PreparedStatement(` and `setStatement(` out.
CTOR_CALL_RE = re.compile(r"(?<![A-Za-z0-9_])Statement\s*\(")

# A printf conversion specifier, minus the `%` and minus `%%`.
CONV_RE = re.compile(
    r"%[-+ #0']*[0-9*]*(?:\.[0-9*]+)?(?:hh|h|ll|l|L|j|z|t|q|I64)?[diouxXeEfFgGaAcspn]"
)

SOURCE_EXTS = (".cpp", ".h", ".inl")

MASK = "\x01"  # stands in for one character of masked literal/comment text

# The database API's own declarations and definitions. Their parameter
# lists look exactly like sink calls with a non-literal SQL argument
# (`executeQuery(const char* fmt, ...)`), so scanning them would report
# the gate's own plumbing as five dynamic-SQL sites.
API_FILES = ("Statement.cpp", "Statement.h", "PreparedStatement.cpp", "PreparedStatement.h")

CATEGORIES = ("format", "stream", "splice")


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


def split_args(code, open_idx, close_idx):
    """Yield (start, end) of each top-level argument between the parens.

    Literals are already masked, so a comma inside SQL text cannot split an
    argument. Nesting is tracked so `getConnection("DARKEDEN")` and
    `IN(1, 2, 3)` stay inside one argument.
    """
    depth = 0
    start = open_idx + 1
    for k in range(open_idx + 1, close_idx):
        ch = code[k]
        if ch in "([{":
            depth += 1
        elif ch in ")]}":
            depth -= 1
        elif ch == "," and depth == 0:
            yield (start, k)
            start = k + 1
    yield (start, close_idx)


def find_sinks(code):
    """Yield (match, label, sql_arg_index, variadic) for every SQL sink."""
    for m in METHOD_RE.finditer(code):
        yield (m, m.group(1), 0, m.group(1) in VARIADIC)
    for m in CTOR_DECL_RE.finditer(code):
        yield (m, m.group(1), 1, m.group(1) in VARIADIC)
    for m in CTOR_CALL_RE.finditer(code):
        # `PreparedStatement(` — the lookbehind above only guards the
        # identifier characters immediately before, and `Prepared` ends in
        # one, so this cannot actually fire; keep the guard anyway in case
        # the regex is loosened later.
        if code[max(0, m.start() - 20) : m.start()].rstrip().endswith("Prepared"):
            continue
        yield (m, "Statement", 1, True)


def classify(seg, arg_literals, variadic):
    """Return a category for one SQL argument, or None if it is static."""
    # Remove the masked literals from the expression; what is left is the
    # C++ that contributes to the SQL text at runtime. `+` and whitespace
    # are just concatenation glue.
    rem = re.sub(r'"[^"\n]*"', "", seg.replace(MASK, ""))
    rem = re.sub(r"'[^'\n]*'", "", rem)
    rem = re.sub(r"[\s+]", "", rem)
    if rem:
        return "stream" if ".toString" in seg else "splice"
    if not arg_literals:
        return None  # no SQL at all (a declaration, or an empty arg list)
    if variadic and any(has_conversion(lit) for lit in arg_literals):
        return "format"
    return None


def line_of(code, offset):
    return code.count("\n", 0, offset) + 1


def scan_file(path):
    """Yield (line_no, category, callee, snippet) for each site in path."""
    with open(path, "r", encoding="utf-8", errors="replace") as fh:
        text = fh.read()
    if not any(name in text for name in METHOD_SINKS + CTOR_SINKS):
        return
    code, literals = strip_comments_and_mask_literals(text)
    lit_offsets = sorted(literals)

    for m, label, sql_arg, variadic in find_sinks(code):
        open_idx = m.end() - 1
        close_idx = matching_paren(code, open_idx)
        args = list(split_args(code, open_idx, close_idx))
        if sql_arg >= len(args):
            continue
        start, end = args[sql_arg]
        seg = code[start:end]
        arg_literals = [literals[o] for o in lit_offsets if start <= o < end]
        cat = classify(seg, arg_literals, variadic)
        if cat is None:
            continue
        if cat == "format":
            snippet = next(lit for lit in arg_literals if has_conversion(lit))
        else:
            snippet = " ".join(seg.split())
        yield (line_of(code, m.start()), cat, label, snippet)


def excluded(path):
    base = os.path.basename(path)
    if base == "testdb.cpp" or base in API_FILES:
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


def collect(root):
    """Return (counts_by_category, per_file_counts, sites)."""
    counts = dict((c, 0) for c in CATEGORIES)
    per_file = {}
    sites = []
    for path in iter_sources(root):
        rel = os.path.relpath(path, root).replace(os.sep, "/")
        for line_no, cat, callee, snippet in scan_file(path):
            counts[cat] += 1
            per_file[rel] = per_file.get(rel, 0) + 1
            sites.append((rel, line_no, cat, callee, snippet))
    return counts, per_file, sites


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("root")
    ap.add_argument("--list", action="store_true", help="print each site")
    ap.add_argument("--by-file", action="store_true", help="print per-file totals")
    ap.add_argument(
        "--breakdown", action="store_true", help="print per-category counts (key=value)"
    )
    ap.add_argument(
        "--only", choices=CATEGORIES, help="restrict --list/--by-file to one category"
    )
    args = ap.parse_args()

    counts, per_file, sites = collect(args.root)

    if args.only:
        sites = [s for s in sites if s[2] == args.only]
        per_file = {}
        for rel, _line, _cat, _callee, _snip in sites:
            per_file[rel] = per_file.get(rel, 0) + 1

    if args.list:
        for rel, line_no, cat, callee, snippet in sites:
            text = snippet.strip()
            if len(text) > 90:
                text = text[:90] + "..."
            print("%s:%d: [%s] %s( %s" % (rel, line_no, cat, callee, text))
    if args.by_file:
        for rel, n in sorted(per_file.items(), key=lambda kv: (-kv[1], kv[0])):
            print("%4d  %s" % (n, rel))
    if args.breakdown:
        for cat in CATEGORIES:
            print("%s=%d" % (cat, counts[cat]))
        print("total=%d" % sum(counts.values()))
    if not (args.list or args.by_file or args.breakdown):
        print(sum(counts.values()))
    return 0


if __name__ == "__main__":
    sys.exit(main())
