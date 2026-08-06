//////////////////////////////////////////////////////////////////////////////
// Filename    : CGAddGearToMouseHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGAddGearToMouse.h"

#ifdef __GAME_SERVER__
#include "GCCannotAdd.h"
#include "GamePlayer.h"
#include "Inventory.h"
#include "Item.h"
#include "ItemInfo.h"
#include "ItemInfoManager.h"
#include "Ousters.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Zone.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGAddGearToMouseHandler::execute(CGAddGearToMouse* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    try {
        GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
        Creature* pCreature = pGamePlayer->getCreature();
        bool bSuccess = false;
        SlotID_t SlotID = pPacket->getSlotID();

        Assert(pCreature != NULL);

        if (pCreature->isSlayer()) {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
            Assert(pSlayer != NULL);

            
            
            if (pSlayer->getExtraInventorySlotItem() == NULL && pSlayer->isWear((Slayer::WearPart)SlotID)) {
                
                Item* pItem = pSlayer->getWearItem((Slayer::WearPart)SlotID);

                
                if (pItem != NULL && pItem->getObjectID() == pPacket->getObjectID()) {
                    
                    
                    pSlayer->takeOffItem((Slayer::WearPart)SlotID, true, true);
                    bSuccess = true;
                }
            }
        } else if (pCreature->isVampire()) {
            Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

            Assert(pVampire != NULL);

            
            
            if (pVampire->getExtraInventorySlotItem() == NULL && pVampire->isWear((Vampire::WearPart)SlotID)) {
                
                Item* pItem = pVampire->getWearItem((Vampire::WearPart)SlotID);

                
                if (pItem != NULL && pItem->getObjectID() == pPacket->getObjectID()) {
                    
                    
                    pVampire->takeOffItem((Vampire::WearPart)SlotID, true, true);
                    bSuccess = true;
                }
            }
        } else if (pCreature->isOusters()) {
            Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

            Assert(pOusters != NULL);

            
            
            if (pOusters->getExtraInventorySlotItem() == NULL && pOusters->isWear((Ousters::WearPart)SlotID)) {
                
                Item* pItem = pOusters->getWearItem((Ousters::WearPart)SlotID);

                
                if (pItem != NULL && pItem->getObjectID() == pPacket->getObjectID()) {
                    
                    
                    pOusters->takeOffItem((Ousters::WearPart)SlotID, true, true);
                    bSuccess = true;
                }
            }
        }

        if (!bSuccess) {
            GCCannotAdd _GCCannotAdd;
            _GCCannotAdd.setObjectID(pPacket->getObjectID());
            pPlayer->sendPacket(&_GCCannotAdd);
        }
    } catch (Throwable& t) {
        // cerr << t.toString();
    }

#endif // __GAME_SERVER__

    __END_DEBUG_EX __END_CATCH
}
