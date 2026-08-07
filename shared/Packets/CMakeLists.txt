# ============================================================================
# shared/Packets — canonical packet-class tree (Phase 12)
# ============================================================================
#
# Neutral third top-level directory that dkrix/ and dkrixserver/ both reach
# into symmetrically via add_subdirectory(). See README.md for the full
# rationale (why not parked on either sibling tree) and the per-pair
# migration recipe; docs/MODERNIZATION.md's Phase 12 section records the
# decision.
#
# Exposed as an INTERFACE library so each caller absorbs the .cpp sources
# into ITS OWN compilation unit set, picking up its own __GAME_SERVER__ /
# __LOGIN_SERVER__ / __SHARED_SERVER__ (server) or bare (client) defines.
# A plain STATIC library would only ever get one set of defines baked in —
# wrong for every consumer but the first to link it.
#
# CLGetWorldList is the pilot pair (first class migrated through this
# mechanism on main). The rest of the ~62 style-only pairs land in later,
# separately-reviewed batches per docs/MODERNIZATION.md's Phase 12 plan —
# this file is not meant to gain many entries per commit.
# ============================================================================

# Force CMP0076 NEW (absolute-path target_sources() on INTERFACE targets)
# regardless of the including project's own cmake_minimum_required — dkrix's
# floor is 3.12, below where CMP0076 defaults to NEW. Relative source paths
# would resolve against each CALLER's CMAKE_CURRENT_SOURCE_DIR and break as
# soon as dkrix/ and dkrixserver/ add_subdirectory() this from different
# parents; absolute paths side-step that entirely.
cmake_minimum_required(VERSION 3.13)

# Guard against being add_subdirectory()'d twice — harmless today (each
# tree only pulls this in once), but keeps the door open if a future
# combined build ever puts dkrix/ and dkrixserver/ under one superproject.
if(TARGET shared_packets)
    return()
endif()

# ----------------------------------------------------------------------
# Per-family source lists, aligned with the family breakdown in
# dkrixserver/src/Core/CMakeLists.txt (CG/GC/CL/LC/GS/SG/GG/GM). Keeping
# them separate — even while empty — makes it obvious at a glance which
# server target(s) a future addition needs linked (absorbing the .cpp) vs.
# merely include-only (PacketFactoryManager.cpp #includes every packet
# header unconditionally from all three server packet libraries; only the
# owning library should compile the .cpp).
# ----------------------------------------------------------------------
set(_SHARED_PACKETS_CG_SOURCES) # Client -> Game
set(_SHARED_PACKETS_GC_SOURCES) # Game -> Client
set(_SHARED_PACKETS_CL_SOURCES  # Client -> Login
    CLGetWorldList.cpp
)
set(_SHARED_PACKETS_LC_SOURCES) # Login -> Client

set(_SHARED_PACKETS_SOURCES
    ${_SHARED_PACKETS_CG_SOURCES}
    ${_SHARED_PACKETS_GC_SOURCES}
    ${_SHARED_PACKETS_CL_SOURCES}
    ${_SHARED_PACKETS_LC_SOURCES}
)

add_library(shared_packets INTERFACE)

if(_SHARED_PACKETS_SOURCES)
    set(_SHARED_PACKETS_ABS_SOURCES)
    foreach(_src IN LISTS _SHARED_PACKETS_SOURCES)
        list(APPEND _SHARED_PACKETS_ABS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${_src}")
    endforeach()
    target_sources(shared_packets INTERFACE ${_SHARED_PACKETS_ABS_SOURCES})
endif()

target_include_directories(shared_packets INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
