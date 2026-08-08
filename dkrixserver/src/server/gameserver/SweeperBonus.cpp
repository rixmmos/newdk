//////////////////////////////////////////////////////////////////////////////
// Filename    : SweeperBonus.cpp
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "SweeperBonus.h"

#include "DB.h"
#include "PreparedStatement.h"

void SweeperBonus::setRace(Race_t race) {
    __BEGIN_TRY

    if (m_Race != race) {
        m_Race = race;
        Statement* pStmt = NULL;

        BEGIN_DB {
            Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
            PreparedStatement updateOwnerRaceStmt(pConn, "UPDATE SweeperBonusInfo SET OwnerRace = ? WHERE Type = ?");
            updateOwnerRaceStmt.bindInt(1, m_Race);
            updateOwnerRaceStmt.bindInt(2, m_Type);
            updateOwnerRaceStmt.execute();
        }
        END_DB(pStmt)
    }

    __END_CATCH
}

string SweeperBonus::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "SweeperBonus(\n";

    OptionTypeListConstItor itr = m_OptionTypeList.begin();
    for (; itr != m_OptionTypeList.end(); itr++) {
        msg << (int)(*itr) << ",";
    }

    msg << ")\n";

    return msg.toString();

    __END_CATCH
}
