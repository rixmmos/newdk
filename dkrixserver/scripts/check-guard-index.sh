#!/bin/sh
#
# check-guard-index.sh — "guard that indexes a container to test it" ratchet.
#
#--------------------------------------------------------------------------------
# WHY THIS EXISTS
#--------------------------------------------------------------------------------
#
# Section 4 pattern 2 of docs/SECURITY-AUDIT-2026-08-10.md. The tree keeps
# growing checks that are themselves the access they were meant to
# prevent:
#
#     Assert(m_pItemInfos[itemType] != NULL);   // this IS the OOB read
#     return m_pItemInfos[itemType];
#
# The subscript happens before anything has established that the subscript
# is in range, so an out-of-range index faults inside the guard rather
# than being caught by it — and under NDEBUG the guard is `((void)0)` and
# the caller reads the slot anyway. The `if` form is the same defect:
#
#     if (m_Advances[questLevel] != NULL)       // same read, same fault
#         throw DuplicatedException(...);
#
# The audit records this recurring across 18-K, 18-U, 18-AD, 18-AG,
# 18-AH, 18-AL and 18-AI — and then it recurred again: 18-AW (`b2e7d48`,
# 2026-08-13) found `Assert(m_pItemInfos[0] != NULL)` in
# `InfoClassManager::init()`, one line below a defect the audit's own §1
# row 12 had described without noticing it. That is the argument for a
# machine reading instead of a human one.
#
# Counting is done by scripts/guard_index_sites.py, which lexes each file
# rather than grepping it. Two things there are load-bearing and are not
# available to a grep. Comments are stripped, because 18-AL's
# `InfoClassManager::getItemInfo` had its only real bounds check *inside a
# comment*, which a grep would score as a guard. And `__BEGIN_TRY` /
# `__END_CATCH` / `__ENTER_CRITICAL_SECTION` are normalised to the braces
# they expand to, because otherwise brace matching is off by one from the
# first one in the file onwards and every enclosing-loop test silently
# returns the wrong answer.
#
#--------------------------------------------------------------------------------
# WHAT COUNTS, AND WHERE "PROVABLY BOUNDED" IS DRAWN
#--------------------------------------------------------------------------------
#
# A site is an `Assert(...)`, or an `if (...)` whose whole body is one
# `return` or `throw`, whose condition tests an indexed element for
# *existence* — `X[i] == NULL`, `X[i] != NULL`, `!X[i]`, and the `.at(i)`
# forms. Existence is the question you cannot ask without already being in
# bounds. A guard that range-checks the element's *value*
# (`if (m_STR[attrType] > maxSlayerAttr) throw`) is not counted; there are
# nine of those in the `PC*Info2.h` headers and none of them is a bounds
# check.
#
# No lexical tool can decide whether an index is in range — that needs the
# container's runtime length, which here is almost always a DB row count.
# So the checker does not try. It asks the decidable question instead:
# *did anyone write a bound for this index that survives NDEBUG, anywhere
# the guard can see?* An access is dropped when an enclosing `for`/`while`/
# `if` header compares one of its identifiers, when an earlier real `if` in
# the same function tests one and transfers control (`return`/`throw`/
# `continue`/`break`) — the chokepoint idiom 18-U introduced — or when the
# same condition bounds it to the left (`Assert(i < n && a[i] != NULL)`).
# Map/set containers are dropped too, since `operator[]` inserts there, as
# are compile-time-constant indices into containers declared with an
# explicit array extent.
#
# Two honest limits. A written bound is accepted **without being checked
# for correctness** — 18-AH's `j <= 150` over arrays holding 101 was a
# bound, and wrong; wrong constants are audit pattern 3 and need the DB row
# counts, not a lexer. And another `Assert` is **never** a bound, which is
# deliberate: `Assert(verifyIndex(i)); Assert(m_ppItem[i] == NULL);` leaves
# the second one a site, because under NDEBUG both vanish together.
#
#--------------------------------------------------------------------------------
# VALIDATED AGAINST THE DEFECTS IT EXISTS FOR
#--------------------------------------------------------------------------------
#
# Run over pre-fix trees extracted with `git archive`, the count falls
# exactly as each wave landed, and every drop is a named instance
# [measured 2026-08-13]:
#
#   3e02f6c^  43   the oldest tree checked
#   326c298^  42   -1  Slayer.h addWearItem            (18-I / 18-R)
#   053b69f^  41   -1  ItemRack::insert                (18-U)
#   8a26163^  41    0
#   00a5372^  41    0
#   1177bd0^  34   -7  SkillDomainInfoManager x3,
#                      VampEXPInfo x2, OustersEXPInfo x2   (18-AH)
#   1f1ded5^  34    0
#   b2e7d48^  27   -7  InfoClassManager x2, EventQuestAdvance x2 (18-AL),
#                      FameLimitInfo x3 (deleted, a8503bf)
#   HEAD      25   -2  InfoClassManager::init          (18-AW),
#                      QuestFactoryManager (deleted, 229340d)
#
# Zero false negatives against every instance of *this* shape, and zero
# false positives: every one of the 18 drops is a fix or a deletion, none
# is the checker changing its mind.
#
# 18-K (`3e02f6c`), 18-AD (`053b69f`), 18-AG (`8a26163`) and 18-AI
# (`1177bd0`) show no movement, and that is correct rather than a miss:
# those are the *other* sub-shapes the audit files under pattern 2 —
# side-effecting calls inside `Assert`, and `Assert(index < MAX)`
# comparisons. Nothing in 18-U's `Zone::getTile`, `NPC` shop-rack,
# `Datagram` or Slayer-phone conversions indexed inside its own guard
# either; only `ItemRack::insert` did, and that one is caught. This gate
# covers one sub-shape of pattern 2, not the pattern.
#
#--------------------------------------------------------------------------------
# THE BASELINE IS A RATCHET
#--------------------------------------------------------------------------------
#
# `.guard-index-baseline` records today's count. CI fails if the count
# goes above it. Baseline 25 [measured 2026-08-13], and unlike the
# reserve-sizing gate it does NOT start near zero — §1 row 10 records that
# 18-U's conversions were deliberately scoped to wire-reachable checks and
# that the server-side-index Asserts in `ItemInfoManager` and friends were
# kept on purpose. Those are in the baseline by decision, not by neglect:
#
#   7  m_pPlayers[fd]      IncomingPlayerManager x4, ZonePlayerManager x2,
#                          LoginPlayerManager — `fd` from accept(); the
#                          fd_set family, hardened at the select() end by
#                          18-AK
#   8  load-time DB keys   AttrBalanceInfo x3, RankBonusInfo, RankEXPInfo,
#                          SkillInfo, SkillParentInfo::addSkillParentInfo,
#                          DarkLightInfo — duplicate checks that index on a
#                          DB-sourced key. `SkillParentInfo` is the residue
#                          of 18-AW, which bounded `load()` and not this
#                          twin
#   5  ItemInfoManager     the server-side-index Asserts 18-U names as
#                          deliberately not converted
#   2  LCPCList.h          getPCInfo/setPCInfo, fixed `[SLOT_MAX]` array,
#                          `Slot`-typed index
#   2  addWearItem         Ousters.h, Vampire.h — real shape, currently
#                          unreachable: the function has zero callers in
#                          the tree. Slayer's twin was fixed by 18-R;
#                          these two were not
#   1  TimeChecker         bounded on the line above — by another Assert,
#                          which is exactly what does not survive NDEBUG
#
# Lower the baseline. Never raise it. A new site is not a baseline update,
# it is a real `if` placed before the guard.
#
# Usage:
#   dkrixserver/scripts/check-guard-index.sh            # CI mode
#   dkrixserver/scripts/check-guard-index.sh --count    # print count
#   dkrixserver/scripts/check-guard-index.sh --list     # print sites
#   dkrixserver/scripts/check-guard-index.sh --update   # rewrite baseline
#
# Exit status:
#   0 — count <= baseline (OK, or --update / --count / --list mode)
#   1 — count > baseline (a new site; bound the index, do not update)
#   2 — usage error / baseline file missing / counter missing / no python3
#--------------------------------------------------------------------------------

set -eu

# Resolve script dir so --update rewrites the sibling baseline file.
script_dir=$(cd -- "$(dirname -- "$0")" >/dev/null 2>&1 && pwd)
baseline_file="$script_dir/.guard-index-baseline"
counter="$script_dir/guard_index_sites.py"

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
        # `[ "25<CR>" -gt "25" ]` into an illegal-number error rather than a
        # comparison. Same guard as check-sql-injection.sh.
        baseline=$(tr -d '\r' < "$baseline_file")
        count=$(count_sites)

        if [ "$count" -gt "$baseline" ]; then
            printf 'FAIL: guard-indexes-container site count went UP: %s (baseline %s)\n' \
                "$count" "$baseline" >&2
            printf '\nSites:\n' >&2
            list_sites >&2
            printf '\nA guard that subscripts a container in order to test it IS the\n' >&2
            printf 'out-of-bounds access it looks like it prevents. It faults instead\n' >&2
            printf 'of catching, and under NDEBUG an Assert form does not even fault --\n' >&2
            printf 'it disappears and the caller reads the slot regardless.\n' >&2
            printf '\nOptions:\n' >&2
            printf '  - Put a real `if` on the INDEX before the guard, and make it\n' >&2
            printf '    return or throw. Bound it on the container length that was\n' >&2
            printf '    actually loaded, never on a constant from another table.\n' >&2
            printf '  - Fix it at the accessor, not at each call site; leave the\n' >&2
            printf '    Assert in place after the if, so Debug and Release agree.\n' >&2
            printf '  - Another Assert is not a bound. Both vanish under NDEBUG.\n' >&2
            printf '  - Do NOT --update to make this pass. A new site is a bug.\n' >&2
            exit 1
        fi
        if [ "$count" -lt "$baseline" ]; then
            printf 'OK: %s guard-indexes-container site(s) (baseline %s — %s fixed; run --update to ratchet down).\n' \
                "$count" "$baseline" "$((baseline - count))"
        else
            printf 'OK: %s guard-indexes-container site(s) (matches baseline).\n' "$count"
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
