#!/usr/bin/env bash
#
# extract_rpks.sh — expand all .rpk archives in the retail data pack into
# the same-named directories the client's CRarFile stub expects.
#
# Why this exists: during Phase 3, CRarFile (Client/VS_UI/RarFile.{h,cpp})
# was rewritten to drop the unrar dependency. The stub maps
# "Data/Info/infodata.rpk" → "Data/Info/infodata/" and reads loose files
# from that directory via fopen. If the directory isn't there the client
# aborts during RegenTowerInfoManager init ("Cannot Open RTI File"), which
# is exactly where the WSLg smoke test fails on a fresh retail pack.
#
# The .rpk archives are standard RAR v2.0 with the password "darkeden"
# (baked into Client.cpp:499). We need real `unrar` (not `unrar-free`,
# which can't handle passwords) from Ubuntu multiverse:
#     sudo apt install unrar
#
# Usage:
#     ./extract_rpks.sh                      # defaults to /mnt/c/newdk/Darkeden data
#     DKRIX_DATA_DIR=/path/to/data ./extract_rpks.sh
#     ./extract_rpks.sh -f                   # -f forces re-extract even if dir exists
#
# The script is idempotent unless you pass -f.
#

set -u

DKRIX_DATA_DIR="${DKRIX_DATA_DIR:-/mnt/c/newdk/Darkeden data}"
PASSWORD="darkeden"
FORCE=0

if [ "${1:-}" = "-f" ] || [ "${1:-}" = "--force" ]; then
    FORCE=1
fi

red()    { printf '\033[31m%s\033[0m\n' "$*" >&2; }
green()  { printf '\033[32m%s\033[0m\n' "$*"; }
yellow() { printf '\033[33m%s\033[0m\n' "$*"; }

bail() { red "[FAIL] $*"; exit 1; }

# ---------------------------------------------------------------
# Prereqs
# ---------------------------------------------------------------
[ -d "$DKRIX_DATA_DIR" ] || bail "data dir not found: $DKRIX_DATA_DIR"

if ! command -v unrar >/dev/null 2>&1; then
    bail "unrar not installed.
       Install with: sudo add-apt-repository multiverse && \\
                     sudo apt update && sudo apt install -y unrar
       (do NOT use unrar-free — it can't read password-protected RARs.)"
fi

# Reject unrar-free which sometimes installs the 'unrar' command name
# but silently ignores passwords.
if unrar 2>&1 | head -1 | grep -qi 'unrar-free'; then
    bail "detected unrar-free — it can't handle encrypted archives.
       sudo apt remove unrar-free && \\
       sudo apt install -y unrar   (from multiverse)"
fi

yellow "Extracting .rpk archives under: $DKRIX_DATA_DIR"
echo

# ---------------------------------------------------------------
# Find and extract every .rpk
# ---------------------------------------------------------------
# Use -print0/read -d to survive the space in "Darkeden data".
extracted=0
skipped=0
failed=0

while IFS= read -r -d '' rpk; do
    # Derive target dir: strip ".rpk" off the archive path.
    # Example: .../Data/Info/infodata.rpk -> .../Data/Info/infodata
    dir="${rpk%.rpk}"

    if [ -d "$dir" ] && [ "$FORCE" -eq 0 ]; then
        green "[skip] $(basename "$rpk") — $(basename "$dir")/ already exists"
        skipped=$((skipped + 1))
        continue
    fi

    mkdir -p "$dir" || { red "[fail] mkdir: $dir"; failed=$((failed + 1)); continue; }

    # unrar x: extract with full paths. -p<pw> with no space. -o+ overwrite.
    # Run from the target dir so any in-archive directory layout stays
    # relative to it.
    if ( cd "$dir" && unrar x -p"$PASSWORD" -o+ -inul "$rpk" . ) 2>/dev/null; then
        count=$(find "$dir" -maxdepth 5 -type f | wc -l)
        green "[ok  ] $(basename "$rpk") → $(basename "$dir")/ ($count files)"
        extracted=$((extracted + 1))
    else
        red "[fail] $(basename "$rpk") — unrar returned non-zero. Wrong password?"
        failed=$((failed + 1))
    fi
done < <(find "$DKRIX_DATA_DIR" -type f -iname '*.rpk' -print0)

echo
yellow "Summary: extracted=$extracted, skipped=$skipped, failed=$failed"

if [ "$failed" -gt 0 ]; then
    exit 1
fi
exit 0
