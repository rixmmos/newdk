////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionBuy.cpp
// Written By  :
// Description :



////////////////////////////////////////////////////////////////////////////////

#include "ActionBuy.h"

#include "Creature.h"
#include "GCNPCResponse.h"
#include "GCShopMarketCondition.h"
#include "GamePlayer.h"
#include "NPC.h"

////////////////////////////////////////////////////////////////////////////////


//



////////////////////////////////////////////////////////////////////////////////
void ActionBuy::read(PropertyBuffer& propertyBuffer)

{
    __BEGIN_TRY
    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void ActionBuy::execute(Creature* pCreature1, Creature* pCreature2)

{
    __BEGIN_TRY

    Assert(pCreature1 != NULL);
    Assert(pCreature2 != NULL);
    Assert(pCreature1->isNPC());
    Assert(pCreature2->isPC());

    NPC* pNPC = dynamic_cast<NPC*>(pCreature1);
    Player* pPlayer = pCreature2->getPlayer();
    Assert(pPlayer != NULL);

    
    GCNPCResponse okpkt;
    pPlayer->sendPacket(&okpkt);

    
    GCShopMarketCondition pkt;
    pkt.setObjectID(pNPC->getObjectID());
    pkt.setMarketCondBuy(pNPC->getMarketCondBuy());
    pkt.setMarketCondSell(pNPC->getMarketCondSell());
    pPlayer->sendPacket(&pkt);

    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionBuy::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "ActionBuy(" << ")";
    return msg.toString();

    __END_CATCH
}
