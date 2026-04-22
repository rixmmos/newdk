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
    CGBuyStoreItem.cpp
    CGGQuestCancel.cpp
    CGUndisplayItem.cpp
    CGStoreOpen.cpp
    CGStoreClose.cpp
    CGStoreSign.cpp
    CGRequestStoreInfo.cpp
    CGDisplayItem.cpp
    CGShopRequestBuy.cpp
    CGShopRequestList.cpp
    CGShopRequestSell.cpp
    CGStashDeposit.cpp
    CGStashList.cpp
    CGStashRequestBuy.cpp
    CGStashToMouse.cpp
    CGMouseToStash.cpp
    CGStashWithdraw.cpp
    CGTradePrepare.cpp
    CGTradeAddItem.cpp
    CGTradeRemoveItem.cpp
    CGTradeMoney.cpp
    CGTradeFinish.cpp
    CGAddSMSAddress.cpp
    CGDeleteSMSAddress.cpp
    CGSMSAddressList.cpp
    CGAcceptUnion.cpp
    CGDenyUnion.cpp
    CGQuitUnionAccept.cpp
    CGQuitUnionDeny.cpp
    CGRequestUnion.cpp
    CGRequestUnionInfo.cpp
    CGQuitUnion.cpp
    CGExpelGuild.cpp
    CGRequestGuildList.cpp
    CGAppointSubmaster.cpp
    CGModifyTaxRatio.cpp
    CGRangerSay.cpp
    CGRequestGuildMemberList.cpp
    CGModifyGuildIntro.cpp
    CGModifyGuildMemberIntro.cpp
    CGModifyGuildMember.cpp
    CGSelectGuild.cpp
    CGSelectGuildMember.cpp
    CGSelectBloodBible.cpp
    CGSelectNickname.cpp
    CGSelectPortal.cpp
    CGSelectQuest.cpp
    CGSelectRegenZone.cpp
    CGSelectTileEffect.cpp
    CGSelectWayPoint.cpp
    CGDepositPet.cpp
    CGWithdrawPet.cpp
    CGResurrect.cpp
    CGTameMonster.cpp
    CGTryJoinGuild.cpp
    CGJoinGuild.cpp
    CGRegistGuild.cpp
    CGGuildChat.cpp
    CGWithdrawTax.cpp
    CGAddGearToMouse.cpp
    CGAddInventoryToMouse.cpp
    CGAddMouseToGear.cpp
    CGAddMouseToInventory.cpp
    CGAddMouseToQuickSlot.cpp
    CGAddMouseToZone.cpp
    CGAddQuickSlotToMouse.cpp
    CGAddZoneToInventory.cpp
    CGAddZoneToMouse.cpp
    CGRequestNewbieItem.cpp
    CGRequestPowerPoint.cpp
    CGRequestIP.cpp
    CGRequestInfo.cpp
    CGAuthKey.cpp
    CGCrashReport.cpp
    CGDialUp.cpp
    CGPhoneDisconnect.cpp
    CGPhoneSay.cpp
    CGFailQuest.cpp
    CGGQuestAccept.cpp
    CGLogout.cpp
    CGDownSkill.cpp
    CGConnect.cpp
    CGDissectionCorpse.cpp
    CGDonationMoney.cpp
    CGGetOffMotorCycle.cpp
    CGLotterySelect.cpp
    CGModifyNickname.cpp
    CGPortCheck.cpp
    CGNPCAskAnswer.cpp
    CGPetGamble.cpp
    CGPickupMoney.cpp
    CGReady.cpp
    CGAddItemToItem.cpp
    CGAddItemToCodeSheet.cpp
    CGGlobalChat.cpp
    CGPartyPosition.cpp
    CGPartySay.cpp
    CGPartyInvite.cpp
    CGPartyLeave.cpp
    CGSetSlayerHotKey.cpp
    CGSetVampireHotKey.cpp
    CGSelectRankBonus.cpp
    CGReloadFromInventory.cpp
    CGReloadFromQuickSlot.cpp
    CGUseBonusPoint.cpp
    CGUseItemFromGear.cpp
    CGUseItemFromInventory.cpp
    CGUsePotionFromInventory.cpp
    CGUsePotionFromQuickSlot.cpp
    CGUsePowerPoint.cpp
    CGUseMessageItemFromInventory.cpp
    CGUseItemFromGQuestInventory.cpp
    CGSkillToInventory.cpp
    CGSkillToNamed.cpp
    CGSkillToObject.cpp
    CGSkillToSelf.cpp
    CGSkillToTile.cpp
    CGBloodDrain.cpp
    CGAttack.cpp
    CGAbsorbSoul.cpp
    CGCastingSkill.cpp
    CGDropMoney.cpp
    CGLearnSkill.cpp
    CGSay.cpp
    CGTypeStringList.cpp
    CGThrowBomb.cpp
    CGThrowItem.cpp
    CGMakeItem.cpp
    CGConnectSetKey.cpp
    CGGetEventItem.cpp
    CGMixItem.cpp
    CGNPCTalk.cpp
    CGRequestRepair.cpp
    CGRelicToObject.cpp
    CGRideMotorCycle.cpp
    CGSilverCoating.cpp
    CGSubmitScore.cpp
    CGTakeOutGood.cpp
    CGUnburrow.cpp
    CGUntransform.cpp
    CGVisible.cpp
    CGVerifyTime.cpp
    CGWhisper.cpp
    CGExchangeBuy.cpp
    CGExchangeList.cpp
    CGMove.cpp
    CGSMSSend.cpp
)

set(_SHARED_PACKETS_GC_SOURCES
    GCMyStoreInfo.cpp
    GCOtherStoreInfo.cpp
    GCRemoveStoreItem.cpp
    GCAddStoreItem.cpp
)

# CL Packets (Client → Login) — migrated from
# dkrixserver/src/Core/CL*.cpp / dkrix/Client/Packet/Cpackets/CL*.cpp
# Split out into its own group (and its own INTERFACE target below) so
# LoginServerPackets absorbs the CL sources with __LOGIN_SERVER__ while
# GameServerPackets stays CG-only. Previously these sat inside the CG
# list, which left LoginServerPackets without CL vtables and produced
# "undefined reference to `vtable for CLLogin'" etc. at link time.
set(_SHARED_PACKETS_CL_SOURCES
    CLChangeServer.cpp
    CLGetPCList.cpp
    CLGetServerList.cpp
    CLGetWorldList.cpp
    CLLogout.cpp
    CLSelectServer.cpp
    CLSelectWorld.cpp
    CLVersionCheck.cpp
    CLCreatePC.cpp
    CLDeletePC.cpp
    CLLogin.cpp
    CLQueryCharacterName.cpp
    CLQueryPlayerID.cpp
    CLReconnectLogin.cpp
    CLRegisterPlayer.cpp
    CLSelectPC.cpp
    # CGConnectSetKey is registered in PacketFactoryManager.cpp under
    # BOTH __LOGIN_SERVER__ and __GAME_SERVER__ guards — the login
    # server sees CGConnectSetKey as a pre-auth handshake packet. It
    # already compiles into GameServerPackets via the CG list; this
    # duplicate entry makes it ALSO compile into LoginServerPackets so
    # the vtable is visible when PacketFactoryManager.cpp instantiates
    # CGConnectSetKeyFactory under __LOGIN_SERVER__. Separate archives,
    # so no ODR collision at final-link time.
    CGConnectSetKey.cpp
)

# Future families will add their own groups here, parallel to the
# naming in dkrixserver/src/Core/CMakeLists.txt:
#   set(_SHARED_PACKETS_LC_SOURCES ...)
#   set(_SHARED_PACKETS_GS_SOURCES ...)
# etc. Keep the family breakdown aligned so it is obvious which
# server variant needs to link which source group.

# Build the absolute-path source lists. INTERFACE target_sources under
# CMP0076 NEW accepts absolute paths; relative paths would be
# interpreted against each caller's CMAKE_CURRENT_SOURCE_DIR, which
# would break once dkrix and dkrixserver call add_subdirectory from
# different parent directories.
set(_SHARED_PACKETS_SOURCES "")
foreach(_src IN LISTS _SHARED_PACKETS_CG_SOURCES)
    list(APPEND _SHARED_PACKETS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${_src}")
endforeach()
foreach(_src IN LISTS _SHARED_PACKETS_GC_SOURCES)
    list(APPEND _SHARED_PACKETS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${_src}")
endforeach()

set(_SHARED_PACKETS_CL_SOURCES_ABS "")
foreach(_src IN LISTS _SHARED_PACKETS_CL_SOURCES)
    list(APPEND _SHARED_PACKETS_CL_SOURCES_ABS "${CMAKE_CURRENT_SOURCE_DIR}/${_src}")
endforeach()

add_library(shared_packets INTERFACE)

target_sources(shared_packets INTERFACE ${_SHARED_PACKETS_SOURCES})

target_include_directories(shared_packets INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Separate INTERFACE target for the CL family so LoginServerPackets
# can absorb just those sources (not the whole CG/GC set) and compile
# them under __LOGIN_SERVER__. See comment above.
add_library(shared_packets_cl INTERFACE)

target_sources(shared_packets_cl INTERFACE ${_SHARED_PACKETS_CL_SOURCES_ABS})

target_include_directories(shared_packets_cl INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
