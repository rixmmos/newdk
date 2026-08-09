# Why the server `build (ubsan)` CI leg is red

**Status:** diagnosed 2026-08-09, not fixed. The fix is a one-line change to
`dkrixserver/CMakeLists.txt`, which was outside the scope of the session that
diagnosed it.

**Scope of this note:** it explains one specific, reproducible link failure. It
does *not* claim the tree is otherwise UBSan-clean — no server binary has ever
been *run* under UBSan, so every runtime finding is still unknown.

## The symptom

`server.yml`'s `sanitizers` matrix leg `build (ubsan)` has failed on **every run
it has ever had** — server runs #15 (`eb9fd2d1`) through #31 (`cc4ea8ba`), 17 for
17 [measured 2026-08-09, job-level, off the GitHub Actions API]. It fails in its
own `Build (debug-ubsan)` step; `Verify binaries exist` is always skipped. This
includes the 13 runs where the plain `make debug (ubuntu)` job and the sibling
`build (asan)` leg were both green, so it is a leg-specific break and not the
tree being broken.

Two things made this hard to read from the outside and are worth recording:

- **Actions logs are not public even on a public repo.** Both
  `/actions/runs/{id}/logs` and `/actions/jobs/{id}/logs` return
  `403 Must have admin rights to Repository` unauthenticated, and the
  `/commit/{sha}/checks/{id}/logs` web route 404s. The check-run annotations
  *are* public but carry only `Process completed with exit code 2` — no compiler
  or linker text. Diagnosis requires either a signed-in `gh` or a local repro.
- **The duration is misleading.** On run #31 the ubsan leg ran 12m28s and the
  asan leg ran 12m27s, which reads like "ubsan fails at the very end". It does
  not. The failure is at roughly 26% of the build; `make` then prints
  `*** Waiting for unfinished jobs....` and the independent `gameserver` target
  keeps compiling to completion, so the wall-clock time looks like a full build.

## The failure

Reproduced locally 2026-08-09 [measured] — WSL Ubuntu 24.04, g++ 13.3.0, cmake
3.28.3, `make debug-ubsan` at `cd8c3c6`:

```
/usr/bin/ld: CMakeFiles/sharedserver.dir/Guild.cpp.o:(.data.rel+0xb98): undefined reference to `typeinfo for GCGuildMemberList'
/usr/bin/ld: CMakeFiles/sharedserver.dir/GuildManager.cpp.o:(.data.rel+0xf58): undefined reference to `typeinfo for GCWaitGuildList'
/usr/bin/ld: CMakeFiles/sharedserver.dir/GuildManager.cpp.o:(.data.rel+0x1118): undefined reference to `typeinfo for GCActiveGuildList'
collect2: error: ld returned 1 exit status
gmake[3]: *** [.../sharedserver.dir/build.make:263: .../bin/sharedserver] Error 1
```

It is a **link** failure of the `sharedserver` target only, and there are exactly
three undefined symbols. Nothing fails to compile.

## Which UBSan check fires, and why

`-fsanitize=undefined` is a group. One of its members is **`vptr`** — the check
that a polymorphic object's vpointer really points at the type the code thinks it
does. GCC implements it by emitting, at each instrumented access, a static
descriptor in `.data.rel` holding a pointer to the **RTTI object** for the static
type: `typeinfo for T`. That pointer is a hard link-time dependency that the
uninstrumented build simply does not have.

The three call sites, all in the sharedserver tree:

| Site | Symbol |
| --- | --- |
| `dkrixserver/src/server/sharedserver/Guild.cpp:893` — `Guild::makeMemberInfo(GCGuildMemberList&)` | `typeinfo for GCGuildMemberList` |
| `dkrixserver/src/server/sharedserver/GuildManager.cpp:439` — `GuildManager::makeWaitGuildList(GCWaitGuildList&, GuildRace_t)` | `typeinfo for GCWaitGuildList` |
| `dkrixserver/src/server/sharedserver/GuildManager.cpp:460` — `GuildManager::makeActiveGuildList(GCActiveGuildList&, GuildRace_t)` | `typeinfo for GCActiveGuildList` |

Each takes a **reference to a polymorphic class** (`class GCGuildMemberList :
public Packet`, `dkrixserver/src/Core/GCGuildMemberList.h:30`) and calls a member
on it — e.g. `gcGuildMemberList.addGuildMemberInfo(pGuildMemberInfo)` at
`Guild.cpp:910`. That member access through a polymorphic reference is what
`vptr` instruments.

The reason the symbol is missing rather than merely referenced is the **key
function** rule. `GCGuildMemberList` declares `~GCGuildMemberList();`
(`GCGuildMemberList.h:36`) — virtual through `Packet`, declared but not defined
inline. It is therefore the class's key function, and GCC emits the vtable *and*
the typeinfo in exactly one translation unit: `src/Core/GCGuildMemberList.cpp`.
That file is compiled into the **`GameServerPackets`** library
(`dkrixserver/src/Core/CMakeLists.txt:183`), and `sharedserver` does not link it —
it links `SharedServerPackets` (`src/server/sharedserver/CMakeLists.txt:43-51`).

So in an ordinary build sharedserver never needs anything out-of-line from these
classes. `addGuildMemberInfo` is defined inline in the header
(`GCGuildMemberList.h:69`), sharedserver only ever *fills in* a packet the
gameserver constructed, and it never constructs or destroys one — so it needs no
vtable, no typeinfo, and links clean. `-fsanitize=vptr` is what invents the
dependency. That is also why `asan` is unaffected: `-fsanitize=address` emits no
RTTI references.

## The fix

Turn `vptr` off for this tree, in `dkrixserver/CMakeLists.txt:72-75`:

```cmake
        if(USE_UBSAN)
            set(SANITIZER_FLAGS "${SANITIZER_FLAGS} -fsanitize=undefined -fno-sanitize=vptr")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined -fno-sanitize=vptr")
        endif()
```

Order matters and the negative flag must come **after** `-fsanitize=undefined` —
GCC applies these left to right, so the reverse order silently re-enables `vptr`.
For the same reason this cannot be worked around from `dkrixserver/Makefile` by
passing `-DCMAKE_CXX_FLAGS=-fno-sanitize=vptr`: CMake puts the cache value
*before* the appended `SANITIZER_FLAGS`, and the flag would be cancelled again.
The change has to be in `CMakeLists.txt`.

Why disable rather than satisfy the dependency: linking `GameServerPackets` into
`sharedserver` would satisfy the linker, but it would drag the entire gameserver
packet set into a binary that deliberately does not have it, purely to feed a
sanitizer. That inverts a real layering boundary for no runtime benefit.
`vptr` is also the least valuable member of the group here — it catches
bad-downcast/lifetime bugs, which this code expresses through hand-rolled packet
factories rather than through `dynamic_cast`, while the checks that would
actually pay off on 2000s-era C++ (signed overflow, shift, null deref, alignment,
bounds) are all still on.

If `vptr` coverage is wanted later, the clean route is to give these packet
classes an inline (or `= default`) destructor so they lose their key function and
GCC emits the vtable and typeinfo as COMDAT weak symbols in every TU that needs
them. That is a broad change across the packet families and is not worth doing
for this alone.

## What is still unverified

- Whether `sharedserver` is the *only* target with this problem. The local repro
  stops at the first link failure; `gameserver` and `loginserver` had not linked
  when the build aborted. There may be more `typeinfo for …` symbols behind this
  one. [unverified]
- Whether the tree links clean with `-fno-sanitize=vptr` applied. [unverified]
- What UBSan reports at *runtime*. No server binary has ever been executed under
  it, and the CI leg only builds and checks that the binaries exist — even fully
  green it proves the tree compiles and links under UBSan, nothing more.
