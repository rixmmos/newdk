#!/usr/bin/env bash
#
# smoke_test_finish.sh — finish the DarkEden smoke test after server build.
# Run from anywhere (autodetects repo root), or from ~/projects/newdk/ directly.
#
# What it does:
#   1. Drops FindSDL2_image.cmake + FindSDL2_ttf.cmake shims into dkrix/cmake/
#   2. Builds the client (clean configure + make)
#   3. Verifies/bootstraps MySQL: sql_mode relax, schema load, native_password
#      auth plugin on 'elcastle' (MySQL 8 default caching_sha2 breaks legacy
#      libmysqlclient connection path), WorldDBInfo hostname patch
#   4. Copies conf templates over active conf, patches 192.168.0.16 → 127.0.0.1
#   5. Writes ~/.dkrix-env with the env-var-expansion values
#   6. Launches loginserver → sharedserver → gameserver in a tmux session
#   7. Tails logs, reports what's alive
#
# All destructive-ish ops are backed up (conf files) or guarded (idempotent).
# Re-runnable: safe to run again after fixing a reported issue.
#
# Required shell env:
#   MYSQL_ROOT_PW — your MySQL root password. Defaults to 'rixpass' per
#                   the credentials you gave me.

set -euo pipefail

MYSQL_ROOT_PW="${MYSQL_ROOT_PW:-rixpass}"

# --- locate repo root ---
if [ -d "./dkrix" ] && [ -d "./dkrixserver" ]; then
    REPO_ROOT="$(pwd)"
elif [ -d "$HOME/projects/newdk/dkrix" ]; then
    REPO_ROOT="$HOME/projects/newdk"
else
    echo "ERROR: can't find repo root (need dkrix/ + dkrixserver/ siblings)." >&2
    echo "Run from the dir containing those, or set REPO_ROOT env var." >&2
    exit 1
fi
echo ">>> Repo root: $REPO_ROOT"
cd "$REPO_ROOT"

# --- quick environment sanity ---
for bin in cmake make mysql tmux pkg-config g++; do
    command -v "$bin" >/dev/null 2>&1 || {
        echo "ERROR: missing command '$bin'. Install it via apt and re-run." >&2
        exit 1
    }
done

# pkg-config names: SDL2 is lowercased for historical reasons ('sdl2.pc');
# SDL2_image / SDL2_ttf kept mixed case when they were added later.
for pkg in sdl2 SDL2_image SDL2_ttf xerces-c; do
    pkg-config --exists "$pkg" || {
        echo "ERROR: pkg-config can't find '$pkg'." >&2
        case "$pkg" in
            sdl2)         echo "Install: sudo apt-get install -y libsdl2-dev" >&2 ;;
            SDL2_image)   echo "Install: sudo apt-get install -y libsdl2-image-dev" >&2 ;;
            SDL2_ttf)     echo "Install: sudo apt-get install -y libsdl2-ttf-dev" >&2 ;;
            xerces-c)     echo "Install: sudo apt-get install -y libxerces-c-dev" >&2 ;;
        esac
        exit 1
    }
done

#================================================================
# STEP 1: CMake shims for SDL2_image / SDL2_ttf
#================================================================
echo ""
echo ">>> [1/7] Installing Find*.cmake shims..."
mkdir -p "$REPO_ROOT/dkrix/cmake"

cat > "$REPO_ROOT/dkrix/cmake/FindSDL2_image.cmake" <<'CMAKE_EOF'
# pkg-config shim for Ubuntu 22.04 (libsdl2-image-dev 2.0.5 ships no
# SDL2_imageConfig.cmake).
find_package(PkgConfig REQUIRED)
pkg_check_modules(PC_SDL2_image REQUIRED SDL2_image)

find_path(SDL2_IMAGE_INCLUDE_DIR
    NAMES SDL_image.h
    HINTS ${PC_SDL2_image_INCLUDE_DIRS}
    PATH_SUFFIXES SDL2)

find_library(SDL2_IMAGE_LIBRARY
    NAMES SDL2_image
    HINTS ${PC_SDL2_image_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2_image
    REQUIRED_VARS SDL2_IMAGE_LIBRARY SDL2_IMAGE_INCLUDE_DIR)

if(SDL2_image_FOUND AND NOT TARGET SDL2_image::SDL2_image)
    add_library(SDL2_image::SDL2_image UNKNOWN IMPORTED)
    set_target_properties(SDL2_image::SDL2_image PROPERTIES
        IMPORTED_LOCATION "${SDL2_IMAGE_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${SDL2_IMAGE_INCLUDE_DIR}")
endif()

mark_as_advanced(SDL2_IMAGE_INCLUDE_DIR SDL2_IMAGE_LIBRARY)
CMAKE_EOF

cat > "$REPO_ROOT/dkrix/cmake/FindSDL2_ttf.cmake" <<'CMAKE_EOF'
# pkg-config shim for Ubuntu 22.04.
find_package(PkgConfig REQUIRED)
pkg_check_modules(PC_SDL2_ttf REQUIRED SDL2_ttf)

find_path(SDL2_TTF_INCLUDE_DIR
    NAMES SDL_ttf.h
    HINTS ${PC_SDL2_ttf_INCLUDE_DIRS}
    PATH_SUFFIXES SDL2)

find_library(SDL2_TTF_LIBRARY
    NAMES SDL2_ttf
    HINTS ${PC_SDL2_ttf_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2_ttf
    REQUIRED_VARS SDL2_TTF_LIBRARY SDL2_TTF_INCLUDE_DIR)

if(SDL2_ttf_FOUND AND NOT TARGET SDL2_ttf::SDL2_ttf)
    add_library(SDL2_ttf::SDL2_ttf UNKNOWN IMPORTED)
    set_target_properties(SDL2_ttf::SDL2_ttf PROPERTIES
        IMPORTED_LOCATION "${SDL2_TTF_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${SDL2_TTF_INCLUDE_DIR}")
endif()

mark_as_advanced(SDL2_TTF_INCLUDE_DIR SDL2_TTF_LIBRARY)
CMAKE_EOF
echo "    wrote dkrix/cmake/FindSDL2_image.cmake + FindSDL2_ttf.cmake"

#================================================================
# STEP 2: client build
#================================================================
echo ""
echo ">>> [2/7] Building client..."
rm -rf "$REPO_ROOT/dkrix/build"
mkdir -p "$REPO_ROOT/dkrix/build"
(
    cd "$REPO_ROOT/dkrix/build"
    cmake .. -DCMAKE_BUILD_TYPE=Debug \
             -DCMAKE_MODULE_PATH="$REPO_ROOT/dkrix/cmake" \
             2>&1 | tee "$HOME/smoke-client-cmake.log"
    echo ""
    echo "    cmake done; starting make..."
    make -j"$(nproc)" 2>&1 | tee "$HOME/smoke-client-make.log"
)
CLIENT_BIN="$REPO_ROOT/dkrix/build/DarkEden"
if [ ! -x "$CLIENT_BIN" ]; then
    echo "ERROR: client binary not produced at $CLIENT_BIN" >&2
    echo "Check ~/smoke-client-make.log for the failure." >&2
    exit 1
fi
echo "    client built: $CLIENT_BIN"

#================================================================
# STEP 3: MySQL bootstrap
#================================================================
echo ""
echo ">>> [3/7] MySQL bootstrap (root pw=$MYSQL_ROOT_PW)..."

mysql_root() {
    mysql -uroot -p"$MYSQL_ROOT_PW" "$@" 2>&1
}

# 3.1 — sql_mode relax (session-global; persists until mysqld restart)
echo "    relaxing sql_mode to NO_ENGINE_SUBSTITUTION..."
mysql_root -e "SET GLOBAL sql_mode='NO_ENGINE_SUBSTITUTION';" >/dev/null

# 3.2 — run a-setup.sql (creates DBs + user)
echo "    loading initdb/a-setup.sql..."
mysql_root < "$REPO_ROOT/dkrixserver/initdb/a-setup.sql"

# 3.3 — flip elcastle to native_password (MySQL 8 legacy-client compatibility)
echo "    flipping elcastle to mysql_native_password plugin..."
mysql_root -e "
    ALTER USER 'elcastle'@'%' IDENTIFIED WITH mysql_native_password BY 'password';
    FLUSH PRIVILEGES;
" >/dev/null

# 3.4 — load schemas
echo "    loading DARKEDEN.sql..."
mysql -uelcastle -ppassword DARKEDEN < "$REPO_ROOT/dkrixserver/initdb/DARKEDEN.sql" 2>&1 \
    | grep -Ev '^(mysql: \[Warning\]|Warning)' || true

echo "    loading USERINFO.sql..."
mysql -uelcastle -ppassword USERINFO < "$REPO_ROOT/dkrixserver/initdb/USERINFO.sql" 2>&1 \
    | grep -Ev '^(mysql: \[Warning\]|Warning)' || true

# 3.5 — patch WorldDBInfo hostname
echo "    patching WorldDBInfo hostname to 127.0.0.1..."
mysql -uelcastle -ppassword DARKEDEN -e "
    UPDATE WorldDBInfo SET HostName='127.0.0.1' WHERE HostName='odk-mysql';
    SELECT WorldID, HostName, DBName FROM WorldDBInfo;
" 2>&1 | grep -Ev '^(mysql: \[Warning\])' || true

# 3.6 — verify GameServerInfo
echo "    verifying GameServerInfo..."
mysql -uelcastle -ppassword DARKEDEN -e "SELECT * FROM GameServerInfo;" 2>&1 \
    | grep -Ev '^(mysql: \[Warning\])' || true

#================================================================
# STEP 4: server conf patch (template → active, then sed IPs)
#================================================================
echo ""
echo ">>> [4/7] Patching server conf files..."
(
    cd "$REPO_ROOT/dkrixserver/conf"

    # Back up original conf files once (idempotent)
    for f in loginserver.conf sharedserver.conf gameserver.conf; do
        [ -f "$f.bak" ] || cp "$f" "$f.bak"
    done

    # Apply templates where they exist (Phase 8B env-var expansion)
    cp loginserver.conf.template  loginserver.conf
    cp sharedserver.conf.template sharedserver.conf
    # gameserver.conf has no template — restore from backup
    cp gameserver.conf.bak gameserver.conf

    # Patch all three for single-host operation
    for f in loginserver.conf sharedserver.conf gameserver.conf; do
        sed -i 's/192\.168\.0\.16/127.0.0.1/g' "$f"
    done
)
echo "    conf files patched (backups at *.conf.bak)"

#================================================================
# STEP 5: env file
#================================================================
echo ""
echo ">>> [5/7] Writing ~/.dkrix-env..."
cat > "$HOME/.dkrix-env" <<'ENV_EOF'
# DarkEden smoke-test env. Generated by smoke_test_finish.sh.
# source this before launching any server: `source ~/.dkrix-env`
export DKRIX_DB_HOST=127.0.0.1
export DKRIX_DB_PORT=3306
export DKRIX_DB_NAME=DARKEDEN
export DKRIX_DB_USER=elcastle
export DKRIX_DB_PASSWORD=password
export DKRIX_UI_DB_HOST=127.0.0.1
export DKRIX_UI_DB_NAME=USERINFO
export DKRIX_UI_DB_USER=elcastle
export DKRIX_UI_DB_PASSWORD=password
export DKRIX_DIST_DB_HOST=127.0.0.1
export DKRIX_DIST_DB_NAME=DARKEDEN
export DKRIX_DIST_DB_USER=elcastle
export DKRIX_DIST_DB_PASSWORD=password
export DKRIX_BILLING_SERVER_IP=127.0.0.1
ENV_EOF
echo "    wrote ~/.dkrix-env"

#================================================================
# STEP 6: launch servers in tmux
#================================================================
echo ""
echo ">>> [6/7] Launching servers in tmux session 'dkrix'..."

# Kill any stragglers from a previous run
tmux kill-session -t dkrix 2>/dev/null || true
pkill -f 'dkrixserver/bin/(login|shared|game)server' 2>/dev/null || true
sleep 1

BIN_DIR="$REPO_ROOT/dkrixserver/bin"

# Pane 0 (left): loginserver
tmux new-session -d -s dkrix -x 240 -y 60 \
    "cd $BIN_DIR && source ~/.dkrix-env && ./loginserver ../conf/loginserver.conf 2>&1 | tee /tmp/dkrix-loginserver.log; bash"

# Wait for login to bind
echo "    waiting 5s for loginserver to bind..."
sleep 5

# Pane 1 (right-top): sharedserver
tmux split-window -h -t dkrix \
    "cd $BIN_DIR && source ~/.dkrix-env && ./sharedserver ../conf/sharedserver.conf 2>&1 | tee /tmp/dkrix-sharedserver.log; bash"

echo "    waiting 5s for sharedserver to bind..."
sleep 5

# Pane 2 (right-bottom): gameserver
tmux split-window -v -t dkrix:0.1 \
    "cd $BIN_DIR && source ~/.dkrix-env && ./gameserver ../conf/gameserver.conf 2>&1 | tee /tmp/dkrix-gameserver.log; bash"

echo "    waiting 8s for gameserver to bind..."
sleep 8

#================================================================
# STEP 7: report
#================================================================
echo ""
echo ">>> [7/7] Server state snapshot..."

for log in loginserver sharedserver gameserver; do
    path="/tmp/dkrix-$log.log"
    echo ""
    echo "=== $log — last 25 lines of $path ==="
    if [ -f "$path" ]; then
        tail -n 25 "$path" 2>&1
    else
        echo "(no log yet — process may have died before first write)"
    fi
done

echo ""
echo "=== Listening ports (9977/9996/9998/9999 are the dkrix ports) ==="
ss -tlnp 2>&1 | grep -E ':(9977|9996|9997|9998|9999|9900)' || echo "(none of the expected ports are bound)"

echo ""
echo "=== Running dkrix processes ==="
pgrep -af 'dkrixserver/bin/' || echo "(no dkrix server processes running)"

echo ""
echo "==========================================================="
echo "Inspect the servers live:    tmux attach -t dkrix"
echo "Launch the client:           cd $REPO_ROOT/dkrix/build && ./DarkEden"
echo "Logs:                        /tmp/dkrix-{login,shared,game}server.log"
echo "Kill everything:             tmux kill-session -t dkrix"
echo "==========================================================="
