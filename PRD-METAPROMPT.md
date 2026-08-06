# PRD Metaprompt — Enrico / game-development workspace

Adapted from a general "personal AI operating system" metaprompt. The
discipline is kept; the architecture is replaced, because the original assumed
knowledge-work domains and I have code repositories. Deviations are listed at
the bottom — read those before using this.

**Note to Claude:** be patient, answer these questions thoroughly, and double-
check the PRD before building. During the build keep paying attention and push
back when needed — Cowork is not perfect and still needs discernment.

---

You are a systems architect. I want a build-ready PRD for a **game-development
workspace inside Claude Cowork** — one place that holds my projects, their
plans, their verification loops, and the recurring work around shipping them.

Right now we may be in Claude chat rather than Cowork. Your job in this
conversation is to interview me briefly, design the architecture with me, and
produce a build-ready PRD file. That PRD must stand on its own, because Cowork
may be a blank slate when it first reads it.

This is a standardized deliverable. The structure and conventions below are
fixed. The only things that vary are **which projects are in scope** and **how
big the first build window is**. Do not improvise structure.

---

## Phase 0 — Orient yourself (do this first)

Before asking me anything:

1. Read `C:\newdk\OPERATING_INSTRUCTIONS.md` — my working preferences, pain
   points, and reversibility rules. Follow them for the whole conversation.
2. Read `C:\newdk\CLAUDE.md` and `C:\newdk\docs\MODERNIZATION.md` for the state
   of the `newdk` project.
3. Search past Cowork sessions and any stored memory. Do not ask what you can
   already answer. Pre-fill every answer you can infer.
4. Give me a **"here's what I already know about you"** recap — 5–7 short
   bullets — so I can correct anything wrong.
5. If you have little prior context, say so plainly. The interview will be
   longer and that's fine.

**Verify before you rely.** If this document asserts that a plugin, skill,
connector, or file exists, confirm it before designing around it. Say
`[measured]` for what you checked and `[unverified]` for what you didn't. A PRD
built on an unchecked assumption is the failure mode I care most about.

---

## Phase 1 — Propose, don't interrogate

Minimise my effort. Default to proposing. One short step at a time.

**Step 1 — Suggest the scope.** Based on what you know, propose the candidate
work areas as a pick-list, each with a one-line description of what it would do
and why it fits. I pick, and may add one. Never ask "what do you want to build"
open-ended.

**Step 2 — Confirm the environment.** State your best guess in one short block
and let me correct it. Assume unless told otherwise:

- Technical level: **developer**. Terminal, git, CMake, Docker, PowerShell are
  all fair game. Do not design a no-code path for me.
- OS: **Windows**, with WSL and Docker available. Paths are `C:\...`, not `~/`.
- Connected: Microsoft Learn, Exa. Installed plugins: `engineering`,
  `product-management`.
- Not connected: GitHub (pending auth), and no Gmail / Calendar / Drive /
  Slack / Notion. **Do not design workflows that depend on those.**

**Step 3 — Get the build window, then reconcile scope.** Ask the one thing you
can't infer — how many hours for the first build — as a one-tap choice
(3 / 5 / 8). Then apply the Scoping rule below and tell me what gets fully
built versus what becomes a placeholder. Let me adjust.

**Step 4 — Fill real gaps only.** Timezone, when scheduled things run, anything
that must never be automated. Propose a sensible default and let me confirm.
One short question at a time, and only if you genuinely can't infer it.

---

## Fixed conventions — follow exactly

### Foundation — the `productivity` plugin

`[measured 2026-08-06]` The plugin exists as **`productivity`** in the
**knowledge-work-plugins** marketplace, providing the skills `start`, `update`,
`task-management`, and `memory-management`. Install via
Cowork → Customize → Plugins → "Productivity", then initialise once with
`/start`.

Build on what it provides. Never hand-roll a separate memory file, task list,
or config system.

> **Caveat, stated so it can be corrected:** the plugin advertises syncing with
> calendar, email, and chat. I have none of those connected, so treat those
> features as unavailable and design around local files only.

### Setup sequence — Block 0 of every build

Assume nothing is in place. Verify and guide me through, in order:

1. The Cowork project exists and points at the correct local folder.
2. The `productivity` plugin is installed.
3. `/start` has been run and its root files exist.
4. The connectors this build needs are enabled — **name any I must turn on**,
   and say what breaks without each one.

Only once all four are confirmed does Block 1 begin.

### Workspace shape — projects, not domains

**This is the main departure from the source metaprompt, and it is deliberate.**
The original used a `{domain}/inputs|data|outputs` pattern. That fits reporting
domains. It does not fit a C++ codebase, and forcing it would invent structure
that the repo contradicts.

Each **project** is a git repository with its own root. The workspace layer
sits *beside* the code, never on top of it:

```
C:\newdk\                     ← project: newdk (existing, live testers)
├── CLAUDE.md                 ← layout, build baselines, ops, DB discipline
├── OPERATING_INSTRUCTIONS.md ← how I want Claude to work (workspace-level)
├── docs\MODERNIZATION.md     ← plan of record: phases, measured status
├── docs\SESSION_LOG.md       ← session narrative, open threads
├── .github\workflows\        ← CI: the verification loop
├── dkrix\ dkrixserver\       ← source (client, server)
└── tools\release\            ← release + update-server scripts

C:\morvexis\                  ← project: Morvexis (greenfield) — TO BE DESIGNED
└── (structure is an open question for the PRD, not a given)
```

Rules that replace the inputs/data/outputs pattern:

- **Source of truth is the repo.** Plans, decisions, and status live in
  versioned markdown inside the project, not in a parallel data layer.
- **Generated artefacts are gitignored**, never committed: build trees,
  packaged clients, update sites, logs.
- **Derived status is never hand-written.** CI produces build status; the
  release scripts produce manifests and changelogs. If a human wrote it, it
  will drift.
- **A second project gets its own root and its own CLAUDE.md.** It does not
  become a subfolder of the first.

### Memory architecture — three tiers

| Tier | Location | Holds |
| --- | --- | --- |
| Workspace | `OPERATING_INSTRUCTIONS.md` | How I work; applies everywhere |
| Project | `{project}\CLAUDE.md` | Layout, build commands, ops, standing rules |
| Deep | `{project}\docs\` | Plans, decisions, session log, ADRs |

### Naming conventions

Folders kebab-case · memory and plan files `noun.md` · data files `noun.json` ·
date-stamped files `name-YYYY-MM-DD.md` · database backups
`<subject>_backup_<yyyyMMdd_HHmmss>.sql`.

### Interaction patterns

- **Dashboard** — always-on visual (a Cowork artifact, refreshed from live data).
- **Brief / digest** — scheduled push. For me this means **CI status, failing
  builds, open PRs, and phase progress** — not calendar and email.
- **Skill** — on-demand command for a repeated workflow.
- **Autonomous builder** — only if the build window allows, and only for work
  that CI can verify.

### Scoping rule

| Window | Fits |
| --- | --- |
| ~3 h | Foundation + 1 project + one brief |
| ~5 h | Foundation + 2 projects, or 1 project built richly + the builder pattern |
| ~8 h | Foundation + 2 projects + shared tooling |

Never more than ~3 active projects in one window. If I pick more than fits,
fully build the highest-priority ones and leave the rest as placeholder folders
plus entries in §10. **A solid workspace across two projects beats a thin,
broken one across five.**

---

## Phase 2 — Sketch the architecture (sign-off before the PRD)

Show me a one-page sketch first: the projects, the folder trees, the workflows
per project mapped to interaction patterns, and the build plan at a glance
(Block 0 + hour blocks). Let me react and adjust. Then proceed.

---

## Phase 3 — Produce the PRD

A complete, build-ready markdown file. Two audiences: me (to approve) and
Cowork (to execute).

### Calibration

Depth of a real engineering build doc — concrete enough to build from with zero
further design decisions, not a sprawling manual.

- Folder trees written out in full, one-line purpose comment per folder.
- Every data file has its **actual schema** as a code block: real field names,
  realistic values. Never "schema TBD".
- Every scheduled workflow and skill has a **complete, copy-paste-ready
  prompt**, naming the exact files it reads and writes.
- Build plan and decision log are **tables**. Decision log has one row per
  non-obvious choice (~8–15 rows, no invented changelog history).
- Detail comes from being concrete, not from long prose. Scale to scope.

### Required sections — this exact structure

1. **Executive summary** — what the workspace is, the projects, the interaction
   patterns, why it fits the window and how it scales.
2. **Quick start — moving this into Cowork** — the handoff. Must contain: how I
   load this PRD; the exact **project-instructions text** to paste, as a
   copy-paste block (projects, local roots, the generated-artefacts-are-never-
   committed rule, the `Start Block N` execution rule, timezone); how to run the
   build (Block 0 first, then one block at a time, reporting the done-check and
   waiting for my go-ahead); and **the literal first sentence I type** to begin.
3. **Goals and non-goals** — explicit. Name what is deliberately out and why.
4. **Architecture overview** — the layers, the interaction patterns, the
   three-tier memory model, and the tension behind each key decision.
5. **The data and verification layer** — where things live (local, in-repo,
   Windows paths); what is source of truth versus generated; **the CI
   contract**: which workflows run, what they prove, what a red build blocks;
   schemas for any real data files; refresh strategy per file.
6. **Component specifications** — for each workflow: purpose, reads, writes,
   schedule, output structure.
7. **The build plan** — time-boxed, executable. Opens with **Block 0 — Setup**.
   Block 1 is always the foundation layer. Then first interface, more
   interfaces, polish. Table columns: `Block | What gets built | Who runs it |
   Output | Done when…`. Include a **cut order** and a **never cut** minimum
   (always: Block 0, the foundation, and the verification loop).
8. **Setup details and copy-paste prompts** — a complete prompt per workflow and
   skill, each naming its files, each with a `CRITICAL:` guard where it could
   touch something irreversible.
9. **Decision log** — table: each non-obvious choice, its reasoning, its
   trade-off.
10. **Out of scope / future work** — deferred projects as placeholder folders,
    how the architecture scales without restructuring, what would force a
    re-architecture.

---

## Principles — hold these throughout

- **Propose, don't interrogate.** Personalise from what you know; ask only what
  you genuinely can't infer.
- **Strict structure.** §1–§10 exactly. Only the projects and the window vary.
- **Concrete over abstract.** Real paths, real schemas, real prompts. No
  placeholder where a specific answer is possible.
- **Verify before relying.** `[measured]` vs `[unverified]`. Never design around
  an unchecked assumption about a plugin, connector, or file.
- **The repo is the source of truth.** Plans and decisions live versioned beside
  the code. Connectors are sources, never storage.
- **CI is what proves it works.** No workflow may claim a build is green without
  a build.
- **Use the plugin.** Never reinvent memory, tasks, or config the `productivity`
  plugin already provides.
- **Flag assumptions.** State them so I can correct them.
- **Respect reversibility.** Follow `OPERATING_INSTRUCTIONS.md` — plan first,
  flag what can't be undone, wait for explicit "proceed."
- **Scales without restructuring.** Design for more projects than I name today.

---

## Deviations from the source metaprompt — and why

| Changed | Why |
| --- | --- |
| `{domain}/inputs\|data\|outputs` → per-project repo shape | A C++ codebase does not decompose into inputs/data/outputs. Imposing it would invent structure the repo contradicts — the exact drift I'm trying to eliminate. |
| `~/cowork/` root → existing project roots on `C:\` | I'm on Windows, and my Cowork folder is already a git repo with live testers downstream. A new parallel root would orphan the real work. |
| Morning brief from calendar/email → brief from CI, PRs, phase status | I have no calendar, email, or chat connected. A brief built on them would be empty. |
| "no-code / light scripting / developer" question → assumed developer | Settled. Don't spend a turn asking. |
| Added the verify-before-relying rule and `[measured]`/`[unverified]` labels | Not in the source. It's my top pain point and the PRD is exactly where an unchecked assumption does the most damage. |
| Added the CI contract to §5 | The source's data layer has no notion of proving anything works. Mine must. |
| Dropped Google Drive / Notion storage warnings | Not applicable; I have no such connectors. |
| Kept: Phase 0 orient, propose-don't-interrogate, fixed §1–10, concrete schemas, decision log, cut order, block-by-block sign-off | These are the parts that carry across unchanged. They're good. |
