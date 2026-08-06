//////////////////////////////////////////////////////////////////////
//
// File Name 	: Statement.h
// Written by	: Gday29@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __STATEMENT_H__
#define __STATEMENT_H__

// include files
#include "Connection.h"
#include "Exception.h"
#include "Types.h"

// forward declaration
class Result;

//////////////////////////////////////////////////////////////////////
//
// class Statement;
//

//
//////////////////////////////////////////////////////////////////////

class Statement {
public:
    // constructor
    Statement() throw();

    // constructor
    Statement(char* fmt, ...) throw(Error);

    // destructor
    ~Statement() throw();

public:
    
    Result* executeQuery() throw(SQLQueryException, Error);

    
    Result* executeQuery(char*, ...) throw(SQLQueryException, Error);
    Result* executeQuery(const string& sqlStatement) throw(SQLQueryException, Error);

    // get SQL statement
    string getStatement() const throw() {
        return m_Statement;
    }

    
    void setStatement(char* fmt, ...) throw(Error);

    // get connection object
    Connection* getConnection() const throw() {
        return m_pConnection;
    }

    // set connection object
    void setConnection(Connection* pConnection) throw() {
        m_pConnection = pConnection;
    }

    // get warning/error string
    string getError() const throw() {
        return (m_pConnection == NULL) ? ("Not Associated with Connection Object") : (m_pConnection->getError());
    }

    // get affected rows
    uint getAffectedRowCount() const throw() {
        return m_nAffectedRows;
    }


private:
    // Connection
    Connection* m_pConnection;

    
    string m_Statement;

    // Query Result
    Result* m_pResult;

    
    uint m_nAffectedRows;
};

#endif // __STATEMENT_H__
