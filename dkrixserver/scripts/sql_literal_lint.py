#!/usr/bin/env python3
"""Syntax-lint the SQL string literals handed to the database APIs.

Used by check-sql-literals.sh. This is the gate that did not exist when
Bug 18-A shipped: `PreparedStatement s(pConn, "... FROM KeyInfoSELECT
ItemType, ... FROM KeyInfo")` — two statements fused into one literal
because a migration folded a live query and a commented-out alternative
together and dropped the `//`. `executeQuery` passed SQL through at call
time, but `PreparedStatement` sends it at construction, so the migration
turned latent garbage into a boot fatal: the gameserver could not start
at all for a day.

Nothing checked the SQL. Nothing had to: every check below is a handful
of characters of pattern matching over text the compiler already
concatenated for us.

--------------------------------------------------------------------------
WHAT IS CHECKED
--------------------------------------------------------------------------

For every SQL sink (see sql_injection_sites.py for the sink list and for
why the connection argument must be excluded), the SQL argument's maximal
runs of *adjacent* string literals are concatenated exactly as the C++
compiler would, and each run is checked:

  FUSED_STATEMENT     a statement verb glued to the preceding word with no
                      separator: `... FROM KeyInfoSELECT ItemType ...`.
                      This is Bug 18-A's exact signature.
  GLUED_KEYWORD       any of FROM / WHERE / VALUES / the statement verbs
                      glued to the preceding word. Catches the same
                      accident when the dropped separator lands somewhere
                      other than a statement boundary.
  MULTI_STATEMENT     a `;` with more SQL after it. mysql_real_query
                      rejects multi-statement text unless
                      CLIENT_MULTI_STATEMENTS is set, which this server
                      never sets, so this is always a bug.
  NO_VERB             the SQL does not begin with a recognised statement
                      keyword. Catches `executeQuery("-_-")`
                      (MonsterKillQuest.cpp) and any future placeholder
                      junk.
  MULTI_VERB          two statement verbs at paren depth 0 in one literal
                      — the 18-A shape again, seen from the other side,
                      for the case where the fusion happened at a space.
  UNBALANCED_PAREN    parens do not balance outside quoted strings.
  UNBALANCED_QUOTE    an odd number of single quotes.
  QUOTED_PLACEHOLDER  `'?'` — a bind placeholder inside a quoted literal
                      is a literal question mark, not a parameter.
  BIND_MISMATCH       for `PreparedStatement s(pConn, "...?...")`, the set
                      of distinct bind indices in the enclosing block is
                      not exactly 1..N for the N placeholders outside
                      quotes. Catches missing binds, 0-based indices,
                      gaps, and out-of-range indices.

The last four only run when the SQL argument is a *pure* literal. When a
C++ expression is spliced into the SQL (`"UPDATE " + getObjectTableName()
+ " SET ..."`) the literal runs are fragments by construction, and their
parens, quotes and verbs legitimately do not balance on their own.

--------------------------------------------------------------------------
WHAT IS NOT CHECKED
--------------------------------------------------------------------------

  * SQL assembled by StringStream. The text is spread over `operator<<`
    calls interleaved with values; concatenating the literals would
    produce a fragment soup with false positives in both directions.
    Those sites are counted (not syntax-checked) by
    check-sql-injection.sh's `stream` category.
  * Table and column names. Nothing here talks to a database or reads a
    schema; a query against a table that does not exist passes.
  * Semantics of any kind — join correctness, index use, whether the
    WHERE clause selects what the caller meant.
  * Whether the spliced fragments in a `splice` site produce valid SQL
    once concatenated at runtime. Only the literal parts are visible.
  * A mid-word break at an adjacent-literal boundary (`"...ReqAb"
    "ility..."`) is reported as INFO, never as an error: clang-format
    breaks long literals wherever it must, and the concatenation is
    correct. Only a break that *fuses a keyword* is an error, which is
    what GLUED_KEYWORD covers.

--------------------------------------------------------------------------
THE ALLOWLIST
--------------------------------------------------------------------------

This gate is not a ratchet: any finding is a real defect, so it fails on
the first one. `.sql-literal-allowlist` exists for the case where a
finding is real but cannot be fixed in the same change, keyed by file,
kind and SQL text rather than by line number so it does not need re-keying
every time the file moves. Entries are reprinted loudly on every run, an
entry that stops matching is reported as stale, and `--strict` ignores the
file entirely.

It is currently EMPTY, and that is the state to keep it in. It was created
with one entry — `MonsterKillQuest::save()`'s `executeQuery("-_-")`, open
item 6 of docs/SECURITY-AUDIT-2026-08-10.md — which is how this gate first
proved itself on a live defect; the entry was deleted hours later when
that defect was fixed.

Exit status: 0 if no un-allowlisted ERROR-level finding, 1 otherwise.
[measured 2026-08-10, whole tree: 0 errors, 1 info finding, over 1,572
PreparedStatement literals plus every other sink literal in src/.]
"""

import argparse
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from sql_injection_sites import (  # noqa: E402
    CTOR_DECL_RE,
    MASK,
    METHOD_SINKS,
    CTOR_SINKS,
    find_sinks,
    iter_sources,
    line_of,
    matching_paren,
    split_args,
    strip_comments_and_mask_literals,
)

VERBS = r"(?:SELECT|INSERT|UPDATE|DELETE|REPLACE)"

# Keywords that cannot plausibly be the tail of a real identifier. `SET`
# is deliberately absent: it is the tail of OFFSET, and
# `... LIMIT %d OFFSET %d` is legitimate SQL that would otherwise be
# reported (the one such literal in the tree is ExchangeDB.cpp:238).
GLUE_KW = r"(?:SELECT|INSERT|UPDATE|DELETE|REPLACE|FROM|WHERE|VALUES)"

# Statement keywords a query may legally begin with.
LEADING_KW = re.compile(
    r"(SELECT|INSERT|UPDATE|DELETE|REPLACE|SET|SHOW|CALL|CREATE|DROP|ALTER"
    r"|TRUNCATE|START|BEGIN|COMMIT|ROLLBACK|LOCK|UNLOCK|OPTIMIZE|DESC|DESCRIBE"
    r"|EXPLAIN|USE|FLUSH|GRANT|REVOKE|RENAME|LOAD|HANDLER|DO|ANALYZE|CHECK"
    r"|REPAIR|WITH)\b",
    re.IGNORECASE,
)

FUSED_RE = re.compile(r"[A-Za-z0-9_]" + VERBS + r"\b")
GLUED_RE = re.compile(r"[A-Za-z0-9_]" + GLUE_KW + r"\b")
SEMI_RE = re.compile(r";\s*\S")
VERB_OR_PAREN_RE = re.compile(r"[()]|" + VERBS + r"\b")
BIND_RE = r"(?<![A-Za-z0-9_])%s\s*\.\s*bind[A-Za-z]*\s*\(\s*([0-9]+)"


def unescape(body):
    """Undo the C escapes that matter for quote and paren counting."""
    return body.replace("\\'", "'").replace('\\"', '"').replace("\\\\", "\\")


def strip_quoted(sql):
    """Remove '...' string contents so their parens and ? do not count."""
    out = []
    i, n = 0, len(sql)
    while i < n:
        if sql[i] == "'":
            i += 1
            while i < n and sql[i] != "'":
                i += 1
            i += 1
        else:
            out.append(sql[i])
            i += 1
    return "".join(out)


def literal_runs(code, literals, start, end):
    """Maximal runs of adjacent string literals inside code[start:end).

    Two literals are adjacent when only whitespace separates them — which
    is exactly when the C++ compiler concatenates them.
    """
    offsets = sorted(o for o in literals if start <= o < end)
    runs, current, prev_end = [], [], None
    for off in offsets:
        close = off + 1
        while close < end and code[close] != '"':
            close += 1
        if prev_end is not None and code[prev_end:off].strip() == "":
            current.append((off, literals[off]))
        else:
            if current:
                runs.append(current)
            current = [(off, literals[off])]
        prev_end = close + 1
    if current:
        runs.append(current)
    return runs


def enclosing_block_end(code, idx):
    """Offset of the `}` closing the block that contains idx."""
    depth = 0
    for k in range(idx, len(code)):
        if code[k] == "{":
            depth += 1
        elif code[k] == "}":
            if depth == 0:
                return k
            depth -= 1
    return len(code)


def check_run(sql, pieces, is_first_run, pure, report):
    """Run the text checks over one concatenated literal run."""
    for m in FUSED_RE.finditer(sql):
        report("FUSED_STATEMENT", sql[max(0, m.start() - 40) : m.end() + 30])
    for m in GLUED_RE.finditer(sql):
        report("GLUED_KEYWORD", sql[max(0, m.start() - 40) : m.end() + 30])
    for m in SEMI_RE.finditer(sql):
        report("MULTI_STATEMENT", sql[max(0, m.start() - 30) : m.end() + 30])

    for a, b in zip(pieces, pieces[1:]):
        if a and b and a[-1].isalnum() and b[0].isalnum():
            report("INFO_CONCAT_WORDJOIN", "..." + a[-24:] + " | " + b[:24] + "...")

    if not pure:
        return

    if is_first_run and not LEADING_KW.match(sql.lstrip()):
        report("NO_VERB", sql.strip()[:80])

    bare = strip_quoted(sql)
    if bare.count("(") != bare.count(")"):
        report("UNBALANCED_PAREN", sql.strip()[:100])
    if sql.count("'") % 2:
        report("UNBALANCED_QUOTE", sql.strip()[:100])
    if "'?'" in sql:
        report("QUOTED_PLACEHOLDER", sql.strip()[:100])

    depth, verbs = 0, []
    for m in VERB_OR_PAREN_RE.finditer(bare):
        tok = m.group(0)
        if tok == "(":
            depth += 1
        elif tok == ")":
            depth -= 1
        elif depth == 0:
            verbs.append(tok)
    if len(verbs) > 1:
        report("MULTI_VERB", " + ".join(verbs) + " :: " + sql.strip()[:70])


def scan_file(path, rel):
    """Yield (rel, line, severity, kind, detail)."""
    with open(path, "r", encoding="utf-8", errors="replace") as fh:
        text = fh.read()
    if not any(name in text for name in METHOD_SINKS + CTOR_SINKS):
        return
    code, literals = strip_comments_and_mask_literals(text)

    for m, label, sql_arg, _variadic in find_sinks(code):
        open_idx = m.end() - 1
        close_idx = matching_paren(code, open_idx)
        args = list(split_args(code, open_idx, close_idx))
        if sql_arg >= len(args):
            continue
        start, end = args[sql_arg]
        seg = code[start:end]
        runs = literal_runs(code, literals, start, end)
        if not runs:
            continue
        # "pure" == the argument is nothing but adjacent string literals.
        residue = re.sub(r'"[^"\n]*"', "", seg.replace(MASK, ""))
        residue = re.sub(r"'[^'\n]*'", "", residue)
        pure = len(runs) == 1 and residue.strip() == ""

        findings = []
        for i, run in enumerate(runs):
            pieces = [unescape(body) for _off, body in run]
            sql = "".join(pieces)
            if not sql.strip():
                continue
            line = line_of(code, run[0][0])

            def report(kind, detail, _line=line):
                sev = "INFO" if kind.startswith("INFO_") else "ERROR"
                findings.append((rel, _line, sev, kind.replace("INFO_", ""), detail))

            check_run(sql, pieces, i == 0, pure, report)

        for f in findings:
            yield f

        if label == "PreparedStatement" and pure:
            decl = CTOR_DECL_RE.match(code, m.start())
            var = decl.group(2)
            sql = "".join(unescape(body) for _off, body in runs[0])
            n_ph = strip_quoted(sql).count("?")
            scope = code[close_idx : enclosing_block_end(code, close_idx)]
            idxs = [int(b.group(1)) for b in re.finditer(BIND_RE % re.escape(var), scope)]
            distinct = sorted(set(idxs))
            # Duplicates are fine: a statement re-bound in two branches or
            # in a loop binds index 1 more than once. What must hold is
            # that the distinct indices are exactly 1..N.
            if distinct != list(range(1, n_ph + 1)):
                yield (
                    rel,
                    line_of(code, m.start()),
                    "ERROR",
                    "BIND_MISMATCH",
                    "%s: %d placeholder(s), bind indices %s :: %s"
                    % (var, n_ph, distinct, sql.strip()[:60]),
                )


DEFAULT_ALLOWLIST = os.path.join(
    os.path.dirname(os.path.abspath(__file__)), ".sql-literal-allowlist"
)


def fingerprint(rel, kind, detail):
    """Line-number-independent key for one finding."""
    return "%s|%s|%s" % (rel, kind, " ".join(detail.split()))


def load_allowlist(path):
    """Return {fingerprint: comment}. Missing file is not an error."""
    entries = {}
    if not path or not os.path.isfile(path):
        return entries
    with open(path, "r", encoding="utf-8") as fh:
        for raw in fh:
            line = raw.strip()
            if not line or line.startswith("#"):
                continue
            key, _, comment = line.partition("  #")
            entries[key.strip()] = comment.strip()
    return entries


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("root", help="directory to scan, or a single file")
    ap.add_argument("--info", action="store_true", help="also print INFO findings")
    ap.add_argument("--summary", action="store_true", help="print counts only")
    ap.add_argument(
        "--strict",
        action="store_true",
        help="ignore the allowlist; fail on every finding",
    )
    ap.add_argument("--allowlist", default=DEFAULT_ALLOWLIST)
    ap.add_argument(
        "--print-fingerprints",
        action="store_true",
        help="print the allowlist key for every error (to add one by hand)",
    )
    args = ap.parse_args()

    if os.path.isfile(args.root):
        paths = [(args.root, os.path.basename(args.root))]
    else:
        paths = [
            (p, os.path.relpath(p, args.root).replace(os.sep, "/"))
            for p in iter_sources(args.root)
        ]

    allow = {} if args.strict else load_allowlist(args.allowlist)
    used = set()

    errors = 0
    infos = 0
    suppressed = []
    counts = {}
    for path, rel in paths:
        for rel_, line, sev, kind, detail in scan_file(path, rel):
            if sev == "ERROR":
                key = fingerprint(rel_, kind, detail)
                if key in allow:
                    used.add(key)
                    suppressed.append((rel_, line, kind, allow[key]))
                    continue
                errors += 1
            else:
                infos += 1
                if not args.info:
                    continue
            counts[kind] = counts.get(kind, 0) + 1
            if not args.summary:
                print(
                    "%s:%d: %s %s: %s" % (rel_, line, sev, kind, " ".join(detail.split()))
                )
                if args.print_fingerprints and sev == "ERROR":
                    print("    allowlist key: %s" % fingerprint(rel_, kind, detail))

    if suppressed:
        print("")
        print("KNOWN DEFECTS SUPPRESSED BY %s - these are real bugs, still open:"
              % os.path.basename(args.allowlist))
        for rel_, line, kind, comment in suppressed:
            print("  %s:%d  %s  %s" % (rel_, line, kind, comment))

    stale = set(allow) - used
    if stale:
        print("")
        print("NOTE: %d allowlist entr(ies) matched nothing — either the defect was"
              % len(stale))
        print("fixed (delete the line) or the SQL text changed (re-key it):")
        for key in sorted(stale):
            print("  %s" % key)

    print("")
    print(
        "SQL literal lint: %d error(s), %d info finding(s), %d suppressed."
        % (errors, infos, len(suppressed))
    )
    if counts:
        for kind in sorted(counts):
            print("  %-22s %d" % (kind, counts[kind]))
    return 1 if errors else 0


if __name__ == "__main__":
    sys.exit(main())
