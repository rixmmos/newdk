//////////////////////////////////////////////////////////////////////////////
// Filename    : MonsterNameManager.cpp
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "DB.h"
#include "Monster.h"
#include "MonsterInfo.h"
#include "MonsterNameManager.h"

//////////////////////////////////////////////////////////////////////////////
// global varible
//////////////////////////////////////////////////////////////////////////////
MonsterNameManager* g_pMonsterNameManager = NULL;

//////////////////////////////////////////////////////////////////////////////
// class MonsterNameManager member methods
//////////////////////////////////////////////////////////////////////////////

MonsterNameManager::MonsterNameManager()

{
    __BEGIN_TRY

    m_pFirstName = NULL;
    m_pMiddleName = NULL;
    m_pLastName = NULL;
    m_nFirstNameCount = 0;
    m_nMiddleNameCount = 0;
    m_nLastNameCount = 0;

    __END_CATCH
}

MonsterNameManager::~MonsterNameManager()

{
    __BEGIN_TRY

    SAFE_DELETE_ARRAY(m_pFirstName);
    SAFE_DELETE_ARRAY(m_pMiddleName);
    SAFE_DELETE_ARRAY(m_pLastName);

    m_UsedName.clear();

    __END_CATCH
}

void MonsterNameManager::init()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;
    Result* pResult = NULL;
    int nCount = 0;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        
        pResult = pStmt->executeQuery("SELECT * FROM FirstNameInfo");
        nCount = pResult->getRowCount();
        if (nCount == 0) {
            cerr << "MonsterNameManager::init() : No data exist on FirstNameInfo" << endl;
            throw("MonsterNameManager::init() : No data exist on FirstNameInfo");
        }

        m_nFirstNameCount = nCount;
        m_pFirstName = new string[m_nFirstNameCount];
        nCount = 0;

        while (pResult->next()) {
            m_pFirstName[nCount] = pResult->getString(1);
            nCount++;
        }

        
        pResult = pStmt->executeQuery("SELECT * FROM MiddleNameInfo");
        nCount = pResult->getRowCount();
        if (nCount == 0) {
            cerr << "MonsterNameManager::init() : No data exist on MiddleNameInfo" << endl;
            throw("MonsterNameManager::init() : No data exist on MiddleNameInfo");
        }

        m_nMiddleNameCount = nCount;
        m_pMiddleName = new string[m_nMiddleNameCount];
        nCount = 0;

        while (pResult->next()) {
            m_pMiddleName[nCount] = pResult->getString(1);
            nCount++;
        }

        
        pResult = pStmt->executeQuery("SELECT * FROM LastNameInfo");
        nCount = pResult->getRowCount();
        if (nCount == 0) {
            cerr << "MonsterNameManager::init() : No data exist on LastNameInfo" << endl;
            throw("MonsterNameManager::init() : No data exist on LastNameInfo");
        }

        m_nLastNameCount = nCount;
        m_pLastName = new string[m_nLastNameCount];
        nCount = 0;

        while (pResult->next()) {
            m_pLastName[nCount] = pResult->getString(1);
            nCount++;
        }

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt);

    __END_CATCH
}

string MonsterNameManager::getRandomName(Monster* pMonster)

{
    __BEGIN_TRY

    if (pMonster == NULL)
        return "";

    // const MonsterInfo* pInfo = g_pMonsterInfoManager->getMonsterInfo(pMonster->getMonsterType());

    // Level_t MonsterLevel = pInfo->getLevel();
    string Name = "";
    bool bContinue = true;
    int trial = 0;

    while (bContinue && trial++ < 300) {
        short nFirstNameIndex = -1;
        short nMiddleNameIndex = -1;
        short nLastNameIndex = -1;

        nLastNameIndex = rand() % m_nLastNameCount;

         

         

        if (nFirstNameIndex != -1)
            Name += m_pFirstName[nFirstNameIndex] + " ";
        if (nMiddleNameIndex != -1)
            Name += m_pMiddleName[nMiddleNameIndex] + " ";
        if (nLastNameIndex != -1)
            Name += m_pLastName[nLastNameIndex];

        return Name;
    }

    
    
    if (Name == "")
        Name == "";

    return Name;

    __END_CATCH
}
