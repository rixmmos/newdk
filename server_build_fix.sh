#!/usr/bin/env bash
#
# server_build_fix.sh — runtime-smoke-test server build fixes.
#
# Idempotent. Safe to re-run. Makes .bak-backed edits; a second run
# skips any site already in the fixed state.
#
# Bugs addressed:
#   Q: src/Core/Endian.h shadows glibc <endian.h> on case-insensitive
#      filesystems (DrvFs under WSL2). Transitive include chain:
#      <any glibc header> -> <sys/types.h> -> <endian.h> resolves to
#      OUR Endian.h because CMake puts src/Core on the -I path and
#      `-I` beats system dirs. Our Endian.h then does
#      `#include <endian.h>` on line 65 -> self-include -> header
#      guard bails -> glibc's htole16/32/64 and le16/32/64toh never
#      get defined -> the int16_t/int32_t/int64_t wrappers below
#      reference undeclared names.
#
#      Fix: delete the file. Per its own docstring it's currently
#      unused and deferred until Phase 13.3. When 13.3 reintroduces
#      it, use a non-shadowing name (LEndian.h, ByteOrder.h, etc).
#
#   R: src/server/database/PreparedStatement.cpp:204 assigns
#      `reinterpret_cast<char*>(&p.isNull)` to `MYSQL_BIND::is_null`.
#      On MySQL 5.7 / MariaDB headers `is_null` is `my_bool*` which
#      was `typedef char my_bool`, so the char* cast worked. On
#      MySQL 8.0+ `my_bool` was removed and `is_null` is now `bool*`
#      — char* no longer converts.
#
#      Phase 11B's header has a typedef `my_bool_t = bool` with a
#      comment acknowledging the issue, but the .cpp still hardcodes
#      char*. Fix uses decltype on MYSQL_BIND::is_null to derive the
#      actual pointed-to type from the installed library header —
#      works on both MySQL 5.7/MariaDB (yields char) and MySQL 8.0+
#      (yields bool), no version-sniff #ifdefs.
#
# Run from WSL:
#   bash /mnt/c/newdk/server_build_fix.sh
#
# After it completes:
#   cd /mnt/c/newdk/dkrixserver
#   rm -rf build    # force full reconfigure
#   make debug 2>&1 | tee /tmp/server_build.log

set -euo pipefail

REPO="${REPO:-/mnt/c/newdk/dkrixserver}"

if [[ ! -d "$REPO" ]]; then
    echo "ERROR: \$REPO=$REPO does not exist. Set REPO= or cd first." >&2
    exit 1
fi

cd "$REPO"

echo "=== server_build_fix.sh: running in $REPO ==="

# --------------------------------------------------------------------
# Bug Q: delete shadowing Endian.h
# --------------------------------------------------------------------

echo
echo "--- Bug Q: delete src/Core/Endian.h (shadows glibc <endian.h>) ---"

# DrvFs may have the file as either 'Endian.h' or 'endian.h' — the
# underlying NTFS inode is the same. Try both spellings.
removed=0
for name in Endian.h endian.h; do
    path="src/Core/$name"
    if [[ -f "$path" ]]; then
        # Sanity check: verify this is our Phase 13B file and NOT
        # something legitimate by grepping for the Phase 13B tag.
        if grep -q "Phase 13B" "$path" 2>/dev/null; then
            echo "  removing $path (Phase 13B tagged)"
            rm -f "$path"
            removed=1
        else
            echo "  WARNING: $path exists but is NOT Phase 13B tagged — refusing to delete"
            echo "  first 5 lines:"
            head -5 "$path" | sed 's/^/    /'
            exit 1
        fi
    fi
done

if [[ $removed -eq 0 ]]; then
    echo "  (no Endian.h found — already removed or never shipped)"
fi

# --------------------------------------------------------------------
# Bug R: PreparedStatement MYSQL_BIND::is_null typing
# --------------------------------------------------------------------

echo
echo "--- Bug R: PreparedStatement MYSQL_BIND::is_null type portability ---"

PS_H="src/server/database/PreparedStatement.h"
PS_CPP="src/server/database/PreparedStatement.cpp"

if [[ ! -f "$PS_H" ]] || [[ ! -f "$PS_CPP" ]]; then
    echo "  WARNING: PreparedStatement files not found, skipping Bug R"
    echo "    looked for: $PS_H"
    echo "    looked for: $PS_CPP"
else

# R.1: header — replace the naive `typedef bool my_bool_t;` with a
# decltype-derived typedef so my_bool_t matches the installed
# library's MYSQL_BIND::is_null pointed-to type exactly.
if grep -q "^typedef bool my_bool_t;" "$PS_H"; then
    cp "$PS_H" "$PS_H.bak"
    python3 <<'PY'
import re
path = "src/server/database/PreparedStatement.h"
with open(path) as f:
    src = f.read()

# Replace the 4-line guarded block with the decltype-derived one.
old = (
    "#ifndef MY_BOOL_DEFINED\n"
    "typedef bool my_bool_t;\n"
    "#define MY_BOOL_DEFINED\n"
    "#endif\n"
)
new = (
    "#include <type_traits>\n"
    "// Derive my_bool_t from the installed library's own\n"
    "// MYSQL_BIND::is_null declaration. On MySQL 8.0+ this is\n"
    "// `bool`; on MySQL 5.7 / MariaDB it is `my_bool` (a.k.a.\n"
    "// `char`). This keeps PreparedStatement.cpp's\n"
    "// `b.is_null = &p.isNull` assignment type-correct on both.\n"
    "#ifndef MY_BOOL_DEFINED\n"
    "typedef std::remove_pointer<decltype(MYSQL_BIND::is_null)>::type my_bool_t;\n"
    "#define MY_BOOL_DEFINED\n"
    "#endif\n"
)
if old not in src:
    print("ERROR: expected old typedef block not found verbatim")
    import sys; sys.exit(1)
src = src.replace(old, new, 1)
with open(path, "w") as f:
    f.write(src)
print("  R.1 patched: my_bool_t now decltype-derived")
PY
else
    echo '  R.1 already applied (no "typedef bool my_bool_t;" in header)'
fi

# R.2: cpp — replace `reinterpret_cast<char*>(&p.isNull)` with
# direct `&p.isNull` since my_bool_t now matches.
if grep -q "b.is_null = reinterpret_cast<char\*>(&p.isNull);" "$PS_CPP"; then
    cp "$PS_CPP" "$PS_CPP.bak"
    python3 <<'PY'
path = "src/server/database/PreparedStatement.cpp"
with open(path) as f:
    src = f.read()
old = "b.is_null = reinterpret_cast<char*>(&p.isNull);"
new = "b.is_null = &p.isNull;"
if old not in src:
    print("ERROR: expected old assignment not found verbatim")
    import sys; sys.exit(1)
src = src.replace(old, new, 1)
with open(path, "w") as f:
    f.write(src)
print("  R.2 patched: removed reinterpret_cast<char*>")
PY
else
    echo "  R.2 already applied (no reinterpret_cast<char*>(&p.isNull))"
fi

fi

# --------------------------------------------------------------------
# Verification
# --------------------------------------------------------------------

echo
echo "--- Verification ---"

# V1: file is gone
if [[ -f src/Core/Endian.h ]] || [[ -f src/Core/endian.h ]]; then
    echo "FAIL V1: src/Core/Endian.h (or endian.h) still present"
    exit 1
fi
echo "  V1 OK: src/Core/Endian.h removed"

# V2: nothing in the source tree references Endian.h
refs=$(grep -rE '#include.*"Endian\.h"|#include.*<Endian\.h>' src/ 2>/dev/null || true)
if [[ -n "$refs" ]]; then
    echo "FAIL V2: source tree still references Endian.h:"
    echo "$refs" | sed 's/^/    /'
    exit 1
fi
echo "  V2 OK: no source references to Endian.h"

# V3: self-include probe — compile a trivial TU that pulls in
# <sys/types.h> with src/Core on the include path; it should
# resolve <endian.h> to the system header, not our file.
tmp=$(mktemp -d)
cat > "$tmp/probe.cpp" <<'EOF'
#include <sys/types.h>
#include <cstdint>
int main() {
    uint32_t x = 1;
    uint32_t y = htole32(x);
    uint32_t z = le32toh(y);
    return (int)z;
}
EOF

if g++ -I"$REPO/src/Core" -c "$tmp/probe.cpp" -o "$tmp/probe.o" 2>"$tmp/err"; then
    echo "  V3 OK: glibc <endian.h> resolves cleanly (htole32/le32toh visible)"
else
    echo "FAIL V3: probe compile still broken — glibc <endian.h> not resolving"
    echo "  errors:"
    cat "$tmp/err" | sed 's/^/    /'
    rm -rf "$tmp"
    exit 1
fi
rm -rf "$tmp"

# V4+V5: PreparedStatement invariants
if [[ -f src/server/database/PreparedStatement.h ]]; then
    if grep -q "typedef bool my_bool_t;" src/server/database/PreparedStatement.h; then
        echo "FAIL V4: PreparedStatement.h still has `typedef bool my_bool_t;`"
        exit 1
    fi
    if ! grep -q "std::remove_pointer<decltype(MYSQL_BIND::is_null)>::type" src/server/database/PreparedStatement.h; then
        echo "FAIL V4: PreparedStatement.h missing decltype-derived my_bool_t"
        exit 1
    fi
    echo "  V4 OK: PreparedStatement.h uses decltype-derived my_bool_t"

    if grep -q 'reinterpret_cast<char\*>(&p.isNull)' src/server/database/PreparedStatement.cpp; then
        echo "FAIL V5: PreparedStatement.cpp still has reinterpret_cast<char*>"
        exit 1
    fi
    if ! grep -q 'b.is_null = &p.isNull;' src/server/database/PreparedStatement.cpp; then
        echo "FAIL V5: PreparedStatement.cpp missing direct &p.isNull assignment"
        exit 1
    fi
    echo "  V5 OK: PreparedStatement.cpp uses direct &p.isNull"

    # V6: compile-probe PreparedStatement.cpp against the installed
    # libmysqlclient headers. Only runs if mysql.h is present —
    # it's a correctness check for when the library IS installed,
    # not a gate on running this script. The actual build step
    # after this script will surface any real mismatch.
    if [[ -f /usr/include/mysql/mysql.h ]] || [[ -f /usr/local/include/mysql/mysql.h ]]; then
        tmp2=$(mktemp -d)
        cat > "$tmp2/probe2.cpp" <<'EOF'
#include <mysql/mysql.h>
#include <type_traits>
int main() {
    typedef std::remove_pointer<decltype(MYSQL_BIND::is_null)>::type my_bool_t;
    my_bool_t flag = 0;
    MYSQL_BIND b;
    b.is_null = &flag;   // must compile without cast
    return (int)(long)b.is_null;
}
EOF
        if g++ -std=c++11 -c "$tmp2/probe2.cpp" -o "$tmp2/probe2.o" 2>"$tmp2/err"; then
            echo "  V6 OK: MYSQL_BIND::is_null type probe compiles clean"
        else
            echo "FAIL V6: decltype probe didn't compile against installed libmysqlclient"
            cat "$tmp2/err" | sed 's/^/    /'
            rm -rf "$tmp2"
            exit 1
        fi
        rm -rf "$tmp2"
    else
        echo "  V6 SKIP: libmysqlclient-dev not installed yet — "
        echo "           real test will be the build step. Install with:"
        echo "             sudo apt install libmysqlclient-dev"
    fi
fi

echo
echo "=== server_build_fix.sh: Bugs Q + R resolved ==="
echo
echo "Next: force full reconfigure + rebuild:"
echo "  cd $REPO"
echo "  rm -rf build"
echo "  make debug 2>&1 | tee /tmp/server_build.log"
