#ifndef __SOMETHING_GROWING_UP_H__
#define __SOMETHING_GROWING_UP_H__

#include <cstdio>
#include <string>
#include <vector>

#include "Assert.h"
#include "DB.h"

template <typename _GoalExpType, typename _LevelType, _LevelType _MinLevel, _LevelType _MaxLevel,
          typename _TotalExpType = _GoalExpType>
class ExpTable {
public:
    typedef _GoalExpType GoalExpType;
    typedef _LevelType LevelType;
    typedef _TotalExpType TotalExpType;

    static const LevelType MaxLevel = _MaxLevel;
    static const LevelType MinLevel = _MinLevel;

    // Levels run MinLevel..MaxLevel *inclusive* (MinLevel is 1 for most tables
    // but 0 for AdvancementClassExpTable), and every access
    // indexes m_Records[level] directly (load() below, plus getGoalExp/
    // getAccumExp). Sizing to MaxLevel leaves the last level out of bounds:
    // AddressSanitizer reports a 4-byte heap-buffer-overflow WRITE in load()
    // for level == MaxLevel. Size to MaxLevel + 1 so index MaxLevel is valid;
    // slot 0 is unused, which is what the 1-based indexing already assumes.
    ExpTable() : m_Records(MaxLevel + 1) {}
    virtual ~ExpTable() {}


    virtual void load();
    virtual const string getDBTableName() const = 0;
    virtual const string getDBGoalExpFieldName() const {
        return "GoalExp";
    }
    virtual const string getDBAccumExpFieldName() const {
        return "AccumExp";
    }
    virtual const string getDBLevelFieldName() const {
        return "Level";
    }
    virtual const string getDBQueryCondition() const {
        return "";
    }


    GoalExpType getGoalExp(LevelType level) const {
        Assert(level <= MaxLevel && level >= MinLevel);
        return m_Records[level].m_GoalExp;
    }
    TotalExpType getAccumExp(LevelType level) const {
        Assert(level <= MaxLevel && level >= MinLevel);
        return m_Records[level].m_AccumExp;
    }

private:
    class TableRecord {
    public:
        GoalExpType m_GoalExp;
        TotalExpType m_AccumExp;
    };

    vector<TableRecord> m_Records;
};


template <typename GoalExpType, typename LevelType, LevelType MinLevel, LevelType MaxLevel, typename TotalExpType>
void ExpTable<GoalExpType, LevelType, MinLevel, MaxLevel, TotalExpType>::load() {
    __BEGIN_TRY

    const string QueryTemplate = "SELECT %s, %s, %s FROM %s %s";


    int size = QueryTemplate.size() + getDBTableName().size() + getDBGoalExpFieldName().size() +
               getDBAccumExpFieldName().size() + getDBLevelFieldName().size() + getDBQueryCondition().size();

    // Was new char[size] freed by a SAFE_DELETE_ARRAY placed *after* END_DB, so
    // the buffer leaked on every path END_DB rethrows on. A vector frees on both
    // paths and removes the manual delete entirely.
    vector<char> query(size);

    snprintf(&query[0], size, QueryTemplate.c_str(), getDBLevelFieldName().c_str(), getDBGoalExpFieldName().c_str(),
             getDBAccumExpFieldName().c_str(), getDBTableName().c_str(), getDBQueryCondition().c_str());

    // Was uninitialised: END_DB below does delete STMT on the catch path, which
    // createStatement() itself can reach, so a throw there deleted an
    // indeterminate pointer.
    Statement* pStmt = NULL;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        // executeQuery() is printf-style variadic (database/Statement.h:44), so
        // handing it a runtime-built buffer makes that buffer the *format
        // string*: a '%' arriving from a table or field name would be read as a
        // conversion specifier against no argument. The SQL is already fully
        // expanded by the snprintf above, so use the non-format overload.
        Result* pResult = pStmt->executeQueryString(&query[0]);

        while (pResult->next()) {
            LevelType level = pResult->getInt(1);
            Assert(level >= MinLevel);
            Assert(level <= MaxLevel);

            // Real check as well as the Asserts: this indexes a heap vector with
            // a value that comes from the database, and Assert vanishes under
            // NDEBUG. Widened to long so the comparison is not tautological when
            // LevelType is unsigned and MinLevel is 0. Debug behaviour is
            // unchanged -- the Asserts above still throw first.
            const long checkedLevel = static_cast<long>(level);
            if (checkedLevel < static_cast<long>(MinLevel) || checkedLevel > static_cast<long>(MaxLevel))
                continue;

            m_Records[level].m_GoalExp = pResult->getInt(2);
            m_Records[level].m_AccumExp = pResult->getInt(3);
        }

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt);

    __END_CATCH
}

template <class ExpTableClass> class SomethingGrowingUp {
public:
    typedef typename ExpTableClass::GoalExpType GoalExpType;
    typedef typename ExpTableClass::LevelType LevelType;
    typedef typename ExpTableClass::TotalExpType TotalExpType;

    static const LevelType MaxLevel = ExpTableClass::MaxLevel;
    static const LevelType MinLevel = ExpTableClass::MinLevel;

    SomethingGrowingUp(LevelType level, GoalExpType goalExp, const ExpTableClass& ExpTable)
        : m_Level(level), m_GoalExp(goalExp), m_ExpTable(ExpTable) {}

    LevelType getLevel() const {
        return m_Level;
    }
    GoalExpType getGoalExp() const {
        return m_GoalExp;
    }
    TotalExpType getTotalExp() const {
        return m_ExpTable.getAccumExp(m_Level) - m_GoalExp;
    }

    bool increaseExp(GoalExpType exp, bool canLevelUp = true, bool giveRemainExp = false);
    bool levelUp();
    bool levelDown();


    bool SET_LEVEL(LevelType level);

private:
    void setLevel(LevelType level);

    LevelType m_Level;
    GoalExpType m_GoalExp;
    const ExpTableClass& m_ExpTable;
};

template <class ExpTableClass>
bool SomethingGrowingUp<ExpTableClass>::increaseExp(GoalExpType exp, bool canLevelUp, bool giveRemainExp) {
    if (m_GoalExp <= exp) {
        if (canLevelUp) {
            GoalExpType remainExp = exp - m_GoalExp;
            if (levelUp()) {
                if (giveRemainExp)
                    increaseExp(remainExp, true, true);
                return true;
            }
        } else
            exp = m_GoalExp;
    }

    m_GoalExp -= exp;
    return false;
}

template <class ExpTableClass> bool SomethingGrowingUp<ExpTableClass>::SET_LEVEL(LevelType level) {
    if (level < MinLevel || level > MaxLevel)
        return false;
    setLevel(level);
    return true;
}

template <class ExpTableClass> void SomethingGrowingUp<ExpTableClass>::setLevel(LevelType level) {
    m_Level = level;
    m_GoalExp = m_ExpTable.getGoalExp(level);
}

template <class ExpTableClass> bool SomethingGrowingUp<ExpTableClass>::levelUp() {
    if (m_Level == MaxLevel)
        return false;
    setLevel(m_Level + 1);
    return true;
}

template <class ExpTableClass> bool SomethingGrowingUp<ExpTableClass>::levelDown() {
    if (m_Level == MinLevel)
        return false;
    setLevel(m_Level - 1);
    return true;
}

#endif
