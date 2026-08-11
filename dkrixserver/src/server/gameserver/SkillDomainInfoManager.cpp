//--------------------------------------------------------------------
//
// Filename    : SkillDomainInfoManager.cpp
// Written By  : Elca
//
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// include files
//--------------------------------------------------------------------
#include "SkillDomainInfoManager.h"

#include "Assert.h"
#include "DB.h"
#include "Exception.h"
#include "StringStream.h"

DomainInfo::DomainInfo()

    {__BEGIN_TRY __END_CATCH}

DomainInfo::~DomainInfo()

    {__BEGIN_TRY __END_CATCH_NO_RETHROW}

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string DomainInfo::toString() const

{
    __BEGIN_TRY
    StringStream msg;

    msg << "DomainInfo("
        << "DomainType : " << (int)m_Type << "Level : " << (int)m_Level << "GoalExp: " << (int)m_GoalExp
        << "AccumExp: " << (int)m_AccumExp << "BestItemType: " << (int)m_BestItemType << ")";

    return msg.toString();

    __END_CATCH
}

//--------------------------------------------------------------------
//
// Constructor
//
//--------------------------------------------------------------------
SkillDomainInfoManager::SkillDomainInfoManager()

{
    __BEGIN_TRY

    // The original loop here was commented out (and had `i++` where it meant
    // `j++`), so every member was left indeterminate until init() succeeded --
    // and the destructor walks these unconditionally. Zero them.
    for (int i = 0; i < SKILL_DOMAIN_MAX; i++) {
        m_DomainInfoLists[i] = NULL;
        m_DomainInfoCounts[i] = 0;
    }

    __END_CATCH
}

//--------------------------------------------------------------------
//
// Destructor
//
//--------------------------------------------------------------------
SkillDomainInfoManager::~SkillDomainInfoManager()

{
    __BEGIN_TRY

    // Was `j <= 150` against arrays that init() sized to MAX(Level) + 1 per
    // domain, so this read past the end and called delete on whatever it found
    // -- 50 slots past the end for SKILL_DOMAIN_ETC in the live data.
    for (int i = 0; i < SKILL_DOMAIN_MAX; i++) {
        if (m_DomainInfoLists[i] == NULL)
            continue;

        for (int j = 0; j < m_DomainInfoCounts[i]; j++)
            SAFE_DELETE(m_DomainInfoLists[i][j]);
    }

    __END_CATCH_NO_RETHROW
}

//--------------------------------------------------------------------
//
// SkillDomainInfoManager::init()
//
//--------------------------------------------------------------------
void SkillDomainInfoManager::init()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;
    Result* pResult = NULL;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        for (int i = 0; i < SKILL_DOMAIN_MAX; i++) {
            pResult = pStmt->executeQuery("SELECT MAX(Level) FROM SkillDomainInfo WHERE DomainType = %d", i);

            if (!pResult->next()) {
                SAFE_DELETE(pStmt);
                cerr << "There is no data in DomainInfo Table" << endl;
                throw Error("There is no data in DomainInfo Table");
            }

            //			pResult->next();

            int Count = pResult->getInt(1) + 1;

            Assert(Count > 0);
            Assert(Count <= 151);

            m_DomainInfoLists[i] = new DomainInfo*[Count];
            m_DomainInfoCounts[i] = Count;

            for (int j = 0; j < Count; j++)
                m_DomainInfoLists[i][j] = NULL;

            pResult = pStmt->executeQuery(
                "Select DomainType, Level, GoalExp, AccumExp, BestItemType from SkillDomainInfo WHERE DomainType = %d",
                i);

            while (pResult->next()) {
                DomainInfo* pDomainInfo = new DomainInfo();
                int i = 0;

                pDomainInfo->setType(pResult->getInt(++i));
                pDomainInfo->setLevel(pResult->getInt(++i));
                pDomainInfo->setGoalExp(pResult->getInt(++i));
                pDomainInfo->setAccumExp(pResult->getInt(++i));
                pDomainInfo->setBestItemType(pResult->getInt(++i));
                addDomainInfo(pDomainInfo);
            }
        }

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)

    __END_CATCH
}

//--------------------------------------------------------------------------------
// get item info
//--------------------------------------------------------------------------------
// `Level < 151` was never the right bound: init() allocates MAX(Level) + 1
// entries for each domain independently, and in the live data
// SKILL_DOMAIN_ETC has rows only up to level 100 (101 entries) while the
// Vampire and Ousters domains stop at 149. Reading m_DomainInfoLists[5][101]
// is an out-of-bounds *pointer* read, and the Assert on the next line then
// dereferences whatever it found -- so this is already live in the Debug
// build, not only under NDEBUG. increaseDomainExp reaches it directly:
// levelling SKILL_DOMAIN_ETC from 100 asks for the level-101 row.
//
// Bounded against the count init() actually allocated, and it throws rather
// than returning NULL because all four callers dereference the result on the
// spot (SkillUtil.cpp:4308, :4409, :5766 and GQuestAdvanceClassElement.cpp:20),
// so a sentinel would trade an out-of-bounds read for a NULL dereference --
// the same reasoning as the NPC shop accessors in 326c298.
DomainInfo* SkillDomainInfoManager::getDomainInfo(SkillDomain DomainType, Level_t Level) const {
    __BEGIN_TRY

    if (DomainType >= SKILL_DOMAIN_MAX)
        throw OutOfBoundException("SkillDomainInfoManager::getDomainInfo: domain out of range");

    if (m_DomainInfoLists[DomainType] == NULL || Level >= m_DomainInfoCounts[DomainType])
        throw OutOfBoundException("SkillDomainInfoManager::getDomainInfo: no such domain level");

    Assert(DomainType < SKILL_DOMAIN_MAX);
    Assert(m_DomainInfoLists[DomainType] != NULL);
    Assert(m_DomainInfoLists[DomainType][Level] != NULL);

    return m_DomainInfoLists[DomainType][Level];

    __END_CATCH
}

//--------------------------------------------------------------------------------
// get item info
//--------------------------------------------------------------------------------
void SkillDomainInfoManager::addDomainInfo(DomainInfo* pDomainInfo) const

{
    __BEGIN_TRY

    SkillDomainType_t DomainType = pDomainInfo->getType();
    Level_t Level = pDomainInfo->getLevel();

    // Same real bound on the write side. Both values come straight off the
    // SkillDomainInfo row, and this is a store into a heap array, so a stray
    // row was an out-of-bounds pointer write at startup.
    if (DomainType >= SKILL_DOMAIN_MAX)
        throw OutOfBoundException("SkillDomainInfoManager::addDomainInfo: domain out of range");

    if (m_DomainInfoLists[DomainType] == NULL || Level >= m_DomainInfoCounts[DomainType])
        throw OutOfBoundException("SkillDomainInfoManager::addDomainInfo: level out of range");

    Assert(DomainType < SKILL_DOMAIN_MAX);
    Assert(m_DomainInfoLists[DomainType][Level] == NULL);

    m_DomainInfoLists[DomainType][Level] = pDomainInfo;

    __END_CATCH
}

//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string SkillDomainInfoManager::toString() const

{
    __BEGIN_TRY
    StringStream msg;

    msg << "SkillDomainInfoManager(";

    for (uint i = 0; i < SKILL_DOMAIN_MAX; i++) {
        if (m_DomainInfoLists[i] == NULL)
            continue;

        // Same `j <= 150` overrun as the destructor had.
        for (int j = 0; j < m_DomainInfoCounts[i]; j++) {
            if (m_DomainInfoLists[i][j] == NULL) {
                msg << "NULL";
            } else {
                msg << "DomainInfo[" << (int)i << "][" << (int)j << "](" << m_DomainInfoLists[i][j]->toString();
            }
            msg << "\n";
        }
    }

    msg << ")";

    return msg.toString();
    __END_CATCH
}

// global variable declaration
SkillDomainInfoManager* g_pSkillDomainInfoManager = NULL;
