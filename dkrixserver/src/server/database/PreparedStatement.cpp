//////////////////////////////////////////////////////////////////////
//
// File Name    : PreparedStatement.cpp
// Written by   : Phase 11B (2026-04-19)
// Description  : Parameterised-query wrapper for injection remediation.
//
// See PreparedStatement.h for call-site shape and scope notes.
//
//////////////////////////////////////////////////////////////////////

#include "PreparedStatement.h"

#include <cstdio>
#include <ctime>

#include "Assert.h"
#include "Exception.h"
#include "Result.h"

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////

PreparedStatement::PreparedStatement(Connection* pConnection, const std::string& sqlWithQMarks)
    : m_pConnection(pConnection),
      m_pStmt(NULL),
      m_Statement(sqlWithQMarks),
      m_nParamCount(0),
      m_nAffectedRows(0),
      m_nInsertID(0) {
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

    m_nParamCount = (uint)mysql_stmt_param_count(m_pStmt);
    m_Params.resize(m_nParamCount);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////

PreparedStatement::~PreparedStatement() {
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
// Parameter binding — stores into m_Params; actual MYSQL_BIND assembly
// happens in execute() so we can bind the full array at once.
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
    // Copy into owned storage so the caller can pass a temporary.
    p.s = v;
    p.length = (unsigned long)p.s.size();
    p.isNull = 0;
}

void PreparedStatement::bindTime(uint idx, time_t t) {
    checkIdx(idx);
    Param& p = m_Params[idx - 1];
    p.type = Param::PARAM_TIME;

    // Break time_t into a MYSQL_TIME struct — the server will render
    // back to DATETIME/TIMESTAMP/DATE per the destination column type.
    struct tm localtm;
    // localtime_r is POSIX and present on Linux + macOS.
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
//
// Builds the MYSQL_BIND array from m_Params, binds, executes. Returns
// NULL for write queries (caller reads affected rows / insert id via
// the getters) and for SELECT (see scope note in the header — full
// Result* integration for SELECT is deferred to the first call-site
// migration that needs it).
//////////////////////////////////////////////////////////////////////

Result* PreparedStatement::execute() {
    __BEGIN_TRY

    Assert(m_pStmt != NULL);

    // Sanity: every slot must have been bound before execute().
    for (uint i = 0; i < m_nParamCount; ++i) {
        if (m_Params[i].type == Param::PARAM_UNSET) {
            char msg[256];
            std::snprintf(msg, sizeof(msg),
                          "PreparedStatement::execute: parameter %u unbound for stmt [%s]",
                          i + 1, m_Statement.c_str());
            throw SQLException(std::string(msg));
        }
    }

    // Assemble MYSQL_BIND array per the bound param types.
    std::vector<MYSQL_BIND> binds(m_nParamCount);
    std::memset(binds.data(), 0, sizeof(MYSQL_BIND) * m_nParamCount);

    for (uint i = 0; i < m_nParamCount; ++i) {
        Param& p = m_Params[i];
        MYSQL_BIND& b = binds[i];

        b.is_null = &p.isNull;

        switch (p.type) {
        case Param::PARAM_INT:
            b.buffer_type   = MYSQL_TYPE_LONG;
            b.buffer        = &p.i;
            b.is_unsigned   = 0;
            break;
        case Param::PARAM_UINT:
            b.buffer_type   = MYSQL_TYPE_LONG;
            b.buffer        = &p.u;
            b.is_unsigned   = 1;
            break;
        case Param::PARAM_LONG:
            b.buffer_type   = MYSQL_TYPE_LONGLONG;
            b.buffer        = &p.ll;
            b.is_unsigned   = 0;
            break;
        case Param::PARAM_ULONG:
            b.buffer_type   = MYSQL_TYPE_LONGLONG;
            b.buffer        = &p.ull;
            b.is_unsigned   = 1;
            break;
        case Param::PARAM_DOUBLE:
            b.buffer_type   = MYSQL_TYPE_DOUBLE;
            b.buffer        = &p.d;
            break;
        case Param::PARAM_STRING:
            b.buffer_type   = MYSQL_TYPE_STRING;
            // MYSQL_BIND.buffer is non-const void*; we do not mutate
            // it, but the libmysqlclient API is not const-correct.
            b.buffer        = const_cast<char*>(p.s.data());
            b.buffer_length = p.length;
            b.length        = &p.length;
            break;
        case Param::PARAM_TIME:
            b.buffer_type   = MYSQL_TYPE_DATETIME;
            b.buffer        = &p.mt;
            break;
        case Param::PARAM_NULL:
            b.buffer_type   = MYSQL_TYPE_NULL;
            break;
        case Param::PARAM_UNSET:
            // Already-handled above.
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

    // For write queries, populate affected-rows + insert-id and
    // return NULL. For SELECT, the MYSQL_STMT* has pending rows
    // that need to be fetched via mysql_stmt_bind_result +
    // mysql_stmt_fetch — that plumbing lands with the first
    // SELECT migration (see PreparedStatement.h scope note).
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
