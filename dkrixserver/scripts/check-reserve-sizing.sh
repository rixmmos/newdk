#!/bin/sh
#
# check-reserve-sizing.sh — `reserve()`-for-`resize()` ratchet gate.
#
#--------------------------------------------------------------------------------
# WHY THIS EXISTS
#--------------------------------------------------------------------------------
#
# Open action 17 of docs/SECURITY-AUDIT-2026-08-10.md. The tree kept
# growing the same defect:
#
#     m_Things.reserve(N);          // capacity N, size() still 0
#     for (int i = 0; i < N; i++)
#         m_Things[i] = NULL;       // every write is out of bounds
#
# `reserve()` changes capacity, not size, so every indexed write is
# undefined behaviour and every `begin()`…`end()` walk is empty. It
# usually *looks* like it works, because the storage is allocated. The
# damage shows up second-hand: `clear()` frees nothing and the container
# leaks every object it ever held (`EventQuestAdvance` leaked every
# advance ever allocated), and because `reserve()` does not
# value-initialise, a NULL check on a "reserved" slot tests indeterminate
# memory and passes (`PetTypeInfo::getPetTypeInfo(0)`, `OptionInfoManager`).
#
# Ten call sites of it had to be found by hand across five waves — 18-J,
# 18-P (4 sites), 18-Q, 18-AL (2), 18-AR (2), plus `RareOptionUpgradeInfo`
# — and five of the six instances the audit names were found *after* the
# pattern had already been written down. This gate is the mechanical
# version of that reading.
#
# Verified against the defects it exists for: run over the tree as it
# stood at `3e02f6c^`, before the first of those fixes, it reports 17
# sites and every single historical instance is among them — all four
# `ItemGradeManager` sites, both `RareOptionUpgradeInfo` sites, both
# `DragonEyeManager` sites, `OptionInfoManager`, `SkillPropertyManager`,
# `PetAttrInfo`, `PetExpInfo`, `PetTypeInfo`, `EventQuestAdvance` and
# `ZoneGroupManager`. No false negatives, and no false positives: the
# other two sites it reported were `PacketValidator` and `UserGateway`,
# the same defect, both still in the tree at that point. [measured
# 2026-08-13]
#
# The counting is done by scripts/reserve_sizing_sites.py, which lexes
# each file rather than grepping it — comments are stripped, which is
# load-bearing here because `LuckInfoManager`'s entire body, its two
# `reserve()` calls included, sits inside one `/* */` block. See that
# script's module docstring for the rule and its blind spots.
#
#--------------------------------------------------------------------------------
# WHAT IT DOES NOT FLAG
#--------------------------------------------------------------------------------
#
# `reserve()` followed by `push_back()` is the correct idiom and is never
# a site. `GCPetStashList`, `PlayerCreature::m_PetStash`, `Trigger`'s
# condition and action vectors, and `PreparedStatement`'s row buffer all
# do exactly that; so do the five `string::reserve` sites, which size
# through `assign`, `append` or `+=`. Fourteen of the tree's sixteen live
# `reserve()` calls are correct code and the gate is silent on all
# fourteen [measured 2026-08-13].
#
# It also cannot see across file groups, cannot tell a reachable site
# from an unreachable one, and excludes map/set-family containers, where
# `reserve()` + `operator[]` is correct. Details in the Python module.
#
#--------------------------------------------------------------------------------
# THE BASELINE IS A RATCHET
#--------------------------------------------------------------------------------
#
# `.reserve-sizing-baseline` records today's count. CI fails if the count
# goes above it. Baseline 1 [measured 2026-08-13, lowered from 2]:
#
#   Core/PacketValidator.cpp:18   m_PacketIDSets   — the shape is real but
#       `PLAYER_STATUS_MAX` is 0 in all three server binaries (the enum
#       body is behind `#if defined(__GAME_CLIENT__)`), so the loop never
#       runs. Known and deliberately unfixed: §5 of the audit.
#
# It is latent rather than live, which is the only reason this gate could
# start green instead of demanding a fix first.
#
# The gate opened at baseline 2. The second site — `UserGateway.cpp:32`,
# instance 7 of the pattern and the first one this gate found rather than
# a human — was **deleted** rather than fixed, because the file could not
# have compiled in any build: its header declared six members with no
# exception specification while the `.cpp` defined all six `throw(Error)`,
# which is ill-formed in C++11. That is a stronger death certificate than
# "absent from every CMake source list", which was also true. So the
# ratchet's first catch closed by deletion, not by repair.
#
# Lower the baseline. Never raise it. A new site is not a baseline update,
# it is a `resize()`/`assign()`.
#
# Usage:
#   dkrixserver/scripts/check-reserve-sizing.sh            # CI mode
#   dkrixserver/scripts/check-reserve-sizing.sh --count    # print count
#   dkrixserver/scripts/check-reserve-sizing.sh --list     # print sites
#   dkrixserver/scripts/check-reserve-sizing.sh --update   # rewrite baseline
#
# Exit status:
#   0 — count <= baseline (OK, or --update / --count / --list mode)
#   1 — count > baseline (a new site; size the container, do not update)
#   2 — usage error / baseline file missing / counter missing / no python3
#--------------------------------------------------------------------------------

set -eu

# Resolve script dir so --update rewrites the sibling baseline file.
script_dir=$(cd -- "$(dirname -- "$0")" >/dev/null 2>&1 && pwd)
baseline_file="$script_dir/.reserve-sizing-baseline"
counter="$script_dir/reserve_sizing_sites.py"

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

count_sites() {
    "$PY" "$counter" "$src_dir"
}

list_sites() {
    "$PY" "$counter" "$src_dir" --list
}

mode=${1:-check}

case "$mode" in
    --count|-c)
        count_sites
        exit 0
        ;;
    --list|-l)
        list_sites
        exit 0
        ;;
    --update|-u)
        count=$(count_sites)
        printf '%s\n' "$count" > "$baseline_file"
        printf 'baseline updated: %s site(s)\n' "$count"
        exit 0
        ;;
    check|'')
        if [ ! -f "$baseline_file" ]; then
            printf 'baseline file missing: %s\n' "$baseline_file" >&2
            printf 'run: %s --update\n' "$0" >&2
            exit 2
        fi
        # The \r strip is not cosmetic: this repo is authored on Windows, and a
        # working-tree copy of the baseline can carry CRLF, which turns
        # `[ "2<CR>" -gt "2" ]` into an illegal-number error rather than a
        # comparison. Same guard as check-sql-injection.sh.
        baseline=$(tr -d '\r' < "$baseline_file")
        count=$(count_sites)

        if [ "$count" -gt "$baseline" ]; then
            printf 'FAIL: reserve()-for-resize() site count went UP: %s (baseline %s)\n' \
                "$count" "$baseline" >&2
            printf '\nSites:\n' >&2
            list_sites >&2
            printf '\nreserve() sets capacity, not size. Writing through operator[],\n' >&2
            printf 'at(), front() or an iterator walk afterwards is undefined\n' >&2
            printf 'behaviour on a vector whose size() is 0, and clear() will free\n' >&2
            printf 'nothing because begin()..end() is an empty range.\n' >&2
            printf '\nOptions:\n' >&2
            printf '  - Use assign(N, value) or resize(N, value). Both size AND\n' >&2
            printf '    value-initialise, which is what the NULL checks at the call\n' >&2
            printf '    sites already assume.\n' >&2
            printf '  - If the container is genuinely filled with push_back(), keep\n' >&2
            printf '    the reserve() -- that is the correct idiom and this gate does\n' >&2
            printf '    not flag it.\n' >&2
            printf '  - Bound the accessors on size(), never on capacity().\n' >&2
            printf '  - Do NOT --update to make this pass. A new site is a bug.\n' >&2
            exit 1
        fi
        if [ "$count" -lt "$baseline" ]; then
            printf 'OK: %s reserve()-for-resize() site(s) (baseline %s — %s fixed; run --update to ratchet down).\n' \
                "$count" "$baseline" "$((baseline - count))"
        else
            printf 'OK: %s reserve()-for-resize() site(s) (matches baseline).\n' "$count"
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
