#!/usr/bin/env bash
#
# client_build_fix.sh — fix two GCC-11 incompatibilities in the client tree
# that block Ubuntu 22.04 builds.
#
#   Bug A: bare `std::ifstream;` / `std::ofstream;` statements at global
#          scope in 5 SpriteLib headers. MSVC6 tolerated; GCC 11 rejects
#          with "declaration does not declare anything [-fpermissive]".
#          Fix: delete the lines. They're dead — the STL types are
#          pulled in transitively via other #include directives.
#
#   Bug B: `#define max(a,b) ...` / `#define min(a,b) ...` in
#          basic/Platform.h breaks every subsequent STL header include
#          because `<fstream>` / `<vector>` etc. use `max` and `min` as
#          template method names. The preprocessor rewrites them.
#          Fix: convert the macros to inline function templates. Same
#          call-site surface (bare `max(a,b)` / `min(a,b)` work), but
#          they don't eat STL tokens.
#
#          BUT: Platform.h has its own outer `extern "C" { ... }` block
#          wrapping lines ~226..1915, and the min/max macros live inside
#          it at L1732. Templates can't have C linkage, so the conversion
#          must also close/reopen extern "C" around the templates (same
#          trick Platform.h already uses around <mutex> at L213..226).
#
#   Bug C: `dirname` used on Linux without `#include <libgen.h>`.
#          PlatformSDL.cpp includes libgen.h for Emscripten and macOS
#          but not Linux. dirname() is a POSIX function declared in
#          libgen.h; without the include, GCC emits "not declared in
#          this scope".
#          Fix: add the include to the PLATFORM_LINUX conditional.
#
#   Bug D: `extern "C" { #include "../basic/Platform.h" ... }` in
#          SpriteLibBackend.h wraps C++ STL headers with C linkage.
#          Platform.h pulls in <mutex>/<tuple>/<utility>, and the
#          outer extern "C" block makes them ill-formed (templates
#          can't have C linkage).
#          Fix: move the Platform.h and SDL.h includes OUT of the
#          extern "C" block. Platform.h manages its own linkage.
#
#   Bug G: CPositionList.h:107 uses `POSITION_LIST::const_iterator`
#          as a return type inside a class template. POSITION_LIST is
#          a dependent name (typedef into a template class), so GCC
#          requires `typename` per C++ two-phase lookup. Other lines
#          in the same file (179/224/271/336) already have `typename`;
#          line 107 was missed.
#          Fix: prepend `typename` to the declaration.
#
#   Bug H: MSVC6-era code uses a member named the same as its type,
#          e.g. UserInformation.h:47 `Race Race[3];`. GCC rejects this
#          with "declaration changes meaning of 'Race' [-fpermissive]".
#          Likely multiple similar issues across the tree (standard
#          MSVC6 idiom). The compiler itself suggests -fpermissive.
#          Fix: inject -fpermissive into CMAKE_CXX_FLAGS via the client
#          CMakeLists.txt. Same trade-off every MSVC6→GCC port makes.
#
#   Bug I: MSVC6's STL had generous transitive includes — pulling in
#          <list> also gave you <vector>, <map> gave you <string>, etc.
#          libstdc++ on GCC 11 does not. Headers AND .cpp files that use
#          std::vector without `#include <vector>` (MSkillManager.h:451,
#          PCConfigTable.cpp:191, etc.) fail with "vector in namespace
#          std does not name a template type" / "not a member of 'std'".
#          Fix: bulk-add the missing STL include to each header / .cpp
#          that uses the corresponding std:: type directly. Redundant
#          includes are no-ops (header guards).
#
#   Bug P: 7 Client/VS_UI source files have uppercase `.CPP` extension.
#          CMake's `file(GLOB ... *.cpp)` is case-sensitive on Linux (even
#          when the underlying DrvFs is case-insensitive — glibc's glob()
#          compares literal bytes), so uppercase-extension files get skipped
#          by the glob and never compile. Two of them (BIT_RES.CPP,
#          VS_UI_TITLE_SHOWCHAR.CPP) contain symbols referenced by
#          compiled code (Bit_Reserve::hgetbits, C_VS_UI_NEWCHAR::_ShowCharacter*),
#          causing undefined-reference errors at the final link.
#          Fix: explicitly add the 5 missing Client/*.CPP and 1 missing
#          VS_UI .CPP to the CMake source lists using lowercase extension
#          (FS case-insensitivity handles the actual open; matches existing
#          convention for COGGSTREAM.cpp and MTimeItemManager.cpp, which
#          were added the same way).
#
#   Bug O: Link step fails with `ld: cannot find -liconv`. On Ubuntu/glibc,
#          iconv() is built into libc — there's no separate libiconv.so.
#          The hardcoded `target_link_libraries(... iconv)` only works on
#          macOS / musl / Windows-style systems where iconv ships as a
#          standalone library. CMake's find_package(Iconv) handles this
#          portably: it sets Iconv_IS_BUILT_IN=TRUE on glibc (no link
#          needed) and Iconv_LIBRARIES to the actual library elsewhere.
#          Fix: replace the hardcoded link with a find_package(Iconv) +
#          conditional link-only-when-not-built-in block.
#
#   Bug N: md5.cpp:138 has `Sfile.open(fname, ios::in || ios::binary);`
#          — logical-OR `||` instead of bitwise-OR `|`. Genuine bug; MSVC6
#          accepted it because openmode was a plain int so the `bool` true
#          from `||` coerced back. GCC 11's std::ios_base::openmode is a
#          strongly-typed bitmask class and rejects the bool arg.
#          Full-tree survey confirmed this is the only site.
#          Fix: s/||/|/ on that one line.
#
#   Bug M: COGGSTREAM.CPP:2 references `"DXLib/CSDLAudio.h"` — stale path
#          from before Phase 3 C4 (which moved Client/DXLib/ → Client/Platform/).
#          This file was almost certainly missed by Phase 3's sweep because
#          its extension is uppercase `.CPP` and the sweep globbed `*.cpp`.
#          The correct path today is `"Platform/CSDLAudio.h"`. Full-tree
#          survey confirmed this is the only stale DXLib/ include in Client/.
#          Fix: rewrite the include.
#
#   Bug L: GCMonsterKillQuestInfoHandler.cpp:30 uses `std::auto_ptr`, which
#          was deprecated in C++11 and removed in C++17. Project's C++
#          standard is C++17, so it's gone. Full-tree survey confirmed
#          this is the only auto_ptr site.
#          Fix: s/std::auto_ptr/std::unique_ptr/ on that single line.
#          Same ownership semantics for this construction-from-raw-pointer
#          + RAII-destruction usage; unique_ptr is non-copyable but the
#          local variable is never copied.
#
#   Bug K: QuestStatusInfo.h:11 does `#include <VECTOR>` — uppercase.
#          MSVC6 on NTFS (case-insensitive) was fine. Linux system headers
#          live on a case-sensitive filesystem, and the real STL header is
#          `<vector>`. Full-tree survey found this is the only site with
#          an uppercase STL include.
#          Fix: rewrite the single offending line to lowercase.
#
#   Bug J: ValueList.h (a template class) uses `std::list<T>::const_iterator`
#          without `typename` on lines 34, 35, 118. Dependent name inside
#          a template → GCC requires `typename`. Bug G's regex survey only
#          caught uppercase-named typedefs (POSITION_LIST/PARTINDEX_LIST),
#          so bare `std::<container><T>::iterator` dependent forms slipped
#          through. A full repo-wide scan confirms ValueList.h is the only
#          header with this specific pattern (all other std::<container>::
#          iterator hits in .h files use concrete types, not template
#          parameters — no typename needed there).
#          Fix: prepend `typename` to the three ValueList.h sites.
#
# Run from: anywhere (autodetects /mnt/c/newdk/ or $HOME/projects/newdk/).
# Idempotent: safe to re-run.

set -euo pipefail

if [ -d "/mnt/c/newdk/dkrix" ]; then
    REPO_ROOT="/mnt/c/newdk"
elif [ -d "$HOME/projects/newdk/dkrix" ]; then
    REPO_ROOT="$HOME/projects/newdk"
elif [ -d "./dkrix" ]; then
    REPO_ROOT="$(pwd)"
else
    echo "ERROR: can't find repo root." >&2
    exit 1
fi
echo ">>> Repo root: $REPO_ROOT"

#================================================================
# Bug A: nuke bare std::ifstream; / std::ofstream; statements
#================================================================
echo ""
echo ">>> Bug A: removing bare std::ifstream / std::ofstream statements..."

for f in \
    "$REPO_ROOT/dkrix/Client/SpriteLib/CFilter.h" \
    "$REPO_ROOT/dkrix/Client/SpriteLib/CSpriteDef.h" \
    "$REPO_ROOT/dkrix/Client/SpriteLib/CAlphaSprite.h" \
    "$REPO_ROOT/dkrix/Client/SpriteLib/CSprite.h" \
    "$REPO_ROOT/dkrix/Client/SpriteLib/CShadowSpritePack.h"
do
    if [ ! -f "$f" ]; then
        echo "    SKIP: $f (not found)"
        continue
    fi
    # Backup once
    [ -f "$f.bak" ] || cp "$f" "$f.bak"
    # Delete lines matching exactly std::(i|o)fstream; with optional surrounding whitespace
    sed -i -E '/^[[:space:]]*std::(i|o)fstream;[[:space:]]*$/d' "$f"
    before=$(grep -cE '^[[:space:]]*std::(i|o)fstream;[[:space:]]*$' "$f.bak" || true)
    after=$(grep -cE '^[[:space:]]*std::(i|o)fstream;[[:space:]]*$' "$f" || true)
    echo "    $f: removed $((before - after)) line(s)"
done

#================================================================
# Bug B: convert min/max macros to inline templates
#================================================================
echo ""
echo ">>> Bug B: converting min/max macros in basic/Platform.h..."

PLATFORM_H="$REPO_ROOT/dkrix/basic/Platform.h"
if [ ! -f "$PLATFORM_H" ]; then
    echo "ERROR: $PLATFORM_H not found." >&2
    exit 1
fi

[ -f "$PLATFORM_H.bak" ] || cp "$PLATFORM_H" "$PLATFORM_H.bak"

# Use python for safe in-place multi-line replacement
python3 - "$PLATFORM_H" <<'PY_EOF'
import sys, pathlib
p = pathlib.Path(sys.argv[1])
s = p.read_text()
# The old Windows-style macros / our first-pass templates collide with std::min/std::max
# at any TU that has `using namespace std;` (CTypePack.h line 12, etc.).
# Drop our own definitions entirely, pull <algorithm> in, and hoist std::min/max into
# the global namespace so MSVC-style bare `min(a,b)` call-sites keep working.
# Also close/reopen Platform.h's outer extern "C" around the using-declarations,
# same trick Platform.h already uses around <mutex> at L213..226.
import re

new_block = '''/* Hoist std::min / std::max into global scope so MSVC-style bare
 * `min(a,b)` / `max(a,b)` call-sites keep working. Plus SFINAE-gated
 * heterogeneous-args overloads so mixed-type calls (e.g., min(ushort, int))
 * that the old #define accepted via ternary promotion still compile. Gate
 * is `types differ` so same-type calls resolve to std::min unambiguously
 * even in TUs that do `using namespace std;` (CTypePack.h line 12). */
#ifdef __cplusplus
}  /* close Platform.h\'s outer extern "C" - STL can\'t have C linkage */
#include <algorithm>
#include <type_traits>
#ifndef max
using std::max;
template<typename A, typename B,
         typename = typename std::enable_if<!std::is_same<A,B>::value>::type>
inline auto max(A a, B b) -> typename std::common_type<A,B>::type {
    return (a > b) ? a : b;
}
#endif
#ifndef min
using std::min;
template<typename A, typename B,
         typename = typename std::enable_if<!std::is_same<A,B>::value>::type>
inline auto min(A a, B b) -> typename std::common_type<A,B>::type {
    return (a < b) ? a : b;
}
#endif
extern "C" {  /* reopen */
#endif'''

hybrid_marker = 'typename std::common_type<A,B>::type'
if hybrid_marker in s and 'using std::min' in s and 'using std::max' in s:
    print(f"    hybrid std::min/max + heterogeneous overload already in {p.name}")
else:
    # Catches: (a) original macros, (b) v1 bare templates, (c) v2 extern-C-escaped templates,
    # or (d) v3 plain `using std::min/max` block (no hybrid). Greedy enough to cover all.
    # Strategy: grab from the optional extern-C-close prefix through the last #endif before
    # the __int64 line that follows all four historical forms.
    patterns = [
        # v3: using-declarations form (previous script run's output)
        re.compile(
            r'/\* Hoist std::min / std::max into global scope[\s\S]*?'
            r'using std::max;\s*\n'
            r'#endif\s*\n'
            r'#ifndef min\s*\n'
            r'using std::min;\s*\n'
            r'#endif\s*\n'
            r'extern "C" \{[^\n]*\n'
            r'#endif'
        ),
        # v0/v1/v2: macros/bare templates/extern-C-escaped templates
        re.compile(
            r'(?:#ifdef __cplusplus\s*\n\}[^\n]*\n#endif\s*\n)?'
            r'#ifndef max\s*\n'
            r'(?:#define max\(a, b\) \(\(\(a\) > \(b\)\) \? \(a\) : \(b\)\)'
            r'|template<typename T> inline T max\(T a, T b\) \{ return \(a > b\) \? a : b; \})'
            r'\s*\n#endif\s*\n'
            r'#ifndef min\s*\n'
            r'(?:#define min\(a, b\) \(\(\(a\) < \(b\)\) \? \(a\) : \(b\)\)'
            r'|template<typename T> inline T min\(T a, T b\) \{ return \(a < b\) \? a : b; \})'
            r'\s*\n#endif'
            r'(?:\s*\n#ifdef __cplusplus\s*\nextern "C"[^\n]*\n#endif)?'
        ),
    ]
    for pat in patterns:
        if pat.search(s):
            s_new, n = pat.subn(new_block, s, count=1)
            p.write_text(s_new)
            print(f"    installed hybrid std::min/max + heterogeneous overload in {p.name} ({n} sub)")
            break
    else:
        print(f"    WARN: min/max block not matched in {p.name} - manual inspection needed")
PY_EOF

#================================================================
# Bug C: add #include <libgen.h> for Linux path in PlatformSDL.cpp
#================================================================
echo ""
echo ">>> Bug C: adding <libgen.h> include for Linux in PlatformSDL.cpp..."

PLATSDL="$REPO_ROOT/dkrix/basic/PlatformSDL.cpp"
if [ ! -f "$PLATSDL" ]; then
    echo "ERROR: $PLATSDL not found." >&2
    exit 1
fi
[ -f "$PLATSDL.bak" ] || cp "$PLATSDL" "$PLATSDL.bak"

python3 - "$PLATSDL" <<'PY_EOF'
import sys, pathlib
p = pathlib.Path(sys.argv[1])
s = p.read_text()

# Target block: PLATFORM_LINUX conditional includes (line ~24-27).
# Add <libgen.h> alongside the existing <limits.h>/<stdlib.h> so
# dirname() is declared on Linux builds too.
old = '''#ifdef PLATFORM_LINUX
\t#include <limits.h>
\t#include <stdlib.h>
#endif'''
new = '''#ifdef PLATFORM_LINUX
\t#include <limits.h>
\t#include <stdlib.h>
\t#include <libgen.h>  /* For dirname */
#endif'''

if old in s:
    s = s.replace(old, new)
    p.write_text(s)
    print(f"    added <libgen.h> to PLATFORM_LINUX block in {p.name}")
elif '<libgen.h>' in s.split('#ifdef PLATFORM_LINUX')[1].split('#endif')[0]:
    print(f"    <libgen.h> already in PLATFORM_LINUX block — skipping")
else:
    print(f"    WARN: PLATFORM_LINUX block not matched verbatim; {p.name} may need manual inspection")
PY_EOF

#================================================================
# Bug D: move Platform.h include out of extern "C" in SpriteLibBackend.h
#================================================================
echo ""
echo ">>> Bug D: restructuring extern \"C\" block in SpriteLibBackend.h..."

SLB="$REPO_ROOT/dkrix/Client/SpriteLib/SpriteLibBackend.h"
if [ ! -f "$SLB" ]; then
    echo "ERROR: $SLB not found." >&2
    exit 1
fi
[ -f "$SLB.bak" ] || cp "$SLB" "$SLB.bak"

python3 - "$SLB" <<'PY_EOF'
import sys, pathlib
p = pathlib.Path(sys.argv[1])
s = p.read_text()

old = '''/* Close Platform.h's extern "C" block to manage our own */
#ifdef __cplusplus
extern "C" {
#endif

#include "../basic/Platform.h"

/* ============================================================================
 * Backend Selection
 * ============================================================================ */

/* Define which backend to use */
#ifdef SPRITELIB_BACKEND_SDL
\t#include <SDL.h>
#else
\t/* Windows/DirectDraw backend (original) */
#endif'''

new = '''#include "../basic/Platform.h"

/* ============================================================================
 * Backend Selection
 * ============================================================================ */

/* Define which backend to use */
#ifdef SPRITELIB_BACKEND_SDL
\t#include <SDL.h>
#else
\t/* Windows/DirectDraw backend (original) */
#endif

/* C-linkage block for the C ABI declared below.
 * Platform.h and SDL.h are included BEFORE this block so their C++ STL
 * content doesn't end up with C linkage (which breaks templates). */
#ifdef __cplusplus
extern "C" {
#endif'''

if old in s:
    s = s.replace(old, new)
    p.write_text(s)
    print(f"    moved Platform.h/SDL.h includes out of extern \"C\" block in {p.name}")
else:
    print(f"    WARN: extern \"C\" block not matched verbatim; {p.name} may need manual inspection")
PY_EOF

#================================================================
# Bug G: add `typename` to dependent-type iterator uses in template classes
#================================================================
echo ""
echo ">>> Bug G: adding typename to dependent-type iterators in template classes..."

# Surgical fixes — each entry is (file, old-literal, new-literal).
# Only template classes are affected; non-template classes don't need typename.

CPOSLIST="$REPO_ROOT/dkrix/Client/CPositionList.h"
CPARTMGR="$REPO_ROOT/dkrix/Client/CPartManager.h"
for f in "$CPOSLIST" "$CPARTMGR"; do
    if [ ! -f "$f" ]; then
        echo "ERROR: $f not found." >&2
        exit 1
    fi
    [ -f "$f.bak" ] || cp "$f" "$f.bak"
done

python3 - "$CPOSLIST" "$CPARTMGR" <<'PY_EOF'
import sys, pathlib

# Each item: (path, old, new). Python replace()s are exact matches — whitespace verified.
edits = [
    (sys.argv[1],
     '\t\tPOSITION_LIST::const_iterator\tGetIterator() const\t{ return m_listPosition.begin(); }',
     '\t\ttypename POSITION_LIST::const_iterator\tGetIterator() const\t{ return m_listPosition.begin(); }'),
    (sys.argv[2],
     '\t\ttypedef\tPARTINDEX_LIST::iterator\tPARTINDEX_LIST_ITERATOR;',
     '\t\ttypedef\ttypename PARTINDEX_LIST::iterator\tPARTINDEX_LIST_ITERATOR;'),
    (sys.argv[2],
     '\tPARTINDEX_LIST::const_iterator\tiIndex = m_listLRU.begin();',
     '\ttypename PARTINDEX_LIST::const_iterator\tiIndex = m_listLRU.begin();'),
]

for path, old, new in edits:
    p = pathlib.Path(path)
    s = p.read_text()
    if old in s:
        s = s.replace(old, new)
        p.write_text(s)
        print(f"    added `typename` in {p.name}: {old.strip()[:60]}...")
    elif new in s:
        print(f"    `typename` already present in {p.name}: {new.strip()[:60]}...")
    else:
        print(f"    WARN: pattern not matched verbatim in {p.name}: {old.strip()[:60]}...")
PY_EOF

#================================================================
# Bug H: add -fpermissive to client CMAKE_CXX_FLAGS for MSVC6 idiom tolerance
#================================================================
echo ""
echo ">>> Bug H: injecting -fpermissive into client CMakeLists.txt..."

CLIENT_CMAKE="$REPO_ROOT/dkrix/CMakeLists.txt"
if [ ! -f "$CLIENT_CMAKE" ]; then
    echo "ERROR: $CLIENT_CMAKE not found." >&2
    exit 1
fi
[ -f "$CLIENT_CMAKE.bak" ] || cp "$CLIENT_CMAKE" "$CLIENT_CMAKE.bak"

python3 - "$CLIENT_CMAKE" <<'PY_EOF'
import sys, pathlib
p = pathlib.Path(sys.argv[1])
s = p.read_text()

marker = '# Phase smoke-test: -fpermissive for MSVC6 idioms'
if marker in s:
    print(f"    -fpermissive already injected in {p.name}")
else:
    # Insert after C++ standard block (right after line 19's `CMAKE_CXX_STANDARD_REQUIRED ON`)
    anchor = 'set(CMAKE_CXX_STANDARD_REQUIRED ON)'
    injection = f'''set(CMAKE_CXX_STANDARD_REQUIRED ON)

{marker}
# MSVC6 code uses idioms (member-named-as-its-type, implicit-int,
# unnamed-struct) that GCC rejects by default but downgrades with -fpermissive.
# The compiler itself suggests this flag in its error messages.
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${{CMAKE_CXX_FLAGS}} -fpermissive")
endif()'''
    if anchor in s:
        s = s.replace(anchor, injection, 1)
        p.write_text(s)
        print(f"    injected -fpermissive into {p.name}")
    else:
        print(f"    WARN: anchor not found in {p.name} — manual inspection needed")
PY_EOF

#================================================================
# Bug I: bulk-add missing STL includes (MSVC6 relied on transitive)
#================================================================
echo ""
echo ">>> Bug I: scanning for headers AND .cpp files missing STL includes..."

python3 - "$REPO_ROOT/dkrix" <<'PY_EOF'
import sys, pathlib, re

root = pathlib.Path(sys.argv[1])
search_dirs = [root / 'Client', root / 'basic']

# Map each STL header to a regex detecting direct use of its types
needs = {
    'vector': r'std::vector\b|\bvector<',
    'list':   r'std::list\b|\blist<',
    'map':    r'std::map\b|\bmap<',
    'set':    r'std::set\b|\bset<',
    'string': r'std::string\b',
    'unordered_map': r'std::unordered_map\b',
    'unordered_set': r'std::unordered_set\b',
    # Smart pointers live in <memory>. Matches both the modern forms
    # (unique_ptr / shared_ptr / weak_ptr / make_unique / make_shared)
    # and the C++17-removed std::auto_ptr so the include is added even
    # if Bug L's rewrite hasn't run yet.
    'memory': r'std::(unique_ptr|shared_ptr|weak_ptr|auto_ptr|make_unique|make_shared)\b',
}

inc_last_pat = re.compile(r'^#include\s+[<"][^>"]+[>"]\s*$', re.M)

total_edits = 0
files_touched = 0
# Scan .h AND .cpp — .cpp files hit the same MSVC6-transitive-STL issue
# (e.g. PCConfigTable.cpp:191 uses std::vector without an include).
all_files = []
for d in search_dirs:
    all_files.extend(d.rglob('*.h'))
    all_files.extend(d.rglob('*.cpp'))
for fp in all_files:
    if fp.name.endswith('.bak'): continue
    try: s = fp.read_text(errors='replace')
    except Exception: continue

    to_add = []
    for hdr, pat in needs.items():
        if re.search(pat, s) and f'#include <{hdr}>' not in s:
            to_add.append(hdr)

    if not to_add: continue

    # Find position after the last existing #include — insert new ones right after.
    matches = list(inc_last_pat.finditer(s))
    if not matches: continue  # skip files with no include block
    insert_pos = matches[-1].end()

    addition = '\n' + '\n'.join(f'#include <{h}>' for h in to_add)
    backup = fp.with_suffix(fp.suffix + '.bak')
    if not backup.exists():
        backup.write_text(s)
    s = s[:insert_pos] + addition + s[insert_pos:]
    fp.write_text(s)
    total_edits += len(to_add)
    files_touched += 1

print(f"    added {total_edits} missing STL includes across {files_touched} files")
PY_EOF

#================================================================
# Bug J: add `typename` to dependent STL-iterator uses in template classes
#================================================================
echo ""
echo ">>> Bug J: adding typename to std::list<T>::const_iterator in ValueList.h..."

VALUELIST="$REPO_ROOT/dkrix/Client/Packet/ValueList.h"
if [ ! -f "$VALUELIST" ]; then
    echo "ERROR: $VALUELIST not found." >&2
    exit 1
fi
[ -f "$VALUELIST.bak" ] || cp "$VALUELIST" "$VALUELIST.bak"

python3 - "$VALUELIST" <<'PY_EOF'
import sys, pathlib

p = pathlib.Path(sys.argv[1])
s = p.read_text()

# Whitespace verified via `sed -n ... | cat -A`:
# L34: \tstd::list<T>::const_iterator\t\tBegin() { return m_Values.begin(); }
# L35: \tstd::list<T>::const_iterator\t\tEnd() { return m_Values.end(); }
# L118: \tstd::list<T>::const_iterator itr = m_Values.begin();
edits = [
    ('\tstd::list<T>::const_iterator\t\tBegin() { return m_Values.begin(); }',
     '\ttypename std::list<T>::const_iterator\t\tBegin() { return m_Values.begin(); }'),
    ('\tstd::list<T>::const_iterator\t\tEnd() { return m_Values.end(); }',
     '\ttypename std::list<T>::const_iterator\t\tEnd() { return m_Values.end(); }'),
    ('\tstd::list<T>::const_iterator itr = m_Values.begin();',
     '\ttypename std::list<T>::const_iterator itr = m_Values.begin();'),
]

changes = 0
for old, new in edits:
    if old in s:
        s = s.replace(old, new, 1)
        changes += 1
        print(f"    added `typename`: {old.strip()[:60]}...")
    elif new in s:
        print(f"    `typename` already present: {new.strip()[:60]}...")
    else:
        print(f"    WARN: pattern not matched verbatim: {old.strip()[:60]}...")

if changes:
    p.write_text(s)
PY_EOF

#================================================================
# Bug K: fix case-sensitive uppercase STL include
#================================================================
echo ""
echo ">>> Bug K: lowercasing uppercase STL includes..."

QUESTSTATUS="$REPO_ROOT/dkrix/Client/Packet/QuestStatusInfo.h"
if [ ! -f "$QUESTSTATUS" ]; then
    echo "ERROR: $QUESTSTATUS not found." >&2
    exit 1
fi
[ -f "$QUESTSTATUS.bak" ] || cp "$QUESTSTATUS" "$QUESTSTATUS.bak"

# Defensive: sweep the whole client + basic tree for any uppercase STL include,
# not just the known QuestStatusInfo.h line. Full-tree survey today found only
# one site, but this keeps the script robust against future surprises.
python3 - "$REPO_ROOT/dkrix" <<'PY_EOF'
import sys, pathlib, re
root = pathlib.Path(sys.argv[1])
stl_names = ('VECTOR','LIST','MAP','SET','STRING','ALGORITHM','FSTREAM',
             'IOSTREAM','SSTREAM','MEMORY','DEQUE','QUEUE','STACK',
             'UTILITY','FUNCTIONAL','NUMERIC','ITERATOR')
pat = re.compile(r'^(#include\s+<)(' + '|'.join(stl_names) + r')(>\s*)$', re.M)

total = 0
for sub in ('Client', 'basic'):
    for fp in (root / sub).rglob('*'):
        if fp.suffix not in ('.h', '.cpp'): continue
        if fp.name.endswith('.bak'): continue
        try: s = fp.read_text(errors='replace')
        except Exception: continue
        new = pat.sub(lambda m: m.group(1) + m.group(2).lower() + m.group(3), s)
        if new != s:
            backup = fp.with_suffix(fp.suffix + '.bak')
            if not backup.exists():
                backup.write_text(s)
            fp.write_text(new)
            hits = len(pat.findall(s))
            total += hits
            print(f"    lowercased {hits} STL include(s) in {fp.name}")
print(f"    total: {total} uppercase STL include(s) lowercased")
PY_EOF

#================================================================
# Bug L: replace std::auto_ptr (C++17-removed) with std::unique_ptr
#================================================================
echo ""
echo ">>> Bug L: replacing std::auto_ptr with std::unique_ptr..."

AUTOPTR_FILE="$REPO_ROOT/dkrix/Client/Packet/Gpackets/GCMonsterKillQuestInfoHandler.cpp"
if [ ! -f "$AUTOPTR_FILE" ]; then
    echo "ERROR: $AUTOPTR_FILE not found." >&2
    exit 1
fi
[ -f "$AUTOPTR_FILE.bak" ] || cp "$AUTOPTR_FILE" "$AUTOPTR_FILE.bak"

python3 - "$AUTOPTR_FILE" <<'PY_EOF'
import sys, pathlib
p = pathlib.Path(sys.argv[1])
s = p.read_text()
# Whitespace verified via `sed -n '30p' ... | cat -A`:
# \t\tstd::auto_ptr<GCMonsterKillQuestInfo::QuestInfo> pInfo( pPacket->popQuestInfo() );
old = '\t\tstd::auto_ptr<GCMonsterKillQuestInfo::QuestInfo> pInfo( pPacket->popQuestInfo() );'
new = '\t\tstd::unique_ptr<GCMonsterKillQuestInfo::QuestInfo> pInfo( pPacket->popQuestInfo() );'
if old in s:
    s = s.replace(old, new, 1)
    p.write_text(s)
    print(f"    replaced auto_ptr with unique_ptr in {p.name}")
elif new in s:
    print(f"    unique_ptr already in place in {p.name}")
else:
    print(f"    WARN: pattern not matched verbatim in {p.name}")
PY_EOF

#================================================================
# Bug M: fix stale DXLib/ include path (Phase 3 C4 leftover)
#================================================================
echo ""
echo ">>> Bug M: fixing stale DXLib/ include paths..."

# The file's extension is uppercase (.CPP) on the case-sensitive side.
# Probe both cases so this works regardless of how the FS reports it.
OGG_CPP=""
for cand in \
    "$REPO_ROOT/dkrix/Client/COGGSTREAM.CPP" \
    "$REPO_ROOT/dkrix/Client/COGGSTREAM.cpp"
do
    if [ -f "$cand" ]; then OGG_CPP="$cand"; break; fi
done
if [ -z "$OGG_CPP" ]; then
    echo "ERROR: COGGSTREAM.CPP / .cpp not found." >&2
    exit 1
fi
[ -f "$OGG_CPP.bak" ] || cp "$OGG_CPP" "$OGG_CPP.bak"

python3 - "$OGG_CPP" <<'PY_EOF'
import sys, pathlib
p = pathlib.Path(sys.argv[1])
s = p.read_text()
old = '#include "DXLib/CSDLAudio.h"'
new = '#include "Platform/CSDLAudio.h"'
if old in s:
    s = s.replace(old, new, 1)
    p.write_text(s)
    print(f"    rewrote DXLib/ → Platform/ in {p.name}")
elif new in s:
    print(f"    include already points at Platform/ in {p.name}")
else:
    print(f"    WARN: pattern not matched verbatim in {p.name}")
PY_EOF

#================================================================
# Bug N: fix logical-OR where bitwise-OR was intended in fstream::open flags
#================================================================
echo ""
echo ">>> Bug N: fixing ios::in || ios::binary typo in md5.cpp..."

MD5_CPP="$REPO_ROOT/dkrix/Client/md5.cpp"
if [ ! -f "$MD5_CPP" ]; then
    echo "ERROR: $MD5_CPP not found." >&2
    exit 1
fi
[ -f "$MD5_CPP.bak" ] || cp "$MD5_CPP" "$MD5_CPP.bak"

python3 - "$MD5_CPP" <<'PY_EOF'
import sys, pathlib
p = pathlib.Path(sys.argv[1])
s = p.read_text()
# Whitespace verified via `sed -n '138p' ... | cat -A`:
# \tSfile.open(fname,ios::in||ios::binary);
old = '\tSfile.open(fname,ios::in||ios::binary);'
new = '\tSfile.open(fname,ios::in|ios::binary);'
if old in s:
    s = s.replace(old, new, 1)
    p.write_text(s)
    print(f"    fixed ||→| in {p.name}")
elif new in s:
    print(f"    already fixed in {p.name}")
else:
    print(f"    WARN: pattern not matched verbatim in {p.name}")
PY_EOF

#================================================================
# Bug O: replace hardcoded -liconv with portable find_package(Iconv)
#================================================================
echo ""
echo ">>> Bug O: making iconv link portable via find_package(Iconv)..."

python3 - "$CLIENT_CMAKE" <<'PY_EOF'
import sys, pathlib
p = pathlib.Path(sys.argv[1])
s = p.read_text()
# Whitespace verified via `sed -n '662,665p' ... | cat -A`:
# \t# Link iconv library (not needed for Emscripten - has stub implementations)
# \tif(NOT EMSCRIPTEN)
# \t\ttarget_link_libraries(DarkEden PRIVATE iconv)
# \tendif()
old = '''\t# Link iconv library (not needed for Emscripten - has stub implementations)
\tif(NOT EMSCRIPTEN)
\t\ttarget_link_libraries(DarkEden PRIVATE iconv)
\tendif()'''
new = '''\t# Link iconv library — on glibc (Linux) it's built into libc so no
\t# link is needed; on macOS / musl / Windows it's a separate library.
\t# find_package(Iconv) (CMake >=3.11) sets Iconv_IS_BUILT_IN on glibc.
\tif(NOT EMSCRIPTEN)
\t\tfind_package(Iconv)
\t\tif(Iconv_FOUND AND NOT Iconv_IS_BUILT_IN)
\t\t\ttarget_link_libraries(DarkEden PRIVATE ${Iconv_LIBRARIES})
\t\t\tif(Iconv_INCLUDE_DIRS)
\t\t\t\ttarget_include_directories(DarkEden PRIVATE ${Iconv_INCLUDE_DIRS})
\t\t\tendif()
\t\tendif()
\tendif()'''
if old in s:
    s = s.replace(old, new, 1)
    p.write_text(s)
    print(f"    swapped hardcoded -liconv for find_package(Iconv) in {p.name}")
elif 'find_package(Iconv)' in s:
    print(f"    find_package(Iconv) already in place in {p.name}")
else:
    print(f"    WARN: iconv block not matched verbatim in {p.name}")
PY_EOF

#================================================================
# Bug P: add uppercase-extension (.CPP) source files to CMake lists
#================================================================
echo ""
echo ">>> Bug P: wiring uppercase-extension .CPP files into CMake..."

python3 - "$CLIENT_CMAKE" <<'PY_EOF'
import sys, pathlib
p = pathlib.Path(sys.argv[1])
s = p.read_text()

# Insertion 1: VS_UI_TITLE_SHOWCHAR — add right after VS_UI_Title.cpp line
vsui_old = '\t\tVS_UI/src/VS_UI_Title.cpp\n'
vsui_new = '\t\tVS_UI/src/VS_UI_Title.cpp\n\t\tVS_UI/src/VS_UI_TITLE_SHOWCHAR.cpp\n'
vsui_done = False
if 'VS_UI/src/VS_UI_TITLE_SHOWCHAR.cpp' in s:
    print("    VS_UI_TITLE_SHOWCHAR already wired")
    vsui_done = True
elif vsui_old in s:
    s = s.replace(vsui_old, vsui_new, 1)
    print("    added VS_UI/src/VS_UI_TITLE_SHOWCHAR.cpp to VS_UI explicit list")
    vsui_done = True
else:
    print("    WARN: VS_UI_Title.cpp anchor not matched verbatim")

# Insertion 2: extend the Client list(APPEND CLIENT_MAIN_SOURCES ...) block
# to include the 5 uppercase-extension Client files.
client_old = '''\t# Add additional source files
\tlist(APPEND CLIENT_MAIN_SOURCES
\t\tClient/COGGSTREAM.cpp
\t\t# Time functions extracted from PacketFunction.cpp (which has
\t\t# Windows-specific networking code).
\t\tClient/ClientTimeFunctions.cpp
\t\t# Global variables and small utility functions that used to live
\t\t# in GamePacketFunctions.cpp.
\t\tClient/Globals.cpp
\t)'''
client_new = '''\t# Add additional source files
\tlist(APPEND CLIENT_MAIN_SOURCES
\t\tClient/COGGSTREAM.cpp
\t\t# Time functions extracted from PacketFunction.cpp (which has
\t\t# Windows-specific networking code).
\t\tClient/ClientTimeFunctions.cpp
\t\t# Global variables and small utility functions that used to live
\t\t# in GamePacketFunctions.cpp.
\t\tClient/Globals.cpp
\t\t# Files with uppercase .CPP extension that the Client/*.cpp glob
\t\t# misses on Linux (glibc's glob() is case-sensitive). Listed with
\t\t# lowercase extension; the FS is case-insensitive at open() time
\t\t# so compilation resolves correctly.
\t\tClient/BIT_RES.cpp
\t\tClient/MWarManager.cpp
\t\tClient/MBloodyBreakerEffectGenerator.cpp
\t\tClient/MAttachZoneAroundEffectGenerator.cpp
\t)'''
client_done = False
if 'Client/BIT_RES.cpp' in s:
    print("    Client uppercase-.CPP files already wired")
    client_done = True
elif client_old in s:
    s = s.replace(client_old, client_new, 1)
    print("    added 4 Client/*.CPP files to CLIENT_MAIN_SOURCES")
    client_done = True
else:
    print("    WARN: CLIENT_MAIN_SOURCES append block not matched verbatim")

if vsui_done or client_done:
    p.write_text(s)
PY_EOF

#================================================================
# Verification
#================================================================
echo ""
echo ">>> Verifying..."
bad_statements=$(find "$REPO_ROOT/dkrix/Client/SpriteLib/" -maxdepth 1 -name '*.h' ! -name '*.bak' \
    -exec grep -HE '^[[:space:]]*std::(i|o)fstream;[[:space:]]*$' {} + 2>/dev/null | wc -l)
bad_macros=$(grep -cE '^#define (min|max)\(a, b\)' "$PLATFORM_H" 2>/dev/null || true)
# Bug E/F check: min/max resolved via `using std::min/max` hoisted outside
# Platform.h's outer extern "C" block. Look for the using-declaration.
tmpl_escaped=$(grep -c '^using std::min;' "$PLATFORM_H" 2>/dev/null || true)
has_libgen_linux=$(awk '/#ifdef PLATFORM_LINUX/,/#endif/' "$PLATSDL" | grep -c 'libgen\.h' || true)
extern_c_before_platform=$(awk '/#ifdef __cplusplus/{found=1} /extern "C" \{/{if(found)print NR; exit}' "$SLB")
platform_h_include=$(grep -n '#include "../basic/Platform.h"' "$SLB" | head -1 | cut -d: -f1)
poslist_typename=$(grep -c 'typename POSITION_LIST::const_iterator' "$CPOSLIST" 2>/dev/null || true)
partmgr_typename=$(grep -cE 'typename PARTINDEX_LIST::(iterator|const_iterator)' "$CPARTMGR" 2>/dev/null || true)
fpermissive_on=$(grep -c '\-fpermissive' "$CLIENT_CMAKE" 2>/dev/null || true)
# Bug I check: headers that use std::vector should all now include <vector>
mskill_has_vector=$(grep -c '^#include <vector>' "$REPO_ROOT/dkrix/Client/MSkillManager.h" 2>/dev/null || true)
# Bug J check: ValueList.h's three std::list<T>::const_iterator sites got typename
valuelist_typename=$(grep -c 'typename std::list<T>::const_iterator' "$VALUELIST" 2>/dev/null || true)
# Bug K check: no uppercase STL includes remain anywhere under Client/ or basic/
uppercase_stl_remaining=$(find "$REPO_ROOT/dkrix/Client" "$REPO_ROOT/dkrix/basic" \
    \( -name '*.h' -o -name '*.cpp' \) ! -name '*.bak' -print0 2>/dev/null \
    | xargs -0 grep -lE '^#include[[:space:]]+<(VECTOR|LIST|MAP|SET|STRING|ALGORITHM|FSTREAM|IOSTREAM|SSTREAM|MEMORY|DEQUE|QUEUE|STACK|UTILITY|FUNCTIONAL|NUMERIC|ITERATOR)>' 2>/dev/null | wc -l)
# Bug L check: no std::auto_ptr remains
autoptr_remaining=$(grep -rE '\bstd::auto_ptr\b' "$REPO_ROOT/dkrix/Client" "$REPO_ROOT/dkrix/basic" \
    --include='*.h' --include='*.cpp' 2>/dev/null | grep -v '\.bak:' | wc -l)
# Bug M check: no stale DXLib/ include paths remain
stale_dxlib_includes=$(grep -rE '#include[[:space:]]+"DXLib/' "$REPO_ROOT/dkrix/Client" 2>/dev/null \
    --include='*.h' --include='*.cpp' --include='*.CPP' --include='*.H' \
    | grep -v '\.bak:' | wc -l)
# Bug N check: no ios::X || ios::Y (logical-OR) typos remain
logical_or_ios=$(grep -rE 'ios(_base)?::[a-z]+[[:space:]]*\|\|[[:space:]]*ios(_base)?::[a-z]+' \
    "$REPO_ROOT/dkrix/Client" "$REPO_ROOT/dkrix/basic" 2>/dev/null \
    --include='*.h' --include='*.cpp' --include='*.CPP' --include='*.H' \
    | grep -v '\.bak:' | wc -l)
# Bug O check: client CMakeLists uses find_package(Iconv) not hardcoded -liconv
iconv_portable=$(grep -c 'find_package(Iconv)' "$CLIENT_CMAKE" 2>/dev/null || true)
# Bug P check: the two linker-critical .CPP sources are wired into CMake
bitres_wired=$(grep -c 'Client/BIT_RES\.cpp' "$CLIENT_CMAKE" 2>/dev/null || true)
showchar_wired=$(grep -c 'VS_UI/src/VS_UI_TITLE_SHOWCHAR\.cpp' "$CLIENT_CMAKE" 2>/dev/null || true)

echo "    bare std::[io]fstream statements remaining: $bad_statements (expect 0)"
echo "    min/max preprocessor macros remaining:       $bad_macros (expect 0)"
echo "    std::min hoisted into global (using):        $tmpl_escaped (expect >=1)"
echo "    <libgen.h> in PLATFORM_LINUX block:          $has_libgen_linux (expect >=1)"
echo "    SpriteLibBackend.h — Platform.h at line:     $platform_h_include"
echo "    SpriteLibBackend.h — extern \"C\" at line:     $extern_c_before_platform (should be AFTER Platform.h)"
echo "    CPositionList.h — typename on dependent itr: $poslist_typename (expect >=1)"
echo "    CPartManager.h — typename on dependent itr:  $partmgr_typename (expect >=2)"
echo "    client CMakeLists.txt — -fpermissive flag:   $fpermissive_on (expect >=1)"
echo "    MSkillManager.h — #include <vector> added:   $mskill_has_vector (expect >=1)"
echo "    ValueList.h — typename on std::list<T> itr: $valuelist_typename (expect >=3)"
echo "    uppercase STL includes remaining:            $uppercase_stl_remaining (expect 0)"
echo "    std::auto_ptr uses remaining:                $autoptr_remaining (expect 0)"
echo "    stale DXLib/ include paths remaining:        $stale_dxlib_includes (expect 0)"
echo "    logical-OR in ios::X || ios::Y remaining:    $logical_or_ios (expect 0)"
echo "    client CMake — find_package(Iconv) portable: $iconv_portable (expect >=1)"
echo "    CMake — BIT_RES.cpp wired:                   $bitres_wired (expect >=1)"
echo "    CMake — VS_UI_TITLE_SHOWCHAR.cpp wired:      $showchar_wired (expect >=1)"

ok=true
[ "$bad_statements" -eq 0 ] || ok=false
[ "$bad_macros" -eq 0 ] || ok=false
[ "$tmpl_escaped" -ge 1 ] || ok=false
[ "$has_libgen_linux" -ge 1 ] || ok=false
[ "$poslist_typename" -ge 1 ] || ok=false
[ "$partmgr_typename" -ge 2 ] || ok=false
[ "$fpermissive_on" -ge 1 ] || ok=false
[ "$mskill_has_vector" -ge 1 ] || ok=false
[ "$valuelist_typename" -ge 3 ] || ok=false
[ "$uppercase_stl_remaining" -eq 0 ] || ok=false
[ "$autoptr_remaining" -eq 0 ] || ok=false
[ "$stale_dxlib_includes" -eq 0 ] || ok=false
[ "$logical_or_ios" -eq 0 ] || ok=false
[ "$iconv_portable" -ge 1 ] || ok=false
[ "$bitres_wired" -ge 1 ] || ok=false
[ "$showchar_wired" -ge 1 ] || ok=false
[ -n "$platform_h_include" ] && [ -n "$extern_c_before_platform" ] && \
    [ "$platform_h_include" -lt "$extern_c_before_platform" ] || ok=false

if $ok; then
    echo ""
    echo ">>> Fixes applied. Re-run client build:"
    echo "    cd $REPO_ROOT/dkrix/build && make -j\$(nproc)"
    echo ""
    echo "    (or re-run full smoke_test_finish.sh from $REPO_ROOT)"
else
    echo ""
    echo "ERROR: verification found leftover issues." >&2
    exit 1
fi
