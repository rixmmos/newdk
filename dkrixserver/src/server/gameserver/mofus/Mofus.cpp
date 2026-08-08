/////////////////////////////////////////////////////////////////////////////
// Filename : Mofus.cpp
// Desc		:
/////////////////////////////////////////////////////////////////////////////

#include "Mofus.h"

#include "DB.h"
#include "PreparedStatement.h"

int loadPowerPoint(const string& name) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    int powerpoint = 0;
    try {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement selectPowerPointStmt(pConn, "SELECT Point FROM MofusPowerPoint WHERE OwnerID=?");
        selectPowerPointStmt.bindString(1, name);
        Result* pResult = selectPowerPointStmt.execute();

        if (pResult->next()) {
            powerpoint = pResult->getInt(1);
        }
    } catch (SQLQueryException& sql) {
    } catch (...) {
        SAFE_DELETE(pStmt);

        throw;
    }

    SAFE_DELETE(pStmt);

    return powerpoint;

    __END_CATCH
}

int savePowerPoint(const string& name, int amount) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    int powerpoint = 0;

    try {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement updatePowerPointStmt(pConn, "Update MofusPowerPoint SET Point = Point + ? WHERE OwnerID=?");
        updatePowerPointStmt.bindInt(1, amount);
        updatePowerPointStmt.bindString(2, name);
        updatePowerPointStmt.execute();

        if (updatePowerPointStmt.getAffectedRowCount() == 0) {
            PreparedStatement insertPowerPointStmt(pConn, "Insert Into MofusPowerPoint Values (?,?)");
            insertPowerPointStmt.bindString(1, name);
            insertPowerPointStmt.bindInt(2, amount);
            insertPowerPointStmt.execute();
        }

        PreparedStatement selectPowerPointStmt(pConn, "SELECT Point FROM MofusPowerPoint WHERE OwnerID=?");
        selectPowerPointStmt.bindString(1, name);
        Result* pResult = selectPowerPointStmt.execute();

        if (pResult->next()) {
            powerpoint = pResult->getInt(1);
        }
    } catch (SQLQueryException& sql) {
    } catch (...) {
        SAFE_DELETE(pStmt);

        throw;
    }

    SAFE_DELETE(pStmt);

    return powerpoint;

    __END_CATCH
}

void logPowerPoint(const string& name, int recvPoint, int savePoint) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    try {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement insertMofusLogStmt(
            pConn, "INSERT INTO MofusLog (OwnerID, SaveTime, RecvPoint, SavePoint) VALUES (?, now(), ?, ?)");
        insertMofusLogStmt.bindString(1, name);
        insertMofusLogStmt.bindUInt(2, recvPoint);
        insertMofusLogStmt.bindUInt(3, savePoint);
        insertMofusLogStmt.execute();
    } catch (SQLQueryException& sql) {
    } catch (...) {
        SAFE_DELETE(pStmt);

        throw;
    }

    SAFE_DELETE(pStmt);

    __END_CATCH
}
