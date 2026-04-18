#!/bin/sh
#
# check-sql-injection.sh — Phase 8C baseline grep-gate.
#
# Counts occurrences of the variadic/format-string SQL API in
# compiled server sources. The gate is simple: today's count is
# frozen as a ratchet in `.sql-injection-baseline`; CI fails the
# job if a change pushes the count ABOVE that number. Removing
# sites (by migrating them to a safe API once it exists) is always
# allowed and should be followed by re-running this script with
# --update to lower the baseline.
#
# Patterns matched (compiled .cpp / .h / .inl only — excludes
# Query.txt, *.backup, and testdb.cpp which are not part of any
# CMake target):
#
#   executeQuery(  "... %[sdluxc] ..." , ...)
#   setStatement(  "... %[sdluxc] ..." , ...)
#   Statement     (... %[sdluxc] ... , ...)
#
# These are the three entry points that feed a vsprintf'd buffer
# into mysql_real_query(). Every hit is a candidate SQL-injection
# site. The gate does not try to distinguish "safe" printf-builds
# (e.g. only %d args from trusted ints) from "unsafe" ones (%s from
# player-supplied strings) — in a codebase this large, ratcheting
# the total is the cheapest way to prevent regressions until the
# PreparedStatement wrapper lands in Phase 11.
#
# Usage:
#   dkrixserver/scripts/check-sql-injection.sh            # CI mode
#   dkrixserver/scripts/check-sql-injection.sh --count    # print count
#   dkrixserver/scripts/check-sql-injection.sh --list     # print sites
#   dkrixserver/scripts/check-sql-injection.sh --update   # rewrite baseline
#
# Exit status:
#   0 — count <= baseline (OK, or --update / --count / --list mode)
#   1 — count > baseline (new site(s) added; either migrate or
#       explicitly update the baseline if intentional)
#   2 — usage error / baseline file missing
#--------------------------------------------------------------------------------

set -eu

# Resolve script dir so --update rewrites the sibling baseline file.
script_dir=$(cd -- "$(dirname -- "$0")" >/dev/null 2>&1 && pwd)
baseline_file="$script_dir/.sql-injection-baseline"

# Repo layout: script lives at dkrixserver/scripts/, sources at
# dkrixserver/src/. Default to scanning that tree.
src_dir="$script_dir/../src"

pattern='(executeQuery|setStatement|Statement)[[:space:]]*\([^)]*%[sdluxc]'

count_hits() {
    grep -rE "$pattern" "$src_dir" \
        --include='*.cpp' --include='*.h' --include='*.inl' 2>/dev/null \
    | grep -v -E '\.(backup|txt)(:|$)' \
    | grep -v 'testdb\.cpp:' \
    | wc -l \
    | tr -d ' '
}

list_hits() {
    grep -rnE "$pattern" "$src_dir" \
        --include='*.cpp' --include='*.h' --include='*.inl' 2>/dev/null \
    | grep -v -E '\.(backup|txt):' \
    | grep -v 'testdb\.cpp:'
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
            printf '\nNew sites (run --list to see all):\n' >&2
            list_hits | head -20 >&2
            printf '\nOptions:\n' >&2
            printf '  - Migrate new code to PreparedStatement (Phase 11.1).\n' >&2
            printf '  - If intentional, run: %s --update\n' "$0" >&2
            exit 1
        fi
        if [ "$count" -lt "$baseline" ]; then
            printf 'OK: %s sites (baseline %s — %s sites removed; consider --update).\n' \
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
