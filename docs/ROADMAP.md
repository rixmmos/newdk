# ROADMAP.md — Product / feature follow-ups

This is the backlog of **product-facing work** that came out of the
2026-04-20 end-to-end smoke test. It is deliberately separate from
[`MODERNIZATION.md`](./MODERNIZATION.md), which tracks code-hygiene
and toolchain work.

Modernization changes *how the code is built*; the items here change
*what the game does or ships as*. Both can progress in parallel.

When anything here falls out of date, update this file as part of the
change that made it stale — not after the fact.

## Status legend

- `[ ]` — not started
- `[~]` — in progress
- `[x]` — done
- `[!]` — blocked / deferred (with reason)

## Items

### R1 — UHD / high-DPI resolution support

- [ ] Render the client at 4K (3840×2160) on modern displays without
      the current "postage stamp in the top-left" effect you get when
      the SDL window is resized past the native sprite resolution.

**Background.** Retail DarkEden targeted 640×480 / 800×600 with
a fixed `m_pSurface` backbuffer. The SDL2 port renders into that
same fixed-size surface and then blits 1:1 to the window; at 4K the
effective zoom is ~3×, which is currently either letterboxed or
nearest-neighbour enlarged depending on the renderer path.

**Scope.**
- Decouple the logical game resolution (what sprite coordinates
  mean) from the window/output resolution (what the user sees).
- Add a UI scale factor so HUD, chat, inventory, and cursor
  targeting all respect the same scale.
- Pick a sprite-scaling policy: integer scale (crisp, uneven at 4K),
  bilinear (soft), or a sprite-aware upscaler (expensive). Most
  likely integer-scale for sprites + bilinear for the compositor.
- Ensure mouse input maps from window pixels → logical pixels
  correctly (the click-target code is currently in window coords).
- Remember the chosen window size / scale factor in
  `ClientConfig.inf` (or equivalent) so it survives restarts.

**Blockers / risks.**
- Phase 4 collapsed sprite-format variants but did **not** touch the
  fixed-resolution assumption. Many `MTopView` blits assume a 640-
  or 800-wide surface.
- The darkness filter path (Bug WW null-guard) bypasses the
  `DrawLightBuffer3D` overlay. Before we ship 4K we want that
  path ported to SDL2 properly or darkness will render wrong at
  any non-native scale.
- UI layout coordinates are hard-coded throughout `VS_UI/`. A
  true DPI-aware pass is large; a simple integer-scale wrapper
  is a much smaller first step and probably the right MVP.

**Non-goals.**
- Native rendering at 4K (no sprite re-master). Retail art is what
  it is; we're upscaling, not re-authoring.
- Per-monitor DPI awareness on multi-monitor setups. Nice-to-have,
  out of MVP scope.

**MVP definition.** At 1920×1080 and 3840×2160 window sizes, the
game renders edge-to-edge (no letterbox, no pixel-for-pixel
postage stamp), HUD stays readable, and mouse targeting clicks
the right tile.

### R2 — Localization pass: English-only (was task #138)

- [ ] Strip all Korean (EUC-KR) and Chinese (GB2312) text, file
      comments, and UI strings. Ship an English-only client.

**Background.** The retail DarkEden data files and the legacy
source both carry large amounts of CJK text. Some is
user-visible (item names, NPC dialog, quest text); some is
developer-facing (comments, log strings, tool output).

**Scope.**
- User-visible strings (`VS_UI_*.cpp` hard-coded CJK, item
  name tables, NPC dialog tables, quest descriptions).
- Data files under `Darkeden data/` — item descriptions,
  map names, monster names, zone info.
- Source comments. Phase 6's touch-as-you-go rule already
  says "translate CJK comments to English when the file is
  modified for another reason"; R2 is the explicit sweep
  that finishes the job instead of waiting for incidental
  touches.
- Font fallback: currently the TextSystem fallback loads
  Korean/Chinese system fonts so CJK glyphs render. After
  this pass we can drop those fallbacks in favour of a
  single Latin/Unicode font.

**Non-goals.**
- Translation into additional languages. English-only means
  English-only; adding a second locale is a separate effort
  with a proper `.po`/`.mo` or equivalent message catalog.
- Rewriting the text pipeline. Phase 5 finished the text
  pipeline; R2 just feeds it different strings.

**Gotchas.**
- Some "CJK" strings are actually Latin text double-encoded
  through a legacy code page (e.g. `"ì¤ì¹´"` that decodes
  back to Korean). Detect before translating, or the
  "translation" is a one-way lossy conversion.
- Wire-format strings (chat, whisper, pet names) are UTF-8
  in the modernized client but may be interpreted as
  EUC-KR by an un-modernized peer. If the server cluster
  ever talks to a legacy client, this is a protocol break.

### R3 — Bleeding VFX gated on low HP (was task #139)

- [ ] The "bleeding" particle/overlay effect currently shows all
      the time on players and monsters. Gate it to HP ≤ ~25%.

**Background.** User reported the effect fires continuously on
every living entity, whereas retail gameplay only showed it on
critically-wounded targets.

**Scope.**
- Find the bleeding-effect trigger in `MEffectManager` /
  `MCreature::Draw` (exact location TBD — audit pending).
- Add an `HP / HP_MAX <= 0.25` check at the trigger site.
- Verify the gate applies to both player characters and
  mobs, and that it updates as HP changes during combat.

**Non-goals.**
- Reworking the effect system itself. We just want a gate.
- Making the threshold configurable. Lock it at 25% until
  someone asks for a slider.

**Gotchas.**
- Some boss mobs may have been authored to always bleed as
  a visual identifier. If R3 accidentally hides that, the
  fix is a per-mob override, not lowering the threshold.

### R4 — Strip `dk2th.com` and private-server branding (was task #140)

- [ ] Remove third-party server branding that retail data ships
      baked into item textures, UI splashes, and in-game text
      (e.g. `www.dk2th.com` watermarks on item sprites).

**Background.** The retail data files we extract under
`Darkeden data/` are from a private server and carry that
server's branding. Some of it is in string tables (easy to
edit); some is baked into bitmap art (requires image editing).

**Scope.**
- String-table scrub: grep for `dk2th`, `.com`, `Dark Eden
  2th` etc. across NPC dialog, item descriptions, system
  messages, UI text.
- Bitmap scrub: identify sprite/bitmap files with overlaid
  URL watermarks. For each, either source a clean
  equivalent, re-master from a higher-quality original, or
  mask the watermark with a solid colour that matches the
  surrounding art.
- Splash/loading screens and login-screen art specifically
  tend to carry the biggest branding — prioritize those.
- Website / URL references in the client binary itself
  (any `http://...` literals in string tables).

**Non-goals.**
- Re-branding to a new server name. We're stripping, not
  renaming.
- Legal review of what "cleaned" data can be redistributed.
  Separate conversation.

**Gotchas.**
- Some branding is in the server-side database (item
  description fields loaded at runtime). A client-side
  string scrub won't catch it; the server data needs its
  own pass.

### R5 — Standalone Windows .exe for friends (was task #141)

- [ ] Ship a self-contained Windows client build that a non-
      technical friend can unzip and run, without needing WSL,
      MSYS2, or a compiler.

**Background.** The current build process requires WSL2 +
Ubuntu 22.04 + a full gcc/cmake/SDL2 toolchain. That's fine
for us; it's not fine for "hey, come play on my server this
weekend."

**Scope.**
- Pick a Windows build target. Two realistic options:
  1. **MinGW-w64 cross-compile from Linux** — reuses our
     existing build scripts; produces a native `.exe`
     with bundled MinGW runtime DLLs. Cleanest story.
  2. **Native MSVC build inside Visual Studio** — matches
     how the original codebase was built in 2004 but
     means maintaining a second set of project files.
- Dependency bundling: SDL2.dll, SDL2_ttf.dll, MySQL
  client DLL (if the client reaches MySQL directly — it
  shouldn't, but verify), any C runtime DLLs the chosen
  toolchain requires.
- Packaging: a plain .zip with `DarkEden.exe` +
  `Darkeden data/` + required DLLs + a `README.txt`
  pointing at the server address. Installer is overkill
  for friends-and-family distribution.
- Auto-connect config: `ServerInfo.inf` pre-filled with
  your public IP / DNS name, so the friend unzips and
  clicks play.

**Blockers / risks.**
- SpriteLib / VS_UI haven't been smoke-tested on a Windows
  build this cycle. Phase 5 dropped some Windows-only code
  paths; a cross-compile attempt may surface new compile
  errors analogous to the Bug PP–WW set.
- Firewall / NAT traversal on your end. Playing with
  friends over the internet needs either a public IP or
  a VPN / hole-punch. This is a deployment concern, not
  a client concern, but the .exe is useless if nobody can
  reach your server.
- The game is 2000s-era — expect Windows Defender to flag
  an unsigned executable. Code signing ($$$) or a clear
  "it's not a virus" note in the README.

**Non-goals.**
- macOS / Linux distributables. Native-build-from-source
  is fine for those platforms today.
- Auto-update infrastructure. Manual zip re-download is
  fine at this scale.

**MVP definition.** A .zip that a Windows 10/11 user can
download, unzip, and double-click `DarkEden.exe` to reach
the login screen of your server. Enter username/password,
pick a character, play.

## Cross-cutting order

There's no strict order between R1–R5, but a couple of
soft dependencies:

- R4 (strip branding) and R2 (English-only) overlap on
  string-table work. Doing them in the same pass saves
  one full grep-and-edit cycle over the data files.
- R5 (Windows .exe) wants a stable tree. Ideally land R2
  / R3 / R4 first so friends download a polished client
  rather than one that will visibly change next week.
- R1 (UHD) is the biggest engineering lift. Sequence it
  after R2–R4 unless someone has specific time to spend
  on rendering.

## How this file relates to the existing task tracker

Each item above has a corresponding task ID in the session's
task tracker (R1 = pending / to be created, R2 = #138, R3 =
#139, R4 = #140, R5 = #141). Use this file for narrative and
scope; use the task tracker for day-to-day progress checkoffs.
