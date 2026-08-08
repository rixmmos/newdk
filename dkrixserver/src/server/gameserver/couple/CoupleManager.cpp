#include "CoupleManager.h"

#include "DB.h"
#include "DatabaseManager.h"
#include "PlayerCreature.h"
#include "PreparedStatement.h"

bool CoupleManager::isCouple(PlayerCreature* pPC1, string name2) {
    __BEGIN_TRY

    Assert(pPC1 != NULL);

    Statement* pStmt = NULL;
    bool bRet = false;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // Field names come from a fixed 2-entry lookup table keyed by Sex, never
        // from packet/user input; PreparedStatement cannot bind identifiers.
        PreparedStatement selectCountStmt(pConn, "SELECT count(*) FROM CoupleInfo where " +
                                                     getFieldName(pPC1->getSex()) + "=? and " +
                                                     getCounterFieldName(pPC1->getSex()) + "=?");
        selectCountStmt.bindString(1, pPC1->getName());
        selectCountStmt.bindString(2, name2);
        Result* pResult = selectCountStmt.execute();

        if (pResult->next()) {
            int num = pResult->getInt(1);
            if (num >= 1)
                bRet = true;
        }
    }
    END_DB(pStmt)

    return bRet;

    __END_CATCH
}

bool CoupleManager::isCouple(PlayerCreature* pPC1, PlayerCreature* pPC2) {
    __BEGIN_TRY

    Assert(pPC1 != NULL);
    Assert(pPC2 != NULL);

    if (pPC1->getSex() == pPC2->getSex())
        return false;

    Statement* pStmt = NULL;
    bool bRet = false;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // Field names come from a fixed 2-entry lookup table keyed by Sex, never
        // from packet/user input; PreparedStatement cannot bind identifiers.
        PreparedStatement selectCountStmt(pConn, "SELECT count(*) FROM CoupleInfo where " +
                                                     getFieldName(pPC1->getSex()) + "=? and " +
                                                     getFieldName(pPC2->getSex()) + "=?");
        selectCountStmt.bindString(1, pPC1->getName());
        selectCountStmt.bindString(2, pPC2->getName());
        Result* pResult = selectCountStmt.execute();

        if (pResult->next()) {
            int num = pResult->getInt(1);
            if (num >= 1)
                bRet = true;
        }
    }
    END_DB(pStmt)

    return bRet;

    __END_CATCH
}

bool CoupleManager::hasCouple(PlayerCreature* pPC) {
    __BEGIN_TRY

    Assert(pPC != NULL);

    Statement* pStmt = NULL;
    bool bRet = false;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // Field name comes from a fixed 2-entry lookup table keyed by Sex, never
        // from packet/user input; PreparedStatement cannot bind identifiers.
        PreparedStatement selectCountStmt(pConn, "SELECT count(*) FROM CoupleInfo where " +
                                                     getFieldName(pPC->getSex()) + "=?");
        selectCountStmt.bindString(1, pPC->getName());
        Result* pResult = selectCountStmt.execute();

        if (pResult->next()) {
            int num = pResult->getInt(1);
            if (num >= 1)
                bRet = true;
        }
    }
    END_DB(pStmt)

    return bRet;

    __END_CATCH
}

bool CoupleManager::getPartnerName(PlayerCreature* pPC, string& partnerName) {
    __BEGIN_TRY

    Assert(pPC != NULL);

    Statement* pStmt = NULL;
    bool bRet = false;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // Field names come from a fixed 2-entry lookup table keyed by Sex, never
        // from packet/user input; PreparedStatement cannot bind identifiers.
        PreparedStatement selectPartnerStmt(pConn, "SELECT " + getCounterFieldName(pPC->getSex()) +
                                                       " FROM CoupleInfo where " + getFieldName(pPC->getSex()) + "=?");
        selectPartnerStmt.bindString(1, pPC->getName());
        Result* pResult = selectPartnerStmt.execute();

        if (pResult->next()) {
            partnerName = pResult->getString(1);
            bRet = true;
        }
    }
    END_DB(pStmt)

    return bRet;

    __END_CATCH
}

void CoupleManager::makeCouple(PlayerCreature* pPC1, PlayerCreature* pPC2) {
    __BEGIN_TRY

    Assert(pPC1 != NULL);
    Assert(pPC2 != NULL);

    Assert(pPC1->getRace() == pPC2->getRace());
    Assert(pPC1->getSex() != pPC2->getSex());

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // Field names come from a fixed 2-entry lookup table keyed by Sex, never
        // from packet/user input; PreparedStatement cannot bind identifiers.
        PreparedStatement insertCoupleStmt(pConn, "INSERT INTO CoupleInfo (" + getFieldName(pPC1->getSex()) + ", " +
                                                      getFieldName(pPC2->getSex()) +
                                                      ", Race, CoupleDate ) VALUES (?,?,?, now())");
        insertCoupleStmt.bindString(1, pPC1->getName());
        insertCoupleStmt.bindString(2, pPC2->getName());
        insertCoupleStmt.bindUInt(3, (uint)pPC1->getRace());
        insertCoupleStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

void CoupleManager::removeCouple(PlayerCreature* pPC1, PlayerCreature* pPC2) {
    __BEGIN_TRY

    Assert(pPC1 != NULL);
    Assert(pPC2 != NULL);

    Assert(pPC1->getRace() == pPC2->getRace());
    Assert(pPC1->getSex() != pPC2->getSex());

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // Field names come from a fixed 2-entry lookup table keyed by Sex, never
        // from packet/user input; PreparedStatement cannot bind identifiers.
        PreparedStatement deleteCoupleStmt(pConn, "DELETE FROM CoupleInfo WHERE " + getFieldName(pPC1->getSex()) +
                                                      "=? AND " + getFieldName(pPC2->getSex()) + "=? AND Race=?");
        deleteCoupleStmt.bindString(1, pPC1->getName());
        deleteCoupleStmt.bindString(2, pPC2->getName());
        deleteCoupleStmt.bindUInt(3, (uint)pPC1->getRace());
        deleteCoupleStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

void CoupleManager::removeCoupleForce(PlayerCreature* pPC1, string strPC2) {
    __BEGIN_TRY

    Assert(pPC1 != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // Field names come from a fixed 2-entry lookup table keyed by Sex, never
        // from packet/user input; PreparedStatement cannot bind identifiers.
        PreparedStatement deleteCoupleStmt(pConn, "DELETE FROM CoupleInfo where " + getFieldName(pPC1->getSex()) +
                                                      "=? AND " + getCounterFieldName(pPC1->getSex()) +
                                                      "=? AND Race=?");
        deleteCoupleStmt.bindString(1, pPC1->getName());
        deleteCoupleStmt.bindString(2, strPC2);
        deleteCoupleStmt.bindUInt(3, (uint)(pPC1->getRace()));
        deleteCoupleStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

void CoupleManager::removeCoupleForce(PlayerCreature* pPC1) {
    __BEGIN_TRY

    Assert(pPC1 != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // Field name comes from a fixed 2-entry lookup table keyed by Sex, never
        // from packet/user input; PreparedStatement cannot bind identifiers.
        PreparedStatement deleteCoupleStmt(pConn, "DELETE FROM CoupleInfo where " + getFieldName(pPC1->getSex()) +
                                                      "=? AND Race=?");
        deleteCoupleStmt.bindString(1, pPC1->getName());
        deleteCoupleStmt.bindUInt(2, (uint)(pPC1->getRace()));
        deleteCoupleStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

CoupleManager* g_pCoupleManager = NULL;
