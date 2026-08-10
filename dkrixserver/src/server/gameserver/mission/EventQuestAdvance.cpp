
#include "EventQuestAdvance.h"

#include <algorithm>

#include "DB.h"
#include "PlayerCreature.h"
#include "PreparedStatement.h"

void EventQuestAdvance::save(const string& name) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement updateStatusStmt(pConn,
                                           "UPDATE EventQuestAdvance SET Status=? WHERE OwnerID=? AND QuestLevel=?");
        updateStatusStmt.bindUInt(1, (uint)getStatus());
        updateStatusStmt.bindString(2, name);
        updateStatusStmt.bindUInt(3, (uint)getLevel());
        updateStatusStmt.execute();

        if (updateStatusStmt.getAffectedRowCount() == 0) {
            PreparedStatement insertAdvanceStmt(
                pConn, "INSERT IGNORE INTO EventQuestAdvance (QuestLevel, OwnerID, Status) VALUES (?, ?, ?)");
            insertAdvanceStmt.bindUInt(1, (uint)getLevel());
            insertAdvanceStmt.bindString(2, name);
            insertAdvanceStmt.bindUInt(3, (uint)getStatus());
            insertAdvanceStmt.execute();
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

EventQuestAdvanceManager::EventQuestAdvanceManager(PlayerCreature* pPC) {
    // This was reserve() followed by clear() and then five writes through
    // operator[]. reserve() sets capacity, not size, so the vector was left
    // empty and every one of those writes -- and every m_Advances[i] in this
    // file thereafter -- indexed past size() into the reserved-but-unconstructed
    // buffer. It only appeared to work because libstdc++'s operator[] is
    // unchecked and the writes stayed inside the reserved block. The visible
    // consequence was in clear(): for_each(begin(), end(), ...) spans nothing on
    // an empty vector, so every EventQuestAdvance ever allocated was leaked.
    //
    // resize() gives the vector the five NULL elements the rest of the class
    // already assumes it has.
    m_Advances.resize(EVENT_QUEST_LEVEL_MAX, NULL);

    m_pOwner = pPC;
}

EventQuestAdvanceManager::~EventQuestAdvanceManager() {
    clear();
}

// The old body was for_each(begin(), end(), SafeDelete<EventQuestAdvance>)
// followed by m_Advances.clear(). Two problems, both now fixed by the ctor's
// resize() plus this loop:
//   - SafeDelete took its pointer by value, so SAFE_DELETE nulled a copy and
//     left the vector holding dangling pointers;
//   - clear() dropped the size back to zero, so load() -- which calls this
//     first -- then wrote m_Advances[qLevel] into an empty vector again.
// The slot count is fixed at EVENT_QUEST_LEVEL_MAX for the object's lifetime;
// clearing means emptying the slots, not the vector.
void EventQuestAdvanceManager::clear() {
    for (size_t i = 0; i < m_Advances.size(); ++i)
        SAFE_DELETE(m_Advances[i]);
}

// Every public entry point below took an unvalidated level and reached
// m_Advances[questLevel] with nothing but an Assert in front of it. At least one
// of them is fed straight off the wire -- CGLotterySelectHandler passes
// pPacket->getQuestLevel() to getStatus() and rewarded() -- and load() takes its
// index straight out of the EventQuestAdvance table. Each check below is a real
// `if` in front of the surviving Assert, so Debug and Release take the same
// branch, and each returns the value the function already produces for a level
// with nothing recorded against it. No in-range level (0..4) changes behaviour.
static inline bool isValidQuestLevel(int questLevel) {
    return questLevel >= 0 && questLevel < EventQuestAdvanceManager::EVENT_QUEST_LEVEL_MAX;
}

bool EventQuestAdvanceManager::start(int questLevel) {
    if (!isValidQuestLevel(questLevel))
        return false;

    Assert(questLevel >= 0);
    Assert(questLevel < EVENT_QUEST_LEVEL_MAX);
    if (m_Advances[questLevel] == NULL)
        m_Advances[questLevel] = new EventQuestAdvance(questLevel);
    return m_Advances[questLevel]->start();
}

bool EventQuestAdvanceManager::success(int questLevel) {
    if (!isValidQuestLevel(questLevel))
        return false;

    Assert(questLevel >= 0);
    Assert(questLevel < EVENT_QUEST_LEVEL_MAX);
    if (m_Advances[questLevel] != NULL)
        return m_Advances[questLevel]->success();
    return false;
}

bool EventQuestAdvanceManager::rewarded(int questLevel) {
    if (!isValidQuestLevel(questLevel))
        return false;

    Assert(questLevel >= 0);
    Assert(questLevel < EVENT_QUEST_LEVEL_MAX);
    /*	if ( m_Advances[questLevel] != NULL )
        {
            for ( int i=0; i<=questLevel; ++i ) m_Advances[i]->reset();
            return true;
        }*/

    for (int i = 0; i < EVENT_QUEST_LEVEL_MAX; ++i) {
        if (m_Advances[i] != NULL)
            m_Advances[i]->reset();
    }
    return true;
}

bool EventQuestAdvanceManager::advanced(int questLevel) {
    if (!isValidQuestLevel(questLevel))
        return false;

    Assert(questLevel >= 0);
    Assert(questLevel < EVENT_QUEST_LEVEL_MAX);
    if (m_Advances[questLevel] != NULL)
        return m_Advances[questLevel]->advanced();
    return false;
}

EventQuestAdvance::Status EventQuestAdvanceManager::getStatus(int questLevel) {
    // Not isValidQuestLevel(): a negative level is meaningful here. getQuestLevel()
    // returns -1 for "every level advanced" and callers depend on that mapping, so
    // only the upper bound is new. Out of range above the set answers the same as
    // an empty slot, matching the sentinel this function already returns below.
    if (questLevel >= EVENT_QUEST_LEVEL_MAX)
        return EventQuestAdvance::EVENT_QUEST_INIT;

    Assert(questLevel < EVENT_QUEST_LEVEL_MAX);

    if (questLevel < 0)
        return EventQuestAdvance::EVENT_QUEST_ADVANCED;
    if (m_Advances[questLevel] == NULL)
        return EventQuestAdvance::EVENT_QUEST_INIT;
    return m_Advances[questLevel]->getStatus();
}

void EventQuestAdvanceManager::save() {
    __BEGIN_TRY

    for (int i = 0; i < EVENT_QUEST_LEVEL_MAX; ++i) {
        if (m_Advances[i] != NULL)
            m_Advances[i]->save(m_pOwner->getName());
    }

    __END_CATCH
}

void EventQuestAdvanceManager::save(int questLevel) {
    __BEGIN_TRY

    if (!isValidQuestLevel(questLevel))
        return;

    Assert(questLevel >= 0);
    Assert(questLevel < EVENT_QUEST_LEVEL_MAX);
    if (m_Advances[questLevel] != NULL)
        m_Advances[questLevel]->save(m_pOwner->getName());

    __END_CATCH
}

void EventQuestAdvanceManager::load() {
    __BEGIN_TRY

    clear();

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectAdvancesStmt(pConn, "SELECT QuestLevel, Status FROM EventQuestAdvance WHERE OwnerID=?");
        selectAdvancesStmt.bindString(1, m_pOwner->getName());
        Result* pResult = selectAdvancesStmt.execute();

        while (pResult->next()) {
            int qLevel = pResult->getInt(1);
            EventQuestAdvance::Status status = (EventQuestAdvance::Status)pResult->getInt(2);

            // QuestLevel is a plain int column with no CHECK constraint, and this
            // index had no guard at all -- not even an Assert. A single bad row
            // was an arbitrary heap write. The shipped EventQuestAdvance table is
            // empty [measured 2026-08-11, initdb/DARKEDEN.sql and
            // backup_darkeden_after_english_20260424.sql: 0 rows], so nothing in
            // the current data reaches this, but the rows are written at runtime
            // by EventQuestAdvance::save(). Skip a row we cannot store rather
            // than failing the owner's whole quest load.
            if (!isValidQuestLevel(qLevel)) {
                filelog("EventQuest.log", "EventQuestAdvanceManager::load : skipping out-of-range QuestLevel %d for %s",
                        qLevel, m_pOwner->getName().c_str());
                continue;
            }

            SAFE_DELETE(m_Advances[qLevel]);
            m_Advances[qLevel] = new EventQuestAdvance(qLevel, status);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

bool EventQuestAdvanceManager::canExecute(int questLevel) {
    Assert(questLevel >= 0);
    Assert(questLevel < EVENT_QUEST_LEVEL_MAX);

    EventQuestAdvance::Status stat = getStatus(questLevel);

    if (stat == EventQuestAdvance::EVENT_QUEST_ADVANCED || stat == EventQuestAdvance::EVENT_QUEST_REWARDED)
        return false;

    for (int i = 0; i < questLevel; ++i) {
        if (getStatus(i) != EventQuestAdvance::EVENT_QUEST_ADVANCED)
            return false;
    }

    for (int i = questLevel + 1; i < EVENT_QUEST_LEVEL_MAX; ++i) {
        if (getStatus(i) != EventQuestAdvance::EVENT_QUEST_INIT)
            return false;
    }

    return true;
}

int EventQuestAdvanceManager::getQuestLevel() {
    for (int i = 0; i < EVENT_QUEST_LEVEL_MAX; ++i) {
        EventQuestAdvance::Status stat = getStatus(i);
        if (stat != EventQuestAdvance::EVENT_QUEST_ADVANCED) {
            if (stat == EventQuestAdvance::EVENT_QUEST_REWARDED)
                return 0;
            return i;
        }
    }

    return -1;
}
