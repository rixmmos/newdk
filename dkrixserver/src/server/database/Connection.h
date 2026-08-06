//////////////////////////////////////////////////////////////////////
//
// File Name 	: Connection.h
// Written by	: Gday29@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __CONNECTION_H__
#define __CONNECTION_H__

// include files
#include <mysql/mysql.h>

#include "Exception.h"
#include "Mutex.h"
#include "Types.h"

// forward declaration
class Statement;

//////////////////////////////////////////////////////////////////////
//
// class Connection;
//

//
//////////////////////////////////////////////////////////////////////

class Connection {
public:
    // constructor
    Connection();

    // constructor(1-time connection)
    Connection(string host, string db, string user, string password, uint port = 0);

    // destructor
    ~Connection();

    // close the connection to database
    void close();

    
    void connect(string host, string db, string user, string password, uint port = 0);

    
    void connect();

    // check the connection
    bool isConnected() const {
        return m_bConnected;
    }
    bool operator!() const {
        return m_bConnected == false;
    }

    
    Statement* createStatement();

    // get the MYSQL object
    MYSQL* getMYSQL() {
        return &m_Mysql;
    }

    // get MS's host name(ip)
    string getHost() const {
        return m_Host;
    }

    // get MS's service port
    uint getPort() const {
        return m_Port;
    }

    // get database name
    string getDatabase() const {
        return m_Database;
    }

    // get user id
    string getUser() const {
        return m_User;
    }

    // get user password
    string getPassword() const {
        return m_Password;
    }

    // get connection's name
    string getName() const {
        return m_Name;
    }

    // set connection's name
    void setName(string name) {
        m_Name = name;
    }

    // get/set busy status
    bool isBusy(void) const {
        return m_bBusy;
    }
    void setBusy(bool busy = true) {
        m_bBusy = busy;
    }

    // get error
    string getError() {
        return mysql_error(&m_Mysql);
    }

    // lock/unlock
    void lock() {
        m_Mutex.lock();
    }
    void unlock() {
        m_Mutex.unlock();
    }

private:
    // ??
    MYSQL m_Mysql;

    // is connected?
    bool m_bConnected;

    
    string m_Host;

    
    uint m_Port;

    
    string m_Database;

    
    string m_User;

    
    string m_Password;

    
    string m_Name;

    
    bool m_bBusy;

    
    Mutex m_Mutex;
};

#endif // __CONNECTION_H__
