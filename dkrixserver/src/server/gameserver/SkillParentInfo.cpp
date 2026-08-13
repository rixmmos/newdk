//----------------------------------------------------------------------
//
// Filename    : SkillParentInfo.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

#include "SkillParentInfo.h"

#include "Assert.h"
#include "DB.h"
// #include <algo.h>

//----------------------------------------------------------------------
//
// constructor
//
//----------------------------------------------------------------------
SkillParentInfo::SkillParentInfo(SkillType_t SkillType)

    : m_SkillType(SkillType) {
    __BEGIN_TRY
    init();
    __END_CATCH
}


//----------------------------------------------------------------------
//
// destructor
//
//----------------------------------------------------------------------
SkillParentInfo::~SkillParentInfo()

{
    __BEGIN_TRY
    __END_CATCH_NO_RETHROW
}


//----------------------------------------------------------------------
//
// SkillParentInfo::init()
//
//----------------------------------------------------------------------
void SkillParentInfo::init()

{
    __BEGIN_TRY
    __END_CATCH
}


//----------------------------------------------------------------------
//
// SkillParentInfo::hasParent()
//

//
//----------------------------------------------------------------------
bool SkillParentInfo::hasParent(SkillType_t SkillType) {
    __BEGIN_TRY

    try {
        SkillType_t FirstType = m_Parents.front();

        if (FirstType == 0)
            return true;

        list<SkillType_t>::iterator itr;
        for (itr = m_Parents.begin(); itr != m_Parents.end(); itr++) {
            if (*itr == SkillType) {
                break;
            }
        }

        if (itr == m_Parents.end())
            return false;

        return true;
    } catch (Throwable& t) {
        // cerr << t.toString() << endl;
    }
    return false;

    __END_CATCH
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------
SkillType_t SkillParentInfo::getParents(SkillType_t SkillType) const {
    __BEGIN_TRY

    for (list<SkillType_t>::const_iterator itr = m_Parents.begin(); itr != m_Parents.end(); itr++) {
        if (SkillType == (*itr))
            return *itr;
    }

    cerr << "SkillParentInfo::getParents() : NoSuchElementException" << endl;
    throw NoSuchElementException();

    __END_CATCH
}


//----------------------------------------------------------------------
// get debug string
//----------------------------------------------------------------------
string SkillParentInfo::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "SkillParentInfo("
        << "SkillType:" << (int)m_SkillType << ",Parents(";

    for (list<SkillType_t>::const_iterator i = m_Parents.begin(); i != m_Parents.end(); i++) {
        msg << (int)(*i) << " ";
    }
    msg << ")";
    msg << ")";

    return msg.toString();

    __END_CATCH
}


//--------------------------------------------------------------------
//
// constructor
//
//--------------------------------------------------------------------
SkillParentInfoManager::SkillParentInfoManager()

    : m_SkillCount(0), m_SkillParentInfoList(NULL){__BEGIN_TRY __END_CATCH}

      //--------------------------------------------------------------------
      //
      // destructor
      //
      //--------------------------------------------------------------------
      SkillParentInfoManager::~SkillParentInfoManager()

{
    __BEGIN_TRY

    // The array of pointers was freed but never its elements, so every
    // SkillParentInfo load() allocated leaked. Same shape as
    // InfoClassManager::~InfoClassManager(): delete the elements, then the array.
    if (m_SkillParentInfoList != NULL) {
        for (uint i = 0; i < m_SkillCount; i++)
            SAFE_DELETE(m_SkillParentInfoList[i]);
    }

    SAFE_DELETE_ARRAY(m_SkillParentInfoList);

    __END_CATCH_NO_RETHROW
}

//--------------------------------------------------------------------
//
// SkillParentInfoManager:: init()
//
//--------------------------------------------------------------------
void SkillParentInfoManager::init()

{
    __BEGIN_TRY

    // Loading from DataBase and addSkillParentInfo
    load();

    __END_CATCH
}

//--------------------------------------------------------------------
//
// SkillParentInfoManager:: load()
//
//--------------------------------------------------------------------
void SkillParentInfoManager::load()

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    Statement* pStmt = NULL; // by sigi
    Result* pResult;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        pResult = pStmt->executeQuery("SELECT MAX(SkillType) FROM SkillTreeInfo");

        if (pResult->getRowCount() == 0) {
            SAFE_DELETE(pStmt);
            throw Error("There is no data in SkillTreeInfo Table");
        }

        pResult->next();

        m_SkillCount = pResult->getInt(1) + 1;

        Assert(m_SkillCount > 0);

        m_SkillParentInfoList = new SkillParentInfo*[m_SkillCount];

        for (uint i = 0; i < m_SkillCount; i++)
            m_SkillParentInfoList[i] = NULL;

        pResult = pStmt->executeQuery("SELECT SkillType, Parent FROM SkillTreeInfo ");

        // cout<<"======= SkillParentInfo Manager ==========="<<endl;

        // Grouping used to be inferred from a `tempSkillType` cursor seeded with 0,
        // so a row with SkillType 0 compared equal on the very first iteration and
        // dereferenced the still-NULL slot 0. The shipped table's MIN(SkillType) is 5,
        // which is the only reason that never fired. Keying off the slot itself
        // removes the assumption that rows arrive grouped (the query has no ORDER BY)
        // and yields the same result on grouped data.
        while (pResult->next()) {
            int i = 0;
            SkillType_t SkillType = pResult->getInt(++i);
            SkillType_t Parent = pResult->getInt(++i);

            if (SkillType >= m_SkillCount) {
                cerr << "SkillParentInfoManager::load() : SkillType " << (int)SkillType
                     << " is out of bounds; row skipped" << endl;
                continue;
            }

            if (m_SkillParentInfoList[SkillType] == NULL) {
                SkillParentInfo* pSkillParentInfo = new SkillParentInfo(SkillType);
                pSkillParentInfo->addParents(Parent);
                addSkillParentInfo(pSkillParentInfo);
            } else {
                m_SkillParentInfoList[SkillType]->addParents(Parent);
            }
        }

        // for (uint i = 0; i < m_SkillCount; i++)
        //{
        //	if (m_SkillParentInfoList[ i ] != NULL)
        //		//cout<< m_SkillParentInfoList[ i ]->toString()<<endl;
        // }

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)

    __END_DEBUG
    __END_CATCH
}

//--------------------------------------------------------------------
//
// SkillParentInfoManager:: save()
//
//--------------------------------------------------------------------
void SkillParentInfoManager::save()

{
    __BEGIN_TRY

    throw UnsupportedError(__PRETTY_FUNCTION__);

    __END_CATCH
}


//--------------------------------------------------------------------
//
// SkillParentInfoManager:: addSkillParentInfo()
//
//--------------------------------------------------------------------
void SkillParentInfoManager::addSkillParentInfo(SkillParentInfo* pSkillParentInfo)

{
    __BEGIN_TRY

    Assert(pSkillParentInfo != NULL);

    if (m_SkillParentInfoList[pSkillParentInfo->getSkillType()] != NULL)
        throw DuplicatedException();

    m_SkillParentInfoList[pSkillParentInfo->getSkillType()] = pSkillParentInfo;

    __END_CATCH
}

//--------------------------------------------------------------------
//
// SkillParentInfoManager:: getSkillParentInfo()
//
//--------------------------------------------------------------------
SkillParentInfo* SkillParentInfoManager::getSkillParentInfo(SkillType_t SkillType) const {
    __BEGIN_TRY

    if (SkillType >= m_SkillCount) {
        cerr << "SkillParentInfoManager::getSkillParentInfo() : out of bounds" << endl;
        throw OutOfBoundException();
    }

    if (m_SkillParentInfoList[SkillType] == NULL) {
        cerr << "SkillParentInfoManager::getSkillParentInfo() : NoSuchElementException" << endl;
        throw NoSuchElementException();
    }

    return m_SkillParentInfoList[SkillType];

    __END_CATCH
}

//--------------------------------------------------------------------
//
// SkillParentInfoManager:: toString()
//
//--------------------------------------------------------------------
string SkillParentInfoManager::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "SkillParentInfoManager(";

    for (uint i = 0; i < m_SkillCount; i++) {
        if (m_SkillParentInfoList[i] != NULL) {
            msg << m_SkillParentInfoList[i]->toString();
        } else {
            msg << "NULL";
        }
    }

    msg << ")";

    return msg.toString();

    __END_CATCH
}

// Global Variable definition
SkillParentInfoManager* g_pSkillParentInfoManager = NULL;
