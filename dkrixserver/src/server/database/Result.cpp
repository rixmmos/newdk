//////////////////////////////////////////////////////////////////////////////
// File Name 	: Result.cc
// Written By	: Gday29@ewestsoft.com

//////////////////////////////////////////////////////////////////////////////

#include "Result.h"

#include "Assert.h"
#include "StringStream.h"

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////

Result::Result(T_RESULT* pResult, const string& statement) {
    __BEGIN_TRY

    m_BackendType = BACKEND_MYSQL_RES;
    m_pResult = pResult;
    m_pRow = NULL;
    m_CurrentRowIndex = -1;

    Assert(m_pResult != NULL);


    m_RowCount = mysql_num_rows(m_pResult);
    m_FieldCount = mysql_num_fields(m_pResult);

    m_Statement = statement;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// constructor for materialized (prepared-statement) results
//////////////////////////////////////////////////////////////////////////////

Result::Result(const std::vector<std::vector<FieldValue>>& rows, const string& statement) {
    __BEGIN_TRY

    m_BackendType = BACKEND_MATERIALIZED;
    m_pResult = NULL;
    m_pRow = NULL;
    m_CurrentRowIndex = -1;
    m_Rows = rows;
    m_RowCount = m_Rows.size();
    m_FieldCount = (m_RowCount == 0) ? 0 : m_Rows[0].size();
    m_Statement = statement;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////

Result::~Result() throw() {
    __BEGIN_TRY

    if (m_BackendType == BACKEND_MYSQL_RES && m_pResult != NULL) {
        // If the result set is empty, don't call mysql_free_result
        // otherwise when you call malloc later you might run into
        // some nasty debugging issues.
        // [Note: man malloc has some interesting information to track
        // the problem like system varibles such as MALLOC_CHCK_]
        /*
        if (m_RowCount > 0)
        {
            mysql_free_result(m_pResult);
            m_pResult = NULL;
        }
        */

        mysql_free_result(m_pResult);
        m_pResult = NULL;
    }

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// next()

//////////////////////////////////////////////////////////////////////////////

bool Result::next() {
    __BEGIN_TRY

    if (m_BackendType == BACKEND_MYSQL_RES) {
        Assert(m_pResult != NULL);
        return (m_pRow = mysql_fetch_row(m_pResult)) != NULL;
    }

    if (m_CurrentRowIndex + 1 >= (int)m_Rows.size()) {
        return false;
    }
    ++m_CurrentRowIndex;
    return true;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// getField


//////////////////////////////////////////////////////////////////////////////

char* Result::getField(uint index) {
    __BEGIN_TRY

    if (index == 0 || index > m_FieldCount) {
        StringStream msg;
        msg << "Result::getField() : Out of Bound! - Statement[" << m_Statement << "]";
        filelog("ResultBug.log", "%s", msg.toString().c_str());
        throw OutOfBoundException(msg.toString());
    }

    if (m_BackendType == BACKEND_MYSQL_RES) {
        if (m_pRow == NULL) {
            StringStream msg;
            msg << "Result::getField() : CALL Result::next() - Statement[" << m_Statement << "]";
            filelog("ResultBug.log", "%s", msg.toString().c_str());
            throw Error(msg.toString());
        }

        return m_pRow[index - 1];
    }

    if (m_CurrentRowIndex < 0 || m_CurrentRowIndex >= (int)m_Rows.size()) {
        StringStream msg;
        msg << "Result::getField() : CALL Result::next() - Statement[" << m_Statement << "]";
        throw Error(msg.toString());
    }

    const FieldValue& field = m_Rows[m_CurrentRowIndex][index - 1];
    if (field.isNull) {
        return NULL;
    }
    return const_cast<char*>(field.value.c_str());

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

const char* Result::getString(uint index) {
    __BEGIN_TRY

    static const char* pNULL = "";
    const char* pField = getField(index);

    if (pField == NULL) {
        return pNULL;
    }

    return pField;

    // if (getField(index) == NULL) return "";
    // return getField(index);

    __END_CATCH
}
