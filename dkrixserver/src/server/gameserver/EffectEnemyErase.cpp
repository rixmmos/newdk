//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectEnemyErase.cpp
// Written by  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectEnemyErase.h"

#include "Creature.h"
#include "DB.h"
#include "GCModifyInformation.h"
#include "GCRemoveInjuriousCreature.h"
#include "Monster.h"
#include "Ousters.h"
#include "Player.h"
#include "PreparedStatement.h"
#include "Slayer.h"
#include "Vampire.h"

EffectEnemyErase::EffectEnemyErase(Creature* pCreature)

{
    __BEGIN_TRY

    setTarget(pCreature);


    m_bBroadcastingEffect = false;

    __END_CATCH
}

EffectEnemyErase::~EffectEnemyErase()

{
    __BEGIN_TRY
    __END_CATCH_NO_RETHROW
}

void EffectEnemyErase::affect(Creature* pCreature)

{
    __BEGIN_TRY
    __END_CATCH
}

void EffectEnemyErase::unaffect()

{
    __BEGIN_TRY

    Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
    unaffect(pCreature);

    __END_CATCH
}

void EffectEnemyErase::unaffect(Creature* pCreature)

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    if (pCreature->isSlayer()) {
        Slayer* pTargetSlayer = dynamic_cast<Slayer*>(pCreature);
        pTargetSlayer->deleteEnemy(m_EnemyName);

        GCRemoveInjuriousCreature gcRemoveInjuriousCreature;
        gcRemoveInjuriousCreature.setName(m_EnemyName);

        pTargetSlayer->getPlayer()->sendPacket(&gcRemoveInjuriousCreature);

    } else if (pCreature->isVampire()) {
        Vampire* pTargetVampire = dynamic_cast<Vampire*>(pCreature);
        pTargetVampire->deleteEnemy(m_EnemyName);

        GCRemoveInjuriousCreature gcRemoveInjuriousCreature;
        gcRemoveInjuriousCreature.setName(m_EnemyName);

        pTargetVampire->getPlayer()->sendPacket(&gcRemoveInjuriousCreature);

    } else if (pCreature->isOusters()) {
        Ousters* pTargetOusters = dynamic_cast<Ousters*>(pCreature);
        pTargetOusters->deleteEnemy(m_EnemyName);

        GCRemoveInjuriousCreature gcRemoveInjuriousCreature;
        gcRemoveInjuriousCreature.setName(m_EnemyName);

        pTargetOusters->getPlayer()->sendPacket(&gcRemoveInjuriousCreature);

    } else {
    }
    destroy(pCreature->getName());

    __END_DEBUG
    __END_CATCH
}

void EffectEnemyErase::create(const string& ownerID)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");


        Turn_t currentYearTime;

        getCurrentYearTime(currentYearTime);

        /*
        StringStream sql;
        sql << "INSERT INTO EnemyErase "
            << "(OwnerID , YearTime, DayTime, EnemyName)"
            << " VALUES ('" << ownerID
            << "' , " << currentYearTime
            << " , " << m_Deadline.tv_sec
            << " , '" << m_EnemyName
            << "')";

        pStmt->executeQueryString(sql.toString());
        */


        PreparedStatement insertStmt(
            pConn, "INSERT INTO EnemyErase (OwnerID , YearTime, DayTime, EnemyName) VALUES (?, ?, ?, ?)");
        insertStmt.bindString(1, ownerID);
        insertStmt.bindLong(2, currentYearTime);
        insertStmt.bindLong(3, m_Deadline.tv_sec);
        insertStmt.bindString(4, m_EnemyName);
        insertStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

void EffectEnemyErase::destroy(const string& ownerID)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;
        sql << "DELETE FROM EnemyErase WHERE OwnerID = '" << ownerID << "' AND EnemyName = '" << m_EnemyName << "'";
        pStmt->executeQueryString(sql.toString());
        */


        PreparedStatement deleteStmt(pConn, "DELETE FROM EnemyErase WHERE OwnerID = ? AND EnemyName = ?");
        deleteStmt.bindString(1, ownerID);
        deleteStmt.bindString(2, m_EnemyName);
        deleteStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

void EffectEnemyErase::save(const string& ownerID)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");


        Turn_t currentYearTime;

        getCurrentYearTime(currentYearTime);

        /*
        StringStream sql;
        sql << "UPDATE EnemyErase SET "
            << "YearTime = " << currentYearTime
            << ",DayTime = " << m_Deadline.tv_sec
            << ", EnemyName = '" << m_EnemyName
            << "' WHERE OwnerID = '" << ownerID << "'";

        pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement updateStmt(
            pConn, "UPDATE EnemyErase SET YearTime = ?, DayTime = ?, EnemyName = ? WHERE OwnerID = ?");
        updateStmt.bindLong(1, currentYearTime);
        updateStmt.bindLong(2, m_Deadline.tv_sec);
        updateStmt.bindString(3, m_EnemyName);
        updateStmt.bindString(4, ownerID);
        updateStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

string EffectEnemyErase::toString() const {
    __BEGIN_TRY

    StringStream msg;
    msg << "EffectEnemyErase("
        << "EnemyName:" << getEnemyName() << ")";
    return msg.toString();

    __END_CATCH
}

void EffectEnemyEraseLoader::load(Creature* pCreature)

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

        sql << "SELECT DayTime, EnemyName FROM EnemyErase"
            << " WHERE OwnerID = '" << pCreature->getName()
            << "'";

        Result* pResult = pStmt->executeQueryString(sql.toString());
        */


        PreparedStatement selectStmt(pConn, "SELECT DayTime, EnemyName FROM EnemyErase WHERE OwnerID = ?");
        selectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            int DayTime = pResult->getDWORD(++i);

            Timeval currentTime;
            getCurrentTime(currentTime);

            EffectEnemyErase* pEffectEnemyErase = new EffectEnemyErase(pCreature);

            EffectManager* pEffectManager = pCreature->getEffectManager();

            if (currentTime.tv_sec < DayTime) {
                pEffectEnemyErase->setDeadline((DayTime - currentTime.tv_sec) * 10);
                pEffectEnemyErase->setEnemyName(pResult->getString(++i));
            } else {
                pEffectEnemyErase->setDeadline(100);
                pEffectEnemyErase->setEnemyName(pResult->getString(++i));
            }

            pEffectManager->addEffect(pEffectEnemyErase);

            if (pCreature->isSlayer()) {
                Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
                pSlayer->addEnemy(pEffectEnemyErase->getEnemyName());
            } else if (pCreature->isVampire()) {
                Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
                pVampire->addEnemy(pEffectEnemyErase->getEnemyName());
            } else if (pCreature->isOusters()) {
                Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
                pOusters->addEnemy(pEffectEnemyErase->getEnemyName());
            }
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

EffectEnemyEraseLoader* g_pEffectEnemyEraseLoader = NULL;
