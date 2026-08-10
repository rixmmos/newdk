////////////////////////////////////////////////////////////////////////////////
// Filename    : VampEXPInfo.cpp
// Written By  : beowulf
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "VampEXPInfo.h"

#include "Assert.h"
#include "DB.h"
// #include <algo.h>

////////////////////////////////////////////////////////////////////////////////
// Global Variable definition
////////////////////////////////////////////////////////////////////////////////
VampEXPInfoManager* g_pVampEXPInfoManager = NULL;


////////////////////////////////////////////////////////////////////////////////
// class VampEXPInfo member methods
////////////////////////////////////////////////////////////////////////////////

VampEXPInfo::VampEXPInfo()

    {__BEGIN_TRY __END_CATCH}

VampEXPInfo::~VampEXPInfo()

    {__BEGIN_TRY __END_CATCH_NO_RETHROW}

string VampEXPInfo::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "VampEXPInfo ("
        << " Level : " << (int)m_Level << " GoalExp : " << (int)m_GoalExp << " AccumExp : " << (int)m_AccumExp << ")";
    return msg.toString();

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// class VampEXPInfoManager member methods
////////////////////////////////////////////////////////////////////////////////

VampEXPInfoManager::VampEXPInfoManager()

{
    __BEGIN_TRY

    m_VampEXPCount = 0;
    m_VampEXPInfoList = NULL;

    __END_CATCH
}

VampEXPInfoManager::~VampEXPInfoManager()

{
    __BEGIN_TRY

    if (m_VampEXPInfoList != NULL) {
        for (uint i = 0; i < m_VampEXPCount; i++)
            SAFE_DELETE(m_VampEXPInfoList[i]);

        SAFE_DELETE_ARRAY(m_VampEXPInfoList);
    }

    __END_CATCH_NO_RETHROW
}

void VampEXPInfoManager::init()

{
    __BEGIN_TRY

    load();

    __END_CATCH
}

void VampEXPInfoManager::load()

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    Statement* pStmt = NULL; // by sigi
    Result* pResult = NULL;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        pResult = pStmt->executeQuery("SELECT MAX(Level) FROM VampEXPBalanceInfo");

        if (pResult->getRowCount() == 0) {
            SAFE_DELETE(pStmt);
            throw Error("There is no data in VampEXPInfo Table");
        }

        
        pResult->next();
        m_VampEXPCount = pResult->getInt(1) + 1;

        Assert(m_VampEXPCount > 0);

        m_VampEXPInfoList = new VampEXPInfo*[m_VampEXPCount];

        Assert(m_VampEXPInfoList != NULL);

        
        for (uint i = 0; i < m_VampEXPCount; i++)
            m_VampEXPInfoList[i] = NULL;

        pResult = pStmt->executeQuery("Select Level, GoalExp, AccumExp from VampEXPBalanceInfo");

        while (pResult->next()) {
            VampEXPInfo* pVampEXPInfo = new VampEXPInfo();
            int i = 0;

            pVampEXPInfo->setLevel(pResult->getInt(++i));
            pVampEXPInfo->setGoalExp(pResult->getInt(++i));
            pVampEXPInfo->setAccumExp(pResult->getInt(++i));

            addVampEXPInfo(pVampEXPInfo);
        }

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)

    __END_DEBUG
    __END_CATCH
}

void VampEXPInfoManager::addVampEXPInfo(VampEXPInfo* pVampEXPInfo)

{
    __BEGIN_TRY

    Assert(pVampEXPInfo != NULL);

    // getLevel() is the VampEXPBalanceInfo row's Level and there was no bound
    // on it at all -- the Assert below dereferences the slot it is meant to be
    // checking. Safe by construction today (m_VampEXPCount is MAX(Level) + 1
    // from the same table) but this is a heap pointer write, so make it real.
    if (pVampEXPInfo->getLevel() >= m_VampEXPCount)
        throw OutOfBoundException("VampEXPInfoManager::addVampEXPInfo: level out of range");

    Assert(m_VampEXPInfoList[pVampEXPInfo->getLevel()] == NULL);

    m_VampEXPInfoList[pVampEXPInfo->getLevel()] = pVampEXPInfo;

    __END_CATCH
}

// VampEXPType is the Vampire's level, and m_VampEXPInfoList holds only
// m_VampEXPCount == MAX(Level) + 1 entries, so a character above the highest
// row in VampEXPBalanceInfo indexes past the end -- an out-of-bounds pointer
// read that the Assert on the next line then dereferences, live in Debug as
// well as Release. Throws rather than returning NULL: SkillUtil.cpp:4697 and
// GQuestAdvanceClassElement.cpp:29 both dereference the result immediately.
VampEXPInfo* VampEXPInfoManager::getVampEXPInfo(uint VampEXPType) const {
    __BEGIN_TRY

    if (VampEXPType >= m_VampEXPCount)
        throw OutOfBoundException("VampEXPInfoManager::getVampEXPInfo: level out of range");

    Assert(VampEXPType < m_VampEXPCount);
    Assert(m_VampEXPInfoList[VampEXPType] != NULL);

    return m_VampEXPInfoList[VampEXPType];

    __END_CATCH
}

string VampEXPInfoManager::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "VampEXPInfoManager(";

    for (uint i = 0; i < m_VampEXPCount; i++) {
        if (m_VampEXPInfoList[i] != NULL) {
            msg << m_VampEXPInfoList[i]->toString();
        } else {
            msg << "NULL";
        }
    }

    msg << ")";

    return msg.toString();

    __END_CATCH
}
