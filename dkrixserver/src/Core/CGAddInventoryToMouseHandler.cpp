//////////////////////////////////////////////////////////////////////////////
// Filename    : CGAddInventoryToMouseHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGAddInventoryToMouse.h"

#ifdef __GAME_SERVER__
#include <stdio.h>

#include "GCCannotAdd.h"
#include "GCCreateItem.h"
#include "GCTradeVerify.h"
#include "GamePlayer.h"
#include "Inventory.h"
#include "Item.h"
#include "ItemFactoryManager.h"
#include "ItemUtil.h"
#include "ObjectRegistry.h"
#include "PlayerCreature.h"
#include "Store.h"
#include "TradeManager.h"
#include "Zone.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGAddInventoryToMouseHandler::execute(CGAddInventoryToMouse* pPacket, Player* pPlayer) {
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    Assert(pGamePlayer != NULL);

    Creature* pCreature = pGamePlayer->getCreature();
    Assert(pCreature != NULL);

    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
    Assert(pPC != NULL);

    Zone* pZone = pPC->getZone();
    Assert(pZone != NULL);

    if (pPC->getStore()->isOpen()) {
        GCCannotAdd _GCCannotAdd;
        _GCCannotAdd.setObjectID(pPacket->getObjectID());
        pPlayer->sendPacket(&_GCCannotAdd);
        return;
    }

    CoordInven_t InvenX = pPacket->getX();
    CoordInven_t InvenY = pPacket->getY();
    ObjectID_t ItemOID = pPacket->getObjectID();

    Inventory* pInventory = pPC->getInventory();
    Assert(pInventory != NULL);

    
    if (InvenX >= pInventory->getWidth() || InvenY >= pInventory->getHeight()) {
        GCCannotAdd _GCCannotAdd;
        _GCCannotAdd.setObjectID(pPacket->getObjectID());
        pPlayer->sendPacket(&_GCCannotAdd);
        return;
    }

    Item* pItem = pInventory->getItem(InvenX, InvenY);
    Item* pExtraSlotItem = pPC->getExtraInventorySlotItem();

    if (pPC->getStore()->hasItem(pItem)) {
        GCCannotAdd _GCCannotAdd;
        _GCCannotAdd.setObjectID(pPacket->getObjectID());
        pPlayer->sendPacket(&_GCCannotAdd);
        return;
    }

    
    
    if (pItem == NULL || pExtraSlotItem != NULL) {
        GCCannotAdd _GCCannotAdd;
        _GCCannotAdd.setObjectID(pPacket->getObjectID());
        pPlayer->sendPacket(&_GCCannotAdd);
        return;
    }

    
    if (ItemOID != 0) {
        
        if (pItem->getObjectID() != ItemOID) {
            GCCannotAdd _GCCannotAdd;
            _GCCannotAdd.setObjectID(pPacket->getObjectID());
            pPlayer->sendPacket(&_GCCannotAdd);
            return;
        }

        pInventory->deleteItem(pItem->getObjectID());
        pPC->addItemToExtraInventorySlot(pItem);
        // pItem->save(pPC->getName(), STORAGE_EXTRASLOT, 0, 0, 0);
        
        char pField[80];
        sprintf(pField, "Storage=%d, StorageID=0", STORAGE_EXTRASLOT);
        pItem->tinysave(pField);

        TradeManager* pTradeManager = pZone->getTradeManager();
        TradeInfo* pInfo = pTradeManager->getTradeInfo(pCreature->getName());
        if (pInfo != NULL && pInfo->getStatus() == TRADE_FINISH) {
            GCTradeVerify gcTradeVerify;
            gcTradeVerify.setCode(GC_TRADE_VERIFY_CODE_INVENTORY_TO_MOUSE_OK);
            pPlayer->sendPacket(&gcTradeVerify);
        }
    }
    
    else {
        
        if (!isStackable(pItem) || pItem->getNum() < 2 ||
            (pItem->getItemClass() == Item::ITEM_CLASS_MOON_CARD && pItem->getItemType() == 2 &&
             pItem->getNum() == 99) ||
            pPC->getStore()->hasItem(pItem)) {
            GCCannotAdd _GCCannotAdd;
            _GCCannotAdd.setObjectID(pPacket->getObjectID());
            pPlayer->sendPacket(&_GCCannotAdd);
            return;
        }

        
        Item::ItemClass IClass = pItem->getItemClass();
        ItemType_t IType = pItem->getItemType();
        const list<OptionType_t>& OType = pItem->getOptionTypeList();

        Item* pNewItem = g_pItemFactoryManager->createItem(IClass, IType, OType);
        Assert(pNewItem != NULL);

        
        
        Zone* pZone = pPC->getZone();
        Assert(pZone != NULL);

        ObjectRegistry& OR = pZone->getObjectRegistry();
        OR.registerObject(pNewItem);

        
        
        
        
        pInventory->deleteItem(pItem->getObjectID());
        pPC->addItemToExtraInventorySlot(pItem);

        int NewNum = pItem->getNum() - 1;
        pNewItem->setNum(NewNum);
        pItem->setNum(1);

        pInventory->addItem(InvenX, InvenY, pNewItem);

        
        // pItem->save(pPC->getName(), STORAGE_EXTRASLOT, 0, 0, 0);
        
        char pField[80];
        sprintf(pField, "Num=%d, Storage=%d, StorageID=0", 1, STORAGE_EXTRASLOT);
        pItem->tinysave(pField);

        pNewItem->create(pPC->getName(), STORAGE_INVENTORY, 0, InvenX, InvenY);
        
        // pNewItem->save(pPC->getName(), STORAGE_INVENTORY, 0, InvenX, InvenY);
        
        sprintf(pField, "Num=%d, Storage=%d, StorageID=0", NewNum, STORAGE_INVENTORY);
        pNewItem->tinysave(pField);


        
        
        GCCreateItem gcCreateItem;
        gcCreateItem.setObjectID(pNewItem->getObjectID());
        gcCreateItem.setItemClass((BYTE)pNewItem->getItemClass());
        gcCreateItem.setItemType(pNewItem->getItemType());
        gcCreateItem.setOptionType(pNewItem->getOptionTypeList());
        gcCreateItem.setDurability(pNewItem->getDurability());
        gcCreateItem.setSilver(pNewItem->getSilver());
        gcCreateItem.setEnchantLevel(pNewItem->getEnchantLevel());
        gcCreateItem.setItemNum(pNewItem->getNum());
        gcCreateItem.setInvenX(InvenX);
        gcCreateItem.setInvenY(InvenY);
        pPlayer->sendPacket(&gcCreateItem);
    }

#endif // __GAME_SERVER__

    __END_DEBUG_EX __END_CATCH
}
