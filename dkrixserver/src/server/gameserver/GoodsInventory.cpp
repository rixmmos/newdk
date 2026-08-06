//////////////////////////////////////////////////////////////////////////
// Filename			: GoodsInventory.cpp
// Written By		: bezz

//////////////////////////////////////////////////////////////////////////

#include "GoodsInventory.h"

#include "DB.h"


GoodsInventory::~GoodsInventory()

{
    clear();
}

void GoodsInventory::clear()

{
    __BEGIN_TRY

    ListItemItr itr = m_Goods.begin();
    for (; itr != m_Goods.end(); itr++) {
        SAFE_DELETE((*itr).m_pItem);
    }

    m_Goods.clear();

    __END_CATCH
}

void GoodsInventory::addItem(string ID, Item* pItem)

{
    __BEGIN_TRY

    BuyItem buyItem;

    buyItem.m_ID = ID;
    buyItem.m_pItem = pItem;

    m_Goods.push_back(buyItem);

    __END_CATCH
}

Item* GoodsInventory::getItem(ObjectID_t oid)

{
    __BEGIN_TRY

    if (m_Goods.empty())
        return NULL;

    Item* pItem = NULL;

    ListItemItr itr = m_Goods.begin();

    for (; itr != m_Goods.end(); itr++) {
        if ((*itr).m_pItem->getObjectID() == oid) {
            pItem = (*itr).m_pItem;
            break;
        }
    }

    return pItem;

    __END_CATCH
}
Item* GoodsInventory::popItem(ObjectID_t oid)

{
    __BEGIN_TRY

    if (m_Goods.empty())
        return NULL;

    Item* pItem = NULL;

    ListItemItr itr = m_Goods.begin();

    for (; itr != m_Goods.end(); itr++) {
        if ((*itr).m_pItem->getObjectID() == oid) {
            pItem = (*itr).m_pItem;

            filelog("Goods.log", " . : [%s:%s]", (*itr).m_ID.c_str(),
                    (*itr).m_pItem->toString().c_str());

            Statement* pStmt = NULL;

            BEGIN_DB {
                pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
                Result* pResult = pStmt->executeQuery(
                    "UPDATE GoodsListObject SET Num = Num - 1, Status = IF( NUM < 1, 'GET', 'NOT' ) WHERE ID=%s",
                    (*itr).m_ID.c_str());

                

                if (pStmt->getAffectedRowCount() == 0) {
                    filelog("Goods.log", " DB  . : %s", (*itr).m_ID.c_str());
                }

                SAFE_DELETE(pStmt);
            }
            END_DB(pStmt)

            m_Goods.erase(itr);
            break;
        }
    }

    return pItem;

    __END_CATCH
}
