//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectCanEnterGDRLair.cpp
// Written by  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectCanEnterGDRLair.h"

#include "Creature.h"
#include "DB.h"
#include "GCModifyInformation.h"
#include "GCRemoveEffect.h"
#include "Monster.h"
#include "Ousters.h"
#include "Player.h"
#include "PreparedStatement.h"
#include "Slayer.h"
#include "Vampire.h"

EffectCanEnterGDRLair::EffectCanEnterGDRLair(Creature* pCreature)

{
    __BEGIN_TRY

    setTarget(pCreature);

    __END_CATCH
}

EffectCanEnterGDRLair::~EffectCanEnterGDRLair()

{
    __BEGIN_TRY
    __END_CATCH_NO_RETHROW
}

void EffectCanEnterGDRLair::affect(Creature* pCreature)

{
    __BEGIN_TRY
    __END_CATCH
}

void EffectCanEnterGDRLair::unaffect()

{
    __BEGIN_TRY

    Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
    unaffect(pCreature);

    __END_CATCH
}

void EffectCanEnterGDRLair::unaffect(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    pCreature->removeFlag(getEffectClass());

    Zone* pZone = pCreature->getZone();
    Assert(pZone != NULL);


    GCRemoveEffect gcRemoveEffect;
    gcRemoveEffect.setObjectID(pCreature->getObjectID());
    gcRemoveEffect.addEffectList(getSendEffectClass());

    pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

    destroy(pCreature->getName());

    __END_CATCH
}

void EffectCanEnterGDRLair::create(const string& ownerID)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        Turn_t currentYearTime;

        getCurrentYearTime(currentYearTime);

        /*
        StringStream sql;
        sql << "INSERT INTO CanEnterGDRLair "
            << "(OwnerID , YearTime, DayTime, EnemyName)"
            << " VALUES ('" << ownerID
            << "' , " << currentYearTime
            << " , " << m_Deadline.tv_sec
            << " , '" << m_EnemyName
            << "')";

        pStmt->executeQueryString(sql.toString());
        */


        PreparedStatement insertStmt(pConn,
                                     "INSERT INTO CanEnterGDRLair (OwnerID , YearTime, DayTime) VALUES (?, ?, ?)");
        insertStmt.bindString(1, ownerID);
        insertStmt.bindLong(2, currentYearTime);
        insertStmt.bindLong(3, m_Deadline.tv_sec);
        insertStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

void EffectCanEnterGDRLair::destroy(const string& ownerID)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");


        PreparedStatement deleteStmt(pConn, "DELETE FROM CanEnterGDRLair WHERE OwnerID = ?");
        deleteStmt.bindString(1, ownerID);
        deleteStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

void EffectCanEnterGDRLair::save(const string& ownerID)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        Turn_t currentYearTime;

        getCurrentYearTime(currentYearTime);

        PreparedStatement updateStmt(pConn, "UPDATE CanEnterGDRLair SET YearTime = ?, DayTime = ? WHERE OwnerID = ?");
        updateStmt.bindLong(1, currentYearTime);
        updateStmt.bindLong(2, m_Deadline.tv_sec);
        updateStmt.bindString(3, ownerID);
        updateStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

string EffectCanEnterGDRLair::toString() const {
    __BEGIN_TRY

    StringStream msg;
    msg << "EffectCanEnterGDRLair("
        << ")";
    return msg.toString();

    __END_CATCH
}

void EffectCanEnterGDRLairLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    // Assert(pCreature != NULL);
    if (pCreature == NULL) {
        return;
    }

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "SELECT DayTime, EnemyName FROM CanEnterGDRLair"
            << " WHERE OwnerID = '" << pCreature->getName()
            << "'";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */


        PreparedStatement selectStmt(pConn, "SELECT DayTime FROM CanEnterGDRLair WHERE OwnerID = ?");
        selectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            int DayTime = pResult->getDWORD(++i);

            Timeval currentTime;
            getCurrentTime(currentTime);

            EffectCanEnterGDRLair* pEffectCanEnterGDRLair = new EffectCanEnterGDRLair(pCreature);

            EffectManager* pEffectManager = pCreature->getEffectManager();

            if (currentTime.tv_sec < DayTime) {
                pEffectCanEnterGDRLair->setDeadline((DayTime - currentTime.tv_sec) * 10);
            } else {
                pEffectCanEnterGDRLair->setDeadline(100);
            }

            pEffectManager->addEffect(pEffectCanEnterGDRLair);
            pCreature->setFlag(pEffectCanEnterGDRLair->getEffectClass());
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

EffectCanEnterGDRLairLoader* g_pEffectCanEnterGDRLairLoader = NULL;
