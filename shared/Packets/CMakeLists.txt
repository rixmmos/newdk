# ============================================================================
# shared/Packets — canonical packet-class tree
# ============================================================================
#
# Phase 12 migration landing pad. Packet classes from
# dkrixserver/src/Core/ and dkrix/Client/Packet/Cpackets/ move here as
# they are reconciled. See README.md for the per-PR migration recipe.
#
# Exposed as an INTERFACE library so callers absorb the .cpp sources
# into their OWN compilation units — each caller then gets its own
# __GAME_SERVER__ / __LOGIN_SERVER__ / __SHARED_SERVER__ define applied
# at build time. A plain STATIC library would only pick up one set of
# defines and would break the multi-server compilation pattern that
# dkrixserver/src/Core/CMakeLists.txt already uses for the other
# packet families.
#
# Consumers:
#   - dkrixserver: link into GameServerPackets / LoginServerPackets /
#     SharedServerPackets target_link_libraries as appropriate for the
#     packet family (CG/GC → game only; CL/LC → login only; GS/SG →
#     game + shared; GL/LG → all three).
#   - dkrix: link into the DarkEden target (client consumes every
#     family that carries a client-facing representation).
#
# Minimum CMake: 3.13 (for INTERFACE target_sources with absolute paths
# under CMP0076 NEW; matches parent projects' 3.20 / 3.12 floors).
# ============================================================================

cmake_minimum_required(VERSION 3.13)

# Include guard — make add_subdirectory(...) from both dkrix/ and
# dkrixserver/ safe even if a future combined build puts them under
# one superproject. Both trees currently build separately, but this
# keeps the door open.
if(TARGET shared_packets)
    return()
endif()

# CG Packets (Client → Game) — migrated from
# dkrixserver/src/Core/CG*.cpp / dkrix/Client/Packet/Cpackets/CG*.cpp
set(_SHARED_PACKETS_CG_SOURCES
    CGStoreOpen.cpp
    CGStoreClose.cpp
)

# Future families will add their own groups here, parallel to the
# naming in dkrixserver/src/Core/CMakeLists.txt:
#   set(_SHARED_PACKETS_GC_SOURCES ...)
#   set(_SHARED_PACKETS_CL_SOURCES ...)
# etc. Keep the family breakdown aligned so it is obvious which
# server variant needs to link which source group.

# Build the absolute-path source list. INTERFACE target_sources under
# CMP0076 NEW accepts absolute paths; relative paths would be
# interpreted against each caller's CMAKE_CURRENT_SOURCE_DIR, which
# would break once dkrix and dkrixserver call add_subdirectory from
# different parent directories.
set(_SHARED_PACKETS_SOURCES "")
foreach(_src IN LISTS _SHARED_PACKETS_CG_SOURCES)
    list(APPEND _SHARED_PACKETS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${_src}")
endforeach()

add_library(shared_packets INTERFACE)

target_sources(shared_packets INTERFACE ${_SHARED_PACKETS_SOURCES})

target_include_directories(shared_packets INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
