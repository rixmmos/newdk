# Step 3: Client + retail-data smoke test (boot to login screen)

Goal: take the `DarkEden` binary we built in Step 2's client pass and
drive it all the way to an on-screen login window, talking to the
loginserver running on `127.0.0.1:9999` from Step 2. Scope stops at
"a window with the login UI appears and the client doesn't crash";
we are NOT sending a login packet yet (that bumps into the Phase 17
wire-format divergence).

The whole run happens under WSL2's WSLg — the GPU/X server that ships
with Windows 11 and newer Windows 10 builds. No third-party X server,
no `DISPLAY=:0` dance.

## Why this step is its own doc

Step 2 (`STEP2_SERVER.md`) gets three daemons listening. That proves
the server code compiles and boots. It does NOT prove that the client
can find its retail assets, bind a window, decompress `.spk` sprite
packs, and render the login UI — all of which are independent failure
surfaces we haven't touched since Phase 3 (DXLib → SDL2). We need a
separate bring-up pass for the client before doing anything
packet-level.

Scope deliberately stops short of submitting the login form. That
crosses into packet-layer territory where Phase 17 has already
flagged known divergence between the client's and server's copies of
packet classes. We don't want a login failure here to get blamed on
asset loading or vice versa.

## 1. Prerequisites — what Step 2 should have left you with

```bash
# The binary should exist and be an ELF64 PIE:
file /mnt/c/newdk/dkrix/build/bin/DarkEden
# expect: ELF 64-bit LSB pie executable, x86-64 ...

# Loginserver should be listening on 9999 (from Step 2):
ss -ltnp | grep ':9999 '
# expect: one LISTEN socket
```

If the binary is missing, rerun the client build from SMOKE_TEST.md
step 4. If loginserver isn't listening, go back to STEP2_SERVER.md
section 4.

## 2. WSLg sanity check

WSLg ships with Windows 11 (or any recent Windows 10 + WSL2 update).
It injects `$DISPLAY=:0` and a Wayland/X bridge automatically when
you open a WSL shell. Verify:

```bash
echo "$DISPLAY"
# expect: :0  (or something like :0, not empty)

# A tiny X test without pulling in SDL:
sudo apt install -y x11-apps
xeyes &
# a pair of eyes should pop up in a Windows window. Kill it with `fg`
# then Ctrl-C, or just close the window.
```

If `$DISPLAY` is empty or `xeyes` doesn't appear:

- Your WSL/Windows is too old for WSLg. Run `wsl --update` in
  PowerShell and restart the distro (`wsl --shutdown`).
- You're on WSL1. Confirm with `wsl.exe -l -v` — VERSION must be `2`.
- Corporate policy blocks WSLg. Fallback: install VcXsrv on Windows,
  run it with "Disable access control" checked, then
  `export DISPLAY=$(ip route | awk '/^default/{print $3}'):0.0`.

## 3. Install the SDL2 runtime libs + CJK fonts

The client was built against these via `-dev` packages; at runtime it
only needs the SONAME libs. Ubuntu 22.04:

```bash
sudo apt install -y \
    libsdl2-2.0-0 \
    libsdl2-image-2.0-0 \
    libsdl2-ttf-2.0-0 \
    libsdl2-mixer-2.0-0 \
    libjpeg8 \
    libfreetype6 \
    fonts-noto-cjk \
    fonts-noto-core \
    fonts-dejavu
```

Why each:
- `libsdl2-2.0-0` — the window + input backend (replaces DirectDraw).
- `libsdl2-image-2.0-0` — PNG/JPEG decode used by UI-layer textures.
- `libsdl2-ttf-2.0-0` — glyph rendering. **This is the one the binary
  is most likely to be missing**; without it `DarkEden` exits silently
  at startup (the dynamic linker error goes to stderr and you see
  nothing in the shell unless you look).
- `libsdl2-mixer-2.0-0` — audio. Not strictly required for boot-to-
  login but the client tries to init it early and complains loudly if
  the SO is missing.
- `libjpeg8` — the retail `.jps` textures are JPEG-wrapped.
- `libfreetype6` — pulled in by SDL2_ttf, but good to be explicit.
- `fonts-noto-cjk` — ships `NotoSansCJK-Regular.ttc` at
  `/usr/share/fonts/opentype/noto/`. **Required** or else
  `TextBackendSDL: Failed to load font size 16` spams the log and the
  whole UI renders without text (you see the background image but no
  login form). Bug V — the port's font fallback list only had macOS
  system paths, not Linux ones, before the fix in Phase 13D.
- `fonts-noto-core` — non-CJK Noto fallback.
- `fonts-dejavu` — DejaVu fallback; the port's 3rd-priority path is
  `Data/Font/DejaVuSans.ttf`.

### 3a. Zero-code workaround if you can't / don't want to rebuild

Phase 13D patches `TextBackendSDL.cpp` to probe Linux system font
paths directly. If your current binary predates that patch (i.e. you
built before the Phase 13D commit), you can ship-of-Theseus around it
by dropping a symlink into the data folder:

```bash
mkdir -p "/mnt/c/newdk/Darkeden data/Data/Font"
ln -sf /usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc \
       "/mnt/c/newdk/Darkeden data/Data/Font/NotoSansCJK-Regular.ttc"
```

That matches the first-priority path in the fallback list
(`Data/Font/NotoSansCJK-Regular.ttc`) and works without a rebuild.

Verify all runtime deps resolve:

```bash
ldd /mnt/c/newdk/dkrix/build/bin/DarkEden | grep 'not found'
# expect: no output (empty = all libs found)
```

If anything reports `not found`, install the matching `-0` / `6` / `8`
package. Common ones the base install might miss:
`libxss1`, `libxxf86vm1`, `libxrandr2`, `libxinerama1`, `libxcursor1`,
`libxi6`, `libpulse0`, `libasound2`.

## 4. Place the retail data pack

The client binary has **no shipped `Data/` directory**. It depends
entirely on retail data being present at runtime. Enrico's copy is
already on disk at `/mnt/c/newdk/Darkeden data/` — that's what we
use. The folder must contain at minimum:

```
Darkeden data/
├── Data/
│   ├── Info/            ← FileDef.inf lives here
│   │   ├── FileDef.inf
│   │   └── (~hundreds of .inf files)
│   ├── Image/           ← .spk / .spki / .ispk / .ispki / .jps
│   ├── Map/             ← .smp zone files
│   ├── Music/
│   ├── Sound/
│   └── Ui/
└── UserSet/             ← per-user settings (client creates if missing)
```

Verify:

```bash
ls "/mnt/c/newdk/Darkeden data/Data/Info/FileDef.inf"
# expect: the file exists, ~267 lines
```

`FileDef.inf` is the bootstrap file. It's the ONLY path the client
hardcodes (`Data\Info\FileDef.inf` in `MFileDef.h`); every other
asset path is looked up via `g_pFileDef->getProperty(KEY)`, whose
values come from this file. 75 unique `KEY`s in `Client.cpp`. If
`FileDef.inf` is missing or malformed, the client aborts immediately
with "Cannot open FileDef".

### 4a. Case sensitivity on ext4

Retail is Windows-case (`Data`, `Info`, `Image`). Linux ext4 is
case-sensitive. Two paths — pick one:

**Option A (recommended, zero-touch):** keep the data on
`/mnt/c/...` (DrvFs). DrvFs is case-insensitive by default, which
matches Windows. This is why the runbook points at
`/mnt/c/newdk/Darkeden data/` and not `~/Darkeden data/`.

**Option B (native ext4):** if you later copy the data under `~/`,
you MUST either run `setcap cap_fowner+ep` equivalents or, simpler,
normalize filenames. The client does `\` → `/` conversion at runtime
(`FileOpenBinary` in `GameMain.cpp:4431-4440`) but does NOT case-fold.
So on ext4 you get "Image/etc.spk not found" even though
`Image/Etc.spk` exists. Don't do Option B unless you have a reason.

### 4b. RAR-packed archives — the `.rpk` extraction step

Retail ships 8 `.rpk` archives — password-encrypted RAR v2.0 with the
static password `darkeden`:

```
Data/Info/infodata.rpk        ← the RTI loader blocks on this one
Data/Ui/txt/Book.rpk
Data/Ui/txt/Help.rpk
Data/Ui/txt/item.rpk
Data/Ui/txt/progress.rpk
Data/Ui/txt/skill.rpk
Data/Ui/txt/title.rpk
Data/Ui/txt/TutorialEtc.rpk
```

In the original Windows client these were read through an actual RAR
library. During Phase 3 of this port the unrar dependency was
dropped; the replacement `CRarFile` class in `VS_UI/RarFile.{h,cpp}`
is a stub that **doesn't read RAR archives at all** — it maps
`Data/Info/infodata.rpk` to `Data/Info/infodata/` and reads loose
files via `fopen`. The comment at the top of `RarFile.h:22-28`
explicitly documents this:

> RAR files are mapped to directories with the same name:
> - `Data/Info/infodata.rpk  → Data/Info/infodata/`
> - `Data/Ui/txt/Item.rpk    → Data/Ui/txt/Item/`

So the `.rpk`s on disk are **dead weight** — every one of them needs
to be pre-extracted into a same-named directory or the client aborts
with `[Error] Cannot Open RTI File` during `RegenTowerInfoManager`
init and exits before opening the window.

`extract_rpks.sh` in the repo root does this. Requires real `unrar`
(from the multiverse repo — `unrar-free` can't do passwords):

```bash
sudo add-apt-repository multiverse
sudo apt update
sudo apt install -y unrar

cd /mnt/c/newdk
./extract_rpks.sh
# expect: "[ok  ] infodata.rpk → infodata/ (6 files)" etc., 8 times
```

Verify extraction:

```bash
ls "/mnt/c/newdk/Darkeden data/Data/Info/infodata/"
# expect: Filter.inf, RegenTowerPosition.inf, Language.inf, ...
```

`client_smoke.sh` refuses to launch the client if any `.rpk` doesn't
have a matching extracted directory, so you can't forget this step.

## 5. Launch the client

Invocation format (from `GetFutecAddress` in `Client.cpp:285-315`):

```
DarkEden Futec(<ip>:<port>)
```

The `Futec(...)` wrapper is load-bearing — the client's
command-line parser looks for that exact prefix. No quotes around
`Futec(...)` when called as a shell word; quote it only because the
parentheses are shell metacharacters.

From the directory that holds `Data/`:

```bash
cd "/mnt/c/newdk/Darkeden data"

/mnt/c/newdk/dkrix/build/bin/DarkEden "Futec(127.0.0.1:9999)" \
    2>&1 | tee /tmp/client.log
```

Why `cd` into the data folder: the client's file I/O is relative to
the current working directory for every asset path that doesn't start
with `/`. `FileDef.inf` uses `Data\Image\...`-style relative paths,
so the data-folder-as-cwd is what makes them resolve.

## 6. What success looks like

Within ~3-8 seconds (first launch is slower because SDL2 builds font
caches):

1. A window titled `DarkEden` opens on the Windows desktop (WSLg
   surfaces it like a native app, no DISPLAY plumbing).
2. An intro/login background renders.
3. The login UI (username + password fields + buttons) is drawn.
4. No stack trace or "InitFail" in `/tmp/client.log`.

At this point the smoke test passes. Do NOT click "Connect" / "Log
In" — per the scope we set at the top of this doc, that crosses into
Phase-17 packet territory.

Ctrl-C in the shell or close the window to exit.

## 7. Known ways this fails and what to do

- **Nothing happens, shell returns immediately.** Almost always a
  missing SO. Run `ldd .../DarkEden | grep 'not found'` from step 3
  and install what's missing. On Ubuntu 22.04 the #1 culprit is
  `libsdl2-ttf-2.0-0`.

- **`[Error] File Open Error : [Data\Info\FileDef.inf]`** in the log.
  Your cwd isn't the data directory, or `Data/Info/FileDef.inf` is
  missing. Re-check step 4.

- **`[Error] File Open Error : [Data\Image\Something.spk]`.**
  Specific sprite pack missing from your retail dump. The client
  aborts hard on missing `.spk` unless the asset is optional. Check
  that your `Darkeden data/Data/Image/` is complete — it should be
  ~2 GB. If it's only a few hundred MB you have a partial dump.

- **SDL window opens, then the window closes after a black flash.**
  Usually an OpenGL context problem under WSLg. Set
  `export LIBGL_ALWAYS_SOFTWARE=1` and retry; if that works we have
  a GPU-driver mismatch and we'll revisit.

- **`cannot open shared object file: libGL.so.1`.** Install
  `libgl1-mesa-glx` (or `libgl1` on newer Ubuntu).

- **Font renders as tofu (empty boxes).** Phase 5D verified that the
  Korean/Chinese fallback chain exists, but it needs a CJK font
  installed. `sudo apt install fonts-noto-cjk`.

- **The binary segfaults in `LoadResource` / `CAlphaSprite::Load`.**
  Sprite format divergence. That's a real bug, not a config issue —
  capture the stack trace with `gdb` and open a task.

- **Window opens but input doesn't respond.** SDL2 input device
  problem under WSLg. Confirm by focusing the window and trying to
  drag it — if drag also fails, it's an SDL+WSLg issue. The
  workaround in older WSLg builds was
  `export SDL_VIDEODRIVER=x11` (forces the X path instead of
  Wayland).

## 8. The helper script

`client_smoke.sh` in the repo root runs steps 1-5 as a single
command: verifies the binary, checks loginserver is up, checks all
SOs resolve, cd's to the data folder, launches `DarkEden` with the
right `Futec(...)` argument, and tees the log.

Usage:

```bash
# default: /mnt/c/newdk/... and 127.0.0.1:9999
./client_smoke.sh

# override:
DKRIX_BIN=/custom/DarkEden \
DKRIX_DATA_DIR="/custom/Darkeden data" \
DKRIX_SERVER_IP=192.168.1.50 \
DKRIX_SERVER_PORT=9999 \
    ./client_smoke.sh
```

The script exits non-zero at the first failed precheck, so you can
use it in a CI-ish loop:

```bash
./client_smoke.sh && echo "SMOKE PASS" || echo "SMOKE FAIL"
```

## 9. Report back

Paste me:

1. `ldd /mnt/c/newdk/dkrix/build/bin/DarkEden | grep 'not found'`
   (expect empty).
2. Last 40 lines of `/tmp/client.log`.
3. A Windows screenshot of the DarkEden window if it opened, or a
   description of what appeared/didn't appear on screen.

That tells me whether we're ready to start working on Phase 17's
packet unification against a live client — or whether there's another
asset-layer bug to swat first.
