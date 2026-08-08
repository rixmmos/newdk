#include "TimeLimitItemManager.h"

#include "DB.h"
#include "GCTimeLimitItemInfo.h"
#include "Item.h"
#include "PlayerCreature.h"
#include "PreparedStatement.h"

TimeLimitItemManager::~TimeLimitItemManager() {
    TableRecordList::iterator itr = m_TableRecords.begin();

    for (; itr != m_TableRecords.end(); ++itr) {
        SAFE_DELETE(*itr);
    }

    m_TableRecords.clear();
    m_ItemTimeLimits.clear();
    m_loaded = false;
}

void TimeLimitItemManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement selectTimeLimitStmt(
            pConn, "SELECT ItemClass, ItemID, LimitDateTime from TimeLimitItems where OwnerID=? and Status=?");
        selectTimeLimitStmt.bindString(1, m_pOwnerPC->getName());
        selectTimeLimitStmt.bindUInt(2, (uint)VALID);
        Result* pResult = selectTimeLimitStmt.execute();

        while (pResult->next()) {
            TableRecord* pTableRecord = new TableRecord;

            pTableRecord->m_ItemClass = (Item::ItemClass)pResult->getInt(1);
            pTableRecord->m_ItemID = pResult->getInt(2);

            const string limitDateTime = pResult->getString(3);
            pTableRecord->m_TimeLimit = VSDateTime(limitDateTime);

            m_TableRecords.push_back(pTableRecord);
        }
    }
    END_DB(pStmt)

    m_loaded = true;

    __END_CATCH
}


bool TimeLimitItemManager::registerItem(Item* pItem)

{
    __BEGIN_TRY

    if (m_ItemTimeLimits.find(pItem->getObjectID()) != m_ItemTimeLimits.end())
        return false;

    TableRecordList::iterator itr = m_TableRecords.begin();
    TableRecordList::iterator endItr = m_TableRecords.end();

    for (; itr != endItr; ++itr) {
        TableRecord* pTableRecord = *itr;
        if (pTableRecord != NULL) {
            if (pTableRecord->m_ItemClass == pItem->getItemClass() && pTableRecord->m_ItemID == pItem->getItemID()) {
                m_ItemTimeLimits[pItem->getObjectID()] = pTableRecord->m_TimeLimit;
                pItem->setTimeLimitItem();
                return true;
            }
        }
    }

    return false;

    __END_CATCH
}

bool TimeLimitItemManager::checkTimeLimit(Item* pItem)

{
    __BEGIN_TRY

    Assert(m_loaded);

    ObjectID_t objectID = pItem->getObjectID();

    ItemTimeLimitMap::iterator itr = m_ItemTimeLimits.find(objectID);

    if (itr == m_ItemTimeLimits.end()) {
        if (!registerItem(pItem)) {
            return true;
        }
    }

    VSDateTime currentTime = VSDateTime::currentDateTime();
    if (currentTime > m_ItemTimeLimits[objectID]) {
        cout << pItem->toString() << "   : " << currentTime.toString() << " > " << m_ItemTimeLimits[objectID].toString()
             << endl;

        return false;
    }

    return true;

    __END_CATCH
}

bool TimeLimitItemManager::wasteIfTimeOver(Item* pItem)

{
    __BEGIN_TRY

    if (checkTimeLimit(pItem))
        return false;

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement updateStatusStmt(
            pConn, "UPDATE TimeLimitItems SET Status=? where OwnerID=? and ItemClass=? and ItemID=?");
        updateStatusStmt.bindUInt(1, (uint)EXPIRED);
        updateStatusStmt.bindString(2, m_pOwnerPC->getName());
        updateStatusStmt.bindUInt(3, (uint)pItem->getItemClass());
        updateStatusStmt.bindUInt(4, (uint)pItem->getItemID());
        updateStatusStmt.execute();

        if (updateStatusStmt.getAffectedRowCount() == 0) {
            return false;
        }
    }
    END_DB(pStmt)

    return true;

    __END_CATCH
}

bool TimeLimitItemManager::makeTimeLimitItemInfo(GCTimeLimitItemInfo& gcTLII) const

{
    __BEGIN_TRY

    Assert(m_loaded);

    if (m_ItemTimeLimits.empty())
        return false;

    //	gcTLII.clear();

    VSDateTime currentTime = VSDateTime::currentDateTime();
    ItemTimeLimitMap::const_iterator itr = m_ItemTimeLimits.begin();
    ItemTimeLimitMap::const_iterator endItr = m_ItemTimeLimits.end();

    for (; itr != endItr; ++itr) {
        int diffSecs = currentTime.secsTo(itr->second);

        if (diffSecs < 0)
            diffSecs = 0;

        gcTLII.addTimeLimit(itr->first, diffSecs);
    }

    return true;

    __END_CATCH
}

bool TimeLimitItemManager::updateItemTimeLimit(Item* pItem, DWORD time) {
    __BEGIN_TRY

    if (pItem->getCreateType() != Item::CREATE_TYPE_TIME_EXTENSION)
        return false;
    if (!changeStatus(pItem, EXTENDED))
        return false;
    addTimeLimitItem(pItem, time);
    return true;

    __END_CATCH
}


void TimeLimitItemManager::addTimeLimitItem(Item* pItem, DWORD time)

{
    __BEGIN_TRY

    Assert(pItem != NULL);
    Assert(time != 0);

    VSDateTime timeLimit = VSDateTime::currentDateTime().addSecs(time);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement insertTimeLimitStmt(
            pConn, "INSERT INTO TimeLimitItems (OwnerID, ItemClass, ItemID, LimitDateTime) VALUES (?,?,?,?)");
        insertTimeLimitStmt.bindString(1, m_pOwnerPC->getName());
        insertTimeLimitStmt.bindUInt(2, (uint)pItem->getItemClass());
        insertTimeLimitStmt.bindUInt(3, (uint)pItem->getItemID());
        insertTimeLimitStmt.bindString(4, timeLimit.toDateTime());
        insertTimeLimitStmt.execute();
    }
    END_DB(pStmt)

    TableRecord* pTableRecord = new TableRecord;

    pTableRecord->m_ItemClass = pItem->getItemClass();
    pTableRecord->m_ItemID = pItem->getItemID();
    pTableRecord->m_TimeLimit = timeLimit;

    m_TableRecords.push_back(pTableRecord);

    registerItem(pItem);

    __END_CATCH
}


bool TimeLimitItemManager::changeStatus(Item* pItem, TimeLimitStatus status) {
    __BEGIN_TRY

    Assert(status != VALID);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement updateStatusStmt(
            pConn, "UPDATE TimeLimitItems SET Status=? where OwnerID=? and ItemClass=? and ItemID=?");
        updateStatusStmt.bindUInt(1, (uint)status);
        updateStatusStmt.bindString(2, m_pOwnerPC->getName());
        updateStatusStmt.bindUInt(3, (uint)pItem->getItemClass());
        updateStatusStmt.bindUInt(4, (uint)pItem->getItemID());
        updateStatusStmt.execute();

        if (updateStatusStmt.getAffectedRowCount() == 0) {
            return false;
        }
    }
    END_DB(pStmt)

    TableRecordList::iterator itr = m_TableRecords.begin();
    TableRecordList::iterator endItr = m_TableRecords.end();

    bool erased = false;

    for (; itr != endItr; ++itr) {
        if ((*itr)->m_ItemClass == pItem->getItemClass() && (*itr)->m_ItemID == pItem->getItemID()) {
            m_TableRecords.erase(itr);
            erased = true;
            break;
        }
    }

    if (!erased)
        filelog("QuestItem.log", "[%u,%u] :      .", (uint)pItem->getItemClass(), (uint)pItem->getItemID());


    ItemTimeLimitMap::iterator itr2 = m_ItemTimeLimits.find(pItem->getObjectID());

    if (itr2 != m_ItemTimeLimits.end()) {
        m_ItemTimeLimits.erase(itr2);
    } else {
        filelog("QuestItem.log", "[%u,%u] : Item Time Limit Map  .", (uint)pItem->getItemClass(),
                (uint)pItem->getItemID());
    }

    return true;

    __END_CATCH
}
