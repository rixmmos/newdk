#!/bin/sh
#
# check-sql-injection.sh — Phase 8C ratchet gate.
#
#--------------------------------------------------------------------------------
# WHAT THIS GATE MEASURES
#--------------------------------------------------------------------------------
#
# The number of *call sites* — not lines — in the compiled server sources
# that pass a printf-style format string to the variadic SQL API:
#
#     executeQuery( "... %s ..." , ...)
#     executeQueryString( "... %s ..." )
#     setStatement( "... %d ..." , ...)
#     Statement( "... %lld ..." , ...)
#
# These are the entry points that vsprintf a caller-supplied format into a
# buffer and hand it to mysql_real_query(). Each one is a candidate
# SQL-injection site: whether it is actually exploitable depends on whether
# any %s argument can carry player-controlled text, which this gate does not
# and cannot decide.
#
# The counting is done by scripts/sql_injection_sites.py, which lexes each
# file rather than grepping it. That matters — the grep this replaced used
#
#     (executeQuery|setStatement|Statement)[[:space:]]*\([^)]*%[sdluxc]
#
# whose `[^)]*` forbade any ')' between the call paren and the specifier.
# Nearly every real SQL string has one — VALUES('%s'), MAX(Level) — so it
# matched almost nothing, was --update'd to a baseline of 0 while broken,
# and was frozen green forever. It also counted lines rather than call
# sites, counted commented-out SQL, could not see a call split across
# lines, and its unanchored `Statement` alternative would have matched the
# *safe* PreparedStatement( form, so migrating code could have raised the
# count.
#
# The replacement:
#   * strips // and /* */ comments first — roughly a tenth of the
#     executeQuery occurrences in this tree are dead SQL kept as reference
#     comments (77 sites counted with comments included, 70 without);
#   * finds the argument list by a balanced-paren scan with string and char
#     literals masked, so parens inside SQL do not terminate it;
#   * therefore handles a call split across any number of lines, and counts
#     it once however many literals it concatenates;
#   * ignores %% ;
#   * excludes PreparedStatement( via an identifier-boundary lookbehind on
#     the bare `Statement` alternative.
#
#--------------------------------------------------------------------------------
# WHAT THIS GATE DELIBERATELY DOES NOT MEASURE
#--------------------------------------------------------------------------------
#
#   * Indirect format strings. `sprintf(buf, "... %s ...", name);` followed
#     by `pStmt->executeQuery(buf);` is invisible here — the executeQuery
#     call has no string literal in its argument list. So is
#     `executeQueryString(sql.toString())` built from a StringStream. These
#     are the same hazard; the metric simply cannot see them without
#     dataflow analysis. A migration that converts a literal-format call
#     into a sprintf-into-a-buffer call would LOWER this number while
#     improving nothing. Do not do that.
#   * Whether a given site is exploitable. A `%d` fed a trusted int is
#     counted exactly like a `%s` fed a player-supplied name.
#   * `Statement stmt("... %d ...");` — a stack construction with the type
#     name separated from the paren by a variable name. There are zero such
#     sites in the tree today (verified 2026-08-10); if one appears it will
#     not be counted.
#   * Anything outside src/ , and anything in *.backup, *.txt, or
#     testdb.cpp — none of which are part of a CMake target.
#
#--------------------------------------------------------------------------------
# THE BASELINE IS A RATCHET
#--------------------------------------------------------------------------------
#
# `.sql-injection-baseline` records today's count. CI fails if the count
# goes ABOVE it. Removing sites is always allowed and should be followed by
# `--update` to lock in the lower number.
#
# Lower the baseline. Never raise it. If a change genuinely needs a new
# format-string SQL site, that is a design discussion, not an --update.
# The number has been descending — ~928 live sites at 1544bbe (2026-08-07),
# 70 at main tip — and the point of the gate is to keep it descending.
#
# Usage:
#   dkrixserver/scripts/check-sql-injection.sh            # CI mode
#   dkrixserver/scripts/check-sql-injection.sh --count    # print count
#   dkrixserver/scripts/check-sql-injection.sh --list     # print sites
#   dkrixserver/scripts/check-sql-injection.sh --by-file  # per-file totals
#   dkrixserver/scripts/check-sql-injection.sh --update   # rewrite baseline
#
# Exit status:
#   0 — count <= baseline (OK, or --update / --count / --list mode)
#   1 — count > baseline (new site(s) added; either migrate or
#       explicitly update the baseline if intentional)
#   2 — usage error / baseline file missing / no python3
#--------------------------------------------------------------------------------

set -eu

# Resolve script dir so --update rewrites the sibling baseline file.
script_dir=$(cd -- "$(dirname -- "$0")" >/dev/null 2>&1 && pwd)
baseline_file="$script_dir/.sql-injection-baseline"
counter="$script_dir/sql_injection_sites.py"

# Repo layout: script lives at dkrixserver/scripts/, sources at
# dkrixserver/src/. Default to scanning that tree.
src_dir="$script_dir/../src"

if command -v python3 >/dev/null 2>&1; then
    PY=python3
elif command -v python >/dev/null 2>&1; then
    PY=python
else
    printf 'python3 not found; cannot run %s\n' "$counter" >&2
    exit 2
fi

if [ ! -f "$counter" ]; then
    printf 'counter missing: %s\n' "$counter" >&2
    exit 2
fi

count_hits() {
    "$PY" "$counter" "$src_dir"
}

list_hits() {
    "$PY" "$counter" "$src_dir" --list
}

by_file() {
    "$PY" "$counter" "$src_dir" --by-file
}

mode=${1:-check}

case "$mode" in
    --count|-c)
        count_hits
        exit 0
        ;;
    --list|-l)
        list_hits
        exit 0
        ;;
    --by-file|-f)
        by_file
        exit 0
        ;;
    --update|-u)
        count=$(count_hits)
        printf '%s\n' "$count" > "$baseline_file"
        printf 'baseline updated: %s sites\n' "$count"
        exit 0
        ;;
    check|'')
        if [ ! -f "$baseline_file" ]; then
            printf 'baseline file missing: %s\n' "$baseline_file" >&2
            printf 'run: %s --update\n' "$0" >&2
            exit 2
        fi
        baseline=$(cat "$baseline_file")
        count=$(count_hits)
        if [ "$count" -gt "$baseline" ]; then
            printf 'FAIL: SQL-injection site count went UP: %s (baseline %s)\n' \
                "$count" "$baseline" >&2
            printf '\nCurrent sites (run --list to see all):\n' >&2
            list_hits | head -20 >&2
            printf '\nOptions:\n' >&2
            printf '  - Migrate new code to PreparedStatement (Phase 11.1).\n' >&2
            printf '  - Do NOT hide a site behind sprintf-into-a-buffer; see the\n' >&2
            printf '    header of this script.\n' >&2
            printf '  - If genuinely intentional, run: %s --update\n' "$0" >&2
            exit 1
        fi
        if [ "$count" -lt "$baseline" ]; then
            printf 'OK: %s sites (baseline %s — %s sites removed; run --update to ratchet down).\n' \
                "$count" "$baseline" "$((baseline - count))"
        else
            printf 'OK: %s sites (matches baseline).\n' "$count"
        fi
        exit 0
        ;;
    -h|--help|help)
        sed -n '2,/^set -eu/p' "$0" | sed 's/^# \{0,1\}//;/^set -eu/d'
        exit 0
        ;;
    *)
        printf 'unknown mode: %s\n' "$mode" >&2
        printf 'try --help\n' >&2
        exit 2
        ;;
esac
