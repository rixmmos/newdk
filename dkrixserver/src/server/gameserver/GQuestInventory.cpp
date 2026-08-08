#include "GQuestInventory.h"

#include "DB.h"
#include "PreparedStatement.h"

void GQuestInventory::load(const string& ownerName) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement selectStmt(pConn, "SELECT ItemType FROM GQuestItemObject WHERE OwnerID=?");
        selectStmt.bindString(1, ownerName);
        Result* pResult = selectStmt.execute();

        while (pResult->next()) {
            getItems().push_back(pResult->getInt(1));
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

void GQuestInventory::removeOne(const string& ownerName, ItemType_t item) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement deleteStmt(pConn, "DELETE FROM GQuestItemObject WHERE OwnerID=? AND ItemType=? LIMIT 1");
        deleteStmt.bindString(1, ownerName);
        deleteStmt.bindUInt(2, item);
        deleteStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

void GQuestInventory::addOne(ItemType_t item) {
    getItems().push_back(item);
}

void GQuestInventory::saveOne(const string& ownerName, ItemType_t item) {
    addOne(item);

    Statement* pStmt = NULL;
    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement insertStmt(pConn, "INSERT INTO GQuestItemObject(ItemType, OwnerID) VALUES (?, ?)");
        insertStmt.bindUInt(1, item);
        insertStmt.bindString(2, ownerName);
        insertStmt.execute();
    }
    END_DB(pStmt)
}
