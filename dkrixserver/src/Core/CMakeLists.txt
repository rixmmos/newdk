# ============================================================================
# Core  - 
# ============================================================================

set(CORE_SOURCES
    SXml.cpp
    WarInfo.cpp
    RaceWarInfo.cpp
    GuildWarInfo.cpp
    LevelWarInfo.cpp
    BloodBibleBonusInfo.cpp
    Assert.cpp
#    Assert1.cpp
    Datagram.cpp
    SerialDatagram.cpp
    DatagramSocket.cpp
    FileAPI.cpp
    GameTime.cpp
    ScriptParameter.cpp
    Player.cpp
    PCSlayerInfo.cpp
    PCVampireInfo.cpp
    PCSlayerInfo2.cpp
    PCVampireInfo2.cpp
    PCSlayerInfo3.cpp
    PCVampireInfo3.cpp
    Properties.cpp
    Resource.cpp
    ResourceManager.cpp
    ServerSocket.cpp
    Socket.cpp
    SocketAPI.cpp
    SocketImpl.cpp
    SocketInputStream.cpp
    SocketOutputStream.cpp
    StringStream.cpp
    Update.cpp
    UpdateManager.cpp
    InventoryInfo.cpp
    InventorySlotInfo.cpp
    RideMotorcycleInfo.cpp
    RideMotorcycleSlotInfo.cpp
    GearSlotInfo.cpp
    GearInfo.cpp
    ExtraInfo.cpp
    ExtraSlotInfo.cpp
    PCItemInfo.cpp
    SubItemInfo.cpp
    ModifyInfo.cpp
    ModifyItemInfo.cpp
    EffectInfo.cpp
    SlayerSkillInfo.cpp
    VampireSkillInfo.cpp
    SubSlayerSkillInfo.cpp
    SubVampireSkillInfo.cpp
    Utility.cpp
    VSDateTime.cpp
    ServerGroupInfo.cpp
    TimeChecker.cpp
    NPCInfo.cpp
    Shape.cpp
    Geometry.cpp
    WorldInfo.cpp
    GuildInfo.cpp
    GuildMemberInfo.cpp
    GuildInfo2.cpp
    GuildMemberInfo2.cpp
    Encrypter.cpp
    SocketEncryptInputStream.cpp
    SocketEncryptOutputStream.cpp
    OustersSkillInfo.cpp
    PCOustersInfo.cpp
    PCOustersInfo2.cpp
    PCOustersInfo3.cpp
    SubOustersSkillInfo.cpp
    ItemNameInfo.cpp
    SweeperBonusInfo.cpp
    PetInfo.cpp
    NicknameInfo.cpp
    QuestStatusInfo.cpp
    BloodBibleSignInfo.cpp
    StoreInfo.cpp
    md5.cpp
)

add_library(Core STATIC ${CORE_SOURCES})
target_include_directories(Core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/server/gameserver
    ${CMAKE_SOURCE_DIR}/src/server/gameserver/item
)

# ============================================================================
#  Packet 
# ============================================================================

# GC Packets (Game -> Client)
set(GC_PACKET_SOURCES
    GCActiveGuildList.cpp GCActiveGuildListHandler.cpp
    GCAddBat.cpp GCAddBatHandler.cpp
    GCAddBurrowingCreature.cpp GCAddBurrowingCreatureHandler.cpp
    GCAddEffect.cpp GCAddEffectHandler.cpp
    GCAddEffectToTile.cpp GCAddEffectToTileHandler.cpp
    GCAddGearToInventory.cpp GCAddGearToInventoryHandler.cpp
    GCAddGearToZone.cpp GCAddGearToZoneHandler.cpp
    GCAddHelicopter.cpp GCAddHelicopterHandler.cpp
    GCAddInjuriousCreature.cpp GCAddInjuriousCreatureHandler.cpp
    GCAddInstalledMineToZone.cpp GCAddInstalledMineToZoneHandler.cpp
    GCAddItemToInventory.cpp
    GCAddItemToItemVerify.cpp GCAddItemToItemVerifyHandler.cpp
    GCAddItemToZone.cpp
    GCAddMonster.cpp GCAddMonsterHandler.cpp
    GCAddMonsterCorpse.cpp GCAddMonsterCorpseHandler.cpp
    GCAddMonsterFromBurrowing.cpp GCAddMonsterFromBurrowingHandler.cpp
    GCAddMonsterFromTransformation.cpp GCAddMonsterFromTransformationHandler.cpp
    GCAddNewItemToZone.cpp GCAddNewItemToZoneHandler.cpp
    GCAddNickname.cpp GCAddNicknameHandler.cpp
    GCAddNPC.cpp GCAddNPCHandler.cpp
    GCAddOusters.cpp GCAddOustersHandler.cpp
    GCAddOustersCorpse.cpp GCAddOustersCorpseHandler.cpp
    GCAddSlayer.cpp GCAddSlayerHandler.cpp
    GCAddSlayerCorpse.cpp GCAddSlayerCorpseHandler.cpp
    GCAddStoreItem.cpp GCAddStoreItemHandler.cpp
    GCAddVampire.cpp GCAddVampireHandler.cpp
    GCAddVampireCorpse.cpp GCAddVampireCorpseHandler.cpp
    GCAddVampireFromBurrowing.cpp GCAddVampireFromBurrowingHandler.cpp
    GCAddVampireFromTransformation.cpp GCAddVampireFromTransformationHandler.cpp
    GCAddVampirePortal.cpp GCAddVampirePortalHandler.cpp
    GCAddWolf.cpp GCAddWolfHandler.cpp
    GCAddressListVerify.cpp GCAddressListVerifyHandler.cpp
    GCAttack.cpp GCAttackHandler.cpp
    GCAttackArmsOK1.cpp GCAttackArmsOK1Handler.cpp
    GCAttackArmsOK2.cpp GCAttackArmsOK2Handler.cpp
    GCAttackArmsOK3.cpp GCAttackArmsOK3Handler.cpp
    GCAttackArmsOK4.cpp GCAttackArmsOK4Handler.cpp
    GCAttackArmsOK5.cpp GCAttackArmsOK5Handler.cpp
    GCAttackMeleeOK1.cpp GCAttackMeleeOK1Handler.cpp
    GCAttackMeleeOK2.cpp GCAttackMeleeOK2Handler.cpp
    GCAttackMeleeOK3.cpp GCAttackMeleeOK3Handler.cpp
    GCAuthKey.cpp GCAuthKeyHandler.cpp
    GCBloodBibleList.cpp GCBloodBibleListHandler.cpp
    GCBloodBibleSignInfo.cpp GCBloodBibleSignInfoHandler.cpp
    GCBloodBibleStatus.cpp GCBloodBibleStatusHandler.cpp
    GCBloodDrainOK1.cpp GCBloodDrainOK1Handler.cpp
    GCBloodDrainOK2.cpp GCBloodDrainOK2Handler.cpp
    GCBloodDrainOK3.cpp GCBloodDrainOK3Handler.cpp
    GCCannotAdd.cpp GCCannotAddHandler.cpp
    GCCannotUse.cpp GCCannotUseHandler.cpp
    GCCastingSkill.cpp GCCastingSkillHandler.cpp
    GCChangeDarkLight.cpp GCChangeDarkLightHandler.cpp
    GCChangeInventoryItemNum.cpp
    GCChangeShape.cpp GCChangeShapeHandler.cpp
    GCChangeWeather.cpp GCChangeWeatherHandler.cpp
    GCCreateItem.cpp GCCreateItemHandler.cpp
    GCCreatureDied.cpp GCCreatureDiedHandler.cpp
    GCCrossCounterOK1.cpp GCCrossCounterOK1Handler.cpp
    GCCrossCounterOK2.cpp GCCrossCounterOK2Handler.cpp
    GCCrossCounterOK3.cpp GCCrossCounterOK3Handler.cpp
    GCDeleteandPickUpOK.cpp GCDeleteandPickUpOKHandler.cpp
    GCDeleteEffectFromTile.cpp GCDeleteEffectFromTileHandler.cpp
    GCDeleteInventoryItem.cpp GCDeleteInventoryItemHandler.cpp
    GCDeleteObject.cpp GCDeleteObjectHandler.cpp
    GCDisconnect.cpp GCDisconnectHandler.cpp
    GCDownSkillFailed.cpp GCDownSkillFailedHandler.cpp
    GCDownSkillOK.cpp GCDownSkillOKHandler.cpp
    GCDropItemToZone.cpp GCDropItemToZoneHandler.cpp
    GCEnterVampirePortal.cpp GCEnterVampirePortalHandler.cpp
    GCExecuteElement.cpp GCExecuteElementHandler.cpp
    GCFakeMove.cpp GCFakeMoveHandler.cpp
    GCFastMove.cpp GCFastMoveHandler.cpp
    GCFlagWarStatus.cpp GCFlagWarStatusHandler.cpp
    GCFriendChatting.cpp GCFriendChattingHandler.cpp
    GCGetDamage.cpp GCGetDamageHandler.cpp
    GCGetOffMotorCycle.cpp GCGetOffMotorCycleHandler.cpp
    GCGetOffMotorCycleFailed.cpp GCGetOffMotorCycleFailedHandler.cpp
    GCGetOffMotorCycleOK.cpp GCGetOffMotorCycleOKHandler.cpp
    GCGlobalChat.cpp GCGlobalChatHandler.cpp
    GCGoodsList.cpp GCGoodsListHandler.cpp
    GCGQuestInventory.cpp GCGQuestInventoryHandler.cpp
    GCGQuestStatusInfo.cpp GCGQuestStatusInfoHandler.cpp
    GCGQuestStatusModify.cpp GCGQuestStatusModifyHandler.cpp
    GCGuildChat.cpp GCGuildChatHandler.cpp
    GCGuildMemberList.cpp GCGuildMemberListHandler.cpp
    GCGuildResponse.cpp GCGuildResponseHandler.cpp
    GCHolyLandBonusInfo.cpp GCHolyLandBonusInfoHandler.cpp
    GCHPRecoveryEndToOthers.cpp GCHPRecoveryEndToOthersHandler.cpp
    GCHPRecoveryEndToSelf.cpp GCHPRecoveryEndToSelfHandler.cpp
    GCHPRecoveryStartToOthers.cpp GCHPRecoveryStartToOthersHandler.cpp
    GCHPRecoveryStartToSelf.cpp GCHPRecoveryStartToSelfHandler.cpp
    GCKickMessage.cpp GCKickMessageHandler.cpp
    GCKnockBack.cpp GCKnockBackHandler.cpp
    GCKnocksTargetBackOK1.cpp GCKnocksTargetBackOK1Handler.cpp
    GCKnocksTargetBackOK2.cpp GCKnocksTargetBackOK2Handler.cpp
    GCKnocksTargetBackOK4.cpp GCKnocksTargetBackOK4Handler.cpp
    GCKnocksTargetBackOK5.cpp GCKnocksTargetBackOK5Handler.cpp
    GCLearnSkillFailed.cpp GCLearnSkillFailedHandler.cpp
    GCLearnSkillOK.cpp GCLearnSkillOKHandler.cpp
    GCLearnSkillReady.cpp GCLearnSkillReadyHandler.cpp
    GCLightning.cpp GCLightningHandler.cpp
    GCMakeItemFail.cpp GCMakeItemFailHandler.cpp
    GCMakeItemOK.cpp GCMakeItemOKHandler.cpp
    GCMineExplosionOK1.cpp GCMineExplosionOK1Handler.cpp
    GCMineExplosionOK2.cpp GCMineExplosionOK2Handler.cpp
    GCMiniGameScores.cpp GCMiniGameScoresHandler.cpp
    GCModifyGuildMemberInfo.cpp GCModifyGuildMemberInfoHandler.cpp
    GCModifyInformation.cpp GCModifyInformationHandler.cpp
    GCModifyMoney.cpp GCModifyMoneyHandler.cpp
    GCModifyNickname.cpp GCModifyNicknameHandler.cpp
    GCModyfyMoney.cpp GCModyfyMoneyHandler.cpp
    GCMonsterKillQuestInfo.cpp GCMonsterKillQuestInfoHandler.cpp
#    GCMonsterKillQuestStatus.cpp GCMonsterKillQuestStatusHandler.cpp
    GCMorph1.cpp GCMorph1Handler.cpp
    GCMorphSlayer2.cpp GCMorphSlayer2Handler.cpp
    GCMorphVampire2.cpp GCMorphVampire2Handler.cpp
    GCMove.cpp GCMoveHandler.cpp
    GCMoveError.cpp GCMoveErrorHandler.cpp
    GCMoveOK.cpp GCMoveOKHandler.cpp
    GCMPRecoveryEnd.cpp GCMPRecoveryEndHandler.cpp
    GCMPRecoveryStart.cpp GCMPRecoveryStartHandler.cpp
    GCMyStoreInfo.cpp GCMyStoreInfoHandler.cpp
    GCNicknameList.cpp GCNicknameListHandler.cpp
    GCNicknameVerify.cpp GCNicknameVerifyHandler.cpp
    GCNoticeEvent.cpp GCNoticeEventHandler.cpp
    GCNotifyWin.cpp GCNotifyWinHandler.cpp
    GCNPCAsk.cpp GCNPCAskHandler.cpp
    GCNPCAskDynamic.cpp GCNPCAskDynamicHandler.cpp
    GCNPCAskVariable.cpp GCNPCAskVariableHandler.cpp
    GCNPCInfo.cpp GCNPCInfoHandler.cpp
    GCNPCResponse.cpp GCNPCResponseHandler.cpp
    GCNPCSay.cpp GCNPCSayHandler.cpp
    GCNPCSayDynamic.cpp GCNPCSayDynamicHandler.cpp
    GCOtherGuildName.cpp GCOtherGuildNameHandler.cpp
    GCOtherModifyInfo.cpp GCOtherModifyInfoHandler.cpp
    GCOtherStoreInfo.cpp GCOtherStoreInfoHandler.cpp
    GCPartyError.cpp GCPartyErrorHandler.cpp
    GCPartyInvite.cpp GCPartyInviteHandler.cpp
    GCPartyJoined.cpp GCPartyJoinedHandler.cpp
    GCPartyLeave.cpp GCPartyLeaveHandler.cpp
    GCPartyPosition.cpp GCPartyPositionHandler.cpp
    GCPartySay.cpp GCPartySayHandler.cpp
    GCPetInfo.cpp GCPetInfoHandler.cpp
    GCPetStashList.cpp GCPetStashListHandler.cpp
    GCPetStashVerify.cpp GCPetStashVerifyHandler.cpp
    GCPetUseSkill.cpp GCPetUseSkillHandler.cpp
    GCPhoneConnected.cpp GCPhoneConnectedHandler.cpp
    GCPhoneConnectionFailed.cpp GCPhoneConnectionFailedHandler.cpp
    GCPhoneDisconnected.cpp GCPhoneDisconnectedHandler.cpp
    GCPhoneSay.cpp GCPhoneSayHandler.cpp
    GCQuestStatus.cpp GCQuestStatusHandler.cpp
    GCRankBonusInfo.cpp GCRankBonusInfoHandler.cpp
    GCRealWearingInfo.cpp GCRealWearingInfoHandler.cpp
    GCReconnect.cpp GCReconnectHandler.cpp
    GCReconnectLogin.cpp GCReconnectLoginHandler.cpp
    GCRegenZoneStatus.cpp GCRegenZoneStatusHandler.cpp
    GCReloadOK.cpp GCReloadOKHandler.cpp
    GCRemoveCorpseHead.cpp GCRemoveCorpseHeadHandler.cpp
    GCRemoveEffect.cpp GCRemoveEffectHandler.cpp
    GCRemoveFromGear.cpp GCRemoveFromGearHandler.cpp
    GCRemoveInjuriousCreature.cpp GCRemoveInjuriousCreatureHandler.cpp
    GCRemoveStoreItem.cpp GCRemoveStoreItemHandler.cpp
    GCRequestedIP.cpp GCRequestedIPHandler.cpp
    GCRequestFailed.cpp GCRequestFailedHandler.cpp
    GCRequestPowerPointResult.cpp GCRequestPowerPointResultHandler.cpp
    GCRideMotorCycle.cpp GCRideMotorCycleHandler.cpp
    GCRideMotorCycleFailed.cpp GCRideMotorCycleFailedHandler.cpp
    GCRideMotorCycleOK.cpp GCRideMotorCycleOKHandler.cpp
    GCRing.cpp GCRingHandler.cpp
    GCSay.cpp GCSayHandler.cpp
    GCSearchMotorcycleFail.cpp GCSearchMotorcycleFailHandler.cpp
    GCSearchMotorcycleOK.cpp GCSearchMotorcycleOKHandler.cpp
    GCSelectQuestID.cpp GCSelectQuestIDHandler.cpp
    GCSelectRankBonusFailed.cpp GCSelectRankBonusFailedHandler.cpp
    GCSelectRankBonusOK.cpp GCSelectRankBonusOKHandler.cpp
    GCSetPosition.cpp GCSetPositionHandler.cpp
    GCShopBought.cpp GCShopBoughtHandler.cpp
    GCShopBuyFail.cpp GCShopBuyFailHandler.cpp
    GCShopBuyOK.cpp GCShopBuyOKHandler.cpp
    GCShopList.cpp GCShopListHandler.cpp
    GCShopListMysterious.cpp GCShopListMysteriousHandler.cpp
    GCShopMarketCondition.cpp GCShopMarketConditionHandler.cpp
    GCShopSellFail.cpp GCShopSellFailHandler.cpp
    GCShopSellOK.cpp GCShopSellOKHandler.cpp
    GCShopSold.cpp GCShopSoldHandler.cpp
    GCShopVersion.cpp GCShopVersionHandler.cpp
    GCShowGuildInfo.cpp GCShowGuildInfoHandler.cpp
    GCShowGuildJoin.cpp GCShowGuildJoinHandler.cpp
    GCShowGuildMemberInfo.cpp GCShowGuildMemberInfoHandler.cpp
    GCShowGuildRegist.cpp GCShowGuildRegistHandler.cpp
    GCShowMessageBox.cpp GCShowMessageBoxHandler.cpp
    GCShowUnionInfo.cpp GCShowUnionInfoHandler.cpp
    GCShowWaitGuildInfo.cpp GCShowWaitGuildInfoHandler.cpp
    GCSkillFailed1.cpp GCSkillFailed1Handler.cpp
    GCSkillFailed2.cpp GCSkillFailed2Handler.cpp
    GCSkillInfo.cpp GCSkillInfoHandler.cpp
    GCSkillToInventoryOK1.cpp GCSkillToInventoryOK1Handler.cpp
    GCSkillToInventoryOK2.cpp GCSkillToInventoryOK2Handler.cpp
    GCSkillToObjectOK1.cpp GCSkillToObjectOK1Handler.cpp
    GCSkillToObjectOK2.cpp GCSkillToObjectOK2Handler.cpp
    GCSkillToObjectOK3.cpp GCSkillToObjectOK3Handler.cpp
    GCSkillToObjectOK4.cpp GCSkillToObjectOK4Handler.cpp
    GCSkillToObjectOK5.cpp GCSkillToObjectOK5Handler.cpp
    GCSkillToObjectOK6.cpp GCSkillToObjectOK6Handler.cpp
    GCSkillToSelfOK1.cpp GCSkillToSelfOK1Handler.cpp
    GCSkillToSelfOK2.cpp GCSkillToSelfOK2Handler.cpp
    GCSkillToSelfOK3.cpp GCSkillToSelfOK3Handler.cpp
    GCSkillToTileOK1.cpp GCSkillToTileOK1Handler.cpp
    GCSkillToTileOK2.cpp GCSkillToTileOK2Handler.cpp
    GCSkillToTileOK3.cpp GCSkillToTileOK3Handler.cpp
    GCSkillToTileOK4.cpp GCSkillToTileOK4Handler.cpp
    GCSkillToTileOK5.cpp GCSkillToTileOK5Handler.cpp
    GCSkillToTileOK6.cpp GCSkillToTileOK6Handler.cpp
    GCSMSAddressList.cpp GCSMSAddressListHandler.cpp
    GCStashList.cpp GCStashListHandler.cpp
    GCStashSell.cpp GCStashSellHandler.cpp
    GCStatusCurrentHP.cpp GCStatusCurrentHPHandler.cpp
    GCSubInventoryInfo.cpp GCSubInventoryInfoHandler.cpp
    GCSweeperBonusInfo.cpp GCSweeperBonusInfoHandler.cpp
    GCSystemAvailabilities.cpp GCSystemAvailabilitiesHandler.cpp
    GCSystemMessage.cpp GCSystemMessageHandler.cpp
    GCTakeOff.cpp GCTakeOffHandler.cpp
    GCTakeOutFail.cpp GCTakeOutFailHandler.cpp
    GCTakeOutOK.cpp GCTakeOutOKHandler.cpp
    GCTeachSkillInfo.cpp GCTeachSkillInfoHandler.cpp
    GCThrowBombOK1.cpp GCThrowBombOK1Handler.cpp
    GCThrowBombOK2.cpp GCThrowBombOK2Handler.cpp
    GCThrowBombOK3.cpp GCThrowBombOK3Handler.cpp
    GCThrowItemOK1.cpp GCThrowItemOK1Handler.cpp
    GCThrowItemOK2.cpp GCThrowItemOK2Handler.cpp
    GCThrowItemOK3.cpp GCThrowItemOK3Handler.cpp
    GCTimeLimitItemInfo.cpp GCTimeLimitItemInfoHandler.cpp
    GCTradeAddItem.cpp GCTradeAddItemHandler.cpp
    GCTradeError.cpp GCTradeErrorHandler.cpp
    GCTradeFinish.cpp GCTradeFinishHandler.cpp
    GCTradeMoney.cpp GCTradeMoneyHandler.cpp
    GCTradePrepare.cpp GCTradePrepareHandler.cpp
    GCTradeRemoveItem.cpp GCTradeRemoveItemHandler.cpp
    GCTradeVerify.cpp GCTradeVerifyHandler.cpp
    GCUnburrowFail.cpp GCUnburrowFailHandler.cpp
    GCUnburrowOK.cpp GCUnburrowOKHandler.cpp
    GCUnionOfferList.cpp GCUnionOfferListHandler.cpp
    GCUntransformFail.cpp GCUntransformFailHandler.cpp
    GCUntransformOK.cpp GCUntransformOKHandler.cpp
    GCUpdateInfo.cpp GCUpdateInfoHandler.cpp
    GCUseBonusPointFail.cpp GCUseBonusPointFailHandler.cpp
    GCUseBonusPointOK.cpp GCUseBonusPointOKHandler.cpp
    GCUseOK.cpp GCUseOKHandler.cpp
    GCUsePowerPointResult.cpp GCUsePowerPointResultHandler.cpp
    GCVisibleFail.cpp GCVisibleFailHandler.cpp
    GCVisibleOK.cpp GCVisibleOKHandler.cpp
    GCWaitGuildList.cpp GCWaitGuildListHandler.cpp
    GCWarList.cpp GCWarListHandler.cpp
    GCWarScheduleList.cpp GCWarScheduleListHandler.cpp
    GCWhisper.cpp GCWhisperHandler.cpp
    GCWhisperFailed.cpp GCWhisperFailedHandler.cpp
    # Exchange System Packets
    GCExchangeList.cpp GCExchangeListHandler.cpp
    GCExchangeBuy.cpp GCExchangeBuyHandler.cpp
)


# CG Packets (Client -> Game)
set(CG_PACKET_SOURCES
    CGAbsorbSoul.cpp CGAbsorbSoulHandler.cpp
    # CGAcceptUnion.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch A.
    CGAcceptUnionHandler.cpp
    CGAddGearToMouse.cpp CGAddGearToMouseHandler.cpp
#    CGAddInjuriousCreature.cpp CGAddInjuriousCreatureHandler.cpp
    CGAddInventoryToMouse.cpp CGAddInventoryToMouseHandler.cpp
    # CGAddItemToCodeSheet.cpp moved to shared/Packets/ — Phase 12 Wave 1.
    CGAddItemToCodeSheetHandler.cpp
    # CGAddItemToItem.cpp moved to shared/Packets/ — Phase 12 Wave 1.
    CGAddItemToItemHandler.cpp
    CGAddMouseToGear.cpp CGAddMouseToGearHandler.cpp
    CGAddMouseToInventory.cpp CGAddMouseToInventoryHandler.cpp
    CGAddMouseToQuickSlot.cpp CGAddMouseToQuickSlotHandler.cpp
    # CGAddMouseToZone.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 2.
    CGAddMouseToZoneHandler.cpp
    CGAddQuickSlotToMouse.cpp CGAddQuickSlotToMouseHandler.cpp
    # CGAddSMSAddress.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 2.
    CGAddSMSAddressHandler.cpp
    # CGAddZoneToInventory.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 2.
    CGAddZoneToInventoryHandler.cpp
    # CGAddZoneToMouse.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 2.
    CGAddZoneToMouseHandler.cpp
    # CGAppointSubmaster.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch A.
    CGAppointSubmasterHandler.cpp
    # CGAttack.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 2.
    CGAttackHandler.cpp
    # CGAuthKey.cpp moved to shared/Packets/ — Phase 12 Wave 1.
    CGAuthKeyHandler.cpp
    CGBloodDrain.cpp CGBloodDrainHandler.cpp
    # CGBuyStoreItem.cpp moved to shared/Packets/ — Phase 12 Wave 1.
    CGBuyStoreItemHandler.cpp
    CGCastingSkill.cpp CGCastingSkillHandler.cpp
    # CGConnect.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 1.
    CGConnectHandler.cpp
    CGConnectSetKey.cpp CGConnectSetKeyHandler.cpp
    # CGCrashReport.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 2.
    CGCrashReportHandler.cpp
    # CGDeleteSMSAddress.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 2.
    CGDeleteSMSAddressHandler.cpp
    # CGDenyUnion.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch A.
    CGDenyUnionHandler.cpp
    # CGDepositPet.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 1.
    CGDepositPetHandler.cpp
    CGDialUp.cpp CGDialUpHandler.cpp
    # CGDisplayItem.cpp moved to shared/Packets/ — Phase 12 Wave 1.
    CGDisplayItemHandler.cpp
    CGDissectionCorpse.cpp CGDissectionCorpseHandler.cpp
    # CGDonationMoney.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 1.
    CGDonationMoneyHandler.cpp
    # CGDownSkill.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch A.
    CGDownSkillHandler.cpp
    # CGDropMoney.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 2.
    CGDropMoneyHandler.cpp
    # CGExpelGuild.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch A.
    CGExpelGuildHandler.cpp
    CGExpelGuildMember.cpp CGExpelGuildMemberHandler.cpp
    # CGFailQuest.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch A.
    CGFailQuestHandler.cpp
    CGGetEventItem.cpp CGGetEventItemHandler.cpp
    CGGetOffMotorCycle.cpp CGGetOffMotorCycleHandler.cpp
    # CGGlobalChat.cpp moved to shared/Packets/ — Phase 12 Wave 1.
    CGGlobalChatHandler.cpp
    # CGGQuestAccept.cpp moved to shared/Packets/ — Phase 12 Wave 1.
    CGGQuestAcceptHandler.cpp
    # CGGQuestCancel.cpp moved to shared/Packets/ — Phase 12 Wave 1.
    CGGQuestCancelHandler.cpp
    # CGGuildChat.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 3.
    CGGuildChatHandler.cpp
    # CGJoinGuild.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 2.
    CGJoinGuildHandler.cpp
    # CGLearnSkill.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 1.
    CGLearnSkillHandler.cpp
    # CGLogout.cpp moved to shared/Packets/ — Phase 12 Wave 1.
    CGLogoutHandler.cpp
    # CGLotterySelect.cpp moved to shared/Packets/ — Phase 12 Wave 1.
    CGLotterySelectHandler.cpp
    CGMakeItem.cpp CGMakeItemHandler.cpp
    CGMixItem.cpp CGMixItemHandler.cpp
    # CGModifyGuildIntro.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch A.
    CGModifyGuildIntroHandler.cpp
    # CGModifyGuildMember.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch A.
    CGModifyGuildMemberHandler.cpp
    # CGModifyGuildMemberIntro.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch A.
    CGModifyGuildMemberIntroHandler.cpp
    # CGModifyNickname.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 3.
    CGModifyNicknameHandler.cpp
    # CGModifyTaxRatio.cpp moved to shared/Packets/ — Phase 12 Wave 1.
    CGModifyTaxRatioHandler.cpp
    CGMouseToStash.cpp CGMouseToStashHandler.cpp
    CGMove.cpp CGMoveHandler.cpp
    # CGNPCAskAnswer.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch A.
    CGNPCAskAnswerHandler.cpp
    CGNPCTalk.cpp CGNPCTalkHandler.cpp
    # CGPartyInvite.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 1.
    CGPartyInviteHandler.cpp
    # CGPartyLeave.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch A.
    CGPartyLeaveHandler.cpp
    CGPartyPosition.cpp CGPartyPositionHandler.cpp
    CGPartySay.cpp CGPartySayHandler.cpp
    # CGPetGamble.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 3.
    CGPetGambleHandler.cpp
    CGPhoneDisconnect.cpp CGPhoneDisconnectHandler.cpp
    # CGPhoneSay.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 1.
    CGPhoneSayHandler.cpp
    # CGPickupMoney.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 3.
    CGPickupMoneyHandler.cpp
    # CGPortCheck.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch A.
    CGPortCheckHandler.cpp
    CGQuitGuild.cpp CGQuitGuildHandler.cpp
    # CGQuitUnion.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch A.
    CGQuitUnionHandler.cpp
    # CGQuitUnionAccept.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch A.
    CGQuitUnionAcceptHandler.cpp
    # CGQuitUnionDeny.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch B.
    CGQuitUnionDenyHandler.cpp
    # CGRangerSay.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 1.
    CGRangerSayHandler.cpp
    # CGReady.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch B.
    CGReadyHandler.cpp
    # CGRegistGuild.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 1.
    CGRegistGuildHandler.cpp
    CGRelicToObject.cpp CGRelicToObjectHandler.cpp
    CGReloadFromInventory.cpp CGReloadFromInventoryHandler.cpp
    CGReloadFromQuickSlot.cpp CGReloadFromQuickSlotHandler.cpp
    # CGRequestGuildList.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch B.
    CGRequestGuildListHandler.cpp
    # CGRequestGuildMemberList.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch B.
    CGRequestGuildMemberListHandler.cpp
    # CGRequestInfo.cpp moved to shared/Packets/ — Phase 12 Wave 1.
    CGRequestInfoHandler.cpp
    CGRequestIP.cpp CGRequestIPHandler.cpp
    # CGRequestNewbieItem.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 1.
    CGRequestNewbieItemHandler.cpp
    CGRequestPowerPoint.cpp CGRequestPowerPointHandler.cpp
    # CGRequestRepair.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 3.
    CGRequestRepairHandler.cpp
    # CGRequestStoreInfo.cpp moved to shared/Packets/ — Phase 12 Wave 1.
    CGRequestStoreInfoHandler.cpp
    # CGRequestUnion.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch B.
    CGRequestUnionHandler.cpp
    # CGRequestUnionInfo.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 1.
    CGRequestUnionInfoHandler.cpp
    # CGResurrect.cpp moved to shared/Packets/ — Phase 12 Wave 1.
    CGResurrectHandler.cpp
    CGRideMotorCycle.cpp CGRideMotorCycleHandler.cpp
    CGSay.cpp CGSayHandler.cpp
    # CGSelectBloodBible.cpp moved to shared/Packets/ — Phase 12 Wave 1.
    CGSelectBloodBibleHandler.cpp
    # CGSelectGuild.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch B.
    CGSelectGuildHandler.cpp
    # CGSelectGuildMember.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch B.
    CGSelectGuildMemberHandler.cpp
    # CGSelectNickname.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch B.
    CGSelectNicknameHandler.cpp
    CGSelectPortal.cpp CGSelectPortalHandler.cpp
    # CGSelectQuest.cpp moved to shared/Packets/ — Phase 12 Wave 1.
    CGSelectQuestHandler.cpp
    # CGSelectRankBonus.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 1.
    CGSelectRankBonusHandler.cpp
    # CGSelectRegenZone.cpp moved to shared/Packets/ — Phase 12 Wave 1.
    CGSelectRegenZoneHandler.cpp
    # CGSelectTileEffect.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch B.
    CGSelectTileEffectHandler.cpp
    # CGSelectWayPoint.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 1.
    CGSelectWayPointHandler.cpp
    # CGSetSlayerHotKey.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch B.
    CGSetSlayerHotKeyHandler.cpp
    # CGSetVampireHotKey.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch B.
    CGSetVampireHotKeyHandler.cpp
    # CGShopRequestBuy.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 3.
    CGShopRequestBuyHandler.cpp
    # CGShopRequestList.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 1.
    CGShopRequestListHandler.cpp
    # CGShopRequestSell.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 3.
    CGShopRequestSellHandler.cpp
    # CGSilverCoating.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch B.
    CGSilverCoatingHandler.cpp
    CGSkillToInventory.cpp CGSkillToInventoryHandler.cpp
    CGSkillToNamed.cpp CGSkillToNamedHandler.cpp
    CGSkillToObject.cpp CGSkillToObjectHandler.cpp
    CGSkillToSelf.cpp CGSkillToSelfHandler.cpp
    # CGSkillToTile.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 3.
    CGSkillToTileHandler.cpp
    # CGSMSAddressList.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch B.
    CGSMSAddressListHandler.cpp
    CGSMSSend.cpp CGSMSSendHandler.cpp
    CGStashDeposit.cpp CGStashDepositHandler.cpp
    CGStashList.cpp CGStashListHandler.cpp
    CGStashRequestBuy.cpp CGStashRequestBuyHandler.cpp
    CGStashToMouse.cpp CGStashToMouseHandler.cpp
    CGStashWithdraw.cpp CGStashWithdrawHandler.cpp
    # CGStoreClose.cpp moved to shared/Packets/ — Phase 12 Wave 1.
    CGStoreCloseHandler.cpp
    # CGStoreOpen.cpp moved to shared/Packets/ — Phase 12 Wave 1.
    CGStoreOpenHandler.cpp
    # CGStoreSign.cpp moved to shared/Packets/ — Phase 12 Wave 1.
    CGStoreSignHandler.cpp
    # CGSubmitScore.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 3.
    CGSubmitScoreHandler.cpp
    # CGTakeOutGood.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 3.
    CGTakeOutGoodHandler.cpp
    # CGTameMonster.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 3.
    CGTameMonsterHandler.cpp
    # CGThrowBomb.cpp moved to shared/Packets/ — Phase 12 Wave 2 batch B.
    CGThrowBombHandler.cpp
    CGThrowItem.cpp CGThrowItemHandler.cpp
    # CGTradeAddItem.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 2.
    CGTradeAddItemHandler.cpp
    # CGTradeFinish.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 2.
    CGTradeFinishHandler.cpp
    # CGTradeMoney.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 2.
    CGTradeMoneyHandler.cpp
    # CGTradePrepare.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 2.
    CGTradePrepareHandler.cpp
    # CGTradeRemoveItem.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 2.
    CGTradeRemoveItemHandler.cpp
    # CGTryJoinGuild.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 1.
    CGTryJoinGuildHandler.cpp
    CGTypeStringList.cpp CGTypeStringListHandler.cpp
    CGUnburrow.cpp CGUnburrowHandler.cpp
    # CGUndisplayItem.cpp moved to shared/Packets/ — Phase 12 Wave 1.
    CGUndisplayItemHandler.cpp
    # CGUntransform.cpp moved to shared/Packets/ — Phase 12 Wave 3.
    CGUntransformHandler.cpp
    CGUseBonusPoint.cpp CGUseBonusPointHandler.cpp
    # CGUseItemFromGear.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 2.
    CGUseItemFromGearHandler.cpp
    # CGUseItemFromGQuestInventory.cpp moved to shared/Packets/ — Phase 12 Wave 1.
    CGUseItemFromGQuestInventoryHandler.cpp
    CGUseItemFromInventory.cpp CGUseItemFromInventoryHandler.cpp
    CGUseMessageItemFromInventory.cpp CGUseMessageItemFromInventoryHandler.cpp
    CGUsePotionFromInventory.cpp CGUsePotionFromInventoryHandler.cpp
    CGUsePotionFromQuickSlot.cpp CGUsePotionFromQuickSlotHandler.cpp
    # CGUsePowerPoint.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 3.
    CGUsePowerPointHandler.cpp
    CGVerifyTime.cpp CGVerifyTimeHandler.cpp
    # CGVisible.cpp moved to shared/Packets/ — Phase 12 Wave 3.
    CGVisibleHandler.cpp
    # CGWhisper.cpp moved to shared/Packets/ — Phase 12 Wave 3.
    CGWhisperHandler.cpp
    # CGWithdrawPet.cpp moved to shared/Packets/ — Phase 12 Wave 1.
    CGWithdrawPetHandler.cpp
    # CGWithdrawTax.cpp moved to shared/Packets/ — Phase 12 Wave 3.
    CGWithdrawTaxHandler.cpp
    # Exchange System Packets
    CGExchangeList.cpp CGExchangeListHandler.cpp
    CGExchangeBuy.cpp CGExchangeBuyHandler.cpp
)

# CL Packets (Client -> Login)
set(CL_PACKET_SOURCES
    CLChangeServer.cpp CLChangeServerHandler.cpp
    CLCreatePC.cpp CLCreatePCHandler.cpp
    # CLDeletePC.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 3.
    CLDeletePCHandler.cpp
    # CLGetPCList.cpp moved to shared/Packets/ — Phase 12 Wave 3.
    CLGetPCListHandler.cpp
    # CLGetServerList.cpp moved to shared/Packets/ — Phase 12 Wave 3.
    CLGetServerListHandler.cpp
    # CLGetWorldList.cpp moved to shared/Packets/ — Phase 12 migration.
    CLGetWorldListHandler.cpp
    CLLogin.cpp CLLoginHandler.cpp
    # CLLogout.cpp moved to shared/Packets/ — Phase 12 Wave 3.
    CLLogoutHandler.cpp
    # CLQueryCharacterName.cpp moved to shared/Packets/ — Phase 12 Wave 3.
    CLQueryCharacterNameHandler.cpp
    # CLQueryPlayerID.cpp moved to shared/Packets/ — Phase 12 Wave 3.
    CLQueryPlayerIDHandler.cpp
    CLReconnectLogin.cpp CLReconnectLoginHandler.cpp
    # CLRegisterPlayer.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 1.
    CLRegisterPlayerHandler.cpp
    # CLSelectPC.cpp moved to shared/Packets/ — Phase 12 Wave 4 batch 3.
    CLSelectPCHandler.cpp
    CLSelectServer.cpp CLSelectServerHandler.cpp
    CLSelectWorld.cpp CLSelectWorldHandler.cpp
    # CLVersionCheck.cpp moved to shared/Packets/ — Phase 12 Wave 3.
    CLVersionCheckHandler.cpp
    GCDisconnect.cpp GCDisconnectHandler.cpp
	CGConnectSetKey.cpp CGConnectSetKeyHandler.cpp 
)

# LC Packets (Login -> Client)
set(LC_PACKET_SOURCES
    LCCreatePCError.cpp LCCreatePCErrorHandler.cpp
    LCCreatePCOK.cpp LCCreatePCOKHandler.cpp
    LCDeletePCError.cpp LCDeletePCErrorHandler.cpp
    LCDeletePCOK.cpp LCDeletePCOKHandler.cpp
    LCLoginError.cpp LCLoginErrorHandler.cpp
    LCLoginOK.cpp LCLoginOKHandler.cpp
    LCPCList.cpp LCPCListHandler.cpp
    LCQueryResultCharacterName.cpp LCQueryResultCharacterNameHandler.cpp
    LCQueryResultPlayerID.cpp LCQueryResultPlayerIDHandler.cpp
    LCReconnect.cpp LCReconnectHandler.cpp
    LCRegisterPlayerError.cpp LCRegisterPlayerErrorHandler.cpp
    LCRegisterPlayerOK.cpp LCRegisterPlayerOKHandler.cpp
    LCSelectPCError.cpp LCSelectPCErrorHandler.cpp
    LCServerList.cpp LCServerListHandler.cpp
    LCVersionCheckError.cpp LCVersionCheckErrorHandler.cpp
    LCVersionCheckOK.cpp LCVersionCheckOKHandler.cpp
    LCWorldList.cpp LCWorldListHandler.cpp
)

# GL Packets (Game -> Login)
set(GL_PACKET_SOURCES
    GLIncomingConnection.cpp GLIncomingConnectionHandler.cpp
    GLIncomingConnectionError.cpp GLIncomingConnectionErrorHandler.cpp
    GLIncomingConnectionOK.cpp GLIncomingConnectionOKHandler.cpp
    GLKickVerify.cpp GLKickVerifyHandler.cpp
    GMServerInfo.cpp GGCommand.cpp GGCommandHandler.cpp
)

# LG Packets (Login -> Game)
set(LG_PACKET_SOURCES
    LGIncomingConnection.cpp LGIncomingConnectionHandler.cpp
    LGIncomingConnectionError.cpp LGIncomingConnectionErrorHandler.cpp
    LGIncomingConnectionOK.cpp LGIncomingConnectionOKHandler.cpp
    LGKickCharacter.cpp LGKickCharacterHandler.cpp
)

# GS Packets (Game -> Shared)
set(GS_PACKET_SOURCES
    GSAddGuild.cpp GSAddGuildHandler.cpp
    GSAddGuildMember.cpp GSAddGuildMemberHandler.cpp
    GSExpelGuildMember.cpp GSExpelGuildMemberHandler.cpp
    GSGuildMemberLogOn.cpp GSGuildMemberLogOnHandler.cpp
    GSModifyGuildIntro.cpp GSModifyGuildIntroHandler.cpp
    GSModifyGuildMember.cpp GSModifyGuildMemberHandler.cpp
    GSQuitGuild.cpp GSQuitGuildHandler.cpp
    GSRequestGuildInfo.cpp GSRequestGuildInfoHandler.cpp
	GGGuildChat.cpp GGGuildChatHandler.cpp
)

# SG Packets (Shared -> Game)
set(SG_PACKET_SOURCES
    SGAddGuildMemberOK.cpp SGAddGuildMemberOKHandler.cpp
    SGAddGuildOK.cpp SGAddGuildOKHandler.cpp
    SGDeleteGuildOK.cpp SGDeleteGuildOKHandler.cpp
    SGExpelGuildMemberOK.cpp SGExpelGuildMemberOKHandler.cpp
    SGGuildInfo.cpp SGGuildInfoHandler.cpp
    SGGuildMemberLogOnOK.cpp SGGuildMemberLogOnOKHandler.cpp
    SGModifyGuildIntroOK.cpp SGModifyGuildIntroOKHandler.cpp
    SGModifyGuildMemberOK.cpp SGModifyGuildMemberOKHandler.cpp
    SGModifyGuildOK.cpp SGModifyGuildOKHandler.cpp
    SGQuitGuildOK.cpp SGQuitGuildOKHandler.cpp
)

# GG Packets (Game -> Game)
set(GG_PACKET_SOURCES
    GGCommand.cpp GGCommandHandler.cpp
    GGGuildChat.cpp GGGuildChatHandler.cpp
    GGServerChat.cpp GGServerChatHandler.cpp
)

# GM Packets
set(GM_PACKET_SOURCES
    GMServerInfo.cpp GMServerInfoHandler.cpp
)

#  Packet 
set(PACKET_COMMON_SOURCES
    PacketFactoryManager.cpp
    PacketIDSet.cpp
    PacketValidator.cpp
)

# ============================================================================
# GameServerPackets  -  __GAME_SERVER__ 
# ============================================================================
add_library(GameServerPackets STATIC
    ${GC_PACKET_SOURCES}
    ${CG_PACKET_SOURCES}
    ${GL_PACKET_SOURCES}
    ${LG_PACKET_SOURCES}
    ${GS_PACKET_SOURCES}
    ${SG_PACKET_SOURCES}
    ${GG_PACKET_SOURCES}
    ${GM_PACKET_SOURCES}
    ${PACKET_COMMON_SOURCES}
)
target_compile_definitions(GameServerPackets PRIVATE __GAME_SERVER__ __COMBAT__)
target_include_directories(GameServerPackets PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/server
    ${CMAKE_SOURCE_DIR}/src/server/database
    ${CMAKE_SOURCE_DIR}/src/server/gameserver
    ${CMAKE_SOURCE_DIR}/src/server/gameserver/item
    ${CMAKE_SOURCE_DIR}/src/server/gameserver/skill
    ${CMAKE_SOURCE_DIR}/src/server/gameserver/billing
    # Phase 12: PacketFactoryManager.cpp (in PACKET_COMMON_SOURCES, compiled
    # into all three server packet libraries) unconditionally #includes
    # every migrated packet's header, including other families' headers
    # like CLGetWorldList.h even though its addFactory() call is guarded
    # to __LOGIN_SERVER__ only. Every shared_packets_<family> target's
    # include directory is this same path, so linking shared_packets_cg
    # below (for this target's own CG family) already covers it — this
    # entry stays as an explicit, redundant-but-harmless include path in
    # case a future family this target doesn't own needs the header
    # before it's ever linked.
    ${CMAKE_SOURCE_DIR}/../shared/Packets
)
# Phase 12 Wave 1: this is the owning target for the CG family — links
# shared_packets_cg (not the old combined "shared_packets") so it absorbs
# only CG sources, compiled with __GAME_SERVER__ applied. Each CG
# packet's *Handler.cpp stays in this directory and is still listed
# above; deliberately NOT linking shared_packets_cl here, since
# CLGetWorldListHandler::execute is only ever defined in
# LoginServerPackets, and absorbing CLGetWorldList.cpp as a compiled
# source here (with no such symbol) would be a link error waiting for
# gameserver to actually reference it.
target_link_libraries(GameServerPackets PRIVATE shared_packets_cg)

# ============================================================================
# LoginServerPackets  -  __LOGIN_SERVER__ 
# ============================================================================
add_library(LoginServerPackets STATIC
    ${CL_PACKET_SOURCES}
    ${LC_PACKET_SOURCES}
    ${GL_PACKET_SOURCES}
    ${LG_PACKET_SOURCES}
    ${GM_PACKET_SOURCES}
    ${PACKET_COMMON_SOURCES}
)
target_compile_definitions(LoginServerPackets PRIVATE __LOGIN_SERVER__)
target_include_directories(LoginServerPackets PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/server
    ${CMAKE_SOURCE_DIR}/src/server/database
    ${CMAKE_SOURCE_DIR}/src/server/loginserver
    ${CMAKE_SOURCE_DIR}/src/server/gameserver
)
# Phase 12: this is the owning target for the CL family — links
# shared_packets_cl (renamed from the original pilot's combined
# "shared_packets" in Phase 12 Wave 1, when the CG family needed its own
# owner-linked target too — see shared/Packets/CMakeLists.txt) so it
# absorbs CLGetWorldList.cpp as a source, compiled with __LOGIN_SERVER__
# applied. CLGetWorldListHandler::execute (defined in
# CLGetWorldListHandler.cpp, still in this directory — handlers never
# move) is only ever called from here.
target_link_libraries(LoginServerPackets PRIVATE shared_packets_cl)

# ============================================================================
# SharedServerPackets  -  __SHARED_SERVER__ 
# ============================================================================
add_library(SharedServerPackets STATIC
    ${GS_PACKET_SOURCES}
    ${SG_PACKET_SOURCES}
    ${LG_PACKET_SOURCES}
    ${PACKET_COMMON_SOURCES}
)
target_compile_definitions(SharedServerPackets PRIVATE __SHARED_SERVER__)
target_include_directories(SharedServerPackets PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/server
    ${CMAKE_SOURCE_DIR}/src/server/database
    ${CMAKE_SOURCE_DIR}/src/server/sharedserver
    ${CMAKE_SOURCE_DIR}/src/server/gameserver
    # Phase 12: see GameServerPackets note above — include-only, no link.
    ${CMAKE_SOURCE_DIR}/../shared/Packets
)
