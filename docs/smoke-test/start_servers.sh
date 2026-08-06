#!/usr/bin/env bash
# start_servers.sh - Launch DarkEden server cluster in correct order.
#
# Ordering: sharedserver (9977)  →  loginserver (9999)  →  gameserver (9998)
# Each is given a moment to bind its port before the next one starts.
#
# Usage (from anywhere — the script locates the repo itself):
#   bash docs/smoke-test/start_servers.sh          # launches all three in background
#   bash docs/smoke-test/start_servers.sh stop     # stops them
#   bash docs/smoke-test/start_servers.sh status   # shows running PIDs + listening ports
#
# Logs: dkrixserver/{shared,login,game}server.log
#
# Assumes:
#   - Binaries already built at dkrixserver/bin/{shared,login,game}server
#   - MariaDB/MySQL reachable at 127.0.0.1:3306 as elcastle/elca110
#   - DARKEDEN and USERINFO dbs exist with schemas loaded
#   - conf/*.conf already patched to 127.0.0.1
#
# Override SERVER_DIR to point at a tree elsewhere.

set -e

# Repo root derived from this script's own location (docs/smoke-test/ -> root),
# so the tree can be moved without editing this file. See docs/MOVE-RUNBOOK.md.
_SMOKE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="${REPO_ROOT:-$(cd "$_SMOKE_DIR/../.." && pwd)}"

SERVER_DIR="${SERVER_DIR:-$REPO_ROOT/dkrixserver}"
BIN_DIR="$SERVER_DIR/bin"
LOG_DIR="$SERVER_DIR"

cd "$SERVER_DIR"

start_one() {
    local name="$1"
    local log="$LOG_DIR/${name}.log"
    local conf="$SERVER_DIR/conf/${name}.conf"
    if pgrep -x "$name" > /dev/null; then
        echo "  [skip] $name already running (pid $(pgrep -x $name))"
        return
    fi
    if [[ ! -f "$conf" ]]; then
        echo "  [fail] $name: config not found at $conf"
        return
    fi
    echo "  [start] $name -f $conf  -->  $log"
    # Run from SERVER_DIR so any relative paths the binary reads (Scripts/,
    # data/, etc.) resolve against dkrixserver/. Pass -f explicitly — all
    # three binaries exit(1) on bare invocation.
    nohup "$BIN_DIR/$name" -f "$conf" > "$log" 2>&1 &
    disown
}

stop_one() {
    local name="$1"
    if pgrep -x "$name" > /dev/null; then
        echo "  [stop] $name (pid $(pgrep -x $name))"
        pkill -TERM -x "$name" || true
        sleep 1
        pkill -KILL -x "$name" 2>/dev/null || true
    else
        echo "  [skip] $name not running"
    fi
}

case "${1:-start}" in
    start)
        echo "Starting DarkEden server cluster..."
        start_one sharedserver
        sleep 2
        start_one loginserver
        sleep 2
        start_one gameserver
        sleep 1
        echo ""
        echo "Status:"
        "$0" status
        ;;
    stop)
        echo "Stopping DarkEden server cluster..."
        stop_one gameserver
        stop_one loginserver
        stop_one sharedserver
        ;;
    status)
        for s in sharedserver loginserver gameserver; do
            pid=$(pgrep -x "$s" || echo "-")
            echo "  $s pid=$pid"
        done
        echo ""
        echo "Listening ports:"
        ss -ltnp 2>/dev/null | grep -E ":(9977|9998|9999) " || echo "  (none of 9977/9998/9999 bound)"
        ;;
    tail)
        echo "Tailing all three logs. Ctrl-C to exit."
        tail -F "$LOG_DIR/sharedserver.log" "$LOG_DIR/loginserver.log" "$LOG_DIR/gameserver.log"
        ;;
    *)
        echo "Usage: $0 {start|stop|status|tail}"
        exit 1
        ;;
esac
