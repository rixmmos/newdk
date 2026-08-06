//////////////////////////////////////////////////////////////////////
//
// File Name	: Connection.cpp
// Written by	: Gday29@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

// include files
#include "Connection.h"

#include "Statement.h"

//////////////////////////////////////////////////////////////////////
//
// constructor
//


//
//////////////////////////////////////////////////////////////////////

Connection::Connection() throw(Error) : m_bConnected(false), m_bBusy(false) {
    __BEGIN_TRY

    m_Mutex.setName("Connection");

    
    if (mysql_init(&m_Mysql) == NULL) {
        throw Error(mysql_error(&m_Mysql));
    }

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// *CAUTION*
//



//
//////////////////////////////////////////////////////////////////////

Connection::Connection(string host, string db, string user, string password, uint port) throw(SQLConnectException,
                                                                                              Error)
    : m_bConnected(false), m_Host(host), m_Port(port), m_Database(db), m_User(user), m_Password(password) {
    __BEGIN_TRY

    m_Mutex.setName("Connection");

    
    if (mysql_init(&m_Mysql) == NULL)
        throw Error(mysql_error(&m_Mysql));

    
    connect(m_Host, m_Database, m_User, m_Password, m_Port);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
//	destructor
//
//////////////////////////////////////////////////////////////////////

Connection::~Connection() throw(Error) {
    __BEGIN_TRY

    
    if (m_bConnected) {
        close();
    }

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// connect()
//



//
// *CAUTION*
//



//

//
// ex> Connection conn;
//     conn.connect( "vampire.ewestsoft.com" , "bbs" , "bbsuser" , "bbspassword" );
//
//////////////////////////////////////////////////////////////////////

void Connection::connect(string host, string db, string user, string password, uint port) throw(SQLConnectException) {
    __BEGIN_TRY

    m_Host = host;
    m_Port = port;
    m_Database = db;
    m_User = user;
    m_Password = password;

    connect();

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// connect()
//


//
//////////////////////////////////////////////////////////////////////

void Connection::connect() throw(SQLConnectException) {
    __BEGIN_TRY

    
    if (m_bConnected) {
        
        close();

        throw SQLConnectException("Already Connected");
    }

    //--------------------------------------------------
    
    //--------------------------------------------------
    m_bConnected = (mysql_real_connect(&m_Mysql, m_Host.c_str(), m_User.c_str(), m_Password.c_str(), m_Database.c_str(),
                                       m_Port, 0, 0) != NULL);
    // cout << "Connection Calls~~~" << endl;

    
    if (!m_bConnected) {
        throw SQLConnectException(mysql_error(&m_Mysql));
    }

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
//	close()
//

//
//////////////////////////////////////////////////////////////////////

void Connection::close() throw(SQLConnectException, Error) {
    __BEGIN_TRY

    if (!m_bConnected) {
        throw SQLConnectException("Not Connected");
    }

    
    mysql_close(&m_Mysql);

    m_bConnected = false;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
//	createStatement
//

//
//////////////////////////////////////////////////////////////////////

Statement* Connection::createStatement() throw() {
    __BEGIN_TRY

    
    Statement* pStatement = new Statement();

    
    pStatement->setConnection(this);

    
    return pStatement;

    __END_CATCH
}
