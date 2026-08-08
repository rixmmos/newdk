//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectSafeForceScroll.cpp
// Written by  : bezz
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectSafeForceScroll.h"

#include "DB.h"
#include "GCRemoveEffect.h"
#include "PlayerCreature.h"
#include "PreparedStatement.h"
#include "Timeval.h"
#include "Zone.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectSafeForceScroll::EffectSafeForceScroll(Creature* pCreature)

{
    __BEGIN_TRY

    setTarget(pCreature);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSafeForceScroll::affect()

{
    __BEGIN_TRY

    Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
    affect(pCreature);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSafeForceScroll::affect(Creature* pCreature)

{
    __BEGIN_TRY

    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
    Assert(pPC != NULL);

    pPC->initAllStatAndSend();

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSafeForceScroll::unaffect()

{
    __BEGIN_TRY

    Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
    unaffect(pCreature);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSafeForceScroll::unaffect(Creature* pCreature)

{
    __BEGIN_TRY

    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
    Assert(pPC != NULL);

    Zone* pZone = pPC->getZone();
    Assert(pZone != NULL);

    pPC->removeFlag(getEffectClass());
    pPC->initAllStatAndSend();

    GCRemoveEffect gcRemoveEffect;
    gcRemoveEffect.setObjectID(pCreature->getObjectID());
    gcRemoveEffect.addEffectList(getEffectClass());
    pZone->broadcastPacket(pPC->getX(), pPC->getY(), &gcRemoveEffect);

    destroy(pPC->getName());

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSafeForceScroll::create(const string& ownerID)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        Timeval currentTime;
        getCurrentTime(currentTime);

        Timeval remainTime = timediff(m_Deadline, currentTime);
        Turn_t remainTurn = remainTime.tv_sec * 10 + remainTime.tv_usec / 100000;

        PreparedStatement insertStmt(pConn, "INSERT INTO EffectSafeForceScroll (OwnerID, RemainTime ) VALUES(?,?)");
        insertStmt.bindString(1, ownerID);
        insertStmt.bindULong(2, remainTurn);
        insertStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSafeForceScroll::destroy(const string& ownerID)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement deleteStmt(pConn, "DELETE FROM EffectSafeForceScroll WHERE OwnerID = ?");
        deleteStmt.bindString(1, ownerID);
        deleteStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSafeForceScroll::save(const string& ownerID)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        Timeval currentTime;
        getCurrentTime(currentTime);

        Timeval remainTime = timediff(m_Deadline, currentTime);
        Turn_t remainTurn = remainTime.tv_sec * 10 + remainTime.tv_usec / 100000;

        PreparedStatement updateStmt(pConn, "UPDATE EffectSafeForceScroll SET RemainTime = ? WHERE OwnerID = ?");
        updateStmt.bindULong(1, remainTurn);
        updateStmt.bindString(2, ownerID);
        updateStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectSafeForceScroll::toString() const {
    __BEGIN_TRY

    StringStream msg;
    msg << "EffectSafeForceScroll("
        << "ObjectID:" << getObjectID() << ")";
    return msg.toString();

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectSafeForceScrollLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectStmt(pConn, "SELECt RemainTime FROM EffectSafeForceScroll WHERE OwnerID = ?");
        selectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectStmt.execute();

        if (pResult->next()) {
            Turn_t remainTurn = pResult->getDWORD(1);

            Timeval currentTime;
            getCurrentTime(currentTime);

            EffectSafeForceScroll* pEffect = new EffectSafeForceScroll(pCreature);

            pEffect->setDeadline(remainTurn);
            pCreature->addEffect(pEffect);
            pCreature->setFlag(pEffect->getEffectClass());
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

EffectSafeForceScrollLoader* g_pEffectSafeForceScrollLoader = NULL;
