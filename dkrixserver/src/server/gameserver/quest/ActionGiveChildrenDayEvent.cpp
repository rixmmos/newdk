////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionGiveChildrenItem.cpp
// Written By  :
// Description :
////////////////////////////////////////////////////////////////////////////////

#include <list>

#include "ActionGiveChildrenItem.h"
#include "DB.h"
#include "GCCreateItem.h"
#include "GCNPCResponse.h"
#include "GCSystemMessage.h"
#include "GamePlayer.h"
#include "Inventory.h"
#include "Item.h"
#include "ItemFactoryManager.h"
#include "ItemUtil.h"
#include "PlayerCreature.h"
#include "StringPool.h"
#include "Thread.h"
#include "Zone.h"
#include "item/Key.h"

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void ActionGiveChildrenItem::read(PropertyBuffer& propertyBuffer)

{
    __BEGIN_TRY
    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////






////////////////////////////////////////////////////////////////////////////////
void ActionGiveChildrenItem::execute(Creature* pCreature1, Creature* pCreature2)

{
    __BEGIN_TRY

    Assert(pCreature1 != NULL);
    Assert(pCreature2 != NULL);
    Assert(pCreature1->isNPC());
    Assert(pCreature2->isPC());

    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature2);
    Assert(pPC != NULL);

    Player* pPlayer = pPC->getPlayer();
    Assert(pPlayer != NULL);

    
    GCNPCResponse okpkt;
    pPlayer->sendPacket(&okpkt);

    Statement* pStmt = NULL;
    Result* pResult = NULL;
    int count = -1;

    Zone* pZone = pPC->getZone();
    ObjectRegistry& OR = pZone->getObjectRegistry();

    Inventory* pInventory = pPC->getInventory();
    TPOINT pt;
    GCCreateItem gcCreateItem;

    StringStream msg;
    msg << "PlayerID[" << pPlayer->getID() << "], " << "CreatureName[" << pPC->getName() << "]\n";

    if (pPC->isSlayer()) {
        
        Item* pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_BRACELET, 3, 50);

        
        OR.registerObject(pItem);

        if (pInventory->addItem(pItem, pt)) {
            
            pItem->create(pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y);

            
            gcCreateItem.setObjectID(pItem->getObjectID());
            gcCreateItem.setItemClass(pItem->getItemClass());
            gcCreateItem.setItemType(pItem->getItemType());
            gcCreateItem.setOptionType(pItem->getOptionType());
            gcCreateItem.setDurability(pItme->getDurability());
            gcCreateItem.setSilver(pItem->getSilver());
            gcCreateItem.setItemNum(pItem->getNum());
            gcCreateItem.setInvenX(pt.x);
            gcCreateItem.setInvenY(pt.y);

            pPlayer->sendPacket(&gcCreateItem);

            msg << "Inventory Adding Success : " << pItem->toString() << "\n";

            
            if (pItem != NULL && pItem->isTraceItem()) {
                remainTraceLog(pItem, pCreature1->getName(), pCreature2->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC);
                remainTraceLogNew(pItem, pCreature2->getName(), ITL_GET, ITLD_EVENTNPC,
                                  pCreature1->getZone()->getZoneID(), pCreature1->getX(), pCreature1->getY());
            }
        } else {
            
            StringStream buf;
            buf << pPlayer->getID() << g_pStringPool->getString(STRID_NOT_ENOUGH_INVENTORY_SPACE);

            GCSystemMessage gcSystemMessage;
            gcSystemMessage.setMessage(buf.toString());
            pPlayer->sendPacket(&gcSystemMessage);

            return;
        }

    } else {
        Item* pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_VAMPIRE_BRACELET, 3, 50);

        
        OR.registerObject(pItem);

        if (pInventory->addItem(pItem, pt)) {
            pItem->create(pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y);
            pItem->save(pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y);

            gcCreateItem.setObjectID(pItem->getObjectID());
            gcCreateItem.setItemClass(pItem->getItemClass());
            gcCreateItem.setItemType(pItem->getItemType());
            gcCreateItem.setOptionType(pItem->getOptionType());
            gcCreateItem.setDurability(pItem->getDurability());
            gcCreateItem.setSilver(pItem->getSilver());
            gcCreateItem.setItemNum(pItem->getNum());
            gcCreateItem.setInvenX(pt.x);
            gcCreateItem.setInvenY(pt.y);

            pPlayer->sendPacket(&gcCreateItem);

            msg << "Inventory Adding Succeeded : " << pItem->toString() << "\n";

            
            if (pItem != NULL && pItem->isTraceItem()) {
                remainTraceLog(pItem, pCreature1->getName(), pCreature2->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC);
                remainTraceLogNew(pItem, pCreature2->getName(), ITL_GET, ITLD_EVENTNPC,
                                  pCreature1->getZone()->getZoneID(), pCreature1->getX(), pCreature1->getY());
            }
        } else {
            
            StringStream buf;
            buf << pPlayer->getID() << g_pStringPool->getString(STRID_NOT_ENOUGH_INVENTORY_SPACE);

            GCSystemMessage gcSystemMessage;
            gcSystemMessage.setMessage(buf.toString());
            pPlayer->sendPacket(&gcSystemMessage);

            return;
        }
    }

    msg << "Finished\n";

    filelog("SpecialEvent.log", "%s", msg.toString().c_str());

    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionGiveChildrenItem::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "ActionGiveChildrenItem("
        << ")";
    return msg.toString();

    __END_CATCH
}
