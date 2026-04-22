#!/bin/sh
#
# check-socket-raw-casts.sh — Phase 13.4 raw m_Buffer-cast gate.
#
# Counts raw cast patterns of the shape:
#   *(T*)(m_Buffer + ...)
# which Phase 13.3 removes from SocketInputStream/SocketOutputStream.
# Once the baseline is pinned to 0, CI fails if the pattern reappears.
#
# Usage:
#   dkrixserver/scripts/check-socket-raw-casts.sh
#   dkrixserver/scripts/check-socket-raw-casts.sh --count
#   dkrixserver/scripts/check-socket-raw-casts.sh --list
#   dkrixserver/scripts/check-socket-raw-casts.sh --update

set -eu

script_dir=$(cd -- "$(dirname -- "$0")" >/dev/null 2>&1 && pwd)
baseline_file="$script_dir/.socket-raw-casts-baseline"
repo_root="$script_dir/../.."

pattern='\*\s*\(\s*[A-Za-z_][A-Za-z0-9_]*\s*\*\s*\)\s*\(\s*m_Buffer\s*\+'
scan_roots="
$repo_root/shared/Core
$repo_root/dkrixserver/src/Core
$repo_root/dkrix/Client/Packet
"

list_hits() {
    found=0
    for root in $scan_roots; do
        [ -d "$root" ] || continue
        matches=$(grep -R -n -E "$pattern" "$root" --include='*.h' --include='*.cpp' || true)
        if [ -n "$matches" ]; then
            found=1
            printf '%s\n' "$matches"
        fi
    done
    return 0
}

count_hits() {
    list_hits | wc -l | tr -d ' '
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
        printf 'baseline updated: %s raw-cast hit(s)\n' "$count"
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
            printf 'FAIL: socket raw-cast count went UP: %s (baseline %s)\n' "$count" "$baseline" >&2
            printf '\nMatches:\n' >&2
            list_hits >&2
            exit 1
        fi
        if [ "$count" -lt "$baseline" ]; then
            printf 'OK: %s raw-cast hit(s) (baseline %s — %s removed; consider --update).\n' \
                "$count" "$baseline" "$((baseline - count))"
        else
            printf 'OK: %s raw-cast hit(s) (matches baseline).\n' "$count"
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
