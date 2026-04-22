//////////////////////////////////////////////////////////////////////////////
// File Name	: Result.h
// Written By	: Gday29@ewestsoft.com
// Description	: Definition of Result Class
//////////////////////////////////////////////////////////////////////////////

#ifndef __RESULT_H__
#define __RESULT_H__

#include <mysql/mysql.h>
#include <sys/time.h>

#include <vector>

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
// Result ė” ģ‚­ģ ķ•  ķ•„ģ”ź°€ ģ—†ė‹¤. ģ‚¬ģ©ģ˛ė” Statement ė§ ģ‚­ģ ķ•ė©´ ėė‹¤.
//////////////////////////////////////////////////////////////////////////////

class Result {
public:
    struct FieldValue {
        bool   isNull;
        string value;

        FieldValue() : isNull(true) {}
        FieldValue(const string& v) : isNull(false), value(v) {}
    };

public:
    Result(T_RESULT*, const string& statement);
    Result(const std::vector<std::vector<FieldValue> >& rows, const string& statement);
    ~Result();

public:
    // ė‹¤ģ¯ rowė ė„ģ–´ź°„ė‹¤.
    bool next();

    // ķ¹ģ • ķ•„ė“(ģ»¬ė¼) ź°’ģ¯„ ź°€ģ§€ź³  ģØė‹¤.
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

    // ģæ¼ė¦¬ ź²°ź³¼ź°’ģ¯´ ķ¸¬ķ•Øķ•ė” row/columnģ¯ ģ«ģ˛ė¼ ė¦¬ķ„´ķ•ė‹¤.
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
    enum BackendType {
        BACKEND_MYSQL_RES,
        BACKEND_MATERIALIZED
    };

    BackendType m_BackendType;
    T_RESULT* m_pResult; // ź²°ź³¼ź°’ģ¯„ ė‚ķ€ė‚´ė” MYSQL structure
    MYSQL_ROW m_pRow;    // ķ„ģ˛¬ ģ²ė¦¬ķ•ź³  ģ˛ė” row
    uint m_RowCount;     // ģæ¼ė¦¬ ź²°ź³¼ė ģ–»ģ–´ė‚ø rowģ¯ ģ«ģ˛
    uint m_FieldCount;
    string m_Statement; // ģ–´ė–¤ queryė¬øģ— ģ¯ķ• ź²°ź³¼ģ¯øź°€...?
    std::vector<std::vector<FieldValue> > m_Rows;
    int m_CurrentRowIndex;
};

#endif // __RESULT_H__
