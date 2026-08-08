//////////////////////////////////////////////////////////////////////////////
// Filename    : VampireSkillSlot.cpp
// Written By  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "VampireSkillSlot.h"

#include "DB.h"
#include "PreparedStatement.h"

VampireSkillSlot::VampireSkillSlot() throw() {
    __BEGIN_TRY
    m_SkillType = 0;
    m_Interval = 0;
    m_CastingTime = 0;
    __END_CATCH
}

VampireSkillSlot::VampireSkillSlot(SkillType_t SkillType, ulong Interval, ulong CastingTime) throw() {
    __BEGIN_TRY

    m_SkillType = SkillType;
    m_Interval = Interval;
    m_CastingTime = CastingTime;

    __END_CATCH
}

VampireSkillSlot::~VampireSkillSlot() throw() {
    __BEGIN_TRY

    m_SkillType = 0;
    m_Interval = 0;
    m_CastingTime = 0;

    __END_CATCH
}

void VampireSkillSlot::create(const string& OwnerID)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;
        sql << "INSERT INTO VampireSkillSave "
            << "(OwnerID , SkillType , Delay , CastingTime , NextTime)"
            << " VALUES ('" << OwnerID
            <<"' , " << (int)m_SkillType
            << " , " << (int)m_Interval
            << " , " << (int)m_CastingTime
            << " , " << (int)m_runTime.tv_sec
            << ")";

        pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement insertVampireSkillSaveStmt(
            pConn, "INSERT INTO VampireSkillSave (OwnerID, SkillType, Delay, CastingTime, NextTime) VALUES ( "
                   "?, ?, ?, ?, ? )");
        insertVampireSkillSaveStmt.bindString(1, OwnerID);
        insertVampireSkillSaveStmt.bindInt(2, m_SkillType);
        insertVampireSkillSaveStmt.bindInt(3, m_Interval);
        insertVampireSkillSaveStmt.bindInt(4, m_CastingTime);
        insertVampireSkillSaveStmt.bindInt(5, m_runTime.tv_sec);
        insertVampireSkillSaveStmt.execute(); // by sigi
    }
    END_DB(pStmt)

    __END_CATCH
}

void VampireSkillSlot::save(const string& OwnerID)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE VampireSkillSave SET "
            << "Delay = " << (int)m_Interval
            << " WHERE OwnerID = '" << OwnerID
            << "' AND SkillType = " << (int)m_SkillType;

        pStmt->executeQueryString(sql.toString());
        */

        PreparedStatement updateVampireSkillSaveStmt(
            pConn, "UPDATE VampireSkillSave SET Delay=? WHERE OwnerID=? AND SkillType=?");
        updateVampireSkillSaveStmt.bindInt(1, m_Interval);
        updateVampireSkillSaveStmt.bindString(2, OwnerID);
        updateVampireSkillSaveStmt.bindInt(3, m_SkillType);
        updateVampireSkillSaveStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

void VampireSkillSlot::save()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        /*
        StringStream sql;

        sql << "UPDATE VampireSkillSave SET "
            << "Delay = " << (int)m_Interval
            << " WHERE OwnerID = '" << m_Name
            << "' AND SkillType = " << (int)m_SkillType;

        pStmt->executeQueryString(sql.toString());
        */


        PreparedStatement updateVampireSkillSaveStmt(
            pConn, "UPDATE VampireSkillSave SET Delay=? WHERE OwnerID=? AND SkillType=?");
        updateVampireSkillSaveStmt.bindInt(1, m_Interval);
        updateVampireSkillSaveStmt.bindString(2, m_Name);
        updateVampireSkillSaveStmt.bindInt(3, m_SkillType);
        updateVampireSkillSaveStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

Turn_t VampireSkillSlot::getRemainTurn(Timeval currentTime) const throw() {
    Turn_t remainTurn =
        (m_runTime.tv_sec - currentTime.tv_sec) * 10 + (m_runTime.tv_usec - currentTime.tv_usec) / 100000;

    return remainTurn;
}

void VampireSkillSlot::setRunTime() throw() {
    getCurrentTime(m_runTime);


    m_runTime.tv_sec += m_Interval / 10;
    m_runTime.tv_usec += (m_Interval % 10) * 100000;
}

void VampireSkillSlot::setRunTime(Turn_t delay) throw() {
    getCurrentTime(m_runTime);


    m_runTime.tv_sec += delay / 10;
    m_runTime.tv_usec += (delay % 10) * 100000;

    if (m_Interval != delay) {
        m_Interval = delay;

        switch (m_SkillType) {
        case SKILL_ATTACK_MELEE:
        case SKILL_ATTACK_ARMS:
        case SKILL_SELF:
        case SKILL_TILE:
        case SKILL_OBJECT:
        case SKILL_BLOOD_DRAIN:
        case SKILL_UN_BURROW:
        case SKILL_UN_TRANSFORM:
        case SKILL_UN_INVISIBILITY:
        case SKILL_THROW_HOLY_WATER:
        case SKILL_EAT_CORPSE:
        case SKILL_HOWL:
            break;
        default:
            save(m_Name);
            break;
        }
    }
}
