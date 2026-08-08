//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectKillAftermath.cpp
// Written by  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectKillAftermath.h"

#include "Creature.h"
#include "DB.h"
#include "GamePlayer.h"
#include "Ousters.h"
#include "PreparedStatement.h"
#include "Slayer.h"
#include "Vampire.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectKillAftermath::EffectKillAftermath(Creature* pCreature)

{
    __BEGIN_TRY


    m_bBroadcastingEffect = false;

    setTarget(pCreature);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectKillAftermath::~EffectKillAftermath()

{
    __BEGIN_TRY
    __END_CATCH_NO_RETHROW
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectKillAftermath::affect(Creature* pCreature)

{
    __BEGIN_TRY
    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectKillAftermath::affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject)

{
    __BEGIN_TRY
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectKillAftermath::unaffect(Creature* pFromCreature)

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    // cout << "EffectKillAftermath" << "unaffect BEGIN" << endl;

    Assert(pFromCreature != NULL);
    pFromCreature->removeFlag(Effect::EFFECT_CLASS_KILL_AFTERMATH);
    destroy(pFromCreature->getName());

    // cout << "EffectKillAftermath" << "unaffect END" << endl;

    __END_DEBUG
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectKillAftermath::unaffect()

{
    __BEGIN_TRY

    // cout << "EffectKillAftermath" << "unaffect BEGIN" << endl;

    Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
    unaffect(pCreature);

    // cout << "EffectKillAftermath" << "unaffect END" << endl;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectKillAftermath::unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject)

{
    __BEGIN_TRY
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectKillAftermath::create(const string& ownerID)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");


        Turn_t currentYearTime;

        getCurrentYearTime(currentYearTime);


        PreparedStatement insertStmt(pConn,
                                     "INSERT INTO EffectKillAftermath (OwnerID , YearTime, DayTime) VALUES(?, ?, ?)");
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
void EffectKillAftermath::destroy(const string& ownerID)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;
        sql << "DELETE FROM EffectKillAftermath WHERE OwnerID = '" << ownerID << "'";
        pStmt->executeQueryString(sql.toString());
        */


        PreparedStatement deleteStmt(pConn, "DELETE FROM EffectKillAftermath WHERE OwnerID = ?");
        deleteStmt.bindString(1, ownerID);
        deleteStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectKillAftermath::save(const string& ownerID)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        Turn_t currentYearTime;

        getCurrentYearTime(currentYearTime);

        /*
        StringStream sql;

        sql << "UPDATE EffectKillAftermath SET "
            << "YearTime = " << currentYearTime
            << ",DayTime = " << m_Deadline.tv_sec
            << " WHERE OwnerID = '" << ownerID << "'";
        */


        PreparedStatement updateStmt(pConn,
                                     "UPDATE EffectKillAftermath SET YearTime = ?, DayTime = ? WHERE OwnerID = ?");
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
string EffectKillAftermath::toString() const {
    __BEGIN_TRY

    StringStream msg;

    msg << "EffectKillAftermath("
        << "ObjectID:" << getObjectID() << ")";

    return msg.toString();

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectKillAftermathLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    if (pCreature == NULL) {
        return;
    }

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT DayTime FROM EffectKillAftermath"
            << " WHERE OwnerID = '" << pCreature->getName()
            << "'";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */


        PreparedStatement selectStmt(pConn, "SELECT DayTime FROM EffectKillAftermath WHERE OwnerID = ?");
        selectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectStmt.execute();

        while (pResult->next()) {
            if (pCreature->isSlayer()) {
                uint i = 0;

                int DayTime = pResult->getDWORD(++i);

                Timeval currentTime;
                getCurrentTime(currentTime);

                EffectKillAftermath* pEffectKillAftermath = new EffectKillAftermath(pCreature);

                if (currentTime.tv_sec < DayTime) {
                    pEffectKillAftermath->setDeadline((DayTime - currentTime.tv_sec) * 10);

                    Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
                    pSlayer->setFlag(Effect::EFFECT_CLASS_KILL_AFTERMATH);

                    EffectManager* pEffectManager = pSlayer->getEffectManager();
                    pEffectManager->addEffect(pEffectKillAftermath);
                } else {
                    pEffectKillAftermath->setDeadline(0);

                    Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
                    pSlayer->setFlag(Effect::EFFECT_CLASS_KILL_AFTERMATH);

                    EffectManager* pEffectManager = pSlayer->getEffectManager();
                    pEffectManager->addEffect(pEffectKillAftermath);
                }
            } else if (pCreature->isVampire()) {
                uint i = 0;

                int DayTime = pResult->getDWORD(++i);

                Timeval currentTime;
                getCurrentTime(currentTime);

                EffectKillAftermath* pEffectKillAftermath = new EffectKillAftermath(pCreature);

                if (currentTime.tv_sec < DayTime) {
                    pEffectKillAftermath->setDeadline((DayTime - currentTime.tv_sec) * 10);

                    Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
                    pVampire->setFlag(Effect::EFFECT_CLASS_KILL_AFTERMATH);

                    EffectManager* pEffectManager = pVampire->getEffectManager();
                    pEffectManager->addEffect(pEffectKillAftermath);
                } else {
                    pEffectKillAftermath->setDeadline(0);

                    Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
                    pVampire->setFlag(Effect::EFFECT_CLASS_KILL_AFTERMATH);

                    EffectManager* pEffectManager = pVampire->getEffectManager();
                    pEffectManager->addEffect(pEffectKillAftermath);
                }
            } else if (pCreature->isOusters()) {
                uint i = 0;

                int DayTime = pResult->getDWORD(++i);

                Timeval currentTime;
                getCurrentTime(currentTime);

                EffectKillAftermath* pEffectKillAftermath = new EffectKillAftermath(pCreature);

                if (currentTime.tv_sec < DayTime) {
                    pEffectKillAftermath->setDeadline((DayTime - currentTime.tv_sec) * 10);

                    Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
                    pOusters->setFlag(Effect::EFFECT_CLASS_KILL_AFTERMATH);

                    EffectManager* pEffectManager = pOusters->getEffectManager();
                    pEffectManager->addEffect(pEffectKillAftermath);
                } else {
                    pEffectKillAftermath->setDeadline(0);

                    Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
                    pOusters->setFlag(Effect::EFFECT_CLASS_KILL_AFTERMATH);

                    EffectManager* pEffectManager = pOusters->getEffectManager();
                    pEffectManager->addEffect(pEffectKillAftermath);
                }
            }
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

EffectKillAftermathLoader* g_pEffectKillAftermathLoader = NULL;
