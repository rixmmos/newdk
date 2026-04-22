//////////////////////////////////////////////////////////////////////
//
// File Name    : PreparedStatement.cpp
// Written by   : Phase 11B (2026-04-19)
// Description  : Parameterised-query wrapper for injection remediation.
//
//////////////////////////////////////////////////////////////////////

#include "PreparedStatement.h"

#include <cstdio>
#include <ctime>

#include "Assert.h"
#include "Exception.h"
#include "Result.h"

namespace {
struct PreparedResultColumn {
    enum Kind {
        KIND_INT64,
        KIND_UINT64,
        KIND_DOUBLE,
        KIND_STRING,
        KIND_TIME
    };

    Kind kind;
    enum_field_types mysqlType;
    long long signedValue;
    unsigned long long unsignedValue;
    double doubleValue;
    MYSQL_TIME timeValue;
    std::vector<char> stringValue;
    unsigned long length;
    my_bool_t isNull;
    my_bool_t error;

    PreparedResultColumn()
        : kind(KIND_STRING),
          mysqlType(MYSQL_TYPE_STRING),
          signedValue(0),
          unsignedValue(0),
          doubleValue(0.0),
          length(0),
          isNull(0),
          error(0)
    {
        std::memset(&timeValue, 0, sizeof(timeValue));
    }
};

static std::string formatPreparedTime(const MYSQL_TIME& t, enum_field_types type) {
    char buffer[64];

    if (type == MYSQL_TYPE_DATE || type == MYSQL_TYPE_NEWDATE) {
        std::snprintf(buffer, sizeof(buffer), "%04u-%02u-%02u", t.year, t.month, t.day);
        return buffer;
    }

    if (type == MYSQL_TYPE_TIME) {
        if (t.day > 0) {
            std::snprintf(buffer, sizeof(buffer), "%s%u %02u:%02u:%02u",
                          t.neg ? "-" : "", t.day, t.hour, t.minute, t.second);
        } else {
            std::snprintf(buffer, sizeof(buffer), "%s%02u:%02u:%02u",
                          t.neg ? "-" : "", t.hour, t.minute, t.second);
        }
        return buffer;
    }

    std::snprintf(buffer, sizeof(buffer), "%04u-%02u-%02u %02u:%02u:%02u",
                  t.year, t.month, t.day, t.hour, t.minute, t.second);
    return buffer;
}

static Result::FieldValue materializePreparedColumn(MYSQL_STMT* pStmt, uint columnIndex, PreparedResultColumn& column) {
    if (column.isNull) {
        return Result::FieldValue();
    }

    switch (column.kind) {
    case PreparedResultColumn::KIND_INT64:
        return Result::FieldValue(std::to_string(column.signedValue));

    case PreparedResultColumn::KIND_UINT64:
        return Result::FieldValue(std::to_string(column.unsignedValue));

    case PreparedResultColumn::KIND_DOUBLE: {
        char buffer[64];
        std::snprintf(buffer, sizeof(buffer), "%.17g", column.doubleValue);
        return Result::FieldValue(buffer);
    }

    case PreparedResultColumn::KIND_TIME:
        return Result::FieldValue(formatPreparedTime(column.timeValue, column.mysqlType));

    case PreparedResultColumn::KIND_STRING:
    default: {
        if (column.error && column.length + 1 > column.stringValue.size()) {
            column.stringValue.resize(column.length + 1);

            MYSQL_BIND fetchBind;
            std::memset(&fetchBind, 0, sizeof(fetchBind));
            fetchBind.buffer_type = MYSQL_TYPE_STRING;
            fetchBind.buffer = column.stringValue.data();
            fetchBind.buffer_length = column.stringValue.size();
            fetchBind.length = &column.length;
            fetchBind.is_null = &column.isNull;
            fetchBind.error = &column.error;

            if (mysql_stmt_fetch_column(pStmt, &fetchBind, columnIndex, 0) != 0) {
                throw SQLQueryException(std::string("mysql_stmt_fetch_column failed: ") + mysql_stmt_error(pStmt));
            }
        }

        return Result::FieldValue(std::string(column.stringValue.data(), column.length));
    }
    }
}
} // namespace

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////

PreparedStatement::PreparedStatement(Connection* pConnection, const std::string& sqlWithQMarks)
    : m_pConnection(pConnection),
      m_pStmt(NULL),
      m_Statement(sqlWithQMarks),
      m_nParamCount(0),
      m_nAffectedRows(0),
      m_nInsertID(0),
      m_pResult(NULL) {
    __BEGIN_TRY

    Assert(m_pConnection != NULL);

    MYSQL* mysql = m_pConnection->getMYSQL();

    m_pStmt = mysql_stmt_init(mysql);
    if (m_pStmt == NULL) {
        throw SQLException(std::string("PreparedStatement: mysql_stmt_init failed: ") + mysql_error(mysql));
    }

    if (mysql_stmt_prepare(m_pStmt, sqlWithQMarks.c_str(), sqlWithQMarks.size()) != 0) {
        std::string err = mysql_stmt_error(m_pStmt);
        mysql_stmt_close(m_pStmt);
        m_pStmt = NULL;
        throw SQLException(std::string("PreparedStatement: mysql_stmt_prepare failed: ") + err
                           + " [sql=" + sqlWithQMarks + "]");
    }

    my_bool_t updateMaxLength = 1;
    mysql_stmt_attr_set(m_pStmt, STMT_ATTR_UPDATE_MAX_LENGTH, &updateMaxLength);

    m_nParamCount = (uint)mysql_stmt_param_count(m_pStmt);
    m_Params.resize(m_nParamCount);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////

PreparedStatement::~PreparedStatement() {
    if (m_pResult != NULL) {
        delete m_pResult;
        m_pResult = NULL;
    }

    if (m_pStmt != NULL) {
        mysql_stmt_close(m_pStmt);
        m_pStmt = NULL;
    }
}

//////////////////////////////////////////////////////////////////////
// idx bounds check (1-indexed, matching `?` positions)
//////////////////////////////////////////////////////////////////////

void PreparedStatement::checkIdx(uint idx) const {
    __BEGIN_TRY

    if (idx == 0 || idx > m_nParamCount) {
        char msg[256];
        std::snprintf(msg, sizeof(msg),
                      "PreparedStatement::bind*: idx %u out of range [1..%u] for stmt [%s]",
                      idx, m_nParamCount, m_Statement.c_str());
        throw OutOfBoundException(std::string(msg));
    }

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
// Parameter binding
//////////////////////////////////////////////////////////////////////

void PreparedStatement::bindInt(uint idx, int v) {
    checkIdx(idx);
    Param& p = m_Params[idx - 1];
    p.type = Param::PARAM_INT;
    p.i = v;
    p.isNull = 0;
}

void PreparedStatement::bindUInt(uint idx, uint v) {
    checkIdx(idx);
    Param& p = m_Params[idx - 1];
    p.type = Param::PARAM_UINT;
    p.u = v;
    p.isNull = 0;
}

void PreparedStatement::bindLong(uint idx, long long v) {
    checkIdx(idx);
    Param& p = m_Params[idx - 1];
    p.type = Param::PARAM_LONG;
    p.ll = v;
    p.isNull = 0;
}

void PreparedStatement::bindULong(uint idx, unsigned long long v) {
    checkIdx(idx);
    Param& p = m_Params[idx - 1];
    p.type = Param::PARAM_ULONG;
    p.ull = v;
    p.isNull = 0;
}

void PreparedStatement::bindDouble(uint idx, double v) {
    checkIdx(idx);
    Param& p = m_Params[idx - 1];
    p.type = Param::PARAM_DOUBLE;
    p.d = v;
    p.isNull = 0;
}

void PreparedStatement::bindString(uint idx, const std::string& v) {
    checkIdx(idx);
    Param& p = m_Params[idx - 1];
    p.type = Param::PARAM_STRING;
    p.s = v;
    p.length = (unsigned long)p.s.size();
    p.isNull = 0;
}

void PreparedStatement::bindTime(uint idx, time_t t) {
    checkIdx(idx);
    Param& p = m_Params[idx - 1];
    p.type = Param::PARAM_TIME;

    struct tm localtm;
    localtime_r(&t, &localtm);

    std::memset(&p.mt, 0, sizeof(p.mt));
    p.mt.year   = localtm.tm_year + 1900;
    p.mt.month  = localtm.tm_mon + 1;
    p.mt.day    = localtm.tm_mday;
    p.mt.hour   = localtm.tm_hour;
    p.mt.minute = localtm.tm_min;
    p.mt.second = localtm.tm_sec;
    p.mt.time_type = MYSQL_TIMESTAMP_DATETIME;

    p.isNull = 0;
}

void PreparedStatement::bindNull(uint idx) {
    checkIdx(idx);
    Param& p = m_Params[idx - 1];
    p.type = Param::PARAM_NULL;
    p.isNull = 1;
}

//////////////////////////////////////////////////////////////////////
// execute()
//////////////////////////////////////////////////////////////////////

Result* PreparedStatement::execute() {
    __BEGIN_TRY

    Assert(m_pStmt != NULL);

    if (m_pResult != NULL) {
        delete m_pResult;
        m_pResult = NULL;
    }

    for (uint i = 0; i < m_nParamCount; ++i) {
        if (m_Params[i].type == Param::PARAM_UNSET) {
            char msg[256];
            std::snprintf(msg, sizeof(msg),
                          "PreparedStatement::execute: parameter %u unbound for stmt [%s]",
                          i + 1, m_Statement.c_str());
            throw SQLException(std::string(msg));
        }
    }

    std::vector<MYSQL_BIND> binds(m_nParamCount);
    std::memset(binds.data(), 0, sizeof(MYSQL_BIND) * m_nParamCount);

    for (uint i = 0; i < m_nParamCount; ++i) {
        Param& p = m_Params[i];
        MYSQL_BIND& b = binds[i];

        b.is_null = &p.isNull;

        switch (p.type) {
        case Param::PARAM_INT:
            b.buffer_type = MYSQL_TYPE_LONG;
            b.buffer = &p.i;
            b.is_unsigned = 0;
            break;
        case Param::PARAM_UINT:
            b.buffer_type = MYSQL_TYPE_LONG;
            b.buffer = &p.u;
            b.is_unsigned = 1;
            break;
        case Param::PARAM_LONG:
            b.buffer_type = MYSQL_TYPE_LONGLONG;
            b.buffer = &p.ll;
            b.is_unsigned = 0;
            break;
        case Param::PARAM_ULONG:
            b.buffer_type = MYSQL_TYPE_LONGLONG;
            b.buffer = &p.ull;
            b.is_unsigned = 1;
            break;
        case Param::PARAM_DOUBLE:
            b.buffer_type = MYSQL_TYPE_DOUBLE;
            b.buffer = &p.d;
            break;
        case Param::PARAM_STRING:
            b.buffer_type = MYSQL_TYPE_STRING;
            b.buffer = const_cast<char*>(p.s.data());
            b.buffer_length = p.length;
            b.length = &p.length;
            break;
        case Param::PARAM_TIME:
            b.buffer_type = MYSQL_TYPE_DATETIME;
            b.buffer = &p.mt;
            break;
        case Param::PARAM_NULL:
            b.buffer_type = MYSQL_TYPE_NULL;
            break;
        case Param::PARAM_UNSET:
            break;
        }
    }

    if (m_nParamCount > 0 && mysql_stmt_bind_param(m_pStmt, binds.data()) != 0) {
        throw SQLQueryException(std::string("mysql_stmt_bind_param failed: ") + mysql_stmt_error(m_pStmt)
                                + " [sql=" + m_Statement + "]");
    }

    if (mysql_stmt_execute(m_pStmt) != 0) {
        throw SQLQueryException(std::string("mysql_stmt_execute failed: ") + mysql_stmt_error(m_pStmt)
                                + " [sql=" + m_Statement + "]");
    }

    MYSQL_RES* pMetadata = mysql_stmt_result_metadata(m_pStmt);
    if (pMetadata != NULL) {
        if (mysql_stmt_store_result(m_pStmt) != 0) {
            mysql_free_result(pMetadata);
            throw SQLQueryException(std::string("mysql_stmt_store_result failed: ") + mysql_stmt_error(m_pStmt)
                                    + " [sql=" + m_Statement + "]");
        }

        uint fieldCount = (uint)mysql_num_fields(pMetadata);
        MYSQL_FIELD* pFields = mysql_fetch_fields(pMetadata);
        std::vector<PreparedResultColumn> columns(fieldCount);
        std::vector<MYSQL_BIND> resultBinds(fieldCount);
        if (fieldCount > 0) {
            std::memset(resultBinds.data(), 0, sizeof(MYSQL_BIND) * fieldCount);
        }

        for (uint i = 0; i < fieldCount; ++i) {
            PreparedResultColumn& column = columns[i];
            MYSQL_BIND& bind = resultBinds[i];

            column.mysqlType = pFields[i].type;
            bind.is_null = &column.isNull;
            bind.length = &column.length;
            bind.error = &column.error;

            switch (pFields[i].type) {
            case MYSQL_TYPE_TINY:
            case MYSQL_TYPE_SHORT:
            case MYSQL_TYPE_LONG:
            case MYSQL_TYPE_INT24:
            case MYSQL_TYPE_LONGLONG:
            case MYSQL_TYPE_YEAR:
                if ((pFields[i].flags & UNSIGNED_FLAG) != 0) {
                    column.kind = PreparedResultColumn::KIND_UINT64;
                    bind.buffer_type = MYSQL_TYPE_LONGLONG;
                    bind.buffer = &column.unsignedValue;
                    bind.is_unsigned = 1;
                } else {
                    column.kind = PreparedResultColumn::KIND_INT64;
                    bind.buffer_type = MYSQL_TYPE_LONGLONG;
                    bind.buffer = &column.signedValue;
                    bind.is_unsigned = 0;
                }
                break;

            case MYSQL_TYPE_FLOAT:
            case MYSQL_TYPE_DOUBLE:
                column.kind = PreparedResultColumn::KIND_DOUBLE;
                bind.buffer_type = MYSQL_TYPE_DOUBLE;
                bind.buffer = &column.doubleValue;
                break;

            case MYSQL_TYPE_DATE:
            case MYSQL_TYPE_NEWDATE:
            case MYSQL_TYPE_TIME:
            case MYSQL_TYPE_DATETIME:
            case MYSQL_TYPE_TIMESTAMP:
                column.kind = PreparedResultColumn::KIND_TIME;
                bind.buffer_type = pFields[i].type;
                bind.buffer = &column.timeValue;
                break;

            default: {
                column.kind = PreparedResultColumn::KIND_STRING;
                unsigned long bufferLength = pFields[i].max_length + 1;
                if (bufferLength < 2) {
                    bufferLength = 256;
                }
                column.stringValue.resize(bufferLength);
                bind.buffer_type = MYSQL_TYPE_STRING;
                bind.buffer = column.stringValue.data();
                bind.buffer_length = column.stringValue.size();
                break;
            }
            }
        }

        if (fieldCount > 0 && mysql_stmt_bind_result(m_pStmt, resultBinds.data()) != 0) {
            mysql_free_result(pMetadata);
            throw SQLQueryException(std::string("mysql_stmt_bind_result failed: ") + mysql_stmt_error(m_pStmt)
                                    + " [sql=" + m_Statement + "]");
        }

        std::vector<std::vector<Result::FieldValue> > rows;
        while (true) {
            int fetchResult = mysql_stmt_fetch(m_pStmt);
            if (fetchResult == MYSQL_NO_DATA) {
                break;
            }
            if (fetchResult == 1) {
                mysql_free_result(pMetadata);
                throw SQLQueryException(std::string("mysql_stmt_fetch failed: ") + mysql_stmt_error(m_pStmt)
                                        + " [sql=" + m_Statement + "]");
            }

            std::vector<Result::FieldValue> row;
            row.reserve(fieldCount);
            for (uint i = 0; i < fieldCount; ++i) {
                row.push_back(materializePreparedColumn(m_pStmt, i, columns[i]));
            }
            rows.push_back(row);
        }

        mysql_free_result(pMetadata);
        mysql_stmt_free_result(m_pStmt);

        m_nAffectedRows = rows.size();
        m_nInsertID = 0;
        m_pResult = new Result(rows, m_Statement);
        return m_pResult;
    }

    m_nAffectedRows = (uint)mysql_stmt_affected_rows(m_pStmt);
    m_nInsertID     = (uint)mysql_stmt_insert_id(m_pStmt);

    return NULL;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
// getError()
//////////////////////////////////////////////////////////////////////

std::string PreparedStatement::getError() const {
    if (m_pStmt == NULL) {
        return "PreparedStatement: no active MYSQL_STMT*";
    }
    return mysql_stmt_error(m_pStmt);
}
