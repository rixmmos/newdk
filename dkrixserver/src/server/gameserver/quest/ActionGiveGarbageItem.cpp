////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionGiveGarbageEventItem.cpp
// Written By  : excel96
// Description :
////////////////////////////////////////////////////////////////////////////////

#include <list>

#include "ActionGiveGarbageEventItem.h"
#include "DB.h"
#include "GCCreateItem.h"
#include "GCNPCResponse.h"
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
////////////////////////////////////////////////////////////////////////////////
void ActionGiveGarbageEventItem::read(PropertyBuffer& propertyBuffer)

{
    __BEGIN_TRY
    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void ActionGiveGarbageEventItem::execute(Creature* pCreature1, Creature* pCreature2)

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

    
    if (pPC->getGarbageSize() > 0) {
        TPOINT pt;

        Item* pItem = pPC->popItemFromGarbage();
        Assert(pItem != NULL);

        
        if (pInventory->addItem(pItem, pt)) {
            pItem->save(pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y);

            GCCreateItem gcCreateItem;
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

            
            if (pItem != NULL && pItem->isTraceItem()) {
                remainTraceLog(pItem, pCreature1->getName(), pCreature2->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC);
                remainTraceLogNew(pItem, pCreature2->getName(), ITL_GET, ITLD_EVENTNPC,
                                  pCreature1->getZone()->getZoneID(), pCreature1->getX(), pCreature1->getY());
            }
        }
        
        
        else {
            pPC->addItemToGarbage(pItem);

            GCSystemMessage gcSystemMessage;
            gcSystemMessage.setMessage(g_pStringPool->getString(STRID_NOT_ENOUGH_INVENTORY_SPACE));
            pPlayer->sendPacket(&gcSystemMessage);
        }
    } else {
        GCSystemMessage gcSystemMessage;
        gcSystemMessage.setMessage(g_pStringPool->getString(STRID_ITEM_NOT_EXIST));
        pPlayer->sendPacket(&gcSystemMessage);
    }

    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionGiveGarbageEventItem::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "ActionGiveGarbageEventItem("
        << ")";
    return msg.toString();

    __END_CATCH
}
