//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectMute.cpp
// Written by  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectMute.h"

#include "Creature.h"
#include "DB.h"
#include "EventMorph.h"
#include "EventRegeneration.h"
#include "GCChangeDarkLight.h"
#include "GCModifyInformation.h"
#include "GCMorph1.h"
#include "GCMorphVampire2.h"
#include "GCRemoveEffect.h"
#include "GamePlayer.h"
#include "Monster.h"
#include "PCFinder.h"
#include "PCManager.h"
#include "PreparedStatement.h"
#include "Slayer.h"
#include "Vampire.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectMute::EffectMute(Creature* pCreature)

{
    __BEGIN_TRY

    setTarget(pCreature);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMute::affect(Creature* pCreature)

{
    __BEGIN_TRY
    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMute::affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject)

{
    __BEGIN_TRY
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMute::unaffect(Creature* pFromCreature)

{
    __BEGIN_TRY

    Assert(pFromCreature != NULL);

    pFromCreature->removeFlag(Effect::EFFECT_CLASS_MUTE);

    GCRemoveEffect gcRemoveEffect;
    gcRemoveEffect.setObjectID(pFromCreature->getObjectID());
    gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_MUTE);
    pFromCreature->getPlayer()->sendPacket(&gcRemoveEffect);

    destroy(pFromCreature->getName());

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMute::unaffect()

{
    __BEGIN_TRY

    // cout << "EffectMute" << "unaffect BEGIN" << endl;

    Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
    unaffect(pCreature);

    // cout << "EffectMute" << "unaffect END" << endl;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMute::unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject)

{
    __BEGIN_TRY
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMute::create(const string& ownerID)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");


        Turn_t currentYearTime;

        getCurrentYearTime(currentYearTime);

        PreparedStatement insertStmt(pConn, "INSERT INTO EffectMute (OwnerID , YearTime, DayTime) VALUES(?, ?, ?)");
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
void EffectMute::destroy(const string& ownerID)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;
        sql << "DELETE FROM EffectMute WHERE OwnerID = '" << ownerID << "'";
        pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement deleteStmt(pConn, "DELETE FROM EffectMute WHERE OwnerID = ?");
        deleteStmt.bindString(1, ownerID);
        deleteStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMute::save(const string& ownerID)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        Turn_t currentYearTime;

        getCurrentYearTime(currentYearTime);

        /*
        StringStream sql;

        sql << "UPDATE EffectMute SET "
            << "YearTime = " << currentYearTime
            << ",DayTime = " << m_Deadline.tv_sec
            << ", Level = " <<(int)m_Level
            << " WHERE OwnerID = '" << ownerID << "'";

        pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement updateStmt(pConn, "UPDATE EffectMute SET YearTime=?, DayTime=? WHERE OwnerID=?");
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
string EffectMute::toString() const {
    __BEGIN_TRY

    StringStream msg;

    msg << "EffectMute("
        << "ObjectID:" << getObjectID() << ")";

    return msg.toString();

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMuteLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectStmt(pConn, "SELECT DayTime FROM EffectMute WHERE OwnerID=?");
        selectStmt.bindString(1, pCreature->getName());
        Result* pResult = selectStmt.execute();

        while (pResult->next()) {
            uint i = 0;

            int DayTime = pResult->getDWORD(++i);

            Timeval currentTime;
            getCurrentTime(currentTime);

            EffectMute* pEffectMute = new EffectMute(pCreature);

            if (currentTime.tv_sec < DayTime) {
                pEffectMute->setDeadline((DayTime - currentTime.tv_sec) * 10);

                pCreature->addEffect(pEffectMute);
                pCreature->setFlag(Effect::EFFECT_CLASS_MUTE);
            } else {
                pEffectMute->destroy(pCreature->getName());
            }
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

EffectMuteLoader* g_pEffectMuteLoader = NULL;
