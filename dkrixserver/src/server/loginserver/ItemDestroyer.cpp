//--------------------------------------------------------------------------------
//
// Filename   : ItemDestroyer.cpp
// Written By : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "ItemDestroyer.h"

#include "database/PreparedStatement.h"

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
ItemDestroyer::ItemDestroyer() {}


//--------------------------------------------------------------------------------
// destroy all item whose owner is ...
//
// ownerID is a player character name that arrives verbatim off the wire
// (CLDeletePC::read -> getName), so it must never be spliced into SQL text.
// Every statement below is a compile-time literal with the owner bound as a
// parameter; table names and column order are unchanged.
//--------------------------------------------------------------------------------
void ItemDestroyer::destroyAll(const string& ownerID) {
    __BEGIN_TRY

    try {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement deleteMotorcycleObjectStmt(pConn, "DELETE FROM MotorcycleObject WHERE OwnerID = ?");
        deleteMotorcycleObjectStmt.bindString(1, ownerID);
        deleteMotorcycleObjectStmt.execute();
        PreparedStatement deletePotionObjectStmt(pConn, "DELETE FROM PotionObject WHERE OwnerID = ?");
        deletePotionObjectStmt.bindString(1, ownerID);
        deletePotionObjectStmt.execute();
        PreparedStatement deleteWaterObjectStmt(pConn, "DELETE FROM WaterObject WHERE OwnerID = ?");
        deleteWaterObjectStmt.bindString(1, ownerID);
        deleteWaterObjectStmt.execute();
        PreparedStatement deleteHolyWaterObjectStmt(pConn, "DELETE FROM HolyWaterObject WHERE OwnerID = ?");
        deleteHolyWaterObjectStmt.bindString(1, ownerID);
        deleteHolyWaterObjectStmt.execute();
        PreparedStatement deleteMagazineObjectStmt(pConn, "DELETE FROM MagazineObject WHERE OwnerID = ?");
        deleteMagazineObjectStmt.bindString(1, ownerID);
        deleteMagazineObjectStmt.execute();
        PreparedStatement deleteBombMaterialObjectStmt(pConn, "DELETE FROM BombMaterialObject WHERE OwnerID = ?");
        deleteBombMaterialObjectStmt.bindString(1, ownerID);
        deleteBombMaterialObjectStmt.execute();
        PreparedStatement deleteETCObjectStmt(pConn, "DELETE FROM ETCObject WHERE OwnerID = ?");
        deleteETCObjectStmt.bindString(1, ownerID);
        deleteETCObjectStmt.execute();
        PreparedStatement deleteKeyObjectStmt(pConn, "DELETE FROM KeyObject WHERE OwnerID = ?");
        deleteKeyObjectStmt.bindString(1, ownerID);
        deleteKeyObjectStmt.execute();
        PreparedStatement deleteRingObjectStmt(pConn, "DELETE FROM RingObject WHERE OwnerID = ?");
        deleteRingObjectStmt.bindString(1, ownerID);
        deleteRingObjectStmt.execute();
        PreparedStatement deleteBraceletObjectStmt(pConn, "DELETE FROM BraceletObject WHERE OwnerID = ?");
        deleteBraceletObjectStmt.bindString(1, ownerID);
        deleteBraceletObjectStmt.execute();
        PreparedStatement deleteNecklaceObjectStmt(pConn, "DELETE FROM NecklaceObject WHERE OwnerID = ?");
        deleteNecklaceObjectStmt.bindString(1, ownerID);
        deleteNecklaceObjectStmt.execute();
        PreparedStatement deleteCoatObjectStmt(pConn, "DELETE FROM CoatObject WHERE OwnerID = ?");
        deleteCoatObjectStmt.bindString(1, ownerID);
        deleteCoatObjectStmt.execute();
        PreparedStatement deleteTrouserObjectStmt(pConn, "DELETE FROM TrouserObject WHERE OwnerID = ?");
        deleteTrouserObjectStmt.bindString(1, ownerID);
        deleteTrouserObjectStmt.execute();
        PreparedStatement deleteShoesObjectStmt(pConn, "DELETE FROM ShoesObject WHERE OwnerID = ?");
        deleteShoesObjectStmt.bindString(1, ownerID);
        deleteShoesObjectStmt.execute();
        PreparedStatement deleteSwordObjectStmt(pConn, "DELETE FROM SwordObject WHERE OwnerID = ?");
        deleteSwordObjectStmt.bindString(1, ownerID);
        deleteSwordObjectStmt.execute();
        PreparedStatement deleteBladeObjectStmt(pConn, "DELETE FROM BladeObject WHERE OwnerID = ?");
        deleteBladeObjectStmt.bindString(1, ownerID);
        deleteBladeObjectStmt.execute();
        PreparedStatement deleteShieldObjectStmt(pConn, "DELETE FROM ShieldObject WHERE OwnerID = ?");
        deleteShieldObjectStmt.bindString(1, ownerID);
        deleteShieldObjectStmt.execute();
        PreparedStatement deleteCrossObjectStmt(pConn, "DELETE FROM CrossObject WHERE OwnerID = ?");
        deleteCrossObjectStmt.bindString(1, ownerID);
        deleteCrossObjectStmt.execute();
        PreparedStatement deleteMaceObjectStmt(pConn, "DELETE FROM MaceObject WHERE OwnerID = ?");
        deleteMaceObjectStmt.bindString(1, ownerID);
        deleteMaceObjectStmt.execute();
        PreparedStatement deleteGloveObjectStmt(pConn, "DELETE FROM GloveObject WHERE OwnerID = ?");
        deleteGloveObjectStmt.bindString(1, ownerID);
        deleteGloveObjectStmt.execute();
        PreparedStatement deleteHelmObjectStmt(pConn, "DELETE FROM HelmObject WHERE OwnerID = ?");
        deleteHelmObjectStmt.bindString(1, ownerID);
        deleteHelmObjectStmt.execute();
        PreparedStatement deleteSGObjectStmt(pConn, "DELETE FROM SGObject WHERE OwnerID = ?");
        deleteSGObjectStmt.bindString(1, ownerID);
        deleteSGObjectStmt.execute();
        PreparedStatement deleteSMGObjectStmt(pConn, "DELETE FROM SMGObject WHERE OwnerID = ?");
        deleteSMGObjectStmt.bindString(1, ownerID);
        deleteSMGObjectStmt.execute();
        PreparedStatement deleteARObjectStmt(pConn, "DELETE FROM ARObject WHERE OwnerID = ?");
        deleteARObjectStmt.bindString(1, ownerID);
        deleteARObjectStmt.execute();
        PreparedStatement deleteSRObjectStmt(pConn, "DELETE FROM SRObject WHERE OwnerID = ?");
        deleteSRObjectStmt.bindString(1, ownerID);
        deleteSRObjectStmt.execute();
        PreparedStatement deleteBombObjectStmt(pConn, "DELETE FROM BombObject WHERE OwnerID = ?");
        deleteBombObjectStmt.bindString(1, ownerID);
        deleteBombObjectStmt.execute();
        PreparedStatement deleteMineObjectStmt(pConn, "DELETE FROM MineObject WHERE OwnerID = ?");
        deleteMineObjectStmt.bindString(1, ownerID);
        deleteMineObjectStmt.execute();
        PreparedStatement deleteBeltObjectStmt(pConn, "DELETE FROM BeltObject WHERE OwnerID = ?");
        deleteBeltObjectStmt.bindString(1, ownerID);
        deleteBeltObjectStmt.execute();
        PreparedStatement deleteLearningItemObjectStmt(pConn, "DELETE FROM LearningItemObject WHERE OwnerID = ?");
        deleteLearningItemObjectStmt.bindString(1, ownerID);
        deleteLearningItemObjectStmt.execute();
        PreparedStatement deleteVampireRingObjectStmt(pConn, "DELETE FROM VampireRingObject WHERE OwnerID = ?");
        deleteVampireRingObjectStmt.bindString(1, ownerID);
        deleteVampireRingObjectStmt.execute();
        PreparedStatement deleteVampireBraceletObjectStmt(pConn, "DELETE FROM VampireBraceletObject WHERE OwnerID = ?");
        deleteVampireBraceletObjectStmt.bindString(1, ownerID);
        deleteVampireBraceletObjectStmt.execute();
        PreparedStatement deleteVampireNecklaceObjectStmt(pConn, "DELETE FROM VampireNecklaceObject WHERE OwnerID = ?");
        deleteVampireNecklaceObjectStmt.bindString(1, ownerID);
        deleteVampireNecklaceObjectStmt.execute();
        PreparedStatement deleteVampireCoatObjectStmt(pConn, "DELETE FROM VampireCoatObject WHERE OwnerID = ?");
        deleteVampireCoatObjectStmt.bindString(1, ownerID);
        deleteVampireCoatObjectStmt.execute();
        PreparedStatement deleteSkullObjectStmt(pConn, "DELETE FROM SkullObject WHERE OwnerID = ?");
        deleteSkullObjectStmt.bindString(1, ownerID);
        deleteSkullObjectStmt.execute();
        // MaceObject is deleted twice in the original list. The repeat is a
        // no-op, but it is kept so that this change stays a pure
        // parameterisation and nothing about the executed sequence moves.
        PreparedStatement deleteMaceObjectStmt2(pConn, "DELETE FROM MaceObject WHERE OwnerID = ?");
        deleteMaceObjectStmt2.bindString(1, ownerID);
        deleteMaceObjectStmt2.execute();
        PreparedStatement deleteSerumObjectStmt(pConn, "DELETE FROM SerumObject WHERE OwnerID = ?");
        deleteSerumObjectStmt.bindString(1, ownerID);
        deleteSerumObjectStmt.execute();
        PreparedStatement deleteVampireETCObjectStmt(pConn, "DELETE FROM VampireETCObject WHERE OwnerID = ?");
        deleteVampireETCObjectStmt.bindString(1, ownerID);
        deleteVampireETCObjectStmt.execute();
        PreparedStatement deleteSlayerPortalItemObjectStmt(pConn,
                                                           "DELETE FROM SlayerPortalItemObject WHERE OwnerID = ?");
        deleteSlayerPortalItemObjectStmt.bindString(1, ownerID);
        deleteSlayerPortalItemObjectStmt.execute();
        PreparedStatement deleteVampirePortalItemObjectStmt(pConn,
                                                            "DELETE FROM VampirePortalItemObject WHERE OwnerID = ?");
        deleteVampirePortalItemObjectStmt.bindString(1, ownerID);
        deleteVampirePortalItemObjectStmt.execute();
        PreparedStatement deleteEventGiftBoxObjectStmt(pConn, "DELETE FROM EventGiftBoxObject WHERE OwnerID = ?");
        deleteEventGiftBoxObjectStmt.bindString(1, ownerID);
        deleteEventGiftBoxObjectStmt.execute();
        PreparedStatement deleteEventStarObjectStmt(pConn, "DELETE FROM EventStarObject WHERE OwnerID = ?");
        deleteEventStarObjectStmt.bindString(1, ownerID);
        deleteEventStarObjectStmt.execute();
    } catch (SQLQueryException& sqe) {
        throw Error(sqe.toString());
    }

    __END_CATCH
}

ItemDestroyer* g_pItemDestroyer = NULL;
