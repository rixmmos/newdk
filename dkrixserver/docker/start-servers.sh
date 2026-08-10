#!/usr/bin/env bash
set -euo pipefail

BASE_DIR="/home/darkeden/vs"
BIN_DIR="$BASE_DIR/bin"
CONF_DIR="$BASE_DIR/conf"

# The real *.conf are untracked (they carry DB credentials). Only the
# *.conf.template files ship, so materialise them here if a conf is missing.
# Properties::expandEnvVars substitutes the ${DKRIX_*} placeholders at load,
# so the template can be copied verbatim.
for _svc in loginserver sharedserver gameserver; do
    if [ ! -f "$CONF_DIR/$_svc.conf" ] && [ -f "$CONF_DIR/$_svc.conf.template" ]; then
        cp "$CONF_DIR/$_svc.conf.template" "$CONF_DIR/$_svc.conf"
    fi
done

LOG_DIR="$BASE_DIR/log"

mkdir -p "$LOG_DIR" "$BASE_DIR/pid" "$BASE_DIR/output"

cd "$BIN_DIR"

./loginserver -f "$CONF_DIR/loginserver.conf" >"$LOG_DIR/loginserver.out" 2>&1 &
LOGIN_PID=$!

./sharedserver -f "$CONF_DIR/sharedserver.conf" >"$LOG_DIR/sharedserver.out" 2>&1 &
SHARED_PID=$!

./gameserver -f "$CONF_DIR/gameserver.conf" >"$LOG_DIR/gameserver.out" 2>&1 &
GAME_PID=$!

cleanup() {
    kill "$GAME_PID" "$SHARED_PID" "$LOGIN_PID" 2>/dev/null || true
    wait "$GAME_PID" "$SHARED_PID" "$LOGIN_PID" 2>/dev/null || true
}

trap cleanup EXIT INT TERM

wait -n "$LOGIN_PID" "$SHARED_PID" "$GAME_PID"
cleanup
exit 1
