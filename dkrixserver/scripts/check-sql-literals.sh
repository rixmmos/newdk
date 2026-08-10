#!/bin/sh
#
# check-sql-literals.sh — SQL string-literal syntax gate.
#
#--------------------------------------------------------------------------------
# WHY THIS EXISTS
#--------------------------------------------------------------------------------
#
# Bug 18-A: a Phase 11.2 migration folded a live 9-column query and a
# commented-out 7-column alternative into one string literal and dropped
# the `//`, producing
#
#     "... TargetType FROM KeyInfoSELECT ItemType, ... FROM KeyInfo"
#
# `executeQuery` had passed SQL through at call time; `PreparedStatement`
# sends it at construction, so the migration turned latent garbage into a
# boot fatal. The gameserver could not start at all from 2026-08-09 until
# 9422e9c fixed it. It was found by a human launching a server, because
# nothing in CI looked at the SQL.
#
# This gate looks at the SQL. Verified against the defect it exists for:
# run over `git show 9422e9c^:.../item/Key.cpp`, it reports three
# independent errors (FUSED_STATEMENT, GLUED_KEYWORD, MULTI_VERB) and
# exits 1; over the fixed file it exits 0. [measured 2026-08-10]
#
# The checks, what they cannot see, and the allowlist mechanism are
# documented in scripts/sql_literal_lint.py's module docstring. Short
# version: adjacent string literals are concatenated exactly as the
# compiler would and then checked for fused statements, glued keywords,
# stray `;`, missing statement verb, unbalanced parens/quotes, quoted `?`
# placeholders, and placeholder/bind-index mismatch on every
# PreparedStatement. It does not check StringStream-assembled SQL, table
# or column names, or anything semantic.
#
#--------------------------------------------------------------------------------
# NOT A RATCHET
#--------------------------------------------------------------------------------
#
# Every finding is a real defect, so this fails on the first one. It found
# one on the day it landed: `executeQuery("-_-")` in
# MonsterKillQuest::save(), open item 6 of
# docs/SECURITY-AUDIT-2026-08-10.md. That was fixed hours later, so
# `.sql-literal-allowlist` is empty and the gate is green on its own
# merits, not by suppression. Keep it that way; `--strict` ignores the
# allowlist and is the honest view of the tree.
#
# Usage:
#   dkrixserver/scripts/check-sql-literals.sh          # CI mode
#   dkrixserver/scripts/check-sql-literals.sh --strict # ignore allowlist
#   dkrixserver/scripts/check-sql-literals.sh --info   # include INFO findings
#   dkrixserver/scripts/check-sql-literals.sh --keys   # print allowlist keys
#   dkrixserver/scripts/check-sql-literals.sh <file>   # lint one file
#
# Exit status:
#   0 — no un-allowlisted error
#   1 — at least one error
#   2 — usage error / linter missing / no python3
#--------------------------------------------------------------------------------

set -eu

script_dir=$(cd -- "$(dirname -- "$0")" >/dev/null 2>&1 && pwd)
linter="$script_dir/sql_literal_lint.py"
src_dir="$script_dir/../src"

if command -v python3 >/dev/null 2>&1; then
    PY=python3
elif command -v python >/dev/null 2>&1; then
    PY=python
else
    printf 'python3 not found; cannot run %s\n' "$linter" >&2
    exit 2
fi

if [ ! -f "$linter" ]; then
    printf 'linter missing: %s\n' "$linter" >&2
    exit 2
fi

target="$src_dir"
extra=""

case "${1:-}" in
    '')
        ;;
    --strict)
        extra="--strict"
        ;;
    --info)
        extra="--info"
        ;;
    --keys)
        extra="--print-fingerprints"
        ;;
    -h|--help|help)
        sed -n '2,/^set -eu/p' "$0" | sed 's/^# \{0,1\}//;/^set -eu/d'
        exit 0
        ;;
    -*)
        printf 'unknown option: %s\n' "$1" >&2
        printf 'try --help\n' >&2
        exit 2
        ;;
    *)
        target="$1"
        ;;
esac

if "$PY" "$linter" "$target" $extra; then
    exit 0
fi

printf '\n' >&2
printf 'FAIL: malformed SQL in a string literal.\n' >&2
printf 'This is the Bug 18-A class: the server will not run the query you think\n' >&2
printf 'it will, and with PreparedStatement it will not boot at all.\n' >&2
printf 'Fix the literal. If a finding is genuinely a false positive, say why in\n' >&2
printf 'the commit message and add it to scripts/.sql-literal-allowlist\n' >&2
printf '(get the key with: %s --keys).\n' "$0" >&2
exit 1
