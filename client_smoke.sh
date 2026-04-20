#!/usr/bin/env bash
#
# client_smoke.sh — boot the DarkEden client to its login screen against
# a local loginserver. Companion script for STEP3_CLIENT.md.
#
# What it does, in order:
#   1. Resolves config (env-var overrides allowed; sane defaults for
#      Enrico's WSL2 layout).
#   2. Verifies the DarkEden binary exists and is an ELF.
#   3. Verifies all dynamic-linker dependencies resolve. Bails on first
#      "not found" with the missing SO printed.
#   4. Verifies $DISPLAY is set (WSLg or external X server).
#   5. Verifies loginserver is reachable on the configured IP:port.
#   6. Verifies the retail data pack has Data/Info/FileDef.inf.
#   7. cd's into the data directory (so relative asset paths resolve)
#      and execs DarkEden with the Futec(IP:Port) command-line that
#      Client.cpp::GetFutecAddress expects, teeing output to /tmp/client.log.
#
# Exits 0 on a clean launch, non-zero with a diagnostic on any precheck
# failure. The actual client run is foreground — Ctrl-C to quit.
#
# Override anything via env:
#   DKRIX_BIN          — path to DarkEden binary
#                        (default /mnt/c/newdk/dkrix/build/bin/DarkEden)
#   DKRIX_DATA_DIR     — directory containing Data/
#                        (default /mnt/c/newdk/Darkeden data)
#   DKRIX_SERVER_IP    — loginserver IP (default 127.0.0.1)
#   DKRIX_SERVER_PORT  — loginserver port (default 9999)
#   DKRIX_LOG          — output log file (default /tmp/client.log)
#

set -u  # NOTE: NOT -e — we do explicit error handling so the diagnostics
        # stay under our control, not bash's.

DKRIX_BIN="${DKRIX_BIN:-/mnt/c/newdk/dkrix/build/bin/DarkEden}"
DKRIX_DATA_DIR="${DKRIX_DATA_DIR:-/mnt/c/newdk/Darkeden data}"
DKRIX_SERVER_IP="${DKRIX_SERVER_IP:-127.0.0.1}"
DKRIX_SERVER_PORT="${DKRIX_SERVER_PORT:-9999}"
DKRIX_LOG="${DKRIX_LOG:-/tmp/client.log}"

red()    { printf '\033[31m%s\033[0m\n' "$*" >&2; }
green()  { printf '\033[32m%s\033[0m\n' "$*"; }
yellow() { printf '\033[33m%s\033[0m\n' "$*"; }

bail() {
    red "[FAIL] $*"
    exit 1
}

ok() { green "[ OK ] $*"; }

# -----------------------------------------------------------------------
# 1. Echo config
# -----------------------------------------------------------------------
yellow "DarkEden client smoke test"
echo "  binary:        $DKRIX_BIN"
echo "  data dir:      $DKRIX_DATA_DIR"
echo "  server target: $DKRIX_SERVER_IP:$DKRIX_SERVER_PORT"
echo "  log:           $DKRIX_LOG"
echo

# -----------------------------------------------------------------------
# 2. Binary exists + is executable
# -----------------------------------------------------------------------
[ -f "$DKRIX_BIN" ] || bail "binary not found: $DKRIX_BIN
       Build the client first (see SMOKE_TEST.md section 4)."
[ -x "$DKRIX_BIN" ] || bail "binary not executable: $DKRIX_BIN
       Try: chmod +x \"$DKRIX_BIN\""

if command -v file >/dev/null 2>&1; then
    if ! file "$DKRIX_BIN" | grep -q 'ELF'; then
        bail "$DKRIX_BIN doesn't look like an ELF. Aborting."
    fi
fi
ok "binary present and executable"

# -----------------------------------------------------------------------
# 3. Dynamic linker prereqs
# -----------------------------------------------------------------------
if command -v ldd >/dev/null 2>&1; then
    missing="$(ldd "$DKRIX_BIN" 2>/dev/null | awk '/not found/ {print $1}')"
    if [ -n "$missing" ]; then
        red "[FAIL] missing dynamic libraries:"
        printf '  %s\n' $missing >&2
        red "       Install the matching Ubuntu packages — common ones:"
        red "         libsdl2-2.0-0 libsdl2-image-2.0-0 libsdl2-ttf-2.0-0"
        red "         libsdl2-mixer-2.0-0 libjpeg8 libfreetype6"
        exit 1
    fi
    ok "all dynamic libraries resolve"
else
    yellow "[WARN] ldd not found; skipping SO check"
fi

# -----------------------------------------------------------------------
# 4. Display backend
# -----------------------------------------------------------------------
if [ -z "${DISPLAY:-}" ] && [ -z "${WAYLAND_DISPLAY:-}" ]; then
    bail "neither \$DISPLAY nor \$WAYLAND_DISPLAY is set.
       On WSL2: make sure you're on Windows 11 with WSLg, and that
       'wsl --update' has run recently. On WSL1 / no WSLg: install
       VcXsrv on Windows and export DISPLAY=<host_ip>:0.0"
fi
ok "display backend present (DISPLAY=${DISPLAY:-} WAYLAND_DISPLAY=${WAYLAND_DISPLAY:-})"

# -----------------------------------------------------------------------
# 5. Loginserver reachability
# -----------------------------------------------------------------------
# Prefer `nc -z` (in netcat-openbsd); fall back to bash's /dev/tcp probe.
probe_tcp() {
    local host="$1" port="$2"
    if command -v nc >/dev/null 2>&1; then
        nc -z -w 2 "$host" "$port" >/dev/null 2>&1
    else
        # bash builtin; not in dash/sh.
        (exec 3<>"/dev/tcp/$host/$port") >/dev/null 2>&1
    fi
}

if probe_tcp "$DKRIX_SERVER_IP" "$DKRIX_SERVER_PORT"; then
    ok "loginserver reachable at $DKRIX_SERVER_IP:$DKRIX_SERVER_PORT"
else
    bail "no listener at $DKRIX_SERVER_IP:$DKRIX_SERVER_PORT
       Start the server stack first (STEP2_SERVER.md section 4)."
fi

# -----------------------------------------------------------------------
# 6. Retail data pack
# -----------------------------------------------------------------------
[ -d "$DKRIX_DATA_DIR" ] || bail "data dir not found: $DKRIX_DATA_DIR
       Set DKRIX_DATA_DIR=/path/to/Darkeden\ data and retry."

filedef="$DKRIX_DATA_DIR/Data/Info/FileDef.inf"
[ -f "$filedef" ] || bail "missing bootstrap file: $filedef
       Your retail pack is incomplete. FileDef.inf is the path index
       the client needs to find every other asset."
ok "retail data pack present (FileDef.inf at $filedef)"

# Quick sanity ping on .spk presence — not exhaustive, just a smell test.
spk_count=$(find "$DKRIX_DATA_DIR/Data/Image" -maxdepth 1 -iname '*.spk' 2>/dev/null | wc -l)
if [ "$spk_count" -lt 5 ]; then
    yellow "[WARN] only $spk_count .spk files in Data/Image — pack may be incomplete"
else
    ok "$spk_count .spk sprite packs found in Data/Image"
fi

# -----------------------------------------------------------------------
# 7. Launch
# -----------------------------------------------------------------------
echo
yellow "Launching DarkEden — Ctrl-C in this terminal to quit"
echo "  log: $DKRIX_LOG"
echo

# cd into data dir so the client's relative paths (Data\\Image\\...)
# resolve against it. Don't quote-strip $DKRIX_BIN; absolute path.
cd "$DKRIX_DATA_DIR" || bail "cd into data dir failed"

# The Futec(...) command-line format is what GetFutecAddress() in
# Client.cpp:285-315 parses to extract server IP:port. Single shell
# word, parens quoted because they're shell metacharacters.
exec "$DKRIX_BIN" "Futec(${DKRIX_SERVER_IP}:${DKRIX_SERVER_PORT})" \
    2>&1 | tee "$DKRIX_LOG"
