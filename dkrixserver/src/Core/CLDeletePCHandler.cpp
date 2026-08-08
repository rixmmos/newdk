//////////////////////////////////////////////////////////////////////////////
// Filename    : CLDeletePCHandler.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CLDeletePC.h"

#ifdef __LOGIN_SERVER__
#include <cstdio>

#include "Assert.h"
#include "DB.h"
#include "ItemDestroyer.h"
#include "LCDeletePCError.h"
#include "LCDeletePCOK.h"
#include "LoginPlayer.h"
#include "PreparedStatement.h"
#include "Properties.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CLDeletePCHandler::execute(CLDeletePC* pPacket, Player* pPlayer) {
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __LOGIN_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    cout << pPacket->toString() << endl;

    LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);
    LCDeletePCError lcDeletePCError;
    WorldID_t WorldID = pLoginPlayer->getWorldID();

    try {
        Connection* pLoginConn = g_pDatabaseManager->getConnection("DARKEDEN");
        Connection* pConn = g_pDatabaseManager->getConnection(WorldID);

        ////////////////////////////////////////////////////////////
        
        ////////////////////////////////////////////////////////////
        //		printf("SELECT Active FROM Slayer WHERE Name = '%s' AND Active='ACTIVE' AND PlayerID='%s'",
        // pPacket->getName().c_str(), pPlayer->getID().c_str());
        PreparedStatement selectSlayerStmt(pConn, "SELECT PlayerID FROM Slayer WHERE Name = ? AND Active='ACTIVE'");
        selectSlayerStmt.bindString(1, pPacket->getName());
        Result* pResult = selectSlayerStmt.execute();
        if (pResult->getRowCount() != 1) {
            lcDeletePCError.setErrorID(NOT_FOUND_PLAYER);
            throw InvalidProtocolException("no such slayer exist.");
        }

        if (pResult->next()) {
            string id = pResult->getString(1);
            if (id != pPlayer->getID()) {
                filelog("DeletePC.log", "Illegal PC Delete : [%s:%s]", pPlayer->getID().c_str(),
                        pPacket->getName().c_str());
                throw InvalidProtocolException("illegal pc delete");
            }
        }

        // Character deletion is confirmation-only in this build. The old client
        // asked for an SSN/code, but this server only needs to verify ownership.

        ////////////////////////////////////////////////////////////
        
        ////////////////////////////////////////////////////////////
#if defined(__CHINA_SERVER__) || defined(__THAILAND_SERVER__) || defined(__NETMARBLE_SERVER__)
        PreparedStatement deleteSlayerStmt(pConn, "DELETE FROM Slayer WHERE Name = ? AND Slot = ?");
        deleteSlayerStmt.bindString(1, pPacket->getName());
        deleteSlayerStmt.bindString(2, Slot2String[pPacket->getSlot()]);
        deleteSlayerStmt.execute();
        uint nSlayerAffected = deleteSlayerStmt.getAffectedRowCount();
#else
        PreparedStatement inactivateSlayerStmt(
            pConn, "UPDATE Slayer SET Active='INACTIVE' WHERE Name = ? AND Slot = ?");
        inactivateSlayerStmt.bindString(1, pPacket->getName());
        inactivateSlayerStmt.bindString(2, Slot2String[pPacket->getSlot()]);
        inactivateSlayerStmt.execute();
        uint nSlayerAffected = inactivateSlayerStmt.getAffectedRowCount();
#endif

        if (nSlayerAffected != 1) {
            lcDeletePCError.setErrorID(NOT_FOUND_ID);
            throw InvalidProtocolException("no such slayer exist.");
        }

#if !defined(__CHINA_SERVER__) && !defined(__THAILAND_SERVER__) && !defined(__NETMARBLE_SERVER__)
        PreparedStatement insertDeleteCharStmt(
            pLoginConn, "INSERT INTO DeleteChar (PlayerID, WorldID, Name, delDate) VALUES (?,?,?,now())");
        insertDeleteCharStmt.bindString(1, pLoginPlayer->getID());
        insertDeleteCharStmt.bindUInt(2, WorldID);
        insertDeleteCharStmt.bindString(3, pPacket->getName());
        insertDeleteCharStmt.execute();
#endif

        ////////////////////////////////////////////////////////////
        
        ////////////////////////////////////////////////////////////
#if defined(__CHINA_SERVER__) || defined(__THAILAND_SERVER__) || defined(__NETMARBLE_SERVER__)
        PreparedStatement deleteVampireStmt(pConn, "DELETE FROM Vampire WHERE Name = ? AND Slot = ?");
        deleteVampireStmt.bindString(1, pPacket->getName());
        deleteVampireStmt.bindString(2, Slot2String[pPacket->getSlot()]);
        deleteVampireStmt.execute();
#else
        PreparedStatement inactivateVampireStmt(
            pConn, "UPDATE Vampire SET Active='INACTIVE' WHERE Name = ? AND Slot = ?");
        inactivateVampireStmt.bindString(1, pPacket->getName());
        inactivateVampireStmt.bindString(2, Slot2String[pPacket->getSlot()]);
        inactivateVampireStmt.execute();
#endif
        // if (pStmt->getAffectedRowCount() == 1) //cout << "Vampire(" << pPacket->getName() << ") deleted
        // successfully...." << endl;

        ////////////////////////////////////////////////////////////
        
        ////////////////////////////////////////////////////////////
#if defined(__CHINA_SERVER__) || defined(__THAILAND_SERVER__) || defined(__NETMARBLE_SERVER__)
        PreparedStatement deleteOustersStmt(pConn, "DELETE FROM Ousters WHERE Name = ? AND Slot = ?");
        deleteOustersStmt.bindString(1, pPacket->getName());
        deleteOustersStmt.bindString(2, Slot2String[pPacket->getSlot()]);
        deleteOustersStmt.execute();
#else
        PreparedStatement inactivateOustersStmt(
            pConn, "UPDATE Ousters SET Active='INACTIVE' WHERE Name = ? AND Slot = ?");
        inactivateOustersStmt.bindString(1, pPacket->getName());
        inactivateOustersStmt.bindString(2, Slot2String[pPacket->getSlot()]);
        inactivateOustersStmt.execute();
#endif

#if defined(__CHINA_SERVER__) || defined(__THAILAND_SERVER__) || defined(__NETMARBLE_SERVER__)
        ////////////////////////////////////////////////////////////
        
        ////////////////////////////////////////////////////////////
        PreparedStatement deleteSkillSaveStmt(pConn, "DELETE FROM SkillSave WHERE OwnerID = ?");
        deleteSkillSaveStmt.bindString(1, pPacket->getName());
        deleteSkillSaveStmt.execute();
        // cout << "Slayer(" << pPacket->getName() << ") deleted successfully...." << endl;

        ////////////////////////////////////////////////////////////
        
        ////////////////////////////////////////////////////////////
        PreparedStatement deleteVampireSkillSaveStmt(pConn, "DELETE FROM VampireSkillSave WHERE OwnerID = ?");
        deleteVampireSkillSaveStmt.bindString(1, pPacket->getName());
        deleteVampireSkillSaveStmt.execute();

        ////////////////////////////////////////////////////////////
        
        ////////////////////////////////////////////////////////////
        PreparedStatement deleteOustersSkillSaveStmt(pConn, "DELETE FROM OustersSkillSave WHERE OwnerID = ?");
        deleteOustersSkillSaveStmt.bindString(1, pPacket->getName());
        deleteOustersSkillSaveStmt.execute();
#endif

        ////////////////////////////////////////////////////////////
        
        ////////////////////////////////////////////////////////////
        PreparedStatement deleteRankBonusDataStmt(pConn, "DELETE FROM RankBonusData WHERE OwnerID = ?");
        deleteRankBonusDataStmt.bindString(1, pPacket->getName());
        deleteRankBonusDataStmt.execute();

        ////////////////////////////////////////////////////////////
        
        ////////////////////////////////////////////////////////////
        //		g_pItemDestroyer->destroyAll(pPacket->getName());
        string ownerID = pPacket->getName();
        PreparedStatement deleteARObjectStmt(pConn, "DELETE FROM ARObject WHERE OwnerID = ?");
        deleteARObjectStmt.bindString(1, ownerID);
        deleteARObjectStmt.execute();
        PreparedStatement deleteBeltObjectStmt(pConn, "DELETE FROM BeltObject WHERE OwnerID = ?");
        deleteBeltObjectStmt.bindString(1, ownerID);
        deleteBeltObjectStmt.execute();
        PreparedStatement deleteBladeObjectStmt(pConn, "DELETE FROM BladeObject WHERE OwnerID = ?");
        deleteBladeObjectStmt.bindString(1, ownerID);
        deleteBladeObjectStmt.execute();
        PreparedStatement deleteBloodBibleObjectStmt(pConn, "DELETE FROM BloodBibleObject WHERE OwnerID = ?");
        deleteBloodBibleObjectStmt.bindString(1, ownerID);
        deleteBloodBibleObjectStmt.execute();
        PreparedStatement deleteBombMaterialObjectStmt(pConn, "DELETE FROM BombMaterialObject WHERE OwnerID = ?");
        deleteBombMaterialObjectStmt.bindString(1, ownerID);
        deleteBombMaterialObjectStmt.execute();
        PreparedStatement deleteBombObjectStmt(pConn, "DELETE FROM BombObject WHERE OwnerID = ?");
        deleteBombObjectStmt.bindString(1, ownerID);
        deleteBombObjectStmt.execute();
        PreparedStatement deleteBraceletObjectStmt(pConn, "DELETE FROM BraceletObject WHERE OwnerID = ?");
        deleteBraceletObjectStmt.bindString(1, ownerID);
        deleteBraceletObjectStmt.execute();
        PreparedStatement deleteCastleSymbolObjectStmt(pConn, "DELETE FROM CastleSymbolObject WHERE OwnerID = ?");
        deleteCastleSymbolObjectStmt.bindString(1, ownerID);
        deleteCastleSymbolObjectStmt.execute();
        PreparedStatement deleteCoatObjectStmt(pConn, "DELETE FROM CoatObject WHERE OwnerID = ?");
        deleteCoatObjectStmt.bindString(1, ownerID);
        deleteCoatObjectStmt.execute();
        PreparedStatement deleteCrossObjectStmt(pConn, "DELETE FROM CrossObject WHERE OwnerID = ?");
        deleteCrossObjectStmt.bindString(1, ownerID);
        deleteCrossObjectStmt.execute();
        PreparedStatement deleteETCObjectStmt(pConn, "DELETE FROM ETCObject WHERE OwnerID = ?");
        deleteETCObjectStmt.bindString(1, ownerID);
        deleteETCObjectStmt.execute();
        PreparedStatement deleteEventETCObjectStmt(pConn, "DELETE FROM EventETCObject WHERE OwnerID = ?");
        deleteEventETCObjectStmt.bindString(1, ownerID);
        deleteEventETCObjectStmt.execute();
        PreparedStatement deleteEventGiftBoxObjectStmt(pConn, "DELETE FROM EventGiftBoxObject WHERE OwnerID = ?");
        deleteEventGiftBoxObjectStmt.bindString(1, ownerID);
        deleteEventGiftBoxObjectStmt.execute();
        PreparedStatement deleteEventStarObjectStmt(pConn, "DELETE FROM EventStarObject WHERE OwnerID = ?");
        deleteEventStarObjectStmt.bindString(1, ownerID);
        deleteEventStarObjectStmt.execute();
        PreparedStatement deleteEventTreeObjectStmt(pConn, "DELETE FROM EventTreeObject WHERE OwnerID = ?");
        deleteEventTreeObjectStmt.bindString(1, ownerID);
        deleteEventTreeObjectStmt.execute();
        PreparedStatement deleteGloveObjectStmt(pConn, "DELETE FROM GloveObject WHERE OwnerID = ?");
        deleteGloveObjectStmt.bindString(1, ownerID);
        deleteGloveObjectStmt.execute();
        PreparedStatement deleteHelmObjectStmt(pConn, "DELETE FROM HelmObject WHERE OwnerID = ?");
        deleteHelmObjectStmt.bindString(1, ownerID);
        deleteHelmObjectStmt.execute();
        PreparedStatement deleteHolyWaterObjectStmt(pConn, "DELETE FROM HolyWaterObject WHERE OwnerID = ?");
        deleteHolyWaterObjectStmt.bindString(1, ownerID);
        deleteHolyWaterObjectStmt.execute();
        PreparedStatement deleteKeyObjectStmt(pConn, "DELETE FROM KeyObject WHERE OwnerID = ?");
        deleteKeyObjectStmt.bindString(1, ownerID);
        deleteKeyObjectStmt.execute();
        PreparedStatement deleteLearningItemObjectStmt(pConn, "DELETE FROM LearningItemObject WHERE OwnerID = ?");
        deleteLearningItemObjectStmt.bindString(1, ownerID);
        deleteLearningItemObjectStmt.execute();
        PreparedStatement deleteMaceObjectStmt(pConn, "DELETE FROM MaceObject WHERE OwnerID = ?");
        deleteMaceObjectStmt.bindString(1, ownerID);
        deleteMaceObjectStmt.execute();
        PreparedStatement deleteMagazineObjectStmt(pConn, "DELETE FROM MagazineObject WHERE OwnerID = ?");
        deleteMagazineObjectStmt.bindString(1, ownerID);
        deleteMagazineObjectStmt.execute();
        PreparedStatement deleteMineObjectStmt(pConn, "DELETE FROM MineObject WHERE OwnerID = ?");
        deleteMineObjectStmt.bindString(1, ownerID);
        deleteMineObjectStmt.execute();
        PreparedStatement deleteMoneyObjectStmt(pConn, "DELETE FROM MoneyObject WHERE OwnerID = ?");
        deleteMoneyObjectStmt.bindString(1, ownerID);
        deleteMoneyObjectStmt.execute();
        PreparedStatement deleteMotorcycleObjectStmt(pConn, "DELETE FROM MotorcycleObject WHERE OwnerID = ?");
        deleteMotorcycleObjectStmt.bindString(1, ownerID);
        deleteMotorcycleObjectStmt.execute();
        PreparedStatement deleteNecklaceObjectStmt(pConn, "DELETE FROM NecklaceObject WHERE OwnerID = ?");
        deleteNecklaceObjectStmt.bindString(1, ownerID);
        deleteNecklaceObjectStmt.execute();
        PreparedStatement deletePotionObjectStmt(pConn, "DELETE FROM PotionObject WHERE OwnerID = ?");
        deletePotionObjectStmt.bindString(1, ownerID);
        deletePotionObjectStmt.execute();
        PreparedStatement deleteQuestItemObjectStmt(pConn, "DELETE FROM QuestItemObject WHERE OwnerID = ?");
        deleteQuestItemObjectStmt.bindString(1, ownerID);
        deleteQuestItemObjectStmt.execute();
        PreparedStatement deleteRelicObjectStmt(pConn, "DELETE FROM RelicObject WHERE OwnerID = ?");
        deleteRelicObjectStmt.bindString(1, ownerID);
        deleteRelicObjectStmt.execute();
        PreparedStatement deleteSGObjectStmt(pConn, "DELETE FROM SGObject WHERE OwnerID = ?");
        deleteSGObjectStmt.bindString(1, ownerID);
        deleteSGObjectStmt.execute();
        PreparedStatement deleteSMGObjectStmt(pConn, "DELETE FROM SMGObject WHERE OwnerID = ?");
        deleteSMGObjectStmt.bindString(1, ownerID);
        deleteSMGObjectStmt.execute();
        PreparedStatement deleteSRObjectStmt(pConn, "DELETE FROM SRObject WHERE OwnerID = ?");
        deleteSRObjectStmt.bindString(1, ownerID);
        deleteSRObjectStmt.execute();
        PreparedStatement deleteSerumObjectStmt(pConn, "DELETE FROM SerumObject WHERE OwnerID = ?");
        deleteSerumObjectStmt.bindString(1, ownerID);
        deleteSerumObjectStmt.execute();
        PreparedStatement deleteShieldObjectStmt(pConn, "DELETE FROM ShieldObject WHERE OwnerID = ?");
        deleteShieldObjectStmt.bindString(1, ownerID);
        deleteShieldObjectStmt.execute();
        PreparedStatement deleteShoesObjectStmt(pConn, "DELETE FROM ShoesObject WHERE OwnerID = ?");
        deleteShoesObjectStmt.bindString(1, ownerID);
        deleteShoesObjectStmt.execute();
        PreparedStatement deleteSkullObjectStmt(pConn, "DELETE FROM SkullObject WHERE OwnerID = ?");
        deleteSkullObjectStmt.bindString(1, ownerID);
        deleteSkullObjectStmt.execute();
        PreparedStatement deleteSlayerPortalItemObjectStmt(
            pConn, "DELETE FROM SlayerPortalItemObject WHERE OwnerID = ?");
        deleteSlayerPortalItemObjectStmt.bindString(1, ownerID);
        deleteSlayerPortalItemObjectStmt.execute();
        PreparedStatement deleteSwordObjectStmt(pConn, "DELETE FROM SwordObject WHERE OwnerID = ?");
        deleteSwordObjectStmt.bindString(1, ownerID);
        deleteSwordObjectStmt.execute();
        PreparedStatement deleteTrouserObjectStmt(pConn, "DELETE FROM TrouserObject WHERE OwnerID = ?");
        deleteTrouserObjectStmt.bindString(1, ownerID);
        deleteTrouserObjectStmt.execute();
        PreparedStatement deleteRingObjectStmt(pConn, "DELETE FROM RingObject WHERE OwnerID = ?");
        deleteRingObjectStmt.bindString(1, ownerID);
        deleteRingObjectStmt.execute();
        PreparedStatement deleteCoupleRingObjectStmt(pConn, "DELETE FROM CoupleRingObject WHERE OwnerID = ?");
        deleteCoupleRingObjectStmt.bindString(1, ownerID);
        deleteCoupleRingObjectStmt.execute();
        PreparedStatement deleteVampireAmuletObjectStmt(pConn, "DELETE FROM VampireAmuletObject WHERE OwnerID = ?");
        deleteVampireAmuletObjectStmt.bindString(1, ownerID);
        deleteVampireAmuletObjectStmt.execute();
        PreparedStatement deleteVampireBraceletObjectStmt(pConn, "DELETE FROM VampireBraceletObject WHERE OwnerID = ?");
        deleteVampireBraceletObjectStmt.bindString(1, ownerID);
        deleteVampireBraceletObjectStmt.execute();
        PreparedStatement deleteVampireCoatObjectStmt(pConn, "DELETE FROM VampireCoatObject WHERE OwnerID = ?");
        deleteVampireCoatObjectStmt.bindString(1, ownerID);
        deleteVampireCoatObjectStmt.execute();
        PreparedStatement deleteVampireETCObjectStmt(pConn, "DELETE FROM VampireETCObject WHERE OwnerID = ?");
        deleteVampireETCObjectStmt.bindString(1, ownerID);
        deleteVampireETCObjectStmt.execute();
        PreparedStatement deleteVampireEarringObjectStmt(pConn, "DELETE FROM VampireEarringObject WHERE OwnerID = ?");
        deleteVampireEarringObjectStmt.bindString(1, ownerID);
        deleteVampireEarringObjectStmt.execute();
        PreparedStatement deleteVampireNecklaceObjectStmt(pConn, "DELETE FROM VampireNecklaceObject WHERE OwnerID = ?");
        deleteVampireNecklaceObjectStmt.bindString(1, ownerID);
        deleteVampireNecklaceObjectStmt.execute();
        PreparedStatement deleteVampirePortalItemObjectStmt(
            pConn, "DELETE FROM VampirePortalItemObject WHERE OwnerID = ?");
        deleteVampirePortalItemObjectStmt.bindString(1, ownerID);
        deleteVampirePortalItemObjectStmt.execute();
        PreparedStatement deleteVampireRingObjectStmt(pConn, "DELETE FROM VampireRingObject WHERE OwnerID = ?");
        deleteVampireRingObjectStmt.bindString(1, ownerID);
        deleteVampireRingObjectStmt.execute();
        PreparedStatement deleteVampireWeaponObjectStmt(pConn, "DELETE FROM VampireWeaponObject WHERE OwnerID = ?");
        deleteVampireWeaponObjectStmt.bindString(1, ownerID);
        deleteVampireWeaponObjectStmt.execute();
        PreparedStatement deleteVampireCoupleRingObjectStmt(
            pConn, "DELETE FROM VampireCoupleRingObject WHERE OwnerID = ?");
        deleteVampireCoupleRingObjectStmt.bindString(1, ownerID);
        deleteVampireCoupleRingObjectStmt.execute();
        PreparedStatement deleteWaterObjectStmt(pConn, "DELETE FROM WaterObject WHERE OwnerID = ?");
        deleteWaterObjectStmt.bindString(1, ownerID);
        deleteWaterObjectStmt.execute();
        PreparedStatement deleteEventItemObjectStmt(pConn, "DELETE FROM EventItemObject WHERE OwnerID = ?");
        deleteEventItemObjectStmt.bindString(1, ownerID);
        deleteEventItemObjectStmt.execute();
        PreparedStatement deleteDyePotionObjectStmt(pConn, "DELETE FROM DyePotionObject WHERE OwnerID = ?");
        deleteDyePotionObjectStmt.bindString(1, ownerID);
        deleteDyePotionObjectStmt.execute();
        PreparedStatement deleteResurrectItemObjectStmt(pConn, "DELETE FROM ResurrectItemObject WHERE OwnerID = ?");
        deleteResurrectItemObjectStmt.bindString(1, ownerID);
        deleteResurrectItemObjectStmt.execute();
        PreparedStatement deleteMixingItemObjectStmt(pConn, "DELETE FROM MixingItemObject WHERE OwnerID = ?");
        deleteMixingItemObjectStmt.bindString(1, ownerID);
        deleteMixingItemObjectStmt.execute();
        PreparedStatement deleteOustersArmsbandObjectStmt(pConn, "DELETE FROM OustersArmsbandObject WHERE OwnerID = ?");
        deleteOustersArmsbandObjectStmt.bindString(1, ownerID);
        deleteOustersArmsbandObjectStmt.execute();
        PreparedStatement deleteOustersBootsObjectStmt(pConn, "DELETE FROM OustersBootsObject WHERE OwnerID = ?");
        deleteOustersBootsObjectStmt.bindString(1, ownerID);
        deleteOustersBootsObjectStmt.execute();
        PreparedStatement deleteOustersChakramObjectStmt(pConn, "DELETE FROM OustersChakramObject WHERE OwnerID = ?");
        deleteOustersChakramObjectStmt.bindString(1, ownerID);
        deleteOustersChakramObjectStmt.execute();
        PreparedStatement deleteOustersCircletObjectStmt(pConn, "DELETE FROM OustersCircletObject WHERE OwnerID = ?");
        deleteOustersCircletObjectStmt.bindString(1, ownerID);
        deleteOustersCircletObjectStmt.execute();
        PreparedStatement deleteOustersCoatObjectStmt(pConn, "DELETE FROM OustersCoatObject WHERE OwnerID = ?");
        deleteOustersCoatObjectStmt.bindString(1, ownerID);
        deleteOustersCoatObjectStmt.execute();
        PreparedStatement deleteOustersPendentObjectStmt(pConn, "DELETE FROM OustersPendentObject WHERE OwnerID = ?");
        deleteOustersPendentObjectStmt.bindString(1, ownerID);
        deleteOustersPendentObjectStmt.execute();
        PreparedStatement deleteOustersRingObjectStmt(pConn, "DELETE FROM OustersRingObject WHERE OwnerID = ?");
        deleteOustersRingObjectStmt.bindString(1, ownerID);
        deleteOustersRingObjectStmt.execute();
        PreparedStatement deleteOustersStoneObjectStmt(pConn, "DELETE FROM OustersStoneObject WHERE OwnerID = ?");
        deleteOustersStoneObjectStmt.bindString(1, ownerID);
        deleteOustersStoneObjectStmt.execute();
        PreparedStatement deleteOustersWristletObjectStmt(pConn, "DELETE FROM OustersWristletObject WHERE OwnerID = ?");
        deleteOustersWristletObjectStmt.bindString(1, ownerID);
        deleteOustersWristletObjectStmt.execute();
        PreparedStatement deleteLarvaObjectStmt(pConn, "DELETE FROM LarvaObject WHERE OwnerID = ?");
        deleteLarvaObjectStmt.bindString(1, ownerID);
        deleteLarvaObjectStmt.execute();
        PreparedStatement deletePupaObjectStmt(pConn, "DELETE FROM PupaObject WHERE OwnerID = ?");
        deletePupaObjectStmt.bindString(1, ownerID);
        deletePupaObjectStmt.execute();
        PreparedStatement deleteComposMeiObjectStmt(pConn, "DELETE FROM ComposMeiObject WHERE OwnerID = ?");
        deleteComposMeiObjectStmt.bindString(1, ownerID);
        deleteComposMeiObjectStmt.execute();
        PreparedStatement deleteOustersSummonItemObjectStmt(
            pConn, "DELETE FROM OustersSummonItemObject WHERE OwnerID = ?");
        deleteOustersSummonItemObjectStmt.bindString(1, ownerID);
        deleteOustersSummonItemObjectStmt.execute();
        PreparedStatement deleteEffectItemObjectStmt(pConn, "DELETE FROM EffectItemObject WHERE OwnerID = ?");
        deleteEffectItemObjectStmt.bindString(1, ownerID);
        deleteEffectItemObjectStmt.execute();
        PreparedStatement deleteCodeSheetObjectStmt(pConn, "DELETE FROM CodeSheetObject WHERE OwnerID = ?");
        deleteCodeSheetObjectStmt.bindString(1, ownerID);
        deleteCodeSheetObjectStmt.execute();
        PreparedStatement deleteMoonCardObjectStmt(pConn, "DELETE FROM MoonCardObject WHERE OwnerID = ?");
        deleteMoonCardObjectStmt.bindString(1, ownerID);
        deleteMoonCardObjectStmt.execute();
        PreparedStatement deleteSweeperObjectStmt(pConn, "DELETE FROM SweeperObject WHERE OwnerID = ?");
        deleteSweeperObjectStmt.bindString(1, ownerID);
        deleteSweeperObjectStmt.execute();
        PreparedStatement deletePetItemObjectStmt(pConn, "DELETE FROM PetItemObject WHERE OwnerID = ?");
        deletePetItemObjectStmt.bindString(1, ownerID);
        deletePetItemObjectStmt.execute();
        PreparedStatement deletePetFoodObjectStmt(pConn, "DELETE FROM PetFoodObject WHERE OwnerID = ?");
        deletePetFoodObjectStmt.bindString(1, ownerID);
        deletePetFoodObjectStmt.execute();
        PreparedStatement deletePetEnchantItemObjectStmt(pConn, "DELETE FROM PetEnchantItemObject WHERE OwnerID = ?");
        deletePetEnchantItemObjectStmt.bindString(1, ownerID);
        deletePetEnchantItemObjectStmt.execute();
        PreparedStatement deleteLuckyBagObjectStmt(pConn, "DELETE FROM LuckyBagObject WHERE OwnerID = ?");
        deleteLuckyBagObjectStmt.bindString(1, ownerID);
        deleteLuckyBagObjectStmt.execute();
#ifndef __THAILAND_SERVER__
        PreparedStatement deleteSMSItemObjectStmt(pConn, "DELETE FROM SMSItemObject WHERE OwnerID = ?");
        deleteSMSItemObjectStmt.bindString(1, ownerID);
        deleteSMSItemObjectStmt.execute();
        PreparedStatement deleteCoreZapObjectStmt(pConn, "DELETE FROM CoreZapObject WHERE OwnerID = ?");
        deleteCoreZapObjectStmt.bindString(1, ownerID);
        deleteCoreZapObjectStmt.execute();
        PreparedStatement deleteGQuestItemObjectStmt(pConn, "DELETE FROM GQuestItemObject WHERE OwnerID = ?");
        deleteGQuestItemObjectStmt.bindString(1, ownerID);
        deleteGQuestItemObjectStmt.execute();
        PreparedStatement deleteGQuestSaveStmt(pConn, "DELETE FROM GQuestSave WHERE OwnerID = ?");
        deleteGQuestSaveStmt.bindString(1, ownerID);
        deleteGQuestSaveStmt.execute();
        PreparedStatement deleteTrapItemObjectStmt(pConn, "DELETE FROM TrapItemObject WHERE OwnerID = ?");
        deleteTrapItemObjectStmt.bindString(1, ownerID);
        deleteTrapItemObjectStmt.execute();
#endif
        PreparedStatement deleteCarryingReceiverObjectStmt(
            pConn, "DELETE FROM CarryingReceiverObject WHERE OwnerID = ?");
        deleteCarryingReceiverObjectStmt.bindString(1, ownerID);
        deleteCarryingReceiverObjectStmt.execute();
        PreparedStatement deleteShoulderArmorObjectStmt(pConn, "DELETE FROM ShoulderArmorObject WHERE OwnerID = ?");
        deleteShoulderArmorObjectStmt.bindString(1, ownerID);
        deleteShoulderArmorObjectStmt.execute();
        PreparedStatement deleteDermisObjectStmt(pConn, "DELETE FROM DermisObject WHERE OwnerID = ?");
        deleteDermisObjectStmt.bindString(1, ownerID);
        deleteDermisObjectStmt.execute();
        PreparedStatement deletePersonaObjectStmt(pConn, "DELETE FROM PersonaObject WHERE OwnerID = ?");
        deletePersonaObjectStmt.bindString(1, ownerID);
        deletePersonaObjectStmt.execute();
        PreparedStatement deleteFasciaObjectStmt(pConn, "DELETE FROM FasciaObject WHERE OwnerID = ?");
        deleteFasciaObjectStmt.bindString(1, ownerID);
        deleteFasciaObjectStmt.execute();
        PreparedStatement deleteMittenObjectStmt(pConn, "DELETE FROM MittenObject WHERE OwnerID = ?");
        deleteMittenObjectStmt.bindString(1, ownerID);
        deleteMittenObjectStmt.execute();

        ////////////////////////////////////////////////////////////
        
        ////////////////////////////////////////////////////////////
        PreparedStatement deleteCoupleInfoFemalePartnerNameStmt(
            pConn, "DELETE FROM CoupleInfo WHERE FemalePartnerName=?");
        deleteCoupleInfoFemalePartnerNameStmt.bindString(1, ownerID);
        deleteCoupleInfoFemalePartnerNameStmt.execute();
        PreparedStatement deleteCoupleInfoMalePartnerNameStmt(pConn, "DELETE FROM CoupleInfo WHERE MalePartnerName=?");
        deleteCoupleInfoMalePartnerNameStmt.bindString(1, ownerID);
        deleteCoupleInfoMalePartnerNameStmt.execute();

        ////////////////////////////////////////////////////////////
        
        ////////////////////////////////////////////////////////////
        PreparedStatement deleteEffectAcidTouchStmt(pConn, "DELETE FROM EffectAcidTouch where OwnerID=?");
        deleteEffectAcidTouchStmt.bindString(1, pPacket->getName());
        deleteEffectAcidTouchStmt.execute();
        PreparedStatement deleteEffectAftermathStmt(pConn, "DELETE FROM EffectAftermath where OwnerID=?");
        deleteEffectAftermathStmt.bindString(1, pPacket->getName());
        deleteEffectAftermathStmt.execute();
        PreparedStatement deleteEffectBloodDrainStmt(pConn, "DELETE FROM EffectBloodDrain where OwnerID=?");
        deleteEffectBloodDrainStmt.bindString(1, pPacket->getName());
        deleteEffectBloodDrainStmt.execute();
        PreparedStatement deleteEffectDetectHiddenStmt(pConn, "DELETE FROM EffectDetectHidden where OwnerID=?");
        deleteEffectDetectHiddenStmt.bindString(1, pPacket->getName());
        deleteEffectDetectHiddenStmt.execute();
        PreparedStatement deleteEffectFlareStmt(pConn, "DELETE FROM EffectFlare where OwnerID=?");
        deleteEffectFlareStmt.bindString(1, pPacket->getName());
        deleteEffectFlareStmt.execute();
        PreparedStatement deleteEffectLightStmt(pConn, "DELETE FROM EffectLight where OwnerID=?");
        deleteEffectLightStmt.bindString(1, pPacket->getName());
        deleteEffectLightStmt.execute();
        PreparedStatement deleteEffectParalysisStmt(pConn, "DELETE FROM EffectParalysis where OwnerID=?");
        deleteEffectParalysisStmt.bindString(1, pPacket->getName());
        deleteEffectParalysisStmt.execute();
        PreparedStatement deleteEffectPoisonStmt(pConn, "DELETE FROM EffectPoison where OwnerID=?");
        deleteEffectPoisonStmt.bindString(1, pPacket->getName());
        deleteEffectPoisonStmt.execute();
        PreparedStatement deleteEffectPoisonousHandsStmt(pConn, "DELETE FROM EffectPoisonousHands where OwnerID=?");
        deleteEffectPoisonousHandsStmt.bindString(1, pPacket->getName());
        deleteEffectPoisonousHandsStmt.execute();
        PreparedStatement deleteEffectProtectionFromParalysisStmt(
            pConn, "DELETE FROM EffectProtectionFromParalysis where OwnerID=?");
        deleteEffectProtectionFromParalysisStmt.bindString(1, pPacket->getName());
        deleteEffectProtectionFromParalysisStmt.execute();
        PreparedStatement deleteEffectProtectionFromPoisonStmt(
            pConn, "DELETE FROM EffectProtectionFromPoison where OwnerID=?");
        deleteEffectProtectionFromPoisonStmt.bindString(1, pPacket->getName());
        deleteEffectProtectionFromPoisonStmt.execute();
        PreparedStatement deleteEffectRestoreStmt(pConn, "DELETE FROM EffectRestore where OwnerID=?");
        deleteEffectRestoreStmt.bindString(1, pPacket->getName());
        deleteEffectRestoreStmt.execute();
        PreparedStatement deleteEffectYellowPoisonToCreatureStmt(
            pConn, "DELETE FROM EffectYellowPoisonToCreature where OwnerID=?");
        deleteEffectYellowPoisonToCreatureStmt.bindString(1, pPacket->getName());
        deleteEffectYellowPoisonToCreatureStmt.execute();
        PreparedStatement deleteEffectMuteStmt(pConn, "DELETE FROM EffectMute where OwnerID=?");
        deleteEffectMuteStmt.bindString(1, pPacket->getName());
        deleteEffectMuteStmt.execute();
        PreparedStatement deleteEnemyEraseStmt(pConn, "DELETE FROM EnemyErase where OwnerID=?");
        deleteEnemyEraseStmt.bindString(1, pPacket->getName());
        deleteEnemyEraseStmt.execute();

        ////////////////////////////////////////////////////////////
        
        ////////////////////////////////////////////////////////////
        PreparedStatement deleteFlagSetStmt(pConn, "DELETE FROM FlagSet WHERE OwnerID=?");
        deleteFlagSetStmt.bindString(1, pPacket->getName());
        deleteFlagSetStmt.execute();

        ////////////////////////////////////////////////////////////
        
        ////////////////////////////////////////////////////////////
        PreparedStatement deleteTimeLimitItemsStmt(pConn, "DELETE FROM TimeLimitItems WHERE OwnerID=?");
        deleteTimeLimitItemsStmt.bindString(1, pPacket->getName());
        deleteTimeLimitItemsStmt.execute();

        ////////////////////////////////////////////////////////////
        
        ////////////////////////////////////////////////////////////
        PreparedStatement deleteEventQuestAdvanceStmt(pConn, "DELETE FROM EventQuestAdvance WHERE OwnerID=?");
        deleteEventQuestAdvanceStmt.bindString(1, pPacket->getName());
        deleteEventQuestAdvanceStmt.execute();

        ////////////////////////////////////////////////////////////
        
        ////////////////////////////////////////////////////////////
        PreparedStatement deleteMofusPowerPointStmt(pConn, "DELETE FROM MofusPowerPoint WHERE OwnerID=?");
        deleteMofusPowerPointStmt.bindString(1, pPacket->getName());
        deleteMofusPowerPointStmt.execute();

        ////////////////////////////////////////////////////////////
        
        ////////////////////////////////////////////////////////////
        LCDeletePCOK lcDeletePCOK;
        pLoginPlayer->sendPacket(&lcDeletePCOK);

        pLoginPlayer->setPlayerStatus(LPS_WAITING_FOR_CL_GET_PC_LIST);
    } catch (InvalidProtocolException& ipe) {
        cout << "Fail to deletePC : " << ipe.toString() << endl;

        pLoginPlayer->sendPacket(&lcDeletePCError);
    } catch (SQLQueryException& sqe) {
        cout << "Fail to deletePC : " << sqe.toString() << endl;

        pLoginPlayer->sendPacket(&lcDeletePCError);
    }

#endif

    __END_DEBUG_EX __END_CATCH
}
