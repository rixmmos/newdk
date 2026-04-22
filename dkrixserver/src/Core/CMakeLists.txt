# ============================================================================
# Core 库 - 基础核心代码，不依赖服务器类型
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
# 通用 Packet 源文件定义
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
    GCAddStoreItemHandler.cpp
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
    GCMyStoreInfoHandler.cpp
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
    GCOtherStoreInfoHandler.cpp
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
    GCRemoveStoreItemHandler.cpp
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
    CGAbsorbSoulHandler.cpp
    CGAcceptUnionHandler.cpp
    CGAddGearToMouseHandler.cpp
#    CGAddInjuriousCreature.cpp CGAddInjuriousCreatureHandler.cpp
    CGAddInventoryToMouseHandler.cpp
    CGAddItemToCodeSheetHandler.cpp
    CGAddItemToItemHandler.cpp
    CGAddMouseToGearHandler.cpp
    CGAddMouseToInventoryHandler.cpp
    CGAddMouseToQuickSlotHandler.cpp
    CGAddMouseToZoneHandler.cpp
    CGAddQuickSlotToMouseHandler.cpp
    CGAddSMSAddressHandler.cpp
    CGAddZoneToInventoryHandler.cpp
    CGAddZoneToMouseHandler.cpp
    CGAppointSubmasterHandler.cpp
    CGAttackHandler.cpp
    CGAuthKeyHandler.cpp
    CGBloodDrainHandler.cpp
    CGBuyStoreItemHandler.cpp
    CGCastingSkillHandler.cpp
    CGConnectHandler.cpp
    CGConnectSetKeyHandler.cpp
    CGCrashReportHandler.cpp
    CGDeleteSMSAddressHandler.cpp
    CGDenyUnionHandler.cpp
    CGDepositPetHandler.cpp
    CGDialUpHandler.cpp
    CGDisplayItemHandler.cpp
    CGDissectionCorpseHandler.cpp
    CGDonationMoneyHandler.cpp
    CGDownSkillHandler.cpp
    CGDropMoneyHandler.cpp
    CGExpelGuildHandler.cpp
    CGExpelGuildMember.cpp CGExpelGuildMemberHandler.cpp
    CGFailQuestHandler.cpp
    CGGetEventItemHandler.cpp
    CGGetOffMotorCycleHandler.cpp
    CGGlobalChatHandler.cpp
    CGGQuestAcceptHandler.cpp
    CGGQuestCancelHandler.cpp
    CGGuildChatHandler.cpp
    CGJoinGuildHandler.cpp
    CGLearnSkillHandler.cpp
    CGLogoutHandler.cpp
    CGLotterySelectHandler.cpp
    CGMakeItemHandler.cpp
    CGMixItemHandler.cpp
    CGModifyGuildIntroHandler.cpp
    CGModifyGuildMemberHandler.cpp
    CGModifyGuildMemberIntroHandler.cpp
    CGModifyNicknameHandler.cpp
    CGModifyTaxRatioHandler.cpp
    CGMouseToStashHandler.cpp
    CGMoveHandler.cpp
    CGNPCAskAnswerHandler.cpp
    CGNPCTalkHandler.cpp
    CGPartyInviteHandler.cpp
    CGPartyLeaveHandler.cpp
    CGPartyPositionHandler.cpp
    CGPartySayHandler.cpp
    CGPetGambleHandler.cpp
    CGPhoneDisconnectHandler.cpp
    CGPhoneSayHandler.cpp
    CGPickupMoneyHandler.cpp
    CGPortCheckHandler.cpp
    CGQuitGuild.cpp CGQuitGuildHandler.cpp
    CGQuitUnionHandler.cpp
    CGQuitUnionAcceptHandler.cpp
    CGQuitUnionDenyHandler.cpp
    CGRangerSayHandler.cpp
    CGReadyHandler.cpp
    CGRegistGuildHandler.cpp
    CGRelicToObjectHandler.cpp
    CGReloadFromInventoryHandler.cpp
    CGReloadFromQuickSlotHandler.cpp
    CGRequestGuildListHandler.cpp
    CGRequestGuildMemberListHandler.cpp
    CGRequestInfoHandler.cpp
    CGRequestIPHandler.cpp
    CGRequestNewbieItemHandler.cpp
    CGRequestPowerPointHandler.cpp
    CGRequestRepairHandler.cpp
    CGRequestStoreInfoHandler.cpp
    CGRequestUnionHandler.cpp
    CGRequestUnionInfoHandler.cpp
    CGResurrectHandler.cpp
    CGRideMotorCycleHandler.cpp
    CGSayHandler.cpp
    CGSelectBloodBibleHandler.cpp
    CGSelectGuildHandler.cpp
    CGSelectGuildMemberHandler.cpp
    CGSelectNicknameHandler.cpp
    CGSelectPortalHandler.cpp
    CGSelectQuestHandler.cpp
    CGSelectRankBonusHandler.cpp
    CGSelectRegenZoneHandler.cpp
    CGSelectTileEffectHandler.cpp
    CGSelectWayPointHandler.cpp
    CGSetSlayerHotKeyHandler.cpp
    CGSetVampireHotKeyHandler.cpp
    CGShopRequestBuyHandler.cpp
    CGShopRequestListHandler.cpp
    CGShopRequestSellHandler.cpp
    CGSilverCoatingHandler.cpp
    CGSkillToInventoryHandler.cpp
    CGSkillToNamedHandler.cpp
    CGSkillToObjectHandler.cpp
    CGSkillToSelfHandler.cpp
    CGSkillToTileHandler.cpp
    CGSMSAddressListHandler.cpp
    CGSMSSendHandler.cpp
    CGStashDepositHandler.cpp
    CGStashListHandler.cpp
    CGStashRequestBuyHandler.cpp
    CGStashToMouseHandler.cpp
    CGStashWithdrawHandler.cpp
    # CGDisplayItem.cpp / CGGQuestCancel.cpp / CGStoreClose.cpp /
    # CGStoreOpen.cpp / CGStoreSign.cpp / CGRequestStoreInfo.cpp moved to
    # shared/Packets/ —
    # Phase 12 migration. Handlers stay here (handler symbols are still
    # game-server-local until the handler layer is tackled separately).
    CGStoreCloseHandler.cpp
    CGStoreOpenHandler.cpp
    CGStoreSignHandler.cpp
    CGSubmitScoreHandler.cpp
    CGTakeOutGoodHandler.cpp
    CGTameMonsterHandler.cpp
    CGThrowBombHandler.cpp
    CGThrowItemHandler.cpp
    CGTradeAddItemHandler.cpp
    CGTradeFinishHandler.cpp
    CGTradeMoneyHandler.cpp
    CGTradePrepareHandler.cpp
    CGTradeRemoveItemHandler.cpp
    CGTryJoinGuildHandler.cpp
    CGTypeStringListHandler.cpp
    CGUnburrowHandler.cpp
    CGUndisplayItemHandler.cpp
    CGUntransformHandler.cpp
    CGUseBonusPointHandler.cpp
    CGUseItemFromGearHandler.cpp
    CGUseItemFromGQuestInventoryHandler.cpp
    CGUseItemFromInventoryHandler.cpp
    CGUseMessageItemFromInventoryHandler.cpp
    CGUsePotionFromInventoryHandler.cpp
    CGUsePotionFromQuickSlotHandler.cpp
    CGUsePowerPointHandler.cpp
    CGVerifyTimeHandler.cpp
    CGVisibleHandler.cpp
    CGWhisperHandler.cpp
    CGWithdrawPetHandler.cpp
    CGWithdrawTaxHandler.cpp
    # Exchange System Packets
    CGExchangeListHandler.cpp
    CGExchangeBuyHandler.cpp
)

# CL Packets (Client -> Login)
set(CL_PACKET_SOURCES
    CLChangeServerHandler.cpp
    CLCreatePCHandler.cpp
    CLDeletePCHandler.cpp
    CLGetPCListHandler.cpp
    CLGetServerListHandler.cpp
    CLGetWorldListHandler.cpp
    CLLoginHandler.cpp
    CLLogoutHandler.cpp
    CLQueryCharacterNameHandler.cpp
    CLQueryPlayerIDHandler.cpp
    CLReconnectLoginHandler.cpp
    CLRegisterPlayerHandler.cpp
    CLSelectPCHandler.cpp
    CLSelectServerHandler.cpp
    CLSelectWorldHandler.cpp
    CLVersionCheckHandler.cpp
    GCDisconnect.cpp GCDisconnectHandler.cpp
	CGConnectSetKeyHandler.cpp 
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

# 通用 Packet 管理
set(PACKET_COMMON_SOURCES
    PacketFactoryManager.cpp
    PacketIDSet.cpp
    PacketValidator.cpp
)

# ============================================================================
# GameServerPackets 库 - 使用 __GAME_SERVER__ 宏编译
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
)
# Phase 12 migration: INTERFACE dependency pulls in shared/Packets/
# sources (e.g. CGBuyStoreItem.cpp, GCMyStoreInfo.cpp,
# GCOtherStoreInfo.cpp, GCRemoveStoreItem.cpp, GCAddStoreItem.cpp,
# CGDisplayItem.cpp, CGGQuestCancel.cpp, CGStoreOpen.cpp,
# CGStoreClose.cpp, CGStoreSign.cpp, CGRequestStoreInfo.cpp)
# compiled into this target with the target's own __GAME_SERVER__ / __COMBAT__ defines
# applied. See shared/Packets/CMakeLists.txt for the CG/GC/… family
# breakdown.
target_link_libraries(GameServerPackets PRIVATE shared_packets)
target_link_libraries(GameServerPackets PRIVATE GameServerDatabase)

# ============================================================================
# LoginServerPackets 库 - 使用 __LOGIN_SERVER__ 宏编译
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
    # Phase 12: PacketFactoryManager.cpp (in PACKET_COMMON_SOURCES)
    # unconditionally #includes CGBuyStoreItem.h, CGDisplayItem.h,
    # CGGQuestCancel.h, CGStore{Open,Close,Sign}.h, and
    # CGRequestStoreInfo.h, which now live in shared/Packets/. The CG
    # .cpp sources are NOT absorbed here (they compile under __GAME_SERVER__
    # via shared_packets linked into GameServerPackets); only the headers
    # matter for login / shared-server PacketFactoryManager symbols.
    ${CMAKE_SOURCE_DIR}/../shared/Packets
)
# Phase 12 CL migration: CL*.cpp sources (CLLogin, CLGetPCList, ...)
# live in shared/Packets/ and need to compile into LoginServerPackets
# under __LOGIN_SERVER__. shared_packets_cl is a separate INTERFACE
# target that carries ONLY the CL family so we pick them up without
# also absorbing the CG sources (which belong to GameServerPackets).
target_link_libraries(LoginServerPackets PRIVATE shared_packets_cl)
target_link_libraries(LoginServerPackets PRIVATE LoginServerDatabase)

# ============================================================================
# SharedServerPackets 库 - 使用 __SHARED_SERVER__ 宏编译
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
    # Phase 12: see LoginServerPackets note above — include-only, no link.
    ${CMAKE_SOURCE_DIR}/../shared/Packets
)
target_link_libraries(SharedServerPackets PRIVATE SharedServerDatabase)
