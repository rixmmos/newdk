//////////////////////////////////////////////////////////////////////////////
// File Name	: Result.h
// Written By	: Gday29@ewestsoft.com
// Description	: Definition of Result Class
//////////////////////////////////////////////////////////////////////////////

#ifndef __RESULT_H__
#define __RESULT_H__

#include <mysql/mysql.h>
#include <sys/time.h>

#include "Exception.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// forward declaration
//////////////////////////////////////////////////////////////////////////////
class Connection;
class Statement;

#define T_RESULT MYSQL_RES

//////////////////////////////////////////////////////////////////////////////
// class Result;

//////////////////////////////////////////////////////////////////////////////

class Result {
public:
    Result(T_RESULT*, const string& statement);
    ~Result();

public:
    
    bool next();

    
    char* getField(uint index);
    char getChar(uint index) {
        return (getField(index))[0];
    }
    int getInt(uint index) {
        return atoi(getField(index));
    }
    uint getUInt(uint index) {
        return (uint)atoi(getField(index));
    }
    BYTE getBYTE(uint index) {
        return (BYTE)atoi(getField(index));
    }
    WORD getWORD(uint index) {
        return (WORD)atoi(getField(index));
    }
    DWORD getDWORD(uint index) {
        return strtoul(getField(index), (char**)NULL, 10);
    }
    const char* getString(uint index);

    
    uint getRowCount() const {
        return m_RowCount;
    }
    uint getFieldCount() const {
        return m_FieldCount;
    }

    string getStatement(void) const {
        return m_Statement;
    }

private:
    T_RESULT* m_pResult; 
    MYSQL_ROW m_pRow;    
    uint m_RowCount;     
    uint m_FieldCount;
    string m_Statement; 
};

#endif // __RESULT_H__
