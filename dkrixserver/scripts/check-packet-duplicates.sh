#!/bin/sh
#
# check-packet-duplicates.sh — Phase 12.0 baseline duplicate gate.
#
# Counts exact-match filename duplicates between the server's
# packet class tree and the client's Cpackets subdirectory. The
# gate is simple: today's count is frozen as a ratchet in
# `.packet-duplicates-baseline`; CI fails the job if a change
# pushes the count ABOVE that number. Removing duplicates (by
# migrating a packet into shared/Packets/ and deleting its
# Cpackets sibling) is always allowed and should be followed
# by re-running this script with --update to lower the baseline.
#
# Trees scanned:
#
#   dkrixserver/src/Core/*(CG|GC|CL|LC|GS|SG|GT|TG)*.{h,cpp}    (non-Handler)
#   dkrix/Client/Packet/Cpackets/*.{h,cpp}
#
# Ratchet target: once baseline reaches 0, this gate becomes the
# "no packet class exists in both trees" CI structural check
# originally scoped as Phase 12.3. Same guarantee, same
# mechanism.
#
# Patterns counted: intersection of basenames between the two
# trees. "CGAbsorbSoul.h" present in both = 1 hit. "CGAbsorbSoul.h"
# present only in one = 0 hits.
#
# Usage:
#   dkrixserver/scripts/check-packet-duplicates.sh            # CI mode
#   dkrixserver/scripts/check-packet-duplicates.sh --count    # print count
#   dkrixserver/scripts/check-packet-duplicates.sh --list     # print names
#   dkrixserver/scripts/check-packet-duplicates.sh --update   # rewrite baseline
#
# Exit status:
#   0 — count <= baseline (OK, or --update / --count / --list mode)
#   1 — count > baseline (new duplicate(s) added; either migrate
#       the duplicate out or explicitly update the baseline)
#   2 — usage error / baseline file missing / tree not found
#--------------------------------------------------------------------------------

set -eu

# Resolve script dir so --update rewrites the sibling baseline file.
script_dir=$(cd -- "$(dirname -- "$0")" >/dev/null 2>&1 && pwd)
baseline_file="$script_dir/.packet-duplicates-baseline"

# Repo layout: script lives at dkrixserver/scripts/.
# Tree A = dkrixserver/src/Core/
# Tree B = dkrix/Client/Packet/Cpackets/
# Both resolved relative to $script_dir/../..
repo_root="$script_dir/../.."
server_tree="$repo_root/dkrixserver/src/Core"
client_tree="$repo_root/dkrix/Client/Packet/Cpackets"

# Name pattern: starts with one of the packet prefixes, doesn't
# end in Handler.cpp/Handler.h, ends in .cpp or .h.
name_pattern='^(CG|GC|CL|LC|GS|SG|GT|TG)[A-Za-z0-9_]*\.(cpp|h)$'

list_server_packets() {
    # Extended regex grep against basename list. Exclude *Handler.cpp.
    ls -1 "$server_tree" 2>/dev/null \
        | grep -E "$name_pattern" \
        | grep -v -E 'Handler\.(cpp|h)$' \
        | sort -u
}

list_client_packets() {
    ls -1 "$client_tree" 2>/dev/null \
        | grep -E "$name_pattern" \
        | sort -u
}

count_hits() {
    # Use temp files instead of process substitution so this runs
    # under plain POSIX sh (dash) — matches check-sql-injection.sh.
    tmp_a=$(mktemp) || return 2
    tmp_b=$(mktemp) || { rm -f "$tmp_a"; return 2; }
    list_server_packets > "$tmp_a"
    list_client_packets > "$tmp_b"
    comm -12 "$tmp_a" "$tmp_b" | wc -l | tr -d ' '
    rm -f "$tmp_a" "$tmp_b"
}

list_hits() {
    tmp_a=$(mktemp) || return 2
    tmp_b=$(mktemp) || { rm -f "$tmp_a"; return 2; }
    list_server_packets > "$tmp_a"
    list_client_packets > "$tmp_b"
    comm -12 "$tmp_a" "$tmp_b"
    rm -f "$tmp_a" "$tmp_b"
}

# Sanity: both trees must exist before the gate is meaningful.
check_trees() {
    if [ ! -d "$server_tree" ]; then
        printf 'server tree not found: %s\n' "$server_tree" >&2
        exit 2
    fi
    if [ ! -d "$client_tree" ]; then
        # If Cpackets itself is gone, all duplicates are gone.
        # Emit count=0 rather than failing — the ratchet should
        # celebrate this outcome.
        return 1
    fi
    return 0
}

mode=${1:-check}

case "$mode" in
    --count|-c)
        if check_trees; then
            count_hits
        else
            printf '0\n'
        fi
        exit 0
        ;;
    --list|-l)
        if check_trees; then
            list_hits
        fi
        exit 0
        ;;
    --update|-u)
        if check_trees; then
            count=$(count_hits)
        else
            count=0
        fi
        printf '%s\n' "$count" > "$baseline_file"
        printf 'baseline updated: %s duplicates\n' "$count"
        exit 0
        ;;
    check|'')
        if [ ! -f "$baseline_file" ]; then
            printf 'baseline file missing: %s\n' "$baseline_file" >&2
            printf 'run: %s --update\n' "$0" >&2
            exit 2
        fi
        baseline=$(cat "$baseline_file")
        if check_trees; then
            count=$(count_hits)
        else
            count=0
        fi
        if [ "$count" -gt "$baseline" ]; then
            printf 'FAIL: packet-duplicate count went UP: %s (baseline %s)\n' \
                "$count" "$baseline" >&2
            printf '\nNew duplicates (run --list to see all):\n' >&2
            list_hits | head -20 >&2
            printf '\nOptions:\n' >&2
            printf '  - Migrate the duplicate into shared/Packets/\n' >&2
            printf '    (delete the Cpackets sibling in the same commit).\n' >&2
            printf '  - If intentional, run: %s --update\n' "$0" >&2
            exit 1
        fi
        if [ "$count" -lt "$baseline" ]; then
            printf 'OK: %s duplicates (baseline %s — %s migrated; consider --update).\n' \
                "$count" "$baseline" "$((baseline - count))"
        else
            printf 'OK: %s duplicates (matches baseline).\n' "$count"
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
