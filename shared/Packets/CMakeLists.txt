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
# One INTERFACE library PER PACKET FAMILY (shared_packets_cg, _gc, _cl, _lc,
# ...) rather than one combined "shared_packets" target covering every
# family. Each caller absorbs a family's .cpp sources into ITS OWN
# compilation unit set, picking up its own __GAME_SERVER__ / __LOGIN_SERVER__
# / __SHARED_SERVER__ (server) or bare (client) defines — that part is
# unchanged from the original design. What changed (Wave 1, this commit):
# CLGetWorldList (the pilot) was the only migrated pair when this was a
# single combined target, so LoginServerPackets (CL's owner) was the only
# server target that ever linked it "for real" — GameServerPackets and
# SharedServerPackets only used it for header visibility. Wave 1 adds the
# first CG-family pairs, owned by a DIFFERENT server target
# (GameServerPackets). A single combined target_sources would make it
# impossible for GameServerPackets to link "just its own family": linking
# the combined library would also absorb CL's CLGetWorldList.cpp as one of
# GameServerPackets' own compiled sources (and vice versa, LoginServerPackets
# would absorb CG's sources) — each family's *Handler.cpp only exists in its
# owning server target, so an absorbed foreign-family .cpp's handler-dispatch
# call has no definition to link against. Splitting by family means each
# server target links only the family it owns; PacketFactoryManager.cpp's
# unconditional #include of every packet header (regardless of family) is
# still satisfied because every per-family library's include directory is
# the same shared/Packets root.
#
# CLGetWorldList remains the CL pilot pair. Wave 1 batch 1 adds the first
# CG-family pairs. The rest of the ~62 style-only pairs land in later,
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
# shared_packets_cl is always defined below (even when its source list is
# empty), so it's a stable marker for "this file already ran".
if(TARGET shared_packets_cl)
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
set(_SHARED_PACKETS_CG_SOURCES # Client -> Game
    CGAcceptUnion.cpp
    CGAddItemToCodeSheet.cpp
    CGAddItemToItem.cpp
    CGAppointSubmaster.cpp
    CGAuthKey.cpp
    CGBuyStoreItem.cpp
    CGDenyUnion.cpp
    CGDisplayItem.cpp
    CGDownSkill.cpp
    CGExpelGuild.cpp
    CGFailQuest.cpp
    CGGQuestAccept.cpp
    CGGQuestCancel.cpp
    CGGlobalChat.cpp
    CGLogout.cpp
    CGLotterySelect.cpp
    CGModifyGuildIntro.cpp
    CGModifyGuildMember.cpp
    CGModifyGuildMemberIntro.cpp
    CGModifyTaxRatio.cpp
    CGNPCAskAnswer.cpp
    CGPartyLeave.cpp
    CGPortCheck.cpp
    CGQuitUnion.cpp
    CGQuitUnionAccept.cpp
    CGQuitUnionDeny.cpp
    CGReady.cpp
    CGRequestGuildList.cpp
    CGRequestGuildMemberList.cpp
    CGRequestInfo.cpp
    CGRequestStoreInfo.cpp
    CGRequestUnion.cpp
    CGResurrect.cpp
    CGSMSAddressList.cpp
    CGSelectBloodBible.cpp
    CGSelectGuild.cpp
    CGSelectGuildMember.cpp
    CGSelectNickname.cpp
    CGSelectQuest.cpp
    CGSelectRegenZone.cpp
    CGSelectTileEffect.cpp
    CGSetSlayerHotKey.cpp
    CGSetVampireHotKey.cpp
    CGSilverCoating.cpp
    CGStoreClose.cpp
    CGStoreOpen.cpp
    CGStoreSign.cpp
    CGThrowBomb.cpp
    CGUndisplayItem.cpp
    CGUseItemFromGQuestInventory.cpp
    CGWithdrawPet.cpp
)
set(_SHARED_PACKETS_GC_SOURCES) # Game -> Client
set(_SHARED_PACKETS_CL_SOURCES  # Client -> Login
    CLGetWorldList.cpp
)
set(_SHARED_PACKETS_LC_SOURCES) # Login -> Client

# ----------------------------------------------------------------------
# One INTERFACE library per family. A family with an empty source list
# still gets a target (with no target_sources call) so consumers can
# link it unconditionally as soon as it's ever populated, and so the
# guard above has a stable target to check.
# ----------------------------------------------------------------------
function(_add_shared_packets_family _family)
    set(_target "shared_packets_${_family}")
    add_library(${_target} INTERFACE)
    if(ARGN)
        set(_abs_sources)
        foreach(_src IN LISTS ARGN)
            list(APPEND _abs_sources "${CMAKE_CURRENT_SOURCE_DIR}/${_src}")
        endforeach()
        target_sources(${_target} INTERFACE ${_abs_sources})
    endif()
    target_include_directories(${_target} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
endfunction()

_add_shared_packets_family(cg ${_SHARED_PACKETS_CG_SOURCES})
_add_shared_packets_family(gc ${_SHARED_PACKETS_GC_SOURCES})
_add_shared_packets_family(cl ${_SHARED_PACKETS_CL_SOURCES})
_add_shared_packets_family(lc ${_SHARED_PACKETS_LC_SOURCES})
