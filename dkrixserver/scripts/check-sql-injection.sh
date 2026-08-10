#!/bin/sh
#
# check-sql-injection.sh — Phase 8C ratchet gate.
#
#--------------------------------------------------------------------------------
# WHAT THIS GATE MEASURES
#--------------------------------------------------------------------------------
#
# The number of *call sites* — not lines — in the compiled server sources
# where the SQL text handed to MySQL is assembled at runtime rather than
# being a compile-time constant. Three shapes, counted and ratcheted
# separately:
#
#   format  a variadic API vsprintf's caller values into the SQL:
#               executeQuery( "... %s ..." , name)
#               setStatement( "... %d ..." , n)
#               Statement stmt(pConn, "... %lld ..." , id)
#
#   stream  SQL built by operator<< over a StringStream and handed to a
#           sink as `.toString()`:
#               StringStream sql;
#               sql << "SELECT ... WHERE Name = '" << name << "'";
#               pStmt->executeQueryString(sql.toString());
#
#   splice  a C++ expression concatenated into the SQL text:
#               pStmt->executeQueryString(
#                   "DELETE FROM PotionObject WHERE OwnerID = '" + ownerID + "'");
#               PreparedStatement s(pConn, "UPDATE " + getObjectTableName() + " SET ...");
#
# Their sum is the injection *surface*. Whether a given site is actually
# exploitable depends on whether the interpolated value can carry
# player-controlled text, which this gate does not and cannot decide — a
# `%d` fed a trusted int counts exactly like a `%s` fed a player name.
#
# Sites whose SQL argument is a pure string literal are *static* and are
# not counted: MySQL receives exactly what is written in the source.
#
#--------------------------------------------------------------------------------
# WHY THE 2026-08-09 REVISION OF THIS GATE WAS WORTHLESS
#--------------------------------------------------------------------------------
#
# It counted `format` only, so it reported 70 while 188 stream/splice
# sites sat next to it unseen — including 41 in
# loginserver/ItemDestroyer.cpp, every one of them splicing an ownerID
# straight into a quoted SQL literal. Before that it was a grep whose
# `[^)]*` forbade any ')' between the call paren and the specifier, which
# nearly every real SQL string has (VALUES('%s'), MAX(Level)), so it
# matched almost nothing, was --update'd to a baseline of 0 while broken,
# and was frozen green forever.
#
# The counting is done by scripts/sql_injection_sites.py, which lexes each
# file rather than grepping it: comments are stripped (roughly a tenth of
# the executeQuery occurrences in this tree are dead SQL kept as reference
# comments), string and char literals are masked before the balanced-paren
# scan so parens inside SQL do not terminate the argument list, and the
# argument list is split on top-level commas so the *connection* argument
# of a PreparedStatement is never mistaken for SQL. That last point is not
# theoretical — folding argument 0 into the SQL is how a naive scan
# invents the Bug 18-A signature `DARKEDENSELECT Fame, ... FROM Slayer`.
# Two auditors generated exactly that false positive by hand.
#
# The `format` set this revision reports is byte-for-byte the same 70
# sites the previous revision reported [verified 2026-08-10]; `stream` and
# `splice` are new coverage, not a re-interpretation of the old number.
#
#--------------------------------------------------------------------------------
# WHAT THIS GATE STILL CANNOT SEE
#--------------------------------------------------------------------------------
#
#   * SQL that arrives through a variable the checker cannot follow.
#     `sprintf(buf, "... %s ...", name); pStmt->executeQuery(buf);` is
#     counted (as `splice`, because the argument is not a literal) but the
#     format string itself is invisible, and a query assembled three
#     functions away is invisible in the same way. There are no live
#     sprintf-into-a-buffer sinks in the tree today [measured 2026-08-10],
#     so this is a latent hole, not a current gap.
#   * Whether the spliced value is attacker-controlled. ~20 of the
#     `splice` sites are deliberate identifier splices (table and column
#     names — a placeholder cannot bind an identifier) and are documented
#     as such in MODERNIZATION.md. The gate counts them anyway: it counts
#     surface, not verdicts.
#   * Malformed SQL. Bug 18-A was a *syntactically broken literal* — two
#     statements fused into one string — and it is invisible here by
#     construction, because a pure literal is static and static is exactly
#     what this gate is trying to reward. That is a different gate:
#     scripts/check-sql-literals.sh.
#   * Escaping. A site that splices a value through a proper
#     mysql_real_escape_string() counts the same as one that does not.
#   * `Statement` used through a typedef or an alias, and anything outside
#     src/. The database API's own declarations
#     (Statement.{h,cpp}, PreparedStatement.{h,cpp}) are excluded — their
#     parameter lists look exactly like sinks with a dynamic SQL argument.
#   * Anything in *.backup, *.txt, or testdb.cpp — none of which are part
#     of a CMake target.
#
#--------------------------------------------------------------------------------
# THE BASELINE IS A RATCHET
#--------------------------------------------------------------------------------
#
# `.sql-injection-baseline` records today's count per category. CI fails if
# ANY category goes above its baseline. Per-category rather than one total
# so that a change cannot hide a new `format` site behind two removed
# `splice` sites.
#
# Removing sites is always allowed and should be followed by `--update` to
# lock in the lower numbers.
#
# Lower the baseline. Never raise it. If a change genuinely needs a new
# runtime-assembled SQL site, that is a design discussion, not an
# --update.
#
# Usage:
#   dkrixserver/scripts/check-sql-injection.sh              # CI mode
#   dkrixserver/scripts/check-sql-injection.sh --count      # print total
#   dkrixserver/scripts/check-sql-injection.sh --breakdown  # per category
#   dkrixserver/scripts/check-sql-injection.sh --list       # print sites
#   dkrixserver/scripts/check-sql-injection.sh --list format # one category
#   dkrixserver/scripts/check-sql-injection.sh --by-file    # per-file totals
#   dkrixserver/scripts/check-sql-injection.sh --update     # rewrite baseline
#
# Exit status:
#   0 — every category <= its baseline (OK, or an informational mode)
#   1 — some category went UP (migrate, or explicitly update the baseline)
#   2 — usage error / baseline file missing or unparseable / no python3
#--------------------------------------------------------------------------------

set -eu

# Resolve script dir so --update rewrites the sibling baseline file.
script_dir=$(cd -- "$(dirname -- "$0")" >/dev/null 2>&1 && pwd)
baseline_file="$script_dir/.sql-injection-baseline"
counter="$script_dir/sql_injection_sites.py"

# Repo layout: script lives at dkrixserver/scripts/, sources at
# dkrixserver/src/. Default to scanning that tree.
src_dir="$script_dir/../src"

CATEGORIES="format stream splice"

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

breakdown() {
    "$PY" "$counter" "$src_dir" --breakdown
}

# Read one key out of a `key=value` stream.
#
# The \r strip is not cosmetic. This repo is authored on Windows; .gitattributes
# normalises to LF in the repo, so CI is safe, but a working-tree copy of the
# baseline can carry CRLF and then `[ "164<CR>" -gt "164" ]` is an illegal
# number rather than a comparison. Strip it at the one place values are read.
value_of() {
    # $1 = key, stdin = key=value lines
    tr -d '\r' | awk -F= -v k="$1" '$1==k {print $2; found=1} END {if (!found) print ""}'
}

mode=${1:-check}

case "$mode" in
    --count|-c)
        breakdown | value_of total
        exit 0
        ;;
    --breakdown|-b)
        breakdown
        exit 0
        ;;
    --list|-l)
        if [ $# -ge 2 ]; then
            "$PY" "$counter" "$src_dir" --list --only "$2"
        else
            "$PY" "$counter" "$src_dir" --list
        fi
        exit 0
        ;;
    --by-file|-f)
        if [ $# -ge 2 ]; then
            "$PY" "$counter" "$src_dir" --by-file --only "$2"
        else
            "$PY" "$counter" "$src_dir" --by-file
        fi
        exit 0
        ;;
    --update|-u)
        tmp="$baseline_file.tmp.$$"
        {
            printf '# Per-category baseline for check-sql-injection.sh.\n'
            printf '# Ratchet: CI fails if ANY category exceeds its number here.\n'
            printf '# Lower it when sites are removed; never raise it silently.\n'
            printf '# Regenerate with: dkrixserver/scripts/check-sql-injection.sh --update\n'
            breakdown
        } > "$tmp"
        mv "$tmp" "$baseline_file"
        printf 'baseline updated:\n'
        breakdown | sed 's/^/  /'
        exit 0
        ;;
    check|'')
        if [ ! -f "$baseline_file" ]; then
            printf 'baseline file missing: %s\n' "$baseline_file" >&2
            printf 'run: %s --update\n' "$0" >&2
            exit 2
        fi

        current=$(breakdown)
        baseline=$(grep -v '^#' "$baseline_file" | grep '=' || true)

        failed=0
        printf 'SQL construction sites (runtime-assembled SQL text):\n'
        for cat in $CATEGORIES; do
            now=$(printf '%s\n' "$current"  | value_of "$cat")
            was=$(printf '%s\n' "$baseline" | value_of "$cat")
            if [ -z "$was" ]; then
                printf 'FAIL: baseline has no entry for category "%s".\n' "$cat" >&2
                printf 'The baseline file predates the multi-category gate. Review the\n' >&2
                printf 'new numbers, then run: %s --update\n' "$0" >&2
                exit 2
            fi
            if [ "$now" -gt "$was" ]; then
                printf '  %-7s %4s  (baseline %s)  <-- WENT UP\n' "$cat" "$now" "$was"
                failed=1
            elif [ "$now" -lt "$was" ]; then
                printf '  %-7s %4s  (baseline %s, %s removed — run --update to ratchet down)\n' \
                    "$cat" "$now" "$was" "$((was - now))"
            else
                printf '  %-7s %4s  (baseline %s)\n' "$cat" "$now" "$was"
            fi
        done
        printf '  %-7s %4s\n' "total" "$(printf '%s\n' "$current" | value_of total)"

        printf '\nCovered: printf-format sinks, multi-line StringStream construction,\n'
        printf 'and expression splices, across executeQuery / executeQueryString /\n'
        printf 'setStatement / Statement / PreparedStatement, comments stripped.\n'
        printf 'NOT covered: SQL reaching a sink through a variable the lexer cannot\n'
        printf 'follow; whether a spliced value is attacker-controlled; whether it is\n'
        printf 'escaped; and malformed SQL literals (Bug 18-A) — that is\n'
        printf 'scripts/check-sql-literals.sh, a separate gate.\n'

        if [ "$failed" -ne 0 ]; then
            printf '\nFAIL: a category went UP.\n' >&2
            printf 'Options:\n' >&2
            printf '  - Migrate the new code to PreparedStatement with bound parameters.\n' >&2
            printf '  - Do NOT hide a site behind a buffer or a helper; the gate counts\n' >&2
            printf '    surface, and moving surface out of sight is worse than leaving it.\n' >&2
            printf '  - If genuinely intentional (an identifier splice cannot be bound),\n' >&2
            printf '    say so in the commit message and run: %s --update\n' "$0" >&2
            printf '\nRun --list <category> to see the sites.\n' >&2
            exit 1
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
