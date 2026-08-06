# Runbook — moving the workspace to `C:\dev\`

Target layout:

```
C:\dev\
├── OPERATING_INSTRUCTIONS.md   ← moves up: describes you, not newdk
├── PRD-METAPROMPT.md           ← moves up: applies to both projects
├── newdk\                      ← this repo, moved wholesale
└── morvexis\                   ← new, empty for now
```

The tree is now location-independent — every script derives its paths from
`$PSScriptRoot`, so nothing needs editing after the move. **Do the move before
the next release, not during one.**

---

## Before you start

Confirm the tree is clean and committed. From the current workspace root:

```powershell
git status --short          # expect no output
git log --oneline -1        # note the SHA, in case you need to come back
```

Close anything holding a file handle: Visual Studio, the running client, the
update server, VS Code, WSL shells sitting in the tree, and Cowork itself.
A locked `.git` file mid-move is the one way this gets messy.

---

## The move

```powershell
# 1. Make the parent
New-Item -ItemType Directory -Force -Path C:\dev | Out-Null

# 2. Move the repo (same volume, so this is instant and atomic)
Move-Item C:\newdk C:\dev\newdk

# 3. Lift the personal docs up one level — they describe you, not this project
Move-Item C:\dev\newdk\OPERATING_INSTRUCTIONS.md C:\dev\
Move-Item C:\dev\newdk\PRD-METAPROMPT.md         C:\dev\

# 4. Placeholder for the second project
New-Item -ItemType Directory -Force -Path C:\dev\morvexis | Out-Null
```

> **If `C:\dev` is on a different drive**, `Move-Item` copies rather than
> renames — ~4 GB and several minutes. Verify it completed before deleting
> anything.

---

## Re-point Cowork

Open Cowork and point the project's folder at **`C:\dev`** rather than
`C:\dev\newdk`. That way one session can see both projects and the personal
docs at the top level.

---

## Verify

```powershell
cd C:\dev\newdk

# Git survived
git status --short
git log --oneline -1                    # same SHA as before the move

# Nothing hardcodes the old path
Select-String -Path . -Pattern 'C:\\newdk' -Recurse `
  -Include *.ps1,*.md,*.yml,*.cmd,*.json | Select-Object -First 5
# expect: no results

# Scripts parse (this is the check I could not run — no PowerShell in my sandbox)
Get-ChildItem -Recurse -Filter *.ps1 -Path tools,dkrix\tools | ForEach-Object {
    $errs = $null
    [System.Management.Automation.Language.Parser]::ParseFile(
        $_.FullName, [ref]$null, [ref]$errs) | Out-Null
    if ($errs) { Write-Host "PARSE ERROR: $($_.FullName)" -ForegroundColor Red; $errs }
    else       { Write-Host "ok: $($_.Name)" }
}

# Release pipeline resolves correctly WITHOUT running a real release
powershell -File tools\release\New-DarkEdenRelease.ps1 -WhatIf 2>&1 | Select-Object -First 5
```

Then the real check — a build:

```powershell
cmake -S dkrix -B dkrix\build -G "Visual Studio 17 2022" -A x64 `
      -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build dkrix\build --config Debug
```

---

## What to watch for afterwards

| Thing | Why it might bite |
| --- | --- |
| `dkrix\build\`, `dkrixserver\build*\` | CMake caches store absolute paths. **Delete them and re-configure** — do not try to reuse. This is expected, not a failure. |
| `compile_commands.json` | Same reason. Regenerated on configure. |
| Visual Studio recent-projects, `.vs\` | Stale paths; reopen from the new location. |
| WSL paths | `/mnt/c/newdk/...` becomes `/mnt/c/dev/newdk/...` in any shell history or scripts of your own. |
| `dkrixserver\conf\*.conf` `HomePath` | Already wrong (points at `/home/genius/...`). Unrelated to the move, but fix it while you're in there. |
| Scheduled tasks / shortcuts | Anything you set up pointing at `C:\newdk`. |

The build directories are the only guaranteed casualty, and they're
regenerable by design — the root `.gitignore` already excludes them.

---

## Rolling back

Nothing here is destructive; the move is reversible:

```powershell
Move-Item C:\dev\newdk\..\OPERATING_INSTRUCTIONS.md C:\dev\newdk\
Move-Item C:\dev\newdk\..\PRD-METAPROMPT.md         C:\dev\newdk\
Move-Item C:\dev\newdk C:\newdk
```

The path-independence work is in git and survives either way — you can revert
the move without reverting the portability commit.
