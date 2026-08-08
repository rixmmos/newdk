//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectAftermath.cpp
// Written by  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectAftermath.h"

#include "Creature.h"
#include "DB.h"
#include "GamePlayer.h"
#include "PreparedStatement.h"
#include "Slayer.h"
#include "Vampire.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectAftermath::EffectAftermath(Creature* pCreature)

{
    __BEGIN_TRY


    m_bBroadcastingEffect = false;

    setTarget(pCreature);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectAftermath::~EffectAftermath()

{
    __BEGIN_TRY
    __END_CATCH_NO_RETHROW
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectAftermath::affect(Creature* pCreature)

{
    __BEGIN_TRY
    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectAftermath::affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject)

{
    __BEGIN_TRY
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectAftermath::unaffect(Creature* pFromCreature)

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    // cout << "EffectAftermath" << "unaffect BEGIN" << endl;

    Assert(pFromCreature != NULL);
    pFromCreature->removeFlag(Effect::EFFECT_CLASS_AFTERMATH);
    destroy(pFromCreature->getName());

    // cout << "EffectAftermath" << "unaffect END" << endl;

    __END_DEBUG
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectAftermath::unaffect()

{
    __BEGIN_TRY

    // cout << "EffectAftermath" << "unaffect BEGIN" << endl;

    Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
    unaffect(pCreature);

    // cout << "EffectAftermath" << "unaffect END" << endl;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectAftermath::unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject)

{
    __BEGIN_TRY
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectAftermath::create(const string& ownerID)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");


        Turn_t currentYearTime;

        getCurrentYearTime(currentYearTime);

        /*
        StringStream sql;
        sql << "INSERT INTO EffectAftermath "
            << "(OwnerID , YearTime, DayTime) VALUES("
            << "'" << ownerID           << "', "
            << " " << currentYearTime   << " , "
            << " " << m_Deadline.tv_sec << "   "
            << ")";

        pStmt->executeQueryString(sql.toString());
        */


        PreparedStatement insertStmt(pConn,
                                     "INSERT INTO EffectAftermath (OwnerID , YearTime, DayTime) VALUES(?, ?, ?)");
        insertStmt.bindString(1, ownerID);
        insertStmt.bindLong(2, currentYearTime);
        insertStmt.bindLong(3, m_Deadline.tv_sec);
        insertStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectAftermath::destroy(const string& ownerID)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;
        sql << "DELETE FROM EffectAftermath WHERE OwnerID = '" << ownerID << "'";
        pStmt->executeQueryString(sql.toString());
        */


        PreparedStatement deleteStmt(pConn, "DELETE FROM EffectAftermath WHERE OwnerID = ?");
        deleteStmt.bindString(1, ownerID);
        deleteStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectAftermath::save(const string& ownerID)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        Turn_t currentYearTime;

        getCurrentYearTime(currentYearTime);

        /*
        StringStream sql;

        sql << "UPDATE EffectAftermath SET "
            << "YearTime = " << currentYearTime
            << ",DayTime = " << m_Deadline.tv_sec
            << " WHERE OwnerID = '" << ownerID << "'";
        */


        PreparedStatement updateStmt(pConn, "UPDATE EffectAftermath SET YearTime = ?, DayTime = ? WHERE OwnerID = ?");
        updateStmt.bindLong(1, currentYearTime);
        updateStmt.bindLong(2, m_Deadline.tv_sec);
        updateStmt.bindString(3, ownerID);
        updateStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectAftermath::toString() const {
    __BEGIN_TRY

    StringStream msg;

    msg << "EffectAftermath("
        << "ObjectID:" << getObjectID() << ")";

    return msg.toString();

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectAftermathLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    if (pCreature == NULL || (!pCreature->isSlayer() && !pCreature->isOusters())) {
        return;
    }

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT DayTime FROM EffectAftermath"
            << " WHERE OwnerID = '" << pCreature->getName()
            << "'";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */


        PreparedStatement selectStmt(pConn, "SELECT DayTime FROM EffectAftermath WHERE OwnerID = ?");
        selectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            int DayTime = pResult->getDWORD(++i);

            Timeval currentTime;
            getCurrentTime(currentTime);

            EffectAftermath* pEffectAftermath = new EffectAftermath(pCreature);

            if (currentTime.tv_sec < DayTime) {
                pEffectAftermath->setDeadline((DayTime - currentTime.tv_sec) * 10);

                pCreature->setFlag(Effect::EFFECT_CLASS_AFTERMATH);

                EffectManager* pEffectManager = pCreature->getEffectManager();
                pEffectManager->addEffect(pEffectAftermath);
            } else {
                pEffectAftermath->setDeadline(0);

                pCreature->setFlag(Effect::EFFECT_CLASS_AFTERMATH);

                EffectManager* pEffectManager = pCreature->getEffectManager();
                pEffectManager->addEffect(pEffectAftermath);
            }
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

EffectAftermathLoader* g_pEffectAftermathLoader = NULL;
