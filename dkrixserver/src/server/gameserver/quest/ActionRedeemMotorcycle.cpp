////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionRedeemMotorcycle.cpp
// Written By  :
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ActionRedeemMotorcycle.h"

#include "Belt.h"
#include "Creature.h"
#include "DB.h"
#include "GCNPCResponse.h"
#include "GamePlayer.h"
#include "Item.h"
#include "ItemFactoryManager.h"
#include "ItemUtil.h"
#include "ParkingCenter.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Zone.h"
#include "item/Key.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void ActionRedeemMotorcycle::read(PropertyBuffer& propertyBuffer)

{
    __BEGIN_TRY
    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void ActionRedeemMotorcycle::execute(Creature* pCreature1, Creature* pCreature2)

{
    __BEGIN_TRY

    Assert(pCreature1 != NULL);
    Assert(pCreature2 != NULL);
    Assert(pCreature1->isNPC());
    Assert(pCreature2->isPC());

    Player* pPlayer = pCreature2->getPlayer();
    Assert(pPlayer != NULL);

    
    GCNPCResponse answerOKpkt;
    pPlayer->sendPacket(&answerOKpkt);

    
    if (pCreature2->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature2);
        Zone* pZone = pSlayer->getZone();
        Inventory* pInventory = pSlayer->getInventory();
        uint InvenWidth = pInventory->getWidth();
        uint InvenHeight = pInventory->getHeight();
        Item* pItem = NULL;

        Inventory* pBeltInventory = NULL;
        uint BeltInvenWidth = 0;
        uint BeltInvenHeight = 0;
        Item* pBelt = NULL;

        pBelt = pSlayer->getWearItem(Slayer::WEAR_BELT);
        if (pBelt != NULL) {
            pBeltInventory = ((Belt*)pBelt)->getInventory();

            BeltInvenWidth = pBeltInventory->getWidth();
            BeltInvenHeight = pBeltInventory->getHeight();
        }

        
        for (uint y = 0; y < InvenHeight; y++) {
            for (uint x = 0; x < InvenWidth; x++) {
                
                if (pInventory->hasItem(x, y)) {
                    pItem = pInventory->getItem(x, y);
                    if (load(pItem, pSlayer, pZone, pSlayer->getX(), pSlayer->getY())) {
                        return;
                    }
                }
            }
        }

        if (pBelt != NULL) {
            
            for (uint y = 0; y < BeltInvenHeight; y++) {
                for (uint x = 0; x < BeltInvenWidth; x++) {
                    if (pBeltInventory->hasItem(x, y)) {
                        pItem = pBeltInventory->getItem(x, y);
                        if (load(pItem, pSlayer, pZone, pSlayer->getX(), pSlayer->getY())) {
                            return;
                        }
                    }
                }
            }
        }
    } else 
    {
    }

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool ActionRedeemMotorcycle::load(Item* pItem, Slayer* pSlayer, Zone* pZone, ZoneCoord_t x, ZoneCoord_t y) const

{
    bool bFound = false;

    __BEGIN_TRY

    
    if (pItem->getItemClass() != Item::ITEM_CLASS_KEY)
        return false;

    Key* pKey = dynamic_cast<Key*>(pItem);
    ItemID_t targetID = pKey->getTarget();

    try {
        
        Statement* pStmt = NULL;
        Result* pResult = NULL;

        
        
        
        
        // by sigi. 2002.12.25 x-mas T_T;
        if (targetID == 0) {
            targetID = pKey->setNewMotorcycle(pSlayer);
             
        } else {
            
            
            BEGIN_DB {
                pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
                pResult = pStmt->executeQuery("SELECT ItemID FROM MotorcycleObject WHERE ItemID=%lu", targetID);

                if (!pResult->next()) {
                    Key* pKey = dynamic_cast<Key*>(pItem);
                    Assert(pKey != NULL);

                    targetID = pKey->setNewMotorcycle(pSlayer);
                }

                SAFE_DELETE(pStmt);
            }
            END_DB(pStmt);
        }


        
        if (targetID == 0) {
            filelog("errorLog.txt", "[ActionRedeemMotorcycle] itemID=%d, motorItemID=%d", (int)pItem->getItemID(),
                    (int)targetID);
            return false;
        }

        
        if (g_pParkingCenter->hasMotorcycleBox(targetID)) {
            
            
             

            return false;
        }

        ItemID_t itemID;
        ItemType_t itemType;
        string optionType;
        Durability_t durability;

        BEGIN_DB {
            StringStream sql;
            sql << "SELECT ItemID, ItemType, OptionType, Durability "
                << "FROM MotorcycleObject where ItemID = " << targetID;

            pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
            pResult = pStmt->executeQueryString(sql.toString());

            // by sigi. 2002.10.14
            
            if (pResult->getRowCount() <= 0) {
                bFound = false;

                itemID = targetID;
                itemType = 0;
                optionType = "";
                durability = 300;
            } else {
                bFound = true;

                pResult->next();

                itemID = pResult->getInt(1);
                itemType = pResult->getInt(2);
                optionType = pResult->getString(3);
                durability = pResult->getInt(4);
            }


            
            list<OptionType_t> optionTypes;
            setOptionTypeFromField(optionTypes, optionType);
            Motorcycle* pMotorcycle = new Motorcycle(itemType, optionTypes);

            Assert(pMotorcycle != NULL);

            pMotorcycle->setItemID(itemID);
            pMotorcycle->setDurability(durability);

            
            (pZone->getObjectRegistry()).registerObject(pMotorcycle);

            
            TPOINT pt = pZone->addItem(pMotorcycle, x, y, false);
            if (pt.x == -1) {
                
                filelog("motorError.txt",
                        "ActionRedeemMotorcycle::load() :     . zoneID=%d, xy=(%d, %d)",
                        (int)pZone->getZoneID(), (int)x, (int)y); // by sigi. 2002.12.24
                throw Error("ActionRedeemMotorcycle::load() :     ");
            }

            // by sigi. 2002.10.14
            if (!bFound) {
                pStmt->executeQuery(
                    "INSERT IGNORE INTO MotorcycleObject (ItemID, ObjectID, ItemType, OwnerID, Storage, StorageID, X, "
                    "Y, OptionType, Durability) Values (%d, %d, %d, '', %d, %d, %d, %d, '', %d)",
                    itemID, pMotorcycle->getObjectID(), itemType, STORAGE_ZONE, pZone->getZoneID(), pt.x, pt.y,
                    durability);
            }

            
            MotorcycleBox* pBox = new MotorcycleBox(pMotorcycle, pZone, pt.x, pt.y);
            Assert(pBox != NULL);

            try {
                g_pParkingCenter->addMotorcycleBox(pBox);
            } catch (DuplicatedException& de) { // by sigi. 2002.12.24
                filelog("motorError.txt", "%s - itemID=%d, motorid=%d", de.toString().c_str(), itemID,
                        pMotorcycle->getObjectID());
            }

            bFound = true;
            //}

            SAFE_DELETE(pStmt);
        }
        END_DB(pStmt)

    } catch (Throwable& t) { // by sigi. 2002.12.25
        filelog("motorError.txt", "%s - itemID=%d, motorItemID=%d", t.toString().c_str(), (int)pItem->getItemID(),
                (int)targetID);
        
        // throw;
    }

    __END_CATCH

    return bFound;
}

////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionRedeemMotorcycle::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "ActionRedeemMotorcycle(" << ")";
    return msg.toString();

    __END_CATCH
}
