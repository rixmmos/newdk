#!/bin/sh
#
# audit-packet-divergence.sh — Phase 17 divergence classifier.
#
# Companion to check-packet-duplicates.sh. Where that script COUNTS
# name-matched packet files between server and client trees for the
# CI ratchet, this one CLASSIFIES each matched CLASS (a .cpp + .h
# pair — the actual migration unit) into one of:
#
#   identical  — both .cpp and .h are byte-equal across trees (git
#                mv into shared/Packets/ is trivially safe).
#   cosmetic   — both .cpp and .h differ only in whitespace, C++
#                throw() exception specifications, the client-only
#                `#include "Client_PCH.h"`, and the
#                `#ifndef __GAME_CLIENT__` / `#ifdef __DEBUG_OUTPUT__`
#                conditional-compilation guards. Safe-ish migration
#                candidates: pick server version, git mv, delete
#                Cpackets sibling, verify both builds.
#   semantic   — at least one of .cpp or .h still differs after
#                that normalization. Examples:
#                __END_CATCH vs __END_CATCH_NO_RETHROW (different
#                error semantics), method-body inlining in headers,
#                toString() debug-string drift,
#                algebraically-equivalent-but-textually-different
#                getPacketSize() expressions, string vs std::string,
#                include-order / include-set differences. Each of
#                these needs a reconciliation step before migration.
#
# Trees scanned (same as check-packet-duplicates.sh):
#
#   dkrixserver/src/Core/*(CG|GC|CL|LC|GS|SG|GT|TG)*.{h,cpp}    (non-Handler)
#   dkrix/Client/Packet/Cpackets/*.{h,cpp}
#
# Pair-level classification is what matters for migration because
# a class's .cpp and .h always move together. A "pair candidate"
# is a class name where all four files exist (server.cpp + server.h
# + client.cpp + client.h). The pair's class = worst of the two
# file-level classes.
#
# This script is deliberately NOT wired into the ratchets.yml CI
# workflow. It's an investigation tool: the output is meant for
# a human picking the next migration PR, not for a gate that fails
# a build.
#
# Usage:
#   dkrixserver/scripts/audit-packet-divergence.sh                 # summary
#   dkrixserver/scripts/audit-packet-divergence.sh --count         # 4 numbers
#   dkrixserver/scripts/audit-packet-divergence.sh --list-identical
#   dkrixserver/scripts/audit-packet-divergence.sh --list-cosmetic
#   dkrixserver/scripts/audit-packet-divergence.sh --list-semantic
#   dkrixserver/scripts/audit-packet-divergence.sh --by-file       # per-file mode
#
# Exit status:
#   0 — ran successfully (classification completed)
#   2 — usage error / tree not found
#--------------------------------------------------------------------------------

set -eu

script_dir=$(cd -- "$(dirname -- "$0")" >/dev/null 2>&1 && pwd)
repo_root="$script_dir/../.."
server_tree="$repo_root/dkrixserver/src/Core"
client_tree="$repo_root/dkrix/Client/Packet/Cpackets"

name_pattern='^(CG|GC|CL|LC|GS|SG|GT|TG)[A-Za-z0-9_]*\.(cpp|h)$'

# List of all packet classes (basenames without .cpp/.h suffix) that
# have all four files present: server.cpp, server.h, client.cpp,
# client.h. These are the only classes for which a migration PR is
# even meaningful — a class missing a sibling .h can't be migrated
# as a pair.
list_complete_pairs() {
    tmp=$(mktemp) || return 2
    for tree in "$server_tree" "$client_tree"; do
        ls -1 "$tree" 2>/dev/null \
            | grep -E "$name_pattern" \
            | grep -v -E 'Handler\.(cpp|h)$' \
            | sed -E 's/\.(cpp|h)$//'
    done | sort -u > "$tmp"
    while IFS= read -r cls; do
        [ -z "$cls" ] && continue
        [ -f "$server_tree/$cls.cpp" ] || continue
        [ -f "$server_tree/$cls.h" ]   || continue
        [ -f "$client_tree/$cls.cpp" ] || continue
        [ -f "$client_tree/$cls.h" ]   || continue
        echo "$cls"
    done < "$tmp"
    rm -f "$tmp"
}

# Stricter normalizer than plain whitespace-collapse: removes ALL
# inter-token whitespace, strips throw(...) exception specs, the
# client-only PCH include, and the two conditional-compilation
# guards whose placement typically differs between the two copies.
normalize() {
    sed -E \
        -e 's/\bthrow *\([^)]*\)//g' \
        -e '/^#include "Client_PCH.h"$/d' \
        -e '/^#ifndef __GAME_CLIENT__$/d' \
        -e '/^#ifdef __DEBUG_OUTPUT__$/d' \
        -e '/^#endif$/d' \
        -e 's/[[:space:]]+//g' \
        -e '/^$/d' \
        "$1"
}

# Classify one file pair. Echoes identical / cosmetic / semantic.
classify_file() {
    s=$1; c=$2
    if diff -q "$s" "$c" >/dev/null 2>&1; then
        echo identical; return 0
    fi
    ns=$(mktemp) || return 2
    nc=$(mktemp) || { rm -f "$ns"; return 2; }
    normalize "$s" > "$ns"
    normalize "$c" > "$nc"
    if diff -q "$ns" "$nc" >/dev/null 2>&1; then
        echo cosmetic
    else
        echo semantic
    fi
    rm -f "$ns" "$nc"
}

# Ranking helper: picks the worst of two classifications.
# identical < cosmetic < semantic.
rank_of() {
    case "$1" in
        identical) echo 0 ;;
        cosmetic)  echo 1 ;;
        semantic)  echo 2 ;;
    esac
}
worst_of() {
    r1=$(rank_of "$1"); r2=$(rank_of "$2")
    if [ "$r1" -ge "$r2" ]; then echo "$1"; else echo "$2"; fi
}

# Classify one class-pair. Echoes "<class> <name>".
classify_pair() {
    cls=$1
    cpp_class=$(classify_file "$server_tree/$cls.cpp" "$client_tree/$cls.cpp")
    h_class=$(classify_file "$server_tree/$cls.h"   "$client_tree/$cls.h")
    pair_class=$(worst_of "$cpp_class" "$h_class")
    printf '%s %s\n' "$pair_class" "$cls"
}

classify_all_pairs() {
    list_complete_pairs | while IFS= read -r cls; do
        [ -z "$cls" ] && continue
        classify_pair "$cls"
    done
}

# Also support a by-file mode for deeper inspection of which side
# (.cpp or .h) is driving a given pair's semantic classification.
classify_all_files() {
    list_complete_pairs | while IFS= read -r cls; do
        [ -z "$cls" ] && continue
        cpp_class=$(classify_file "$server_tree/$cls.cpp" "$client_tree/$cls.cpp")
        h_class=$(classify_file "$server_tree/$cls.h"   "$client_tree/$cls.h")
        printf '%s %s.cpp\n' "$cpp_class" "$cls"
        printf '%s %s.h\n' "$h_class" "$cls"
    done
}

check_trees() {
    if [ ! -d "$server_tree" ]; then
        printf 'server tree not found: %s\n' "$server_tree" >&2
        exit 2
    fi
    if [ ! -d "$client_tree" ]; then
        # Cpackets gone — no duplicates to classify.
        return 1
    fi
    return 0
}

mode=${1:-summary}

case "$mode" in
    --count|-c)
        if ! check_trees; then
            printf 'pairs:     0\n'
            printf 'identical: 0\n'
            printf 'cosmetic:  0\n'
            printf 'semantic:  0\n'
            exit 0
        fi
        results=$(classify_all_pairs)
        total=$(printf '%s\n' "$results" | grep -c . || true)
        ident=$(printf '%s\n' "$results" | grep -c '^identical ' || true)
        cosm=$(printf '%s\n' "$results" | grep -c '^cosmetic ' || true)
        sem=$(printf '%s\n' "$results" | grep -c '^semantic ' || true)
        printf 'pairs:     %s\n' "$total"
        printf 'identical: %s\n' "$ident"
        printf 'cosmetic:  %s\n' "$cosm"
        printf 'semantic:  %s\n' "$sem"
        exit 0
        ;;
    --list-identical)
        check_trees || exit 0
        classify_all_pairs | awk '$1=="identical"{print $2}'
        exit 0
        ;;
    --list-cosmetic)
        check_trees || exit 0
        classify_all_pairs | awk '$1=="cosmetic"{print $2}'
        exit 0
        ;;
    --list-semantic)
        check_trees || exit 0
        classify_all_pairs | awk '$1=="semantic"{print $2}'
        exit 0
        ;;
    --by-file)
        check_trees || exit 0
        results=$(classify_all_files)
        total=$(printf '%s\n' "$results" | grep -c . || true)
        ident=$(printf '%s\n' "$results" | grep -c '^identical ' || true)
        cosm=$(printf '%s\n' "$results" | grep -c '^cosmetic ' || true)
        sem=$(printf '%s\n' "$results" | grep -c '^semantic ' || true)
        printf 'files:     %s\n' "$total"
        printf 'identical: %s\n' "$ident"
        printf 'cosmetic:  %s\n' "$cosm"
        printf 'semantic:  %s\n' "$sem"
        printf '\n'
        printf 'cosmetic files:\n'
        printf '%s\n' "$results" | awk '$1=="cosmetic"{print "  " $2}'
        exit 0
        ;;
    summary|'')
        if ! check_trees; then
            printf 'No duplicates to classify (Cpackets tree not present).\n'
            exit 0
        fi
        results=$(classify_all_pairs)
        total=$(printf '%s\n' "$results" | grep -c . || true)
        ident=$(printf '%s\n' "$results" | grep -c '^identical ' || true)
        cosm=$(printf '%s\n' "$results" | grep -c '^cosmetic ' || true)
        sem=$(printf '%s\n' "$results" | grep -c '^semantic ' || true)
        printf '=== Packet divergence audit (pair-level) ===\n'
        printf 'Server tree: %s\n' "$server_tree"
        printf 'Client tree: %s\n' "$client_tree"
        printf '\n'
        printf 'Total complete pairs (server.cpp+.h AND client.cpp+.h): %s\n' "$total"
        printf '  identical (both .cpp AND .h byte-equal):       %s\n' "$ident"
        printf '  cosmetic  (both files at most whitespace/etc): %s\n' "$cosm"
        printf '  semantic  (at least one file truly differs):   %s\n' "$sem"
        printf '\n'
        if [ "$ident" -gt 0 ] || [ "$cosm" -gt 0 ]; then
            printf 'First-wave migration candidates (identical + cosmetic pairs):\n'
            printf '%s\n' "$results" \
                | awk '$1=="identical" || $1=="cosmetic" {print "  " $2}'
            printf '\n'
        else
            printf 'No first-wave candidates: every class has at least one\n'
            printf 'semantic-diff file in either its .cpp or its .h. Every\n'
            printf 'migration PR needs a per-file reconciliation step before\n'
            printf 'the git mv can land.\n'
            printf '\n'
        fi
        printf 'For --by-file breakdown (which side drives each pair):\n'
        printf '  %s --by-file\n' "$0"
        printf 'For the full list of semantic pairs:\n'
        printf '  %s --list-semantic\n' "$0"
        printf 'For machine-readable counts:\n'
        printf '  %s --count\n' "$0"
        exit 0
        ;;
    --help|-h)
        sed -n '2,/^#-\{10,\}/p' "$0" | sed 's/^# //; s/^#$//'
        exit 0
        ;;
    *)
        printf 'unknown mode: %s\n' "$mode" >&2
        printf 'usage: %s [--count | --list-identical | --list-cosmetic | --list-semantic | --by-file | --help]\n' "$0" >&2
        exit 2
        ;;
esac
