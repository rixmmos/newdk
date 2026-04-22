# Runtime smoke test — results and findings

**Run date:** 2026-04-20
**Runner:** Enrico (WSL2 Ubuntu 22.04 on WINDOWS-8I5Q22Q)
**Repo root:** `/mnt/c/newdk/` (DrvFs NTFS mount — *not* native ext4)
**Goal (from `SMOKE_TEST.md`):** get one client to the login screen,
talking to a server cluster you just built, against a fresh MySQL.
Write down every failure as you go.

## TL;DR

The three-daemon server cluster builds, runs, binds its ports, exchanges
inter-server handshakes, and accepts client TCP. The client binary
(`DarkEden`) builds. We proved console-level reachability end-to-end;
we did not drive a real login packet through the client because that
requires a display and/or a custom packet-encoding probe neither of which
is smoke-test scope.

Thirteen source bugs were found and fixed during the run (Bugs Q
through CC below). Bugs V, W, X, Y, Z, and AA were uncovered by the
Step-3 WSLg client smoke (the display-required path the initial
console smoke skipped): V blocked text rendering (no Linux font in
the fallback list); W was a latent word-wrap `substr` out-of-range
that only surfaced once V was fixed and a real font actually
loaded; X was a `<mutex>`-inside-`extern "C"` landmine in the
Platform.h include chain, triggered when Bug W's source edit forced
a fresh recompile of `TextService.cpp` after weeks of the broken
chain being incrementally cached; Y was a 28-year-old `#define
max`/`#define min` macro pair colliding with libstdc++11's
`std::max` call inside `stl_bvector.h::_M_check_len`, surfaced when
Bug X's fix forced `RenderTargetSpriteSurface.cpp` to recompile; Z
was `PlatformSDL.cpp` calling `dirname(3)` without including
`<libgen.h>` on non-Emscripten POSIX, surfaced when Bug Y's edit to
Platform.h forced a full basic/ library rebuild; AA was a cluster
of three MSVC6-era constructs that `AcceleratorManager.cpp`'s
recompile unmasked together — bare `std::ifstream;`/`std::ofstream;`
in three SpriteLib headers, a `Race Race;` member shadowing the
`Race` enum type in `VS_UI_Title.h`'s `S_SLOT` struct, and a
missing `typename` on a dependent-scope `const_iterator` in
`CPositionList.h`.
Two non-bug configuration gaps were closed (`odk-mysql` hostname,
CRLF-on-DrvFs confs). The initial write-up also logged a "retail
`.smp` format divergence" finding, which turned out on follow-up to
be a viewer-tool bug, not a data issue — see Bug U for the
retraction.

## Environment deltas from the runbook

The actual run diverged from `SMOKE_TEST.md` in three places. None
affected the outcome; noting them for the record.

- **Database engine:** runbook assumed MySQL 8; run used MariaDB 10.6.
  Wire-compatible, schema loaded cleanly. No `sql_mode` tweaks needed —
  MariaDB 10.6 is permissive enough for the MySQL-5.7-era dumps.
- **Config strategy:** runbook assumed `.conf.template` files with Phase
  8B env-var substitution (`${DKRIX_DB_*}`). That template layer was
  not used — we edited `conf/*.conf` directly via `sed` to swap IPs and
  `HomePath`. Credentials ended up in the confs in plaintext. For a
  smoke test that's fine; for anything past this, re-apply the
  templates.
- **Repo path:** runbook assumed `~/work/dkrix-upstream/`; actual tree
  lives at `/mnt/c/newdk/` on DrvFs. This mattered: DrvFs is
  case-insensitive (→ Bug Q) and carries CRLF line endings through to
  Linux tools (→ Bug T). A future run on native ext4 would avoid both
  issues without patching anything.

## What worked

Concrete evidence for each layer, top to bottom:

**MySQL bootstrap (`STEP1_MYSQL.md`):** Both databases created and loaded.
Verified row counts: DARKEDEN 374 tables, USERINFO 7 tables. User
`elcastle` granted from `%`, `localhost`, and `127.0.0.1` (MariaDB
treats them as distinct hosts).

**Server build (`STEP2_SERVER.md`):** `make debug` produced all three
binaries in `dkrixserver/bin/` after Bugs Q and R were applied.
libmysqlclient, liblua5.1, libxerces-c all discovered via pkg-config
without manual path overrides.

**Client build:** `dkrix/build/bin/DarkEden` exists (plus six console
viewer/parser tools: `zone_parser`, `resource_validator`, `map_viewer`,
`sprite_viewer`, `creature_viewer`, `effect_viewer`, `item_viewer`).

**Server launch:** All three daemons in foreground, all three ports
bound, inter-server handshake flowing. Captured in
`STEP2_GREEN_SNAPSHOT.md`. The money-shot `ss -ltnp`:

```
LISTEN 0 5  0.0.0.0:9977  sharedserver (pid 81989)
LISTEN 0 5  0.0.0.0:9998  gameserver   (pid 82254)
LISTEN 0 5  0.0.0.0:9999  loginserver  (pid 82197)
```

Notable log landmarks seen: `---------- Start SharedServer ----------`,
`Receive:GSRequestGuildInfo` / `Send:473[2,0] SGGuildInfo()` (inter-server
handshake), `>>> ALL INITIALIZATIONS ARE COMPLETED SUCCESSFULLY.`,
`>>> STARTING ClientManager->start() INFINITE LOOP...`.

**Client TCP reachability:** `nc 127.0.0.1 9999` succeeded; loginserver
printed `NEW CONNECTION FROM 127.0.0.1:<port>` and correctly raised
`ConnectException : connect closed.` down through
`Player::processInput() → SocketInputStream::fill() → SocketImpl::receive()`
when we closed the nc without speaking. Protocol is client-initiates
(no greeting banner).

**Port-shipped asset readability:** `zone_parser adam_c.smp` parsed a
port-shipped `.smp` cleanly (ZoneID 72, 256×256 sectors, format header
`=MAP_2000_05_10=`). The port's parser works. (*Note:* the initial
run reported `254×2 sectors` here — that was the `zone_parser` tool
bug later filed and fixed as Bug U below. Real dims are 256×256.)

## Bugs found and fixed during the run

All four live in the repo now. For the first two, `server_build_fix.sh`
carries them idempotently so a clean re-clone can replay them; the
other two are source edits that should be committed.

### Bug Q — shadowing `src/Core/Endian.h`

**Symptom:** build fails with `htole32 was not declared in this scope`
in glibc's `<endian.h>` (really, in our `Endian.h`).

**Cause:** `src/Core/Endian.h` shadows glibc `<endian.h>` on
case-insensitive filesystems (DrvFs). Transitive chain:
*any glibc header* → `<sys/types.h>` → `<endian.h>` resolves to OUR file
first, because `src/Core/` is on the `-I` path and `-I` beats system
dirs. Our file then does `#include <endian.h>` on line 65, which
re-resolves to itself, the header guard bails, and glibc's `htole*` /
`*toh` macros never get declared.

**Fix:** delete the file. Per its own docstring it was an unused
Phase-13B placeholder deferred until Phase 13.3. When 13.3
reintroduces it, use a non-shadowing name (`LEndian.h`, `ByteOrder.h`).

### Bug R — `MYSQL_BIND::is_null` typing (MySQL 5.7 vs 8.0)

**Symptom:** build fails in `PreparedStatement.cpp:204` with
`cannot convert 'char*' to 'bool*' in assignment`.

**Cause:** MySQL 5.7 had `typedef char my_bool`, so
`MYSQL_BIND::is_null` was `my_bool*` (= `char*`). MySQL 8.0 removed
`my_bool` and `is_null` became `bool*`. The `.cpp` hardcoded
`reinterpret_cast<char*>(&p.isNull)` which worked on 5.7 only. Phase 11B
had a header-side `typedef bool my_bool_t` that acknowledged the
problem but the .cpp didn't use it.

**Fix:** replace the typedef with
`typedef std::remove_pointer<decltype(MYSQL_BIND::is_null)>::type my_bool_t;`
(derives from the installed library's own declaration — works on both
MySQL 5.7 / MariaDB *and* MySQL 8.0+, no `#ifdef` version sniffing) and
drop the `reinterpret_cast` at the call site.

### Bug S — `catch (Error&)` in all three server `main.cpp`s

**Symptom:** sharedserver crashed with
`terminate called after throwing an instance of 'IOException'` and
`what(): std::exception`. No useful error text.

**Cause:** `main.cpp` catches only `Error&`. `IOException` lives in
the `Exception` subtree of `Throwable`, parallel to `Error` — so
`IOException` (thrown by `Properties::load`) escaped the catch
entirely and hit `terminate()`. `Throwable::what()` isn't overridden so
it fell through to `std::exception::what()`, printing the literal
string `"std::exception"`.

The other two `try` blocks in each `main.cpp` catch `Throwable&`
correctly — only the first one was narrowed. Gameserver's version was
worse: its catches had the `cout` commented out so config failures
were silently swallowed, leaving `g_pConfig` half-initialised for
subsequent `getProperty` calls to fail obscurely.

**Fix:** widened all five affected catches (one in
`sharedserver/main.cpp`, two each in `loginserver/main.cpp` and
`gameserver/main.cpp`) to `catch (Throwable&)`, and uncommented
gameserver's prints. Config-load catches now call `exit(1)` so failure
can't dereference a broken `g_pConfig`.

### Bug T — CRLF line endings break `Properties::load`

**Symptom:** once Bug S surfaced the real error, it read
`IOException : missing separator`. The config file was visually clean —
every non-comment line had a colon.

**Cause:** confs on `/mnt/c/...` DrvFs have CRLF line endings. `getline()`
strips `\n` but leaves `\r`. `Properties::WhiteSpaces` is `" \t"` only,
so a blank separator line in a CRLF file (just `\r`) isn't empty, isn't
comment-prefixed, and has no `:` — the separator check throws.

**Fix:** one-line change in `Properties::load` — strip trailing `\r`
immediately after `getline()`, before the empty/comment check. This
also silently fixes a latent would-have-been-next-issue: values like
`DB_PASSWORD : elca110` were parsing to `"elca110\r"`, which would have
failed MySQL auth and looked exactly like a wrong-password bug.

### Bug U — `zone_parser` misreads `.smp` format

**Symptom:** `zone_parser <retail>/cave_office.smp` →
`Failed to parse zone file`. Same tool against port-shipped
`adam_c.smp` appeared to succeed but printed nonsensical dimensions
(`Width: 254, Height: 2` for a file that is actually 256×256).
Initial smoke-test write-up mistook this for a format divergence
between port and retail data packs. It was not.

**Cause:** `dkrix/tools/viewers/zone_parser/main.cpp` had two wrong
assumptions about the `.smp` on-disk layout:

1. An 8-byte "Tile FP + ImageObject FP" skip was inserted between the
   description MString and the `width`/`height` fields. No such pair
   exists. Ground-truth reader: `dkrixserver/src/server/gameserver/
   Zone.cpp:866-877`, which reads width/height immediately after desc.
2. The sector region was assumed to be 4 bytes per sector. Real
   layout is **1 byte per tile** (flag bits `0x01` BLOCK_BURROW,
   `0x02` BLOCK_WALK, `0x04` BLOCK_FLY, `0x80` PORTAL), with a
   variable-length portal payload when `(flag & 0x80)`. Ground truth:
   `Zone.cpp:943-1100`.

Combined effect: nearly every `.smp` file failed to parse. Two files
(`adam_c`, `adam_e`) happened to have bytes at the shifted offset
whose subsequent `uint32_t` reads didn't overrun EOF, so they "parsed"
— with fantasy width/height values.

**Fix:** removed the bogus 8-byte skip; rewrote `PrintSectorData()`
to read 1B/tile and decode the flag bits, stopping at the first
portal (payload is variable-length and would require the full
per-portal-type switch from `Zone.cpp` to walk further — out of
scope for a viewer tool). Dropped `numImageObjects` / `numObstacles`
from the header report, since a flat skip can't land on those
fields when the sector region is variable-length.

**Verification:** 164/164 `.smp` files (142 port-shipped + 22 retail)
parse cleanly after the fix. Retail and port use **identical**
`.smp` format; the earlier "divergence" finding was entirely a
viewer-tool bug.

**Impact:** removes a phantom blocker for retail-asset workflows.
Viewer tools now agree with the server's own `Zone::load` reader.

### Bug V — `TextBackendSDL` font fallback had no Linux system paths

**Symptom:** on the first successful Step-3 client boot (post-`.rpk`
extraction), the title screen rendered correctly but every
`TTF_OpenFont` call failed, logging
`TextBackendSDL: Failed to load font size 16` dozens of times. The
UI appeared as the background image only — no login form, no
copyright string, no button labels. Game loop entered, `MAINMENU`
mode active, 0 glyphs drawn.

**Cause:** `dkrix/Client/TextSystem/TextBackendSDL.cpp:90-98` held a
hardcoded 6-path fallback list: four `Data/Font/*` project-relative
paths, then two macOS `/System/Library/Fonts/*` paths. **No Linux
system paths.** On Ubuntu/WSL, none of the first four existed (retail
doesn't ship a `Data/Font/` dir) and neither did the macOS paths, so
every `AcquireFont()` call returned a null handle and the text
layer silently drew nothing.

**Fix:** inserted four Debian/Ubuntu system-font paths into the
fallback list between the `Data/Font/*` entries and the macOS ones:

    /usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc
    /usr/share/fonts/opentype/noto/NotoSansCJK.ttc
    /usr/share/fonts/truetype/noto/NotoSans-Regular.ttf
    /usr/share/fonts/truetype/dejavu/DejaVuSans.ttf

Corresponds to `apt install fonts-noto-cjk fonts-noto-core
fonts-dejavu`. NotoSansCJK is probed first because the smoke-test
build defaults to Chinese locale on a C/POSIX WSL shell (the
language detector falls through to Chinese when it can't identify a
specifically-Korean environment — that's a separate finding that
hasn't reproduced as a blocker yet).

**Verification:** pending rebuild on Enrico's box. Ran 1 smoke-test
cycle showing `[WARN] no TTF/TTC font found on the search path` from
`client_smoke.sh`'s new font precheck, confirming the search
walks the correct paths.

**Zero-code workaround for pre-patch binaries:**

    sudo apt install -y fonts-noto-cjk
    mkdir -p "/mnt/c/newdk/Darkeden data/Data/Font"
    ln -sf /usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc \
           "/mnt/c/newdk/Darkeden data/Data/Font/NotoSansCJK-Regular.ttc"

Matches the first entry in the fallback list so the current binary
finds a font without needing the patched search order.

### Bug W — `TextService::WrapText` substr out-of-range when space itself overflows

**Symptom:** immediately after Bug V was worked around and a font
actually loaded, boot aborted from `SDLMain.cpp:525-528`'s
`catch (std::exception&)` with

    ERROR: std::exception: basic_string::substr: __pos (which is 21) > this->size() (which is 20)
    SDLMain: InitApp failed, exiting

Thrown inside `InitApp`, after `"Initializing application..."` — the
first point where title-screen strings are wrapped for rendering.
Dormant until now: with no font, `GetGlyphMetrics` returned false
and the whole `WrapText` inner body was skipped (`continue` at line
320).

**Cause:** `dkrix/Client/TextSystem/TextService.cpp:295-340`. The
word-wrap loop records the break position **before** checking
overflow:

    if (codepoint == ' ') {
        lastBreakIndex = static_cast<int>(line.size());  // = N
        lastBreakSkip  = len;                            // = 1 for ASCII ' '
    }

    if (maxWidth > 0 && lineWidth + metrics.advance > maxWidth && !line.empty()) {
        if (lastBreakIndex >= 0) {
            lines.push_back(line.substr(0, lastBreakIndex));     // OK: (0, N)
            line = line.substr(lastBreakIndex + lastBreakSkip);  // N + 1 on size-N string → throw
            ...

When the current codepoint is the space itself AND that space's
advance is what tips the line over `maxWidth`, the break is
recorded at `line.size()` (the space hasn't been appended yet), so
`lastBreakIndex + lastBreakSkip` == `line.size() + 1` — one past
the end. `std::string::substr(pos > size())` throws `out_of_range`.

Matches the observed numbers: a 20-character UI string (several
title-screen strings are in that range — `"DARK EDEN EXTREME"` is
17 + the padding strings the client wraps against the logo box)
where a trailing word would push over the bounding width.

**Fix:** `dkrix/Client/TextSystem/TextService.cpp:328-342` — clamp
the resume position before the second `substr`. If the resume
point is at-or-past end-of-line (i.e. the break char is the current
codepoint, not yet in `line`), reset `line` to empty instead of
calling `substr`:

    size_t breakPos = static_cast<size_t>(lastBreakIndex);
    size_t resumeAt = breakPos + static_cast<size_t>(lastBreakSkip);
    lines.push_back(line.substr(0, breakPos));
    if (resumeAt >= line.size()) {
        line.clear();
    } else {
        line = line.substr(resumeAt);
    }

Minimal, localized, preserves existing wrap semantics for all
cases where the break char has already been appended.

**Verification:** pending rebuild. Expected behaviour post-fix:
title screen renders with text strings correctly wrapped, login
form visible, no `ERROR: std::exception` from SDLMain's catch. If
it still throws out of the same catch with a different message,
that's a second latent text-path bug and we'll chase it next.

**Zero-code workaround:** none practical — the only way to avoid
the throw without patching is to have `GetGlyphMetrics` return
false, which means rolling back Bug V's font fix. Not useful.

### Bug X — `SpriteLibBackend.h` wraps `Platform.h` in its own `extern "C"`

**Symptom:** rebuilding `TextService.cpp` after the Bug W patch
produced a cascade of

    /usr/include/c++/11/mutex:515:3: error: template with C linkage
    /usr/include/c++/11/mutex:520:3: error: template with C linkage
    ...
    note: 'extern "C"' linkage started here
       18 | extern "C" {           ← SpriteLibBackend.h:18

across every `std::unique_lock`, `std::lock_guard`, `std::lock`,
`std::try_lock`, `std::call_once`, and `std::async` template in
`<mutex>`. The include chain the compiler reports:

    TextService.cpp:10
     → SpriteLib/CSpriteSurface.h:48
      → SpriteLib/SpriteLibBackend.h:18   extern "C" {     ← level 1
       → basic/Platform.h:21              extern "C" {     ← level 2
        → basic/Platform.h:214            }   close
        → basic/Platform.h:216            #include <mutex>   ← still in level-1 extern "C"
        → basic/Platform.h:226            extern "C" {    reopen

**Cause:** `Platform.h` has its own `<mutex>` close/reopen dance
(lines 211-227, introduced in `0ba8f99 client: back CRITICAL_SECTION
with std::recursive_mutex`). The dance assumes Platform.h is
included at C-linkage depth 1 — it closes exactly one `}`, emits
`<mutex>`, then re-opens one `extern "C" {`. When
`SpriteLibBackend.h` wrapped its OWN `extern "C" {` around the
Platform.h include, Platform.h's single close only popped to
depth 1, not to C++ linkage. libstdc++11's `<mutex>` is full of
templates, templates can't have C linkage, boom.

**Why it didn't blow up before:** the Client+SpriteLib build graph
has `TextService.cpp` and several SpriteLib TUs compiled ONCE per
incremental build; CMake had cached `TextService.cpp.o` at
`build/CMakeFiles/TextSystem.dir/.../TextService.cpp.o` (mtime
2026-04-20 11:29, before my edit). Nothing had forced TextService
to recompile since the `std::recursive_mutex` commit landed, so
the broken include chain was dormant. Bug W's source edit was the
first thing to dirty TextService.cpp and surface it.

Every SpriteLib internal TU has the same chain but they all sit
in `Client/SpriteLib/*.cpp`; those TUs happen to include
`<mutex>` or something that includes `<mutex>` earlier in their
include list, so by the time Platform.h:216 fires, `<mutex>`'s
own include guard (`_GLIBCXX_MUTEX`) is set and the header's
second inclusion is a no-op. That's what carried the port all
the way through — the only TU where `<mutex>` genuinely first
appeared via the SpriteLibBackend path was TextService.

**Fix:** move the `#include "../basic/Platform.h"` in
`dkrix/Client/SpriteLib/SpriteLibBackend.h` to BEFORE the
`#ifdef __cplusplus extern "C" {` block. Platform.h's own
linkage-management is idempotent and correct when invoked from
C++ linkage at depth 0; the only thing that needs to be inside
SpriteLibBackend's own `extern "C"` is the C-callable interface
it defines below.

**Verification:** pending rebuild.

### Bug Y — `Platform.h` `#define max`/`#define min` macros eat `std::max` in STL

**Symptom:** after Bug X unblocked `TextService.cpp`, the next TU to
compile cleanly-from-scratch (`RenderTargetSpriteSurface.cpp`) fell
over in a different spot:

    In file included from /usr/include/c++/11/vector:67,
                     ...
                     from RenderTargetSpriteSurface.cpp:3:
    /usr/include/c++/11/bits/stl_bvector.h:706:45: error: expected
      unqualified-id before '(' token
      706 |         const size_type __len = size() + std::max(size(), __n);
          |                                             ^~~~

Pre-processor view showed the line expanded to
`size() + std::(((size()) > (__n)) ? (size()) : (__n));` — i.e.
`std::max` had been rewritten by the function-like macro into
`std::(...)`, a syntax error.

**Cause:** `dkrix/basic/Platform.h:1730-1742` defined the Win32-era
macros

    #define max(a, b) (((a) > (b)) ? (a) : (b))
    #define min(a, b) (((a) < (b)) ? (a) : (b))

inside the `#ifndef PLATFORM_WINDOWS` block. The preprocessor is
blind to namespaces and C++ syntax — any token sequence matching
`max(x,y)` or `min(x,y)` gets rewritten, including `std::max(...)`.
libstdc++11 uses `std::max` throughout `<vector>`, `<deque>`,
`<bitset>`, `<bits/stl_bvector.h>`, and so on. Every TU that
transitively included both Platform.h and one of those STL headers
was a ticking bomb. It had been cached as clean `.o`s for so long
that the macros' incompatibility was invisible until Bug X's include
reshuffle finally retriggered a fresh compile of this TU.

**Why not just delete the macros?** The tree still has 568 bare
`max(a,b)` and `min(a,b)` call sites — the Win32 idiom — and
sanitizing all of them is a multi-commit refactor we don't want to
do mid-smoke-test.

**Fix:** replace the macros with template functions. Two template
parameters plus decltype trailing return keep mixed-type call sites
(`max(0, someWORD)`) working. Templates can't have C linkage, so
the block uses Platform.h's own close/reopen `extern "C"` dance —
same pattern as the `<mutex>` dance at line 214. Under a C compiler,
the old macro definitions remain as a fallback:

    #ifdef __cplusplus
    }  /* close extern "C" so we can define templates */
    #ifndef DKRIX_MINMAX_DEFINED
    #define DKRIX_MINMAX_DEFINED
    template<typename T, typename U>
    static inline auto max(T a, U b) -> decltype((a > b) ? a : b)
    { return (a > b) ? a : b; }
    template<typename T, typename U>
    static inline auto min(T a, U b) -> decltype((a < b) ? a : b)
    { return (a < b) ? a : b; }
    #endif
    extern "C" {  /* reopen */
    #else
    #ifndef max
    #define max(a, b) (((a) > (b)) ? (a) : (b))
    #endif
    #ifndef min
    #define min(a, b) (((a) < (b)) ? (a) : (b))
    #endif
    #endif

Because these are functions, not macros, `std::max(size(), __n)`
inside `<bits/stl_bvector.h>` resolves correctly to
`std::max<size_type>` without triggering token substitution, and the
568 unqualified `max(a,b)` / `min(a,b)` sites in the tree still
pick up `::max` / `::min` via unqualified lookup.

**Verification:** pending rebuild.

**Future cleanup (not smoke-test-blocking):** migrate the 568
unqualified call sites to `std::max`/`std::min` with proper
`#include <algorithm>`, then delete the template helpers entirely.
Clean win for readability and compile-time; deferred because
mechanical and orthogonal to Step-3.

### Bug Z — `PlatformSDL.cpp` uses `dirname(3)` without `<libgen.h>`

**Symptom:** after Bug Y's Platform.h edit forced every TU that
includes Platform.h to recompile, the basic/ library (the first
thing in the build graph) fell over at 2%:

    /mnt/c/newdk/dkrix/basic/PlatformSDL.cpp: In function
      'int platform_get_executable_dir(char*, size_t)':
    PlatformSDL.cpp:277:21: error: 'dirname' was not declared in this scope
      277 |     char* dir = dirname(path);
          |                 ^~~~~~~
    [ 2%] gmake[3]: *** [basic/CMakeFiles/basic.dir/build.make:146:
      basic/CMakeFiles/basic.dir/PlatformSDL.cpp.o] Error 1

**Cause:** `dkrix/basic/PlatformSDL.cpp:33-41` gates
`#include <libgen.h>` behind `#ifdef __EMSCRIPTEN__`:

    #ifdef __EMSCRIPTEN__
        ...
        #include <libgen.h>  /* For dirname */
    #endif

The PLATFORM_LINUX and PLATFORM_MACOS branches both fall through to
line 277's `dirname(path)` call without ever including the header
that declares it. Worked on Emscripten only. The reason it had never
surfaced: `PlatformSDL.cpp.o` was one of the oldest cached `.o`
files in the tree — pre-dated whatever commit added the Emscripten
gating — and hadn't been recompiled in ages. Bug Y's Platform.h
edit dirtied every dependent, including this TU, and the latent
missing include surfaced.

**Fix:** one line. Move `#include <libgen.h>` to the unconditional
POSIX-header block at the top of the `#ifndef PLATFORM_WINDOWS`
section (alongside `<unistd.h>`, `<errno.h>`, etc.). `<libgen.h>` is
POSIX, present on Linux, macOS, and Emscripten alike — no platform
guard needed. The Emscripten-block include becomes redundant but is
harmless and left alone.

**Verification:** pending rebuild.

### Bug AA — VS_UI MSVC6 construct cluster

**Symptom:** after Bug Z unblocked `basic/`, the build entered the
VS_UI library and `AcceleratorManager.cpp` (the TU that transitively
pulls in most of the VS_UI header tree, including the SpriteLib
headers via `SP.h`) compiled for the first time in a long time. It
produced three distinct errors in parallel, at six source locations:

    CSpriteDef.h:14:6: error: declaration does not declare anything
       14 | std::ifstream;
    CSpriteDef.h:15:6: error: declaration does not declare anything
       15 | std::ofstream;
    CSprite.h:29:6 / :30:6:   (same)
    CFilter.h:21:6 / :22:6:   (same)

    VS_UI_Title.h:241:73: error: declaration of 'Race S_SLOT::Race'
      changes meaning of 'Race' [-fpermissive]
      241 |     Race                                Race;

    CPositionList.h:107:17: error: need 'typename' before
      'CPositionList<Type>::POSITION_LIST::const_iterator' because
      'CPositionList<Type>::POSITION_LIST' is a dependent scope
      107 |         POSITION_LIST::const_iterator  GetIterator() const ...

Three MSVC6-era patterns GCC refuses to accept under strict mode,
all in TUs that hadn't been recompiled since whatever commit first
made them buildable against GCC.

**Causes:**

1. *Bare using-declarations.* `std::ifstream;` / `std::ofstream;`
   outside a `using` directive. MSVC6 treated these as
   class-forward-declarations with namespace qualifiers; standard
   C++ requires `using std::ifstream;`. GCC correctly rejects, and
   `-fpermissive` would downgrade to a warning but we don't enable
   that flag. The bare decls do nothing either way — `<fstream>` is
   already included at the top of each header.

2. *Member name shadows enum type.* `Race Race;` inside `S_SLOT`
   declares a data member named `Race` of type `Race` (the
   `enum Race` from `Client/RaceType.h`). Inside the struct's
   scope, the member name then shadows the type name — GCC flags
   "changes meaning of 'Race'" because subsequent uses of the
   unqualified identifier `Race` in that scope would now resolve
   to the data member. Same `-fpermissive`-downgradable situation.

3. *Missing `typename` in dependent scope.* In a class template,
   `POSITION_LIST::const_iterator` where `POSITION_LIST` depends on
   the template parameter requires the `typename` keyword —
   otherwise the compiler has to assume `const_iterator` is a
   static data member, not a type. MSVC6 guessed; GCC doesn't.

**Fixes:**

1. *Bare decls:* deleted the six lines (two per header in
   `CSpriteDef.h`, `CSprite.h`, `CFilter.h`). Replaced with a short
   comment explaining why. `<fstream>` includes are unaffected.

2. *`Race` shadow:* renamed the `S_SLOT` member `Race` → `eRace`
   (matches the existing `eRace` local-variable convention already
   in the same codebase — e.g. `g_char_slot_ingame.Race = eRace;`
   in GameUI.cpp:1212). Updated 39 call sites across five files:

       VS_UI_Title.cpp       33 sites (m_p_slot->Race, p_slot->Race)
       Client/GameUI.cpp      4 sites
       Client/GameMain.cpp    1 site
       Client/PacketFunction.cpp  1 site
       (header)               1 site (declaration itself)

   WinMain.cpp and `MitemTableInit.cpp` were deliberately NOT
   changed: WinMain.cpp is Windows-only (listed in Platform.h:349
   as a surviving Windows-only TU, not compiled on Linux);
   MitemTableInit.cpp's `.Race = 5` assignments are on a completely
   different struct whose `.Race` field is an `int`, not the `Race`
   enum — no shadow, no need to touch.

3. *Missing `typename`:* added the keyword at `CPositionList.h:107`.

**Why not `-fpermissive` globally?** It's tempting to just flip the
flag — all three of these errors downgrade to warnings under it, no
source changes. Rejected because `-fpermissive` also silently
downgrades unrelated real bugs (implicit `int` declarations, narrow
conversions in function calls, ambiguous base accesses) that we
want GCC to keep flagging on the other ~800 TUs in this tree. Spot
fixes preserve the error signal.

**Verification:** partial pass — the rebuild progressed past the
three original error classes, but GCC found exactly one missed
call site as the mechanical hint predicted:

    VS_UI_GameCommon.cpp:20215:39: error: 'struct S_SLOT' has no
      member named 'Race'; did you mean 'eRace'?

This site wasn't in my initial sweep because my grep patterns
targeted `m_p_slot->Race`, `p_slot->Race`, `slot.Race`, and
`g_char_slot_ingame.Race` across VS_UI_Title.cpp + Client/*.cpp,
but missed the one `g_char_slot_ingame.Race` hit in
VS_UI_GameCommon.cpp. Patched in place (line 20215,
`C_VS_UI_INFO::Process()` level-up gate). Post-patch sweep with
`grep -an '\.Race\b|->Race\b' VS_UI/src Client` now returns zero
S_SLOT hits — the MItemTable.cpp matches are on an unrelated
`Type::Race` int field on `CDataTable<Type>`, not S_SLOT. Rebuild
pending.

**Windows-build follow-up (out of scope for smoke test):** when the
Windows toolchain is revived (task #38 deferred), `WinMain.cpp`'s
3 `g_char_slot_ingame.Race` / `slot.Race` sites need the same
rename. Low-effort, three-line change.

### Bug BB — two more SpriteLib bare using-decls the AA sweep missed

**Symptom:** after Bug AA's three-header fix, `CSpriteSurface_SDL.cpp`
compilation cascade pulled in two SpriteLib headers I hadn't
touched, and they had the same pattern:

    CAlphaSprite.h:19:6: error: declaration does not declare anything
       19 | std::ofstream;
    CAlphaSprite.h:20:6: error: declaration does not declare anything
       20 | std::ifstream;

Plus a sibling I caught pre-emptively via
`grep -al '^std::[io]fstream;'` against every header in the tree:

    CShadowSpritePack.h:48  std::ofstream;
    CShadowSpritePack.h:49  std::ifstream;

**Cause:** same as the three headers fixed in Bug AA — MSVC6-era
bare using-declarations. These two weren't in the error cascade the
first time because ripgrep had been silently skipping files with
trailing NUL padding (DrvFs artifact) as binary. Re-ran with
`grep -a` to force-text to confirm these two are the only remaining
offenders in the tree.

**Fix:** deleted the four lines across the two headers.

**Verification:** pending rebuild. Post-AA/BB the `^std::[io]fstream;`
pattern has zero hits across `dkrix/**/*.h`.

**Lesson:** when sweeping the tree for MSVC6-era patterns, use
`grep -a` (or strip NULs before piping) — ripgrep and plain grep
both flip into binary mode on DrvFs-padded files and silently
skip them.

### Bug CC — second Race-shadow site: UserInformation::Race[3]

**Symptom:** after Bug AA fixed `S_SLOT::Race`, the `VS_UI_Dialog.cpp`
recompile surfaced a second instance of the same shadow pattern:

    UserInformation.h:47:41: error: declaration of
      'Race UserInformation::Race [3]' changes meaning of 'Race'
      [-fpermissive]
       47 |       Race        Race[3];       // slayer 캐릭터인가?

**Cause:** same as Bug AA — a data member named `Race` of enum type
`Race`, this time an array of 3 (one per character slot). The
AA fix only renamed the member on `S_SLOT`; the same pattern on
`UserInformation` is independent and had to be renamed separately.
Didn't pre-catch this during the AA sweep because my initial grep
for `Race Race` with leading-whitespace anchor happened to hit
both files — but the AA-era edit only covered S_SLOT. Mea culpa.

**Fix:** same rename. `Race Race[3];` → `Race eRace[3];` in
`UserInformation.h:47`. Updated 4 call sites:

    Client/Packet/Lpackets/LCPCListHandler.cpp  3 sites
    Client/UIMessageManager.cpp                 1 site

All are `g_pUserInformation->Race[i]` accesses; none are ambiguous
with other structs. Pre-emptive `grep -aHEn '^\s+Race\s+Race\b'`
across every header in the tree now returns zero hits — these two
(S_SLOT and UserInformation) were the only shadow sites.

**Verification:** pending rebuild.

### Bug DD — `MSkillManager.h` missing `#include <vector>`

**Symptom:** after the Bug AA/BB/CC rebuild, with `--target DarkEden`
forcing the full translation-unit cascade, `MSkillManager.h` failed
with:

    MSkillManager.h:451:30: error: 'vector' in namespace 'std' does
      not name a template type
      451 |     typedef std::vector<ACTIONINFO>  SKILL_STEP_LIST;
          |                              ^~~~~~
    MSkillManager.h:454:20: error: 'SKILL_STEP_LIST' was not declared
      in this scope; did you mean 'SKILL_STEP_NULL'?
    MSkillManager.h:528:9:  error: 'SKILL_STEP_LIST' does not name a
      type

Cascade into `MSkillManager.cpp` at lines 1835/1837/1839/1843/1849
and 2555/2557/2568 and 2587-2623 — every `SKILL_STEP_LIST` use after
the broken typedef.

**Cause:** the header uses `std::vector<ACTIONINFO>` for
`SKILL_STEP_LIST` but only `<fstream>`, `<list>`, and `<map>` were
included. MSVC6's STL leaked `<vector>` through `<list>` or `<map>`;
libstdc++ does not. Straightforward missing-include surfaced by
GCC's stricter header hygiene.

**Fix:** added `#include <vector>` at `MSkillManager.h:127` alongside
the other STL includes. `SKILL_STEP_LIST`, `SKILL_STEP_MAP`, and
`GetSkillStepList()` all recover with zero other edits.

**Verification:** pending rebuild.

### Bug EE — `CPartManager.h` dependent-scope typedef missing `typename`

**Symptom:** after Bug DD, the next cascade was in `CPartManager.h`:

    CPartManager.h:85:28: error: need 'typename' before
      'CPartManager<IndexType, PartIndexType, DataType>
      ::PARTINDEX_LIST::iterator' because 'CPartManager<IndexType,
      PartIndexType, DataType>::PARTINDEX_LIST' is a dependent scope
       85 | typedef PARTINDEX_LIST::iterator PARTINDEX_LIST_ITERATOR;
          |                            ^~~~~~~~
    CPartManager.h:144:3:  error: 'PARTINDEX_LIST_ITERATOR' does not
      name a type
    CPartManager.h:177,240,288,340,398,403,461,466:
      'm_pPartIterator' was not declared in this scope

**Cause:** inside the class template `CPartManager<IndexType,
PartIndexType, DataType>`, `PARTINDEX_LIST` is
`std::list<PartIndexType>` — a dependent type. C++ requires
`typename` on any nested-name-specifier that depends on a template
parameter. MSVC6 did not enforce this rule; GCC 11 does. The
downstream 8-site `m_pPartIterator` cascade is the same pattern:
the member declaration at line 144 references the broken typedef,
so the member itself fails to declare, and every method that uses
it breaks.

**Fix:** `CPartManager.h:85`:

    -    typedef  PARTINDEX_LIST::iterator  PARTINDEX_LIST_ITERATOR;
    +    typedef  typename PARTINDEX_LIST::iterator
                                           PARTINDEX_LIST_ITERATOR;

One edit fixes all 10 downstream errors.

**Verification:** pending rebuild.

### Bug FF — `ValueList.h` `std::list<T>::const_iterator` missing `typename`

**Symptom:** `ValueList.h:34` and `:35`:

    ValueList.h:34: need 'typename' before
      'std::__cxx11::list<DataType>::const_iterator'
      because '...' is a dependent scope

**Cause:** same pattern as Bug EE. `ValueList` is a class template
parameterized on `T`; `std::list<T>::const_iterator` is a dependent
type that the MSVC6-era code left unqualified.

**Fix:** added `typename` to both `Begin()` and `End()` return
types at `Packet/ValueList.h:34-35`.

**Verification:** pending rebuild.

### Bug HH — `<vector>`-in-header-without-`#include <vector>` sweep (14 headers)

**Symptom:** after DD/EE/FF/GG cleared the first cascade, the next
rebuild round failed on
`CMakeFiles/VS_UI.dir/Client/MEventManager.cpp.o`. The error was
another missing-`<vector>` site (`MEventManager.h:102`). Gdb
against the still-stale binary reproduced the exact Bug W signature
from before, confirming the rebuild had not yet completed.

**Cause:** same root as Bug DD — MSVC6's STL leaked `<vector>`
through `<map>`/`<list>`/nothing; libstdc++ does not. Rather than
fix-one-surface-one, I swept all headers in `Client/` and
`VS_UI/src/header/` for `std::vector<` without `#include <vector>`
and fixed them all in one pass to break the cycle of single-header
rebuild rounds.

**Fix:** added `#include <vector>` to 14 headers:

    Client/MEventManager.h                    (:12)
    Client/BloodBibleSignInfo.h               (:15)
    Client/MFakeCreature.h                    (:15)
    Client/MParty.h                           (:22)
    Client/MPlayer.h                          (:15)
    Client/MTopView.h                         (:64)
    Client/Packet/QuestStatusInfo.h           (fixed uppercase <VECTOR>)
    Client/ServerInfoFileParser.h             (:4)
    VS_UI/src/header/VS_UI.h                  (:14)
    VS_UI/src/header/VS_UI_ExtraDialog.h      (:12)
    VS_UI/src/header/VS_UI_Game.h             (:21)
    VS_UI/src/header/VS_UI_GameCommon2.h      (:28)
    VS_UI/src/header/VS_UI_GameSlayer.h       (:14)

`QuestStatusInfo.h` had `#include <VECTOR>` uppercase — works on
Windows NTFS (case-insensitive) but not Linux ext4 and case-
sensitive libstdc++. Normalized to `<vector>` same edit.

Post-sweep verification: `grep -ralE 'std::vector<' Client VS_UI
--include='*.h' | xargs -I f sh -c 'grep -aq "^[[:space:]]*#include[[:space:]]*<vector>" "f" || echo MISSING: f'`
returns zero hits.

**Verification:** pending rebuild.

### Bug II — `Client/Client_PCH.h` missing every STL container include

**Symptom:** after Bug HH cleared VS_UI, the next rebuild surfaced
`PCConfigTable.cpp:191` with five errors in a row:

    error: 'vector' is not a member of 'std'
    error: expected primary-expression before '*' token
    error: expected primary-expression before '>' token
    error: 'playerConfigs' was not declared in this scope;
           did you mean 'PlayerConfig'?
    error: 'stable_sort' is not a member of 'std'

**Cause:** `Client/Client_PCH.h` (the precompiled header pulled in
by every `.cpp` under `Client/`) only included C headers
(`<stdio.h>` etc.) plus `<iostream>`. No STL container headers. On
MSVC6 something in the precompiled header chain — likely Windows
SDK or `iostream` itself — leaked `std::vector` / `std::list` /
`std::algorithm` into every TU. libstdc++ does not.

This is the same root cause as Bugs DD and HH but at TU scope: the
port fixes those for individual headers, but any `.cpp` that uses
`std::vector` directly (without a header that includes `<vector>`)
still breaks. Previous rounds were whack-a-mole per `.cpp`.

Peer file `VS_UI/Client_PCH.h` already has the full STL set
(`<vector>`, `<map>`, `<list>`, `<deque>`, `<bitset>`,
`<algorithm>`, `<iostream>`, `<fstream>`, `<cstdio>`, `<cstdarg>`).
The Client PCH had been left in MSVC6-era state.

**Fix:** `Client/Client_PCH.h` — added five STL headers alongside
existing `<iostream>`:

    #include <vector>
    #include <algorithm>
    #include <list>
    #include <map>
    #include <string>

Eliminates the entire bug class at `Client/` TU scope. Any future
`.cpp` under `Client/` using standard containers or algorithms
compiles on the first round.

**Verification:** pending rebuild.

### Bug JJ — `std::auto_ptr` removed in C++17

**Symptom:**

    GCMonsterKillQuestInfoHandler.cpp:30:22:
      error: 'auto_ptr' is not a member of 'std'
    GCMonsterKillQuestInfoHandler.cpp:30:64:
      error: expected primary-expression before '>' token
    GCMonsterKillQuestInfoHandler.cpp:30:66:
      error: 'pInfo' was not declared in this scope

Line 30 was:
    std::auto_ptr<GCMonsterKillQuestInfo::QuestInfo> pInfo( pPacket->popQuestInfo() );

**Cause:** `std::auto_ptr` was deprecated in C++11 and **removed**
in C++17. The port's CMake sets a C++17 (or newer) standard, so
GCC 11 has no `auto_ptr` at all. Tree-wide grep confirms this was
the only live `auto_ptr` use in `Client/` or `VS_UI/`.

**Fix:** replaced with `std::unique_ptr` — same semantics here
since the `auto_ptr` was never copied; it only owned the
pop-returned pointer until end-of-loop-iteration. `unique_ptr` has
the same RAII behavior without the dangerous copy semantics that
got `auto_ptr` removed.

Also added `<memory>` to `Client/Client_PCH.h` (with Bug II's STL
batch) so `std::unique_ptr` is always reachable across the client
tree without having to audit includes.

**Verification:** pending rebuild.

### Bug KK — `Profiler.cpp` stores `ios::app` in a `DWORD`

**Symptom:**

    Profiler.cpp:231:34: error: invalid conversion from
      'DWORD' {aka 'unsigned int'} to 'std::ios_base::openmode'
      [-fpermissive]

Line 229 declared:

    DWORD flag = (bAppend? ios::app : 0);
    ofstream file(pFilename, flag);

**Cause:** `std::ios_base::openmode` is a distinct bitmask type in
libstdc++, not interchangeable with `int` / `unsigned int` / `DWORD`.
MSVC6's STL defined it as a plain int-compat type so the
implicit conversion worked; libstdc++'s stricter type separation
rejects it. `-fpermissive` would downgrade it to a warning but the
port's CMake treats it as hard error.

**Fix:** use the proper type:

    std::ios_base::openmode flag = (bAppend
        ? std::ios::app
        : std::ios::openmode(0));
    ofstream file(pFilename, flag);

Grep sweep: Profiler.cpp is the only site in `Client/` or `VS_UI/`
that stores an openmode flag in an intermediate `DWORD`. Other
`ofstream(name, ios::app)` sites pass the flag directly, which is
already well-typed.

**Verification:** pending rebuild.

### Bug LL — `COGGSTREAM.cpp` still includes stale `DXLib/` path

**Symptom:**

    Client/COGGSTREAM.cpp:2:10: fatal error:
      DXLib/CSDLAudio.h: No such file or directory
        2 | #include "DXLib/CSDLAudio.h"
          |          ^~~~~~~~~~~~~~~~~~~

**Cause:** Phase 3 of the port moved `Client/DXLib/` → `Client/Platform/`
to reflect the SDL-backed replacement for the old DirectX wrappers.
Most call sites were updated at the time, but `COGGSTREAM.cpp` slipped
through because its include is the *only* DXLib reference in the file,
and the TU wasn't in the default build path used during Phase 3
verification. The binary-regeneration chain through Bug W finally
dragged it into the build graph.

**Fix:** one-character path change:

    -#include "DXLib/CSDLAudio.h"
    +#include "Platform/CSDLAudio.h"

Verified `Client/Platform/CSDLAudio.h` exists.

**Preemptive sweep:** `grep -alRn 'include\s*"DXLib/'` across
`Client/` and `VS_UI/` returned zero additional hits, so Bug LL is the
only surviving stale-DXLib include in the tree.

**Verification:** pending rebuild.

### Bug MM — `md5.cpp` uses `||` instead of `|` on openmode flags

**Symptom:**

    md5.cpp:138:36: error: no matching function for call to
      'std::basic_fstream<char>::open(char [260], bool)'
        138 |     Sfile.open(fname, ios::in || ios::binary);
            |                                ^~

**Cause:** classic logical-OR-vs-bitwise-OR typo on a bitmask-flag
argument. `ios::in || ios::binary` evaluates to `bool(true)` →
integer 1 → then fails the openmode implicit conversion (same type
family Bug KK hit). MSVC6 used to accept it because its STL openmode
was an `int` typedef, so the `bool → int → openmode` chain silently
resolved; libstdc++'s stricter typing catches it.

**Fix:** bitwise OR:

    -Sfile.open(fname,ios::in||ios::binary);
    +Sfile.open(fname, ios::in | ios::binary);

**Preemptive sweep:** `grep -alRn 'ios::[a-z_]*\s*||\s*ios::'`
tree-wide returned zero additional hits, so md5.cpp is the only
surviving site of this pattern.

**Verification:** pending rebuild.

### Bug NN — unconditional `-liconv` link fails on glibc Linux

**Symptom:**

    /usr/bin/ld: cannot find -liconv: No such file or directory
    collect2: error: ld returned 1 exit status

This was the *only* remaining error after the DD → MM compile-time
fixes; all 35 TUs produced `.o` files cleanly, and the failure was at
the final link step producing `bin/DarkEden`.

**Cause:** root `CMakeLists.txt:664` had:

    # Link iconv library (not needed for Emscripten -
    #   has stub implementations)
    if(NOT EMSCRIPTEN)
        target_link_libraries(DarkEden PRIVATE iconv)
    endif()

This unconditional link assumes a standalone `libiconv.so` always
exists on non-Emscripten platforms. That's true on macOS/BSD/MinGW,
but false on glibc-based Linux: iconv's symbols (`iconv_open`,
`iconv`, `iconv_close`) are built directly into `libc.so.6`, and
there is no separate `libiconv.so` for the linker to resolve.

CMake's `FindIconv` module already detects this correctly — the
build tree's `CMakeOutput.log:450` shows
`Performing C SOURCE FILE Test Iconv_IS_BUILT_IN succeeded` and
proves the test program linked with **no** `-liconv` flag. But our
CMakeLists never consulted that result; it just hard-coded the `-l`
flag.

**Fix:** call `find_package(Iconv)` and respect its result:

    if(NOT EMSCRIPTEN)
        find_package(Iconv)
        if(Iconv_FOUND AND NOT Iconv_IS_BUILT_IN)
            target_link_libraries(DarkEden PRIVATE Iconv::Iconv)
        endif()
    endif()

This is the canonical CMake 3.11+ pattern. On glibc Linux,
`Iconv_IS_BUILT_IN` is TRUE and the `target_link_libraries` call is
skipped entirely. On macOS/BSD/MinGW where a standalone libiconv is
required, `Iconv::Iconv` is the imported target that resolves to
the correct `-liconv` flag plus any needed include paths.

**Verification:** pending rebuild. This was the last wall between the
DD–MM clean compile and binary regeneration; once the link succeeds,
Bug W's WrapText fix finally gets into the running executable.

### Bug OO — six uppercase-`.CPP` files dropped by case-sensitive CMake glob

**Symptom (post-Bug-NN rebuild):** compile stage 100% clean, link
stage floods with undefined references in two clusters:

    mp3.cpp.o: undefined reference to 'Bit_Reserve::hgetbits(int)'
    mp3.cpp.o: undefined reference to 'Bit_Reserve::hputbuf(unsigned int)'
    mp3.cpp.o: undefined reference to 'Bit_Reserve::~Bit_Reserve()'
    subdecoder.cpp.o: undefined reference to 'Bit_Reserve::*'     ×6
    VS_UI_Title.cpp.o: undefined reference to
      'C_VS_UI_NEWCHAR::_ShowCharacterSlayer(int, S_SLOT*, int, int, int)'
    ... (and _ShowCharacter{Vampire, Ousters, ACSlayer, ACVampire, ACOusters})

**Cause:** MSVC6 preserved DOS 8.3 conventions; six source files in
the tree have uppercase `.CPP` extensions:

    Client/BIT_RES.CPP
    Client/MAttachZoneAroundEffectGenerator.CPP
    Client/MBloodyBreakerEffectGenerator.CPP
    Client/MTimeItemManager.CPP
    Client/MWarManager.CPP
    VS_UI/src/VS_UI_TITLE_SHOWCHAR.CPP

The root `CMakeLists.txt` collects Client sources via
`file(GLOB CLIENT_MAIN_SOURCES CONFIGURE_DEPENDS Client/*.cpp)`.
Phase 14B converted VS_UI to an explicit list built from
`find VS_UI -name "*.cpp" | sort`. Both patterns are case-sensitive
on Linux (Windows hid this for years because NTFS + MSVC both treat
`*.cpp` as matching `*.CPP`).

Result: the six uppercase files were never compiled. Only two of
them are referenced by live TUs (`BIT_RES.CPP` via mp3/subdecoder,
`VS_UI_TITLE_SHOWCHAR.CPP` via VS_UI_Title), so they're what tripped
the linker; the other four are latent.

**Fix:** list them explicitly in CMake (consistent with Phase 14B's
philosophy, no filesystem renames needed):

*Root `CMakeLists.txt`, `list(APPEND CLIENT_MAIN_SOURCES ...)`:* add
the five `Client/*.CPP` files.

*Same file, `VS_UI_SRC_SOURCES` list:* add
`VS_UI/src/VS_UI_TITLE_SHOWCHAR.CPP` next to `VS_UI_Title.cpp`.

Both sites got a `Bug OO (2026-04-20)` comment explaining the
case-sensitivity trap for future maintainers.

**Future cleanup:** Phase 14B's regeneration comment still says
`find VS_UI -name "*.cpp" | sort`, which will miss these files next
time someone regenerates the list. Either rename all `.CPP` →
`.cpp` tree-wide as a follow-up hygiene commit, or update the
regeneration command to `find VS_UI -iname "*.cpp" | sort`. The
latter is one-char cheaper; the former restores the "all `.cpp` in
the tree are compiled" invariant that a case-sensitive filesystem
now actually enforces.

**Verification:** pending rebuild. If this clears, every TU compiles
and every symbol resolves — `bin/DarkEden` should finally be
regenerated with the Bug W WrapText fix live in the binary.

### Bug GG — `BasicException` takes `char*` for string-literal args

**Symptom:** `SimpleDataList.h:117` (and any `_Error` call site):

    SimpleDataList.h:117: ISO C++ forbids converting a string
      constant to 'char*' [-Wwrite-strings]
      CheckMemAlloc(p_temp);
      ↓ expands to
      g_BasicException(MEM_ALLOC, NULL, __FILE__, __LINE__);

where `__FILE__` (`const char*`) is passed to the `char* file`
parameter.

**Cause:** `basic/BasicException.h` declared
`g_BasicException(EXCEPTION_CODE, char*, char*, unsigned long)`. The
two `char*` args have always received string literals (`__FILE__`
and error messages), which are `const char*` in C++11+. MSVC6
implicitly allowed the conversion; GCC 11 treats it as a hard
error under its C++11 profile (not just a warning).

**Fix:** changed both `char*` parameters to `const char*` in the
declaration (`basic/BasicException.h:30`) and the definition
(`basic/BasicException.cpp:11`). Function body didn't need any
changes — it only reads the strings.

**Verification:** pending rebuild.

### Bug PP — server binaries require `-f <conf>` explicitly

**Date:** 2026-04-20 (login-smoke phase 1)

**Symptom:** `start_servers.sh` reports all three as started, but
`Listening ports: (none of 9977/9998/9999 bound)`. Tailing the logs:

```
=== sharedserver.log ===
Usage : sharedserver -f È¯°æÆÄÀÏ
=== loginserver.log ===
Usage : loginserver -f È¯°æÆÄÀÏ [-p port]
=== gameserver.log ===
>>> STARTING GAME SERVER...
>>> RANDOMIZATION INITIALIZATION SUCCESS...
```

All three print their usage banner (or, in gameserver's case, run
through `srand()` then silently `exit(1)`) because each `main()`
requires `-f <config-path>` on the command line.

**Cause:** `start_servers.sh` invoked the binaries bare. From
`src/server/sharedserver/main.cpp:38`,
`src/server/loginserver/main.cpp:41`, and
`src/server/gameserver/main.cpp:90`, each main validates
`argc < 3` → print Usage → exit. There is no fallback to a default
conf path.

**Fix:** pass `-f /mnt/c/newdk/dkrixserver/conf/<name>.conf`
explicitly in the launcher. Absolute path keeps the binary's cwd
choice from mattering for config lookup.

**Verification:** pending rerun of `bash /mnt/c/newdk/start_servers.sh start`.

## Non-bug configuration gaps closed

### `odk-mysql` hostname in seeded `WorldDBInfo`

The shipped `DARKEDEN.sql` seeds the `WorldDBInfo` cross-world DB table
with hostname `odk-mysql` (from `docker/docker-compose.yml` service
name). Works inside the Docker bridge network, doesn't resolve anywhere
else. Sharedserver's DatabaseManager reads this table and uses whatever
hostname it sees to open a second libmysqlclient connection.

Fixed with `/etc/hosts`:

```
127.0.0.1  odk-mysql
```

Chose this over `UPDATE WorldDBInfo SET HostName='127.0.0.1'...` so
that the seeded data stays byte-identical to the shipped dump.
Reversible, one line.

### CRLF on DrvFs

Covered under Bug T above. The fix is a source change, not a conf
rewrite — because the files themselves are fine on Windows, and future
maintainers editing them from Windows will re-introduce CRLFs
indefinitely. Patching the reader is less brittle than `dos2unix` as a
repeated step.

## Findings NOT fixed (for the record)

### ~~Retail `.smp` format != port's `.smp` parser~~ — RETRACTED

**Retraction (Bug U follow-up, 2026-04-20):** this finding was wrong.
The port and the retail pack use **identical** `.smp` formats. All
164 `.smp` files (142 port-shipped + 22 retail) parse cleanly with
the fixed `zone_parser`.

The "divergence" was entirely a bug in `dkrix/tools/viewers/zone_parser/
main.cpp`, not a data issue:

1. The parser skipped a bogus 8-byte "Tile FP + ImageObject FP" pair
   between the description MString and the width/height fields. No
   such pair exists — see `dkrixserver/src/server/gameserver/
   Zone.cpp:866-877` for the ground-truth reader that the real server
   uses to load these files.
2. The parser assumed 4 bytes per sector. The real format is **1 byte
   per tile** (flag bits: `0x01` BLOCK_BURROW, `0x02` BLOCK_WALK,
   `0x04` BLOCK_FLY, `0x80` PORTAL) with a variable-length portal
   payload when `(flag & 0x80)` — see `Zone.cpp:943-1100` for the
   full per-portal-type switch.

Combined symptom: almost every `.smp` failed with `Failed to parse
zone file`. The two that didn't (`adam_c.smp`, `adam_e.smp`) parsed
only because their bytes at the shifted offset happened not to
overrun EOF — and printed fantasy dimensions (`adam_c: 254×2`
instead of the real `256×256`), which tricked the initial smoke test
into believing the port's parser "worked" on port files and "broke"
on retail files. It was broken on both; retail files just failed
louder.

Fix shipped in commit `7834016 fix(zone_parser): correct .smp format
— remove bogus 8B skip, 1B/tile`. Documented here as Bug U.

**Impact:** retail maps are drop-in on the port for smoke-test
purposes. No wire-format conversion needed.

### Loginserver "Loading GameWorldInfoManager" idle loop

While sitting with zero connected clients, loginserver's log shows
`Loading GameWorldInfoManager... / GameWorldInfo(WorldID:1,...) / Size: 0 /
End GameWorldInfoManager Load / addGameServerGroupInfo: 1, 0 : Server1`
repeating on a short interval. No errors, no CPU burn visible, but it's
a polling loop where a cache or an edge-triggered event would do. Not
smoke-test-blocking; flagging for future cleanup.

### Client login packet never sent

`nc` proved TCP reachability and got a clean `ConnectException`
unwind, which is the protocol boundary's *first* behavior. We didn't
encode and send a real login request packet, so we have no evidence
for or against:

- Client-side handshake byte order on the wire.
- Server's first-read validation (length prefix, version byte,
  auth method negotiation).
- Whether a login against a fresh `USERINFO` account actually
  succeeds.

The next probe for this would be either (a) build a small C++ or
Python client that sends a real login request (requires reading the
client's login packet class — probably under
`dkrix/Client/LoginManager.cpp` or similar), or (b) run `DarkEden` with
a display and drive it manually. Either is post-smoke-test work.

## Recommended next steps (priority ordered)

1. **Commit the source fixes.** Bugs Q, R, S, T are in the tree but not
   in any commit. Bundle them as a single "Phase 13C: runtime-smoke-test
   fixes" commit so they're reviewable and revertable as a unit.
   `server_build_fix.sh` can stay as an applicator but the real changes
   should be in the tree.
2. ~~Investigate the `.smp` format divergence.~~ **Done** — see Bug U
   above. There was no divergence; `zone_parser` had two bugs that
   made retail `.smp` files look unreadable. Fixed in commit
   `7834016`. All 164 `.smp` files (142 port + 22 retail) now parse
   cleanly.
3. **Re-apply the Phase 8B env-var templates.** The smoke test ended
   with DB passwords sitting in `conf/*.conf`. Copy the `.template`
   versions back over and put credentials in `~/.dkrix-env`.
4. ~~Apply the same `catch (Error&) → catch (Throwable&)` fix to the
   two submains~~ **Done** — commit `ebd1af0`. Both `billing/main.cpp`
   and `mofus/main.cpp` now catch `Throwable&` and exit non-zero on
   config-load failure with a diagnostic. Neither is currently wired
   into CMake, so this is defensive hardening for whoever reintroduces
   them; no runtime effect today. Trailing-NUL tail-corruption cleaned
   up on both files while in the area.
5. **If you plan to run this on native ext4,** Bug Q (Endian.h shadow)
   and Bug T (CRLF) both stop being concerns. Worth moving the tree
   off DrvFs next time — also a ~3-5× build-time improvement for
   many-small-files compiles.
6. **For a headless client probe,** the lightest lift is writing a
   ~80-line C++ that `connect()`s to `127.0.0.1:9999`, sends the
   client's first login packet, and prints the server's response. That
   gives you a regression test for wire-format changes without needing
   a display.
7. **Client+retail-data boot-to-login smoke test** — written up as
   `STEP3_CLIENT.md` with `client_smoke.sh` as the automated
   precheck + launcher. Scope: WSLg window, login UI renders, no
   login packet sent (crossing into Phase 17 packet-divergence
   territory is out of scope). Unblocks doing packet work against a
   live, asset-loaded client once Enrico runs it.

## Companion artifacts

- `STEP1_MYSQL.md` — MySQL/MariaDB install + schema load runbook.
- `STEP2_SERVER.md` — server build + config + launch runbook.
- `STEP2_GREEN_SNAPSHOT.md` — captured "this is what a clean green
  launch looks like" for future regression diffs.
- `STEP3_CLIENT.md` — client + retail-data boot-to-login runbook.
- `server_build_fix.sh` — applies Bugs Q and R idempotently.
- `client_smoke.sh` — launch-path prechecks + `DarkEden` launcher
  with the `Futec(IP:Port)` command-line format.
