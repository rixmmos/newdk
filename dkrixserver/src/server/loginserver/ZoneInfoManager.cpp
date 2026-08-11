//----------------------------------------------------------------------
//
// Filename    : ZoneInfoManager.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "ZoneInfoManager.h"

#include "database/Connection.h"
#include "database/DatabaseManager.h"
#include "database/Result.h"
#include "database/Statement.h"

//----------------------------------------------------------------------
// constructor
//----------------------------------------------------------------------
ZoneInfoManager::ZoneInfoManager() {}

//----------------------------------------------------------------------
// destructor
//----------------------------------------------------------------------
ZoneInfoManager::~ZoneInfoManager() {
    
    
    
    
    for (HashMapZoneInfo::iterator itr = m_ZoneInfos.begin(); itr != m_ZoneInfos.end(); itr++) {
        delete itr->second;
        itr->second = NULL;
    }

    
    m_ZoneInfos.clear();
}


//----------------------------------------------------------------------
// initialize GSIM
//----------------------------------------------------------------------
void ZoneInfoManager::init() {
    __BEGIN_TRY

    // just load data from ZoneInfo table
    load();

    // just print to cout
    cout << toString() << endl;

    __END_CATCH
}

//----------------------------------------------------------------------
// load data from database
//----------------------------------------------------------------------
void ZoneInfoManager::load() {
    __BEGIN_TRY

    // Assigned as the first statement inside the try below, but the catch's
    // `delete pStmt` is reachable before that if getConnection() or
    // createStatement() throws -- delete on an indeterminate pointer.
    Statement* pStmt = NULL;

    try {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        Result* pResult = pStmt->executeQuery("SELECT ZoneID , ZoneGroupID FROM ZoneInfo");

        while (pResult->next()) {
            ZoneInfo* pZoneInfo = new ZoneInfo();
            pZoneInfo->setZoneID(pResult->getWORD(1));
            pZoneInfo->setZoneGroupID(pResult->getWORD(2));
            addZoneInfo(pZoneInfo);
        }

    } catch (SQLQueryException& sqe) {
        
        delete pStmt;

        throw Error(sqe.toString());
    }

    
    delete pStmt;

    __END_CATCH
}

//----------------------------------------------------------------------
// add info
//----------------------------------------------------------------------
void ZoneInfoManager::addZoneInfo(ZoneInfo* pZoneInfo) {
    __BEGIN_TRY

    HashMapZoneInfo::iterator itr = m_ZoneInfos.find(pZoneInfo->getZoneID());

    if (itr != m_ZoneInfos.end())
        throw DuplicatedException("duplicated zone id");

    m_ZoneInfos[pZoneInfo->getZoneID()] = pZoneInfo;

    __END_CATCH
}

//----------------------------------------------------------------------
// delete info
//----------------------------------------------------------------------
void ZoneInfoManager::deleteZoneInfo(ZoneID_t zoneID) {
    __BEGIN_TRY

    HashMapZoneInfo::iterator itr = m_ZoneInfos.find(zoneID);

    if (itr != m_ZoneInfos.end()) {
        
        delete itr->second;

        
        m_ZoneInfos.erase(itr);

    } else { // not found

        StringStream msg;
        msg << "ZoneID : " << zoneID;
        throw NoSuchElementException(msg.toString());
    }

    __END_CATCH
}

//----------------------------------------------------------------------
// get info
//----------------------------------------------------------------------
ZoneInfo* ZoneInfoManager::getZoneInfo(ZoneID_t zoneID) {
    __BEGIN_TRY

    ZoneInfo* pZoneInfo = NULL;

    HashMapZoneInfo::const_iterator itr = m_ZoneInfos.find(zoneID);

    if (itr != m_ZoneInfos.end()) {
        pZoneInfo = itr->second;

    } else { // not found

        StringStream msg;
        msg << "ZoneID : " << zoneID;
        throw NoSuchElementException(msg.toString());
    }

    return pZoneInfo;

    __END_CATCH
}

//----------------------------------------------------------------------
// get debug string
//----------------------------------------------------------------------
string ZoneInfoManager::toString() const {
    __BEGIN_TRY

    StringStream msg;

    msg << "ZoneInfoManager(\n";

    if (m_ZoneInfos.empty()) {
        msg << "EMPTY";

    } else {
        //--------------------------------------------------
        // *OPTIMIZATION*
        //
        
        //--------------------------------------------------
        for (HashMapZoneInfo::const_iterator itr = m_ZoneInfos.begin(); itr != m_ZoneInfos.end(); itr++)
            msg << itr->second->toString() << '\n';
    }

    msg << ")";

    return msg.toString();

    __END_CATCH
}

// global variable definition
ZoneInfoManager* g_pZoneInfoManager = NULL;
