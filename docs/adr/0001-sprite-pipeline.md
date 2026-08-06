# ADR-0001: Sprite pipeline consolidation (Phase 4)

**Status:** Proposed — **but see the branch warning below before acting**
**Date:** 2026-08-06
**Deciders:** Enrico (sole maintainer)
**Supersedes:** the unwritten Phase 4 premise in `docs/MODERNIZATION.md`

> ## ⚠️ There is a known-good implementation of this — use it
>
> The parked tag `archive/modernization-phases-1-17` **already completed
> Phase 4**, and did it correctly. Commit `ebcfe52` does not merely delete the
> 555/565 classes — the failure mode Finding 3 below warns about. It **folds
> their `SaveToFile`/`LoadFromFile` into the base classes** (+115 lines in
> `CAlphaSprite.cpp`, +111 in `CIndexSprite.cpp`, under the comment
> *"folded from CAlphaSprite565 in Phase 4A"*), picks 565 as the single on-disk
> format, and migrates callers in the same commit:
>
> ```diff
> -#include "CSprite565.h"
> +#include "CSprite.h"
> -  pSprite = new CSprite565;
> +  pSprite = new CSprite;
> ```
>
> An end-to-end login→gameplay smoke test passed two days later, so the approach
> is validated by a running game — the asset evidence this ADR said was required.
>
> **If Phase 4 is ever done on this line, port that commit rather than
> re-deriving it:**
> `git show archive/modernization-phases-1-17 ebcfe52`
>
> The analysis below stands as the reasoning for *why* the naive deletion is
> unsafe, which is worth keeping. Findings 1 and 2 (three sprite paths;
> `SpriteLibBackendSDL.cpp` independent of `engine/sprite`) describe this line
> and remain accurate.

---

## Context

`docs/MODERNIZATION.md` frames Phase 4 as *"One sprite pipeline"* and describes the
situation as:

> `Client/SpriteLib/` still contains the 555/565/4444 pixel-format class explosion
> even though SDL handles pixel-format conversion natively. […] A second sprite
> system lives at `tools/engine/sprite/`. We have two sprite systems today.

Direct measurement of the working tree on 2026-08-06 contradicts that framing in
three ways. The commands used are recorded in the appendix so these numbers can be
re-derived rather than inherited.

### Finding 1 — There are three sprite code paths, not two

| Path | Language | LOC | Built by | Used by |
|---|---|---|---|---|
| `Client/SpriteLib/` C++ classes | C++11 | 53,566 (48,149 in `.cpp`) | always | the game client |
| `SpriteLibBackend.h` + `SpriteLibBackendSDL.cpp` | C ABI over SDL2 | 1,371 | always | the game client, directly |
| `tools/engine/sprite/` | **C**, not C++ | 7,749 (+3,898 tests) | only when `BUILD_ENGINE` | `tools/viewers/` only |

`SpriteLibBackendSDL.cpp` is an **independent SDL2 implementation**. It includes only
`SpriteLibBackendSDL.h`, `stdlib.h`, `string.h`, `stdio.h` — it contains zero
references to `engine/sprite`, `sdl_framework`, or any engine header.

This matters because `Client/SpriteLib/SPRITELIB_BACKEND_README.md` draws an
architecture diagram whose bottom layer reads *"SDL2 Backend (engine/sprite)"* and
marks the whole thing **"Status: ✅ Production Ready — All phases completed
(Phases 1-6), Integration tests passing."** That diagram is wrong and that status is
unverified. Two separate SDL2 rendering implementations exist in this repo and
neither has ever been compiled by CI.

### Finding 2 — Phase 4's stated target is 2.8% of the problem

The "20 pixel-format variant files" are real — exactly 20 files match `555|565|4444`
— but they total **1,500 lines**, against 53,566 in `SpriteLib/`. The actual mass is
concentrated elsewhere:

| File | LOC |
|---|---|
| `CSpriteSurface.cpp` | 13,644 |
| `CSprite.cpp` | 9,593 |
| `CIndexSprite.cpp` | 7,244 |
| `CShadowSprite.cpp` | 3,455 |
| `CAlphaSprite.cpp` | 2,943 |
| `CAlphaSpritePal.cpp` | 2,781 |

Deleting the class explosion in full would remove less code than a single
1,000-line extraction from `CSpriteSurface.cpp`. Phase 4, as scoped, is not the
sprite consolidation it is named after.

### Finding 3 — The variant classes are serializers, not renderers

This is the load-bearing correction. `CSprite565` derives from `CSprite` and
overrides exactly two methods:

```cpp
class CSprite565 : public CSprite {
    public :
        CSprite565() = default;
        virtual ~CSprite565() = default;
        bool SaveToFile(std::ofstream& file);
        bool LoadFromFile(std::ifstream& file);
};
```

`CSprite565::SaveToFile` writes run-length-encoded rows under a `// 5:6:5` comment.
The 555/565 distinction describes **the on-disk encoding of SPK asset data**, not a
runtime rendering path.

The premise "SDL handles pixel-format conversion natively, therefore the class
explosion is dead weight" is therefore false. SDL converts formats *in memory*. It
does not read a 5:5:5-encoded sprite off disk. As long as the shipped assets in
`Darkeden/` are encoded in these formats, this code is load-bearing for asset I/O and
cannot be deleted — only relocated behind a format-tagged reader.

### Constraints

- **No CI has ever run** on this repo (Phase -1 open). Nothing here is compile-verified.
- Rendering changes are on the project's explicit *never delegate unsupervised* list;
  past regressions were resolution-dependent and visible only at FHD.
- Project rule: preserve observable behavior unless explicitly approved.
- Solo maintainer. Review capacity is the scarcest resource; the build is the reviewer.

---

## Decision

**Re-scope Phase 4.** Do not attempt "one sprite pipeline" as a single phase. Split it:

1. **4a — Delete the provably dead.** `CAlphaSpritePackList555` and
   `CAlphaSpritePackList565` have **zero external references**. Four files, no risk.
2. **4b — Collapse the two SDL2 backends into one.** Choose `SpriteLibBackendSDL.cpp`
   (the one the game actually calls) as the survivor. Demote `tools/engine/sprite/`
   to what it already is: the viewer tools' library.
3. **4c — Reframe the 555/565 classes as an asset-format concern**, and defer them
   to a future phase that starts by auditing what encodings the shipped SPK assets
   actually use.
4. **Correct `SPRITELIB_BACKEND_README.md`** — remove the false engine/sprite
   attribution and the unearned "Production Ready" badge, in the same commit.

Nothing in 4b or 4c may land before CI is green.

---

## Options Considered

### Option A — Execute Phase 4 as written: delete the 20 variant files

| Dimension | Assessment |
|---|---|
| Complexity | Medium — 19 external refs each for `CSprite555`/`CSprite565` |
| Payoff | **1,500 LOC (2.8% of SpriteLib)** |
| Risk | **High** — breaks asset loading if any shipped SPK uses these encodings |
| Verifiability | None today |

**Pros:** matches the branch name and the written plan; bounded file list.

**Cons:** rests on a false premise. These classes are the SPK deserializers. The
change is silent-failure-shaped: it compiles, then the client renders garbage or
fails to load assets at runtime — exactly the class of bug CI would *not* catch and
that only shows up in a running client. Worst risk-to-payoff ratio of the four.

### Option B — Adopt `tools/engine/sprite/` as the one true sprite library

| Dimension | Assessment |
|---|---|
| Complexity | **Very high** — port 53,566 LOC of C++ onto a 7,749 LOC C library |
| Payoff | Genuinely one pipeline; inherits the only test suite in the repo (11 test files) |
| Risk | **Very high** — total rewrite of the rendering path |
| Verifiability | None today |

**Pros:** `engine/sprite` is the better-engineered artifact — clean C, header/src
separation, and the **only tested code in either tree**. It is what a greenfield
choice would pick.

**Cons:** it is an 8k-line library (20 headers, 14 `.c` files) that has never rendered
a frame of the actual game; it currently drives two viewer tools. The gap between
"powers a sprite viewer" and "powers MTopView, MCreature, guild marks, and the full
effects pipeline" is the entire 50k lines. Its CMake hookup in `Client/SpriteLib/CMakeLists.txt` is commented
out (`# add_subdirectory`), so the integration was contemplated and abandoned once
already. For a solo maintainer with no CI and no rendering tests, this is a
multi-month rewrite of the highest-risk subsystem. **Rejected.**

### Option C — Make the backend seam the consolidation point *(chosen)*

| Dimension | Assessment |
|---|---|
| Complexity | Low-to-medium, and incremental |
| Payoff | Removes a genuine duplicate implementation; corrects the record |
| Risk | Low for 4a, medium for 4b, deferred for 4c |
| Verifiability | 4a is safe now; 4b/4c gated on CI |

**Pros:** `SpriteLibBackend.h` already *is* the abstraction boundary — `spritectl_*`
is called from `Client.cpp`, `SDLMain.cpp`, and `ClientFunction.cpp`, and sprite
handles are already threaded through `CAlphaSprite`/`CIndexSprite` as
`m_backend_sprite`. The seam exists; it just has two implementations behind it and a
README that misdescribes both. Consolidating there is the smallest change that makes
"one sprite pipeline" true in the sense that matters.

**Cons:** does not reduce the 50k-line C++ mass. Leaves the class explosion standing.
Honest about being a re-scope rather than a completion.

### Option D — Defer Phase 4 entirely until Phase -1 closes

| Dimension | Assessment |
|---|---|
| Complexity | Zero |
| Payoff | Zero code; avoids building on an unverified base |
| Risk | The misleading README and false premise stay in the tree |

**Pros:** strictly correct sequencing — the project's own plan says Phase -1 blocks
everything.

**Cons:** the documentation corrections and the 4a deletions carry no build risk and
need not wait. Deferring them keeps a wrong architecture diagram authoritative.
Partially adopted: everything with build risk *is* deferred.

---

## Trade-off Analysis

The central trade-off is **branch-name fidelity vs. actual leverage**. There is a
branch called `modernize/phase-4-sprite` and a written plan naming 20 files, which
creates pressure to delete those 20 files and call the phase done. Measurement says
that would be the single worst-value change available: 2.8% of the code, on the
riskiest premise, in the subsystem that can least tolerate a silent regression.

The second trade-off is **the better library vs. the load-bearing one**.
`engine/sprite` is better code by every structural measure and has the only tests in
the repo. `SpriteLib` is what actually renders the game. With no CI, no rendering
tests, and one developer, "better" loses to "working" — but `engine/sprite` should be
kept alive and tested rather than deleted, because it is the only place in this
codebase where a test-first habit already exists.

Third: **sequencing**. Options A, B, and the risky half of C all require a build to
judge. Only 4a (four dead files) and the README correction are safely judgeable by
reading. That split — not the phase boundary — is the real line in the sand.

---

## Consequences

**Becomes easier**

- Phase 4 gets a truthful, closeable definition instead of an open-ended one.
- One SDL2 implementation to reason about instead of two that look like one.
- The asset-format question becomes explicit rather than hidden inside a
  "class explosion" framing that implied it was free to delete.

**Becomes harder**

- `MODERNIZATION.md` needs a correction, not just a status tick — the Phase 4
  premise, the "two sprite systems" count, and the SDL/format claim are all wrong.
- Phase 4 will no longer look impressive by LOC removed.

**To revisit**

- Whether `tools/engine/sprite/` earns promotion once CI runs its 11 test files —
  the first automated signal this repo would have.
- Whether the 555/565 encodings appear in shipped assets at all. If they do not, 4c
  becomes a straightforward deletion later.
- `CSpriteSurface.cpp` at 13,644 lines is the real Phase 4-shaped problem and has no
  phase of its own.

---

## Action Items

Safe now (judgeable by reading, no build required):

1. [ ] Delete **8** dead files — `CAlphaSpritePackList{555,565}.{cpp,h}` *and*
       `CSpritePackList{555,565}.{cpp,h}` (312 lines). Follow-up measurement found
       twice as many as first stated: none of the four `.cpp` appear in
       `SPRITELIB_SOURCES`, so they are not compiled, and every external reference
       to `CSpritePackList555/565` is commented out. Also drop lines 103, 104, 118,
       119 from `Client/SpriteLib/CMakeLists.txt` (they populate `SPRITELIB_HEADERS`,
       which is set and never used). See `../TECH-DEBT-AUDIT.md` item 19.
2. [ ] Rewrite `SPRITELIB_BACKEND_README.md`: remove the `engine/sprite` attribution from the diagram, drop "Production Ready" and "Integration tests passing"
3. [ ] Correct the Phase 4 section of `docs/MODERNIZATION.md` with Findings 1–3 and link this ADR
4. [ ] Resolve the branch question — `modernize/phase-4-sprite` is `origin/main`+4 with no upstream, while `origin/modernize/phase4-sprite` holds 106 unrelated commits

Blocked on green CI (Phase -1):

5. [ ] Audit shipped SPK assets in `Darkeden/` for actual pixel encodings — this answers 4c
6. [ ] Fold `tools/engine/sprite/` explicitly under the viewer tools; state in its `CMakeLists.txt` that it is not a client dependency
7. [ ] Wire `engine/sprite`'s 11 test files into CI — cheapest real test signal available
8. [ ] Only then: revisit the 555/565 serializers with asset evidence in hand

---

## Appendix — how these numbers were derived

Run from `dkrix/`:

```bash
# Finding 1: backend independence
grep -n '#include' Client/SpriteLib/SpriteLibBackendSDL.cpp
grep -c "engine/sprite\|sdl_framework\|index_spritepack" \
    Client/SpriteLib/SpriteLibBackendSDL.cpp          # -> 0
grep -rn "engine/sprite" --include=*.txt --include=*.cpp --include=*.h .

# Finding 2: mass distribution
# NOTE: sum per-file with awk. `wc -l | tail -1` is WRONG on large file sets —
# xargs/-exec splits into batches, each emitting its own "total" line.
find Client/SpriteLib \( -name '*.cpp' -o -name '*.h' \) -print \
    | xargs wc -l | awk '$2!="total"{s+=$1} END{print s}'   # -> 53566
find tools/engine/sprite/src -name '*.c' -print \
    | xargs wc -l | awk '$2!="total"{s+=$1} END{print s}'   # -> 4606
find tools/engine/sprite/include -name '*.h' -print \
    | xargs wc -l | awk '$2!="total"{s+=$1} END{print s}'   # -> 3143
ls Client/SpriteLib/ | grep -E '555|565|4444' | wc -l # -> 20
find Client/SpriteLib -name '*.cpp' -exec wc -l {} + | sort -rn | head -8

# Finding 3: variants are serializers
cat Client/SpriteLib/CSprite565.h

# dead-class check
grep -rn "\bCAlphaSpritePackList565\b" --include=*.cpp --include=*.h . \
    | grep -v "SpriteLib/CAlphaSpritePackList565\."    # -> 0 hits
```

All figures measured against the working tree at commit `f19c4d3`, branch
`modernize/phase-4-sprite`. **No figure here has been confirmed by a compile.**
