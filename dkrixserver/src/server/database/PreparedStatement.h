//////////////////////////////////////////////////////////////////////
//
// File Name    : PreparedStatement.h
// Written by   : Phase 11B (2026-04-19)
// Description  : Parameterised-query wrapper for injection remediation.
//
// Replacement target for the `Statement(fmt, ...)` + `executeQuery(
// fmt, ...)` sprintf-style construction path which is the source of
// the ~567 injection-shaped call sites tracked by the Phase 8C CI
// ratchet.
//
// Call-site shape (planned usage, once migrations begin in the
// Phase 11.2 follow-up work):
//
//     PreparedStatement ps(pConnection,
//         "INSERT INTO PlayerMessages (FromID, ToID, Msg, Sent) "
//         "VALUES (?, ?, ?, ?)");
//     ps.bindInt   (1, senderID);
//     ps.bindInt   (2, recipientID);
//     ps.bindString(3, userSuppliedMessage);   // <-- never concatenated
//     ps.bindTime  (4, ::time(NULL));
//     ps.execute();
//
// Parameters are 1-indexed to match the positional `?` marker in
// the SQL (same convention as JDBC and libmysqlclient itself).
// bindString() safely handles embedded quotes/backslashes/NULs —
// the whole point is that user-supplied data never makes it into
// the SQL string through concatenation.
//
// ---------------------------------------------------------------
// Scope of this first cut (11.1 landed, 11.2 deferred):
// ---------------------------------------------------------------
// execute() on INSERT / UPDATE / DELETE / DDL is fully supported —
// affected-row count + last-insert-id are exposed via
// getAffectedRowCount() and getInsertID().
//
// execute() on SELECT currently returns NULL. Full Result*
// integration for SELECT requires extending Result to hold rows
// fetched via `mysql_stmt_bind_result` + `mysql_stmt_fetch`
// (MYSQL_RES vs stmt-mode are different row-iteration APIs in
// libmysqlclient). That extension lands in the first call-site
// migration PR that needs to convert a SELECT — not ahead of it,
// to avoid shipping 100 lines of unused Result-mode state in
// Phase 11.1.
//
// If you hit a SELECT site and need it now, throw a
// NotYetImplementedException from execute() so the call site
// fails loudly in CI rather than silently returning NULL.
//
//////////////////////////////////////////////////////////////////////

#ifndef __PREPARED_STATEMENT_H__
#define __PREPARED_STATEMENT_H__

// include files
#include <mysql/mysql.h>

#include <cstring>
#include <ctime>
#include <string>
#include <vector>

#include "Connection.h"
#include "Exception.h"
#include "Types.h"

// my_bool was the historical MYSQL_BIND.is_null element type.
// MySQL 8.0+ C connector replaced it with plain `bool`; libmysqlclient-
// dev on Ubuntu ships one or the other depending on version. Alias to
// bool to keep the MYSQL_BIND struct members consistent either way.
#include <type_traits>
// Derive my_bool_t from the installed library's own
// MYSQL_BIND::is_null declaration. On MySQL 8.0+ this is
// `bool`; on MySQL 5.7 / MariaDB it is `my_bool` (a.k.a.
// `char`). This keeps PreparedStatement.cpp's
// `b.is_null = &p.isNull` assignment type-correct on both.
#ifndef MY_BOOL_DEFINED
typedef std::remove_pointer<decltype(MYSQL_BIND::is_null)>::type my_bool_t;
#define MY_BOOL_DEFINED
#endif

// forward declaration
class Result;

//////////////////////////////////////////////////////////////////////
//
// class PreparedStatement
//
// Server-side parameterised-query wrapper built on libmysqlclient's
// mysql_stmt_* API (not a client-side escape-and-substitute shim).
// Binding is type-aware: each bind* sets up a MYSQL_BIND slot with
// the right buffer_type so the server handles quoting, encoding,
// and type coercion.
//
//////////////////////////////////////////////////////////////////////

class PreparedStatement {
public:
    // Construct against a live Connection. The Connection must stay
    // valid for the lifetime of this PreparedStatement — same
    // ownership rules as the existing Statement class.
    //
    // `sqlWithQMarks` must contain exactly one `?` per bind slot in
    // the positional order the caller will bind them.
    //
    // Throws SQLException on mysql_stmt_init or mysql_stmt_prepare
    // failure.
    PreparedStatement(Connection* pConnection, const std::string& sqlWithQMarks);

    // Destructor closes the MYSQL_STMT* and frees any row-buffer
    // allocations owned by this object.
    ~PreparedStatement();

    // ---- Parameter binding -------------------------------------
    //
    // All bind*() methods are 1-indexed (idx = 1 for the first `?`).
    // Calling bind*() on an idx out of [1..paramCount] throws
    // OutOfBoundException.
    //
    // The bound value is COPIED into per-parameter storage owned by
    // this PreparedStatement, so callers don't need to keep the
    // input alive past the bind*() call — same convenience as
    // sprintf, without the injection vulnerability.

    void bindInt    (uint idx, int v);
    void bindUInt   (uint idx, uint v);
    void bindLong   (uint idx, long long v);
    void bindULong  (uint idx, unsigned long long v);
    void bindDouble (uint idx, double v);
    void bindString (uint idx, const std::string& v);
    void bindTime   (uint idx, time_t t);
    void bindNull   (uint idx);

    // ---- Execution ---------------------------------------------
    //
    // Binds the parameter buffers to the MYSQL_STMT*, then executes.
    // For INSERT / UPDATE / DELETE / DDL, returns NULL; callers read
    // affected-rows / insert-id via the getters below.
    //
    // For SELECT in the 11.1 scope, returns NULL as well — see
    // header top-of-file for rationale. A follow-up PR will wire
    // stmt-mode Result* construction when the first SELECT
    // migration needs it.
    //
    // Throws SQLQueryException on any mysql_stmt_* error.
    Result* execute();

    // ---- Diagnostics -------------------------------------------

    // Human-readable last error from the driver (safe to call on
    // any PreparedStatement, regardless of state).
    std::string getError() const;

    // Rows affected by the last execute() of a write query. Undefined
    // before execute() has been called successfully.
    uint getAffectedRowCount() const {
        return m_nAffectedRows;
    }

    // Auto-increment id generated by the last execute() of an INSERT.
    // Undefined if the last query didn't INSERT into an
    // AUTO_INCREMENT column.
    uint getInsertID() const {
        return m_nInsertID;
    }

    // Number of `?` placeholders the prepared SQL was parsed into
    // — useful for sanity-checking call-site bind patterns.
    uint getParamCount() const {
        return m_nParamCount;
    }

private:
    // One parameter slot. Owns its buffer via std::vector<char> /
    // std::string so bind*() can be called with temporaries.
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
        Type                type;
        // Scalar storage — which field is valid depends on `type`.
        int                 i;
        uint                u;
        long long           ll;
        unsigned long long  ull;
        double              d;
        std::string         s;
        // MYSQL_TIME storage for PARAM_TIME.
        MYSQL_TIME          mt;
        // The actual length of the string data for PARAM_STRING
        // — needs its own backing variable for MYSQL_BIND.length.
        unsigned long       length;
        // Null-flag for PARAM_NULL — `bool` on MySQL 8+ C connector,
        // `char`-backed my_bool on older headers; aliased to my_bool_t
        // above.
        my_bool_t           isNull;

        Param()
            : type(PARAM_UNSET), i(0), u(0), ll(0), ull(0), d(0.0),
              length(0), isNull(0)
        {
            std::memset(&mt, 0, sizeof(mt));
        }
    };

    // 1-indexed bound-check for idx parameter (throws on out-of-range).
    void checkIdx(uint idx) const;

    Connection*         m_pConnection;
    MYSQL_STMT*         m_pStmt;
    std::string         m_Statement;
    std::vector<Param>  m_Params;
    uint                m_nParamCount;
    uint                m_nAffectedRows;
    uint                m_nInsertID;
};

#endif // __PREPARED_STATEMENT_H__
