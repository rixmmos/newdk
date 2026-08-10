//////////////////////////////////////////////////////////////////////////////
// Filename    : UniqueItemManager.cpp
// Written By  : suigui
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "UniqueItemManager.h"

#include "DB.h"
#include "Item.h"
#include "ItemInfoManager.h"


UniqueItemManager* g_pUniqueItemManager = NULL;

//----------------------------------------------------------------------
// init
//----------------------------------------------------------------------
void UniqueItemManager::init()

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    Statement* pStmt = NULL;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        
        Result* pResult = pStmt->executeQuery("SELECT ItemClass, ItemType FROM UniqueItemInfo");

        
        while (pResult->next()) {
            Item::ItemClass itemClass = (Item::ItemClass)pResult->getInt(1);
            int itemType = pResult->getInt(2);

            ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo(itemClass, itemType);
            Assert(pItemInfo != NULL);

            pItemInfo->setUnique();
        }

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt);

    __END_CATCH
    __END_DEBUG
}

//----------------------------------------------------------------------
// is Possible Create
//----------------------------------------------------------------------
bool UniqueItemManager::isPossibleCreate(Item::ItemClass itemClass, ItemType_t itemType)

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    Statement* pStmt = NULL;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        
        Result* pResult = pStmt->executeQuery(
            "SELECT LimitNumber, CurrentNumber FROM UniqueItemInfo WHERE ItemClass=%d AND ItemType=%d", (int)itemClass,
            (int)itemType);

        if (pResult->next()) {
            int limitNumber = pResult->getInt(1);
            int currentNumber = pResult->getInt(2);
            bool bPossibleCreate = limitNumber == 0 || currentNumber < limitNumber;

            // Free before returning: this early return jumped over the
            // SAFE_DELETE below, leaking the Statement and the Result it owns on
            // every lookup that found a row.
            SAFE_DELETE(pStmt);

            return bPossibleCreate;
        }

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt);

    __END_CATCH
    __END_DEBUG

    return false;
}

//----------------------------------------------------------------------
// createItem
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void UniqueItemManager::createItem(Item::ItemClass itemClass, ItemType_t itemType)

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    Statement* pStmt = NULL;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        
        pStmt->executeQuery(
            "UPDATE UniqueItemInfo SET CurrentNumber=CurrentNumber+1 WHERE ItemClass=%d AND ItemType=%d",
            (int)itemClass, (int)itemType);

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt);

    __END_CATCH
    __END_DEBUG
}

//----------------------------------------------------------------------
// deleteItem
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void UniqueItemManager::deleteItem(Item::ItemClass itemClass, ItemType_t itemType)

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    Statement* pStmt = NULL;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        
        pStmt->executeQuery(
            "UPDATE UniqueItemInfo SET CurrentNumber=CurrentNumber-1 WHERE ItemClass=%d AND ItemType=%d",
            (int)itemClass, (int)itemType);

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt);

    __END_CATCH
    __END_DEBUG
}
