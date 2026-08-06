//////////////////////////////////////////////////////////////////////////////
// Filename    : DatabaseManager.h
// Written By  : elca

//////////////////////////////////////////////////////////////////////////////

#ifndef __DATABASE_MANAGER_H__
#define __DATABASE_MANAGER_H__

#include <unordered_map>

#include "Connection.h"
#include "Exception.h"
#include "Mutex.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class DatabaseManager;
//////////////////////////////////////////////////////////////////////////////

class DatabaseManager {
public:
    DatabaseManager();
    ~DatabaseManager();

public:
    void init();
    void addConnection(int TID, Connection* pConnection);
    void addDistConnection(int TID, Connection* pConnection);
    void addCBillingConnection(int TID, Connection* pConnection);
    //	void addPCRoomConnection(int TID, Connection * pConnection) throw(DuplicatedException);

    Connection* getConnection(const string& ip);
    Connection* getDistConnection(const string& ip);
    Connection* getCBillingConnection(const string& ip);
    //	Connection* getPCRoomConnection(const string& ip) throw(NoSuchElementException);
    Connection* getUserInfoConnection(void) {
        return m_pUserInfoConnection;
    }
    void executeDummyQuery(Connection* pConnection);

    //--------------------------------------------------------------------
    // * elca's NOTE
    
    
    
    
    
    //--------------------------------------------------------------------

    //--------------------------------------------------------------------
    // * elca's NOTE
    
    
    
    
    
    
    
    
    //--------------------------------------------------------------------
    Connection* getConnection(int TID);
    //	void addConnection(WorldID_t WorldID, Connection * pConnection) throw(DuplicatedException);

private:
    
    unordered_map<int, Connection*> m_Connections;

    
    unordered_map<int, Connection*> m_DistConnections;

    //	unordered_map<WorldID_t, Connection*> m_WorldConnections;
    unordered_map<int, Connection*> m_WorldConnections;

    
    unordered_map<int, Connection*> m_CBillingConnections;

    
    //	unordered_map<int, Connection*> m_PCRoomConnections;

    

    
    Connection* m_pDefaultConnection;

    
    Connection* m_pWorldDefaultConnection;

    
    Connection* m_pUserInfoConnection;

    Connection* m_pDistConnection;

    
    //	Connection* m_pPCRoomConnection;

    mutable Mutex m_Mutex;
};

extern DatabaseManager* g_pDatabaseManager;

#endif
