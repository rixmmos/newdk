//////////////////////////////////////////////////////////////////////
//
// File Name    : PreparedStatement.h
// Written by   : Phase 11B (2026-04-19)
// Lifted onto main 2026-08-07 from archive/modernization-phases-1-17 (tag tip)
// Description  : Parameterised-query wrapper for injection remediation.
//
//////////////////////////////////////////////////////////////////////

#ifndef __PREPARED_STATEMENT_H__
#define __PREPARED_STATEMENT_H__

// include files
#include <cstring>
#include <ctime>
#include <string>
#include <vector>

#include <mysql/mysql.h>
#include <type_traits>

#include "Connection.h"
#include "Exception.h"
#include "Types.h"
#ifndef MY_BOOL_DEFINED
typedef std::remove_pointer<decltype(MYSQL_BIND::is_null)>::type my_bool_t;
#define MY_BOOL_DEFINED
#endif

class Result;

class PreparedStatement {
public:
    PreparedStatement(Connection* pConnection, const std::string& sqlWithQMarks);
    ~PreparedStatement();

public:
    void bindInt(uint idx, int v);
    void bindUInt(uint idx, uint v);
    void bindLong(uint idx, long long v);
    void bindULong(uint idx, unsigned long long v);
    void bindDouble(uint idx, double v);
    void bindString(uint idx, const std::string& v);
    void bindTime(uint idx, time_t t);
    void bindNull(uint idx);

    Result* execute();

    std::string getError() const;

    uint getAffectedRowCount() const {
        return m_nAffectedRows;
    }

    uint getInsertID() const {
        return m_nInsertID;
    }

    uint getParamCount() const {
        return m_nParamCount;
    }

private:
    struct Param {
        enum Type {
            PARAM_INT,
            PARAM_UINT,
            PARAM_LONG,
            PARAM_ULONG,
            PARAM_DOUBLE,
            PARAM_STRING,
            PARAM_TIME,
            PARAM_NULL,
            PARAM_UNSET
        };
        Type type;
        int i;
        uint u;
        long long ll;
        unsigned long long ull;
        double d;
        std::string s;
        MYSQL_TIME mt;
        unsigned long length;
        my_bool_t isNull;

        Param() : type(PARAM_UNSET), i(0), u(0), ll(0), ull(0), d(0.0), length(0), isNull(0) {
            std::memset(&mt, 0, sizeof(mt));
        }
    };

    void checkIdx(uint idx) const;

    Connection* m_pConnection;
    MYSQL_STMT* m_pStmt;
    std::string m_Statement;
    std::vector<Param> m_Params;
    uint m_nParamCount;
    uint m_nAffectedRows;
    uint m_nInsertID;
    Result* m_pResult;
};

#endif // __PREPARED_STATEMENT_H__
