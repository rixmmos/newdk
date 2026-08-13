//////////////////////////////////////////////////////////////////////////////
// Filename    : CGMouseToStashHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGMouseToStash.h"

#ifdef __GAME_SERVER__
#include "CheckedCast.h"
#include "GamePlayer.h"
#include "Item.h"
#include "ItemInfo.h"
#include "ItemInfoManager.h"
#include "ItemUtil.h"
#include "PlayerCreature.h"
#include "Stash.h"
#include "Zone.h"
// #include "LogClient.h"
#include <stdio.h>

#include "CreatureUtil.h"
#include "GCCannotAdd.h"
#include "RelicUtil.h"
#include "item/Magazine.h"
#include "item/PetItem.h"
#include "item/Potion.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGMouseToStashHandler::execute(CGMouseToStash* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX __BEGIN_DEBUG

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    try {
        GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
        Creature* pCreature = pGamePlayer->getCreature();
        PlayerCreature* pPC = checkedCast<PlayerCreature*>(pCreature);
        Stash* pStash = pPC->getStash();
        Item* pMouseItem = pPC->getExtraInventorySlotItem();
        bool Success = false;

        
        
        
        if (pMouseItem == NULL || !canPutInStash(pMouseItem)
            //			|| pMouseItem->isUnique())
        ) {
            GCCannotAdd _GCCannotAdd;
            _GCCannotAdd.setObjectID(pPacket->getObjectID());
            pPlayer->sendPacket(&_GCCannotAdd);
            return;
        }

        ObjectID_t MouseItemOID = pMouseItem->getObjectID();
        BYTE rack = pPacket->getRack();
        BYTE index = pPacket->getIndex();

        
        
        if (rack >= STASH_RACK_MAX || index >= STASH_INDEX_MAX || rack >= pPC->getStashNum() ||
            MouseItemOID != pPacket->getObjectID()) {
            GCCannotAdd _GCCannotAdd;
            _GCCannotAdd.setObjectID(pPacket->getObjectID());
            pPlayer->sendPacket(&_GCCannotAdd);
            return;
        }

        
        Item* pStashItem = pStash->get(rack, index);

        
        if (pStashItem != NULL) {
            
            if (isSameItem(pMouseItem, pStashItem) && isStackable(pMouseItem)) {
                int MaxStack = ItemMaxStack[pMouseItem->getItemClass()];

                if (pMouseItem->getNum() + pStashItem->getNum() > MaxStack) {
                    ItemNum_t CurrentNum = pStashItem->getNum();
                    ItemNum_t AddNum = pMouseItem->getNum();

                    pStashItem->setNum(MaxStack);
                    pMouseItem->setNum(AddNum + CurrentNum - MaxStack);

                    
                    // pStashItem->save(pPC->getName(), STORAGE_STASH, 0, rack, index);
                    
                    char pField[80];
                    sprintf(pField, "Num=%d, Storage=%d, X=%d, Y=%d", MaxStack, STORAGE_STASH, rack, index);
                    pStashItem->tinysave(pField);

                    // pMouseItem->save(pPC->getName(), STORAGE_EXTRASLOT, 0, 0, 0);
                    
                    sprintf(pField, "Num=%d, Storage=%d", pMouseItem->getNum(), STORAGE_EXTRASLOT);
                    pMouseItem->tinysave(pField);


                    // log(LOG_STASH_ADD_ITEM, pPC->getName(), "", pMouseItem->toString());

                    Success = true;
                } else {
                    pPC->deleteItemFromExtraInventorySlot();
                    //					pMouseItem->whenPCLost(pPC);

                    pStashItem->setNum(pStashItem->getNum() + pMouseItem->getNum());
                    // pStashItem->save(pPC->getName(), STORAGE_STASH, 0, rack, index);
                    
                    char pField[80];
                    sprintf(pField, "Num=%d, Storage=%d, X=%d, Y=%d", pStashItem->getNum(), STORAGE_STASH, rack, index);
                    pStashItem->tinysave(pField);


                    // log(LOG_STASH_ADD_ITEM, pPC->getName(), "", pMouseItem->toString());

                    
                    
                    pMouseItem->destroy();
                    SAFE_DELETE(pMouseItem);

                    Success = true;
                }
            } else 
            {
                
                pPC->deleteItemFromExtraInventorySlot();
                pPC->addItemToExtraInventorySlot(pStashItem);

                //				pStashItem->whenPCTake(pPC);

                
                pStash->remove(rack, index);
                pStash->insert(rack, index, pMouseItem);

                //				pMouseItem->whenPCLost(pPC);

                // pStashItem->save(pPC->getName(), STORAGE_EXTRASLOT, 0, 0, 0);
                
                char pField[80];
                sprintf(pField, "Storage=%d", STORAGE_EXTRASLOT);
                pStashItem->tinysave(pField);

                // pMouseItem->save(pPC->getName(), STORAGE_STASH, 0, rack, index);
                
                sprintf(pField, "Storage=%d, X=%d, Y=%d", STORAGE_STASH, rack, index);
                pMouseItem->tinysave(pField);

                // log(LOG_STASH_REMOVE_ITEM, pPC->getName(), "", pStashItem->toString());
                // log(LOG_STASH_ADD_ITEM, pPC->getName(), "", pMouseItem->toString());

                Success = true;
            }
        } else 
        {
            
            pStash->insert(rack, index, pMouseItem);
            pPC->deleteItemFromExtraInventorySlot();
            //			pMouseItem->whenPCLost(pPC);
            // pMouseItem->save(pPC->getName(), STORAGE_STASH, 0, rack, index);
            
            char pField[80];
            sprintf(pField, "Storage=%d, X=%d, Y=%d", STORAGE_STASH, rack, index);
            pMouseItem->tinysave(pField);


            // log(LOG_STASH_ADD_ITEM, pPC->getName(), "", pMouseItem->toString());

            Success = true;
        }

        if (!Success) {
            GCCannotAdd _GCCannotAdd;
            _GCCannotAdd.setObjectID(pPacket->getObjectID());
            pPlayer->sendPacket(&_GCCannotAdd);
        } else {
            if (pMouseItem != NULL && pMouseItem->getItemClass() == Item::ITEM_CLASS_PET_ITEM) {
                PetItem* pPetItem = dynamic_cast<PetItem*>(pMouseItem);
                if (pPetItem != NULL && pPetItem->getPetInfo() != NULL && pPetItem->getPetInfo() == pPC->getPetInfo()) {
                    pPC->setPetInfo(NULL);
                    pPC->initAllStatAndSend();
                    sendPetInfo(pGamePlayer, true);
                }
            }
        }
    } catch (Throwable& t) {
        // cout << t.toString();
    }

#endif // __GAME_SERVER__

    __END_DEBUG
    __END_DEBUG_EX __END_CATCH
}
