//////////////////////////////////////////////////////////////////////////////
// Filename    : CGShopRequestListHandler.cpp


//////////////////////////////////////////////////////////////////////////////

#include "CGShopRequestList.h"

#ifdef __GAME_SERVER__
#include "CheckedCast.h"
#include "GCNPCResponse.h"
#include "GCShopList.h"
#include "GCShopListMysterious.h"
#include "GamePlayer.h"
#include "NPC.h"
#include "PlayerCreature.h"
#include "quest/Action.h"
#include "quest/Condition.h"
#include "quest/Trigger.h"
#include "quest/TriggerManager.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGShopRequestListHandler::execute(CGShopRequestList* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    
    ObjectID_t NPCID = pPacket->getObjectID();
    ShopRackType_t type = pPacket->getRackType();

    
    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pGamePlayer->getCreature());
    Zone* pZone = pPC->getZone();
    Creature* pNPCBase = NULL;

    /*
    try
    {
        pNPCBase = pZone->getCreature(NPCID);
    }
    catch (NoSuchElementException & nsee)
    {
        pNPCBase = NULL;
    }
    */

    
    pNPCBase = pZone->getCreature(NPCID);

    if (pNPCBase == NULL || pNPCBase->isNPC() == false) {
        GCNPCResponse gcNPCResponse;
        pPlayer->sendPacket(&gcNPCResponse);
        return;
    }

    NPC* pNPC = checkedCast<NPC*>(pNPCBase);

    if (type == SHOP_RACK_SPECIAL) {
        
        GCShopList pkt;
        pkt.setNPCShopType(pNPC->getShopType());
        pkt.setObjectID(NPCID);
        pkt.setShopVersion(pNPC->getShopVersion(type));
        pkt.setShopType(type);

        for (BYTE i = 0; i < SHOP_RACK_INDEX_MAX; i++) {
            
            Item* pItem = pNPC->getShopItem(type, i);
            if (pItem != NULL)
                pkt.setShopItem(i, pItem);
        }

        pkt.setMarketCondBuy(pNPC->getMarketCondBuy());
        //		pkt.setMarketCondSell(pNPC->getMarketCondSell());
        pkt.setMarketCondSell(pNPC->getTaxRatio(pPC));

        
        pPlayer->sendPacket(&pkt);
    } else if (type == SHOP_RACK_MYSTERIOUS) {
        
        GCShopListMysterious pkt;
        pkt.setObjectID(NPCID);
        pkt.setShopVersion(pNPC->getShopVersion(type));
        pkt.setShopType(type);

        for (BYTE i = 0; i < SHOP_RACK_INDEX_MAX; i++) {
            
            Item* pItem = pNPC->getShopItem(type, i);
            if (pItem != NULL)
                pkt.setShopItem(i, pItem);
        }

        pkt.setMarketCondBuy(pNPC->getMarketCondBuy());
        //		pkt.setMarketCondSell(pNPC->getMarketCondSell());
        pkt.setMarketCondSell(pNPC->getTaxRatio(pPC));

        
        pPlayer->sendPacket(&pkt);
    } else {
        throw ProtocolException("NORMAL shop item list not allowed!!!");
    }

#endif

    __END_DEBUG_EX __END_CATCH
}
