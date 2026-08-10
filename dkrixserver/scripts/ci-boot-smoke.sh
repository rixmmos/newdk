#!/usr/bin/env bash
#
# ci-boot-smoke.sh — seed a database and boot all three servers, asserting
# that each one reaches its listening socket with no sanitizer report.
#
#--------------------------------------------------------------------------------
# WHAT THIS IS FOR, AND WHAT IT IS NOT
#--------------------------------------------------------------------------------
#
# The `sanitizers` matrix in .github/workflows/server.yml runs
# `make debug-asan` and stops there. AddressSanitizer reports at RUNTIME.
# A build-only ASan job structurally cannot catch a single one of the bugs
# it appears to cover — 18-B/D/E/F/J/L were all found by *running* the
# server — and two green ASan builds sat in CI while every login
# segfaulted.
#
# This script is the cheapest possible fix for that: boot the servers
# under ASan against a seeded database and assert they come up clean.
#
#   COVERS   everything that goes wrong between main() and the listening
#            socket. That is not a small class in this tree: Bug 18-A (a
#            malformed SQL literal that killed the gameserver in
#            KeyInfoManager::load()), 18-D (`new char[]` freed with plain
#            `delete` in Zone.cpp), 18-E (a 4-byte heap overflow in
#            ExpTable on every boot), 18-J (PetAttrInfo writing into
#            unconstructed storage) and 18-L (an uninitialised
#            GameServerPlayer* array in the sharedserver) all fire at
#            boot. Every one of them would have been caught here.
#
#   DOES NOT COVER anything that needs a client on the wire. 18-B and 18-C
#            are Result use-after-free in the login and character-select
#            handlers; nothing here logs in, so nothing here sees them.
#            Same for the whole packet-handling surface: 18-G, 18-H, 18-I,
#            18-N. Closing that needs a synthetic packet driver — see
#            docs/ci-runtime-sanitizer-plan.md, which specifies it.
#
#   DOES NOT COVER leaks. detect_leaks is off: the servers are killed
#            rather than shut down, LeakSanitizer only runs at a normal
#            exit, and this tree has a large known leak backlog that would
#            drown the signal.
#
# Boot-only is a large improvement over build-only and a long way short of
# a real runtime gate. Say so in the job name.
#
# Usage:
#   scripts/ci-boot-smoke.sh seed    # create schema + user, load dumps, fix hosts
#   scripts/ci-boot-smoke.sh conf    # materialise conf/*.conf from the templates
#   scripts/ci-boot-smoke.sh boot    # start all three, wait, assert, stop
#   scripts/ci-boot-smoke.sh all     # seed + conf + boot
#
# Environment (all have CI-appropriate defaults):
#   DB_HOST            default 127.0.0.1
#   DB_PORT            default 3306
#   MYSQL_ROOT_USER    default root
#   MYSQL_ROOT_PASSWORD  required for `seed`
#   GAME_USER/GAME_PASSWORD  default elcastle/password — these are the
#                      credentials baked into initdb/DARKEDEN.sql's
#                      WorldDBInfo rows, not a secret and not the live ones
#   BOOT_TIMEOUT       seconds to wait for each port, default 300
#   BIN_DIR            default <repo>/dkrixserver/bin
#
# Exit status: 0 all three listening and no sanitizer report; 1 otherwise.
#--------------------------------------------------------------------------------

set -uo pipefail

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
server_dir="$(cd -- "$script_dir/.." && pwd)"

DB_HOST="${DB_HOST:-127.0.0.1}"
DB_PORT="${DB_PORT:-3306}"
MYSQL_ROOT_USER="${MYSQL_ROOT_USER:-root}"
GAME_USER="${GAME_USER:-elcastle}"
GAME_PASSWORD="${GAME_PASSWORD:-password}"
BOOT_TIMEOUT="${BOOT_TIMEOUT:-300}"
BIN_DIR="${BIN_DIR:-$server_dir/bin}"
LOG_DIR="${LOG_DIR:-$server_dir/ci-boot-logs}"
CONF_DIR="${CONF_DIR:-$server_dir/conf}"

# Ports each server must be listening on before it counts as up.
# [measured 2026-08-06 against conf/*.conf, recorded in CLAUDE.md]
# Overridable only so this script can be exercised on a workstation that
# already has a server cluster running on the real ports.
declare -A PORT=(
    [sharedserver]="${SHARED_PORT:-9977}"
    [loginserver]="${LOGIN_PORT:-9999}"
    [gameserver]="${GAME_PORT:-9998}"
)
ORDER=(sharedserver loginserver gameserver)

log() { printf '[boot-smoke] %s\n' "$*"; }
die() { printf '[boot-smoke] FAIL: %s\n' "$*" >&2; exit 1; }

mysql_root() {
    mysql --protocol=TCP -h "$DB_HOST" -P "$DB_PORT" \
          -u "$MYSQL_ROOT_USER" -p"${MYSQL_ROOT_PASSWORD:-}" "$@"
}

do_seed() {
    [ -n "${MYSQL_ROOT_PASSWORD:-}" ] || die "MYSQL_ROOT_PASSWORD is not set"
    command -v mysql >/dev/null || die "the mysql client is not installed"

    # The dumps predate ONLY_FULL_GROUP_BY/NO_ZERO_DATE becoming defaults
    # and will not load under a strict sql_mode. Same setting the server
    # itself requires (dkrixserver/CLAUDE.md, Database Setup).
    log "relaxing sql_mode"
    mysql_root -e "SET GLOBAL sql_mode='ONLY_FULL_GROUP_BY,NO_ZERO_IN_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION';" \
        || die "could not reach MySQL at $DB_HOST:$DB_PORT"

    # The account must exist BEFORE a-setup.sql runs. a-setup.sql grants to
    # 'elcastle'@'%' but no longer creates it -- the CREATE USER carried a
    # password and this repo is public, so it was removed and the Docker path
    # now relies on the image's MYSQL_USER/MYSQL_PASSWORD instead. A service
    # container has no such bootstrap, and MySQL 5.7's default sql_mode
    # includes NO_AUTO_CREATE_USER, so a GRANT to a missing user is an error
    # rather than an implicit create. Creating all three hosts up front makes
    # this independent of what a-setup.sql does.
    #
    # '%' does not cover 'localhost': MySQL matches the more specific host row
    # first, and some builds resolve a TCP connection from the same host to
    # 'localhost', so both are needed.
    log "creating the game user"
    mysql_root -e "
        CREATE USER IF NOT EXISTS '$GAME_USER'@'%'         IDENTIFIED BY '$GAME_PASSWORD';
        CREATE USER IF NOT EXISTS '$GAME_USER'@'localhost' IDENTIFIED BY '$GAME_PASSWORD';
        CREATE USER IF NOT EXISTS '$GAME_USER'@'127.0.0.1' IDENTIFIED BY '$GAME_PASSWORD';
        FLUSH PRIVILEGES;" || die "could not create $GAME_USER"

    log "creating databases and granting"
    mysql_root < "$server_dir/initdb/a-setup.sql" || die "a-setup.sql failed"
    mysql_root -e "
        GRANT ALL PRIVILEGES ON DARKEDEN.* TO '$GAME_USER'@'localhost';
        GRANT ALL PRIVILEGES ON USERINFO.* TO '$GAME_USER'@'localhost';
        GRANT ALL PRIVILEGES ON DARKEDEN.* TO '$GAME_USER'@'127.0.0.1';
        GRANT ALL PRIVILEGES ON USERINFO.* TO '$GAME_USER'@'127.0.0.1';
        FLUSH PRIVILEGES;" || die "grants failed"

    log "loading initdb/DARKEDEN.sql (374 tables)"
    mysql_root DARKEDEN < "$server_dir/initdb/DARKEDEN.sql" || die "DARKEDEN.sql failed"
    log "loading initdb/USERINFO.sql"
    mysql_root USERINFO < "$server_dir/initdb/USERINFO.sql" || die "USERINFO.sql failed"

    # The dump ships WorldDBInfo pointing at 'odk-mysql', the service name
    # from docker/docker-compose.yml. Here the database is on DB_HOST. If
    # this is not corrected the servers connect to the right database to
    # read WorldDBInfo and then fail to connect to the world it names.
    log "repointing WorldDBInfo/GameServerInfo at $DB_HOST"
    mysql_root DARKEDEN -e "
        UPDATE WorldDBInfo SET Host='$DB_HOST', Port=$DB_PORT;
        UPDATE GameServerInfo SET IP='127.0.0.1';" || die "host fixup failed"

    mysql_root DARKEDEN -e "SELECT COUNT(*) AS keyinfo_rows FROM KeyInfo;" \
        || log "note: KeyInfo not queryable (not fatal here)"
    log "seed complete"
}

do_conf() {
    for svc in "${ORDER[@]}"; do
        tmpl="$CONF_DIR/$svc.conf.template"
        [ -f "$tmpl" ] || die "missing $tmpl"
        # Never clobber a config that already exists. On a CI runner the
        # real *.conf are absent (they are untracked, they carry
        # credentials) so this always writes; on a workstation it refuses,
        # because overwriting conf/gameserver.conf there would point a
        # developer's next `make debug` run at the wrong database.
        if [ -f "$CONF_DIR/$svc.conf" ] && [ "${FORCE_CONF:-0}" != "1" ]; then
            log "keeping existing $CONF_DIR/$svc.conf (set FORCE_CONF=1 to overwrite)"
            continue
        fi
        cp "$tmpl" "$CONF_DIR/$svc.conf"
        # HomePath is plaintext in the template (it is not a secret) and
        # points at the original author's workstation. Everything the
        # gameserver loads from data/ resolves against it.
        sed -i "s|^HomePath : .*|HomePath : $server_dir|" "$CONF_DIR/$svc.conf"
        log "wrote $CONF_DIR/$svc.conf (HomePath=$server_dir)"
    done
}

port_open() {
    # No dependency on ss/netstat/lsof.
    (exec 3<>"/dev/tcp/127.0.0.1/$1") 2>/dev/null && exec 3<&- 3>&- && return 0
    return 1
}

do_boot() {
    mkdir -p "$LOG_DIR"
    rm -f "$LOG_DIR"/asan.* "$LOG_DIR"/*.log

    # symbolize=1 gives a usable stack; detect_leaks=0 because these
    # processes are killed, not shut down, and the tree's leak backlog
    # would bury any real finding. abort_on_error=0 keeps the default
    # exit-on-report behaviour, which is what makes a missing port a
    # reliable signal.
    export ASAN_OPTIONS="detect_leaks=0:symbolize=1:print_stacktrace=1:log_path=$LOG_DIR/asan"
    export UBSAN_OPTIONS="print_stacktrace=1:log_path=$LOG_DIR/ubsan"

    declare -A PID=()
    cd "$server_dir" || die "cannot cd to $server_dir"

    # Pre-flight. Without this the whole gate is a lie whenever something
    # is already squatting one of the ports: the wait loop sees the
    # squatter's socket, calls our server up, and passes green even if our
    # server died in main(). Found by running this script on a workstation
    # that had a cluster up. A CI runner is clean, so this should never
    # fire there — which is exactly why it must be checked rather than
    # assumed.
    for svc in "${ORDER[@]}"; do
        if port_open "${PORT[$svc]}"; then
            die "TCP ${PORT[$svc]} is already in use before $svc was started;
     this run could not tell our server apart from whatever is listening.
     Stop it, or set SHARED_PORT/LOGIN_PORT/GAME_PORT."
        fi
    done

    for svc in "${ORDER[@]}"; do
        [ -x "$BIN_DIR/$svc" ] || die "$BIN_DIR/$svc missing or not executable"
        log "starting $svc (expecting TCP ${PORT[$svc]})"
        "$BIN_DIR/$svc" -f "$CONF_DIR/$svc.conf" > "$LOG_DIR/$svc.log" 2>&1 &
        PID[$svc]=$!

        deadline=$(( SECONDS + BOOT_TIMEOUT ))
        up=0
        while [ "$SECONDS" -lt "$deadline" ]; do
            if ! kill -0 "${PID[$svc]}" 2>/dev/null; then
                log "$svc exited before binding its port"
                break
            fi
            if port_open "${PORT[$svc]}"; then up=1; break; fi
            sleep 2
        done

        if [ "$up" -ne 1 ]; then
            log "----- $svc.log (last 80 lines) -----"
            tail -80 "$LOG_DIR/$svc.log" 2>/dev/null
            report_sanitizer
            stop_all PID
            die "$svc did not reach TCP ${PORT[$svc]} within ${BOOT_TIMEOUT}s"
        fi
        log "$svc is listening on ${PORT[$svc]}"
    done

    # Let the three settle and talk to each other; several boot-time
    # defects (18-L was one) fire on the first inter-server exchange
    # rather than during the listen call itself.
    log "all three up; holding 30s to catch post-listen faults"
    sleep 30

    failed=0
    for svc in "${ORDER[@]}"; do
        if ! kill -0 "${PID[$svc]}" 2>/dev/null; then
            log "$svc died after reaching its port"
            tail -80 "$LOG_DIR/$svc.log" 2>/dev/null
            failed=1
        fi
    done

    if report_sanitizer; then failed=1; fi
    stop_all PID

    if [ "$failed" -ne 0 ]; then
        die "boot smoke failed (see logs above)"
    fi
    log "PASS: all three servers booted and stayed up with no sanitizer report"
}

# Returns 0 (true) when a report exists — i.e. when something is wrong.
report_sanitizer() {
    local f
    shopt -s nullglob
    local files=( "$LOG_DIR"/asan.* "$LOG_DIR"/ubsan.* )
    shopt -u nullglob
    [ ${#files[@]} -gt 0 ] || return 1
    printf '\n[boot-smoke] SANITIZER REPORTS (%d file(s)):\n' "${#files[@]}"
    for f in "${files[@]}"; do
        printf '===== %s =====\n' "$f"
        cat "$f"
    done
    return 0
}

stop_all() {
    local -n pids=$1
    # `svc` must be local: without it this clobbers the caller's loop
    # variable and the failure message names the wrong server. Caught by
    # the fault-injection test, not by review.
    local svc
    for svc in gameserver loginserver sharedserver; do
        [ -n "${pids[$svc]:-}" ] || continue
        kill -TERM "${pids[$svc]}" 2>/dev/null
    done
    sleep 2
    for svc in gameserver loginserver sharedserver; do
        [ -n "${pids[$svc]:-}" ] || continue
        kill -KILL "${pids[$svc]}" 2>/dev/null
    done
    wait 2>/dev/null
    return 0
}

case "${1:-all}" in
    seed) do_seed ;;
    conf) do_conf ;;
    boot) do_boot ;;
    all)  do_seed; do_conf; do_boot ;;
    -h|--help|help)
        sed -n '2,/^set -uo/p' "$0" | sed 's/^# \{0,1\}//;/^set -uo/d'
        ;;
    *) die "unknown subcommand: $1 (try seed|conf|boot|all)" ;;
esac
