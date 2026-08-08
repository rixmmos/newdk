//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectBehemothForceScroll.cpp
// Written by  : bezz
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectBehemothForceScroll.h"

#include "DB.h"
#include "GCRemoveEffect.h"
#include "PlayerCreature.h"
#include "PreparedStatement.h"
#include "Timeval.h"
#include "Zone.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectBehemothForceScroll::EffectBehemothForceScroll(Creature* pCreature)

{
    __BEGIN_TRY

    setTarget(pCreature);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBehemothForceScroll::affect()

{
    __BEGIN_TRY

    Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
    affect(pCreature);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBehemothForceScroll::affect(Creature* pCreature)

{
    __BEGIN_TRY

    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
    Assert(pPC != NULL);

    pPC->initAllStatAndSend();

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBehemothForceScroll::unaffect()

{
    __BEGIN_TRY

    Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
    unaffect(pCreature);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBehemothForceScroll::unaffect(Creature* pCreature)

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
void EffectBehemothForceScroll::create(const string& ownerID)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        Timeval currentTime;
        getCurrentTime(currentTime);

        Timeval remainTime = timediff(m_Deadline, currentTime);
        Turn_t remainTurn = remainTime.tv_sec * 10 + remainTime.tv_usec / 100000;

        PreparedStatement insertStmt(pConn, "INSERT INTO EffectBehemothForceScroll (OwnerID, RemainTime ) VALUES(?,?)");
        insertStmt.bindString(1, ownerID);
        insertStmt.bindULong(2, remainTurn);
        insertStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBehemothForceScroll::destroy(const string& ownerID)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement deleteStmt(pConn, "DELETE FROM EffectBehemothForceScroll WHERE OwnerID = ?");
        deleteStmt.bindString(1, ownerID);
        deleteStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBehemothForceScroll::save(const string& ownerID)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        Timeval currentTime;
        getCurrentTime(currentTime);

        Timeval remainTime = timediff(m_Deadline, currentTime);
        Turn_t remainTurn = remainTime.tv_sec * 10 + remainTime.tv_usec / 100000;

        PreparedStatement updateStmt(pConn, "UPDATE EffectBehemothForceScroll SET RemainTime = ? WHERE OwnerID = ?");
        updateStmt.bindULong(1, remainTurn);
        updateStmt.bindString(2, ownerID);
        updateStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectBehemothForceScroll::toString() const {
    __BEGIN_TRY

    StringStream msg;
    msg << "EffectBehemothForceScroll("
        << "ObjectID:" << getObjectID() << ")";
    return msg.toString();

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBehemothForceScrollLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectStmt(pConn, "SELECt RemainTime FROM EffectBehemothForceScroll WHERE OwnerID = ?");
        selectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectStmt.execute();

        if (pResult->next()) {
            Turn_t remainTurn = pResult->getDWORD(1);

            Timeval currentTime;
            getCurrentTime(currentTime);

            EffectBehemothForceScroll* pEffect = new EffectBehemothForceScroll(pCreature);

            pEffect->setDeadline(remainTurn);
            pCreature->addEffect(pEffect);
            pCreature->setFlag(pEffect->getEffectClass());
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

EffectBehemothForceScrollLoader* g_pEffectBehemothForceScrollLoader = NULL;
