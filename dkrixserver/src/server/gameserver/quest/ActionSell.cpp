////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionSell.cpp
// Written By  :
// Description :



////////////////////////////////////////////////////////////////////////////////

#include "ActionSell.h"

#include "GCNPCResponse.h"
#include "GCShopVersion.h"
#include "GamePlayer.h"
#include "NPC.h"
#include "PlayerCreature.h"
#include "Script.h"
#include "Trigger.h"

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
void ActionSell::read(PropertyBuffer& propertyBuffer)

{
    __BEGIN_TRY
    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void ActionSell::execute(Creature* pCreature1, Creature* pCreature2)

{
    __BEGIN_TRY

    Assert(pCreature1 != NULL);
    Assert(pCreature2 != NULL);
    Assert(pCreature1->isNPC());
    Assert(pCreature2->isPC());

    NPC* pNPC = dynamic_cast<NPC*>(pCreature1);
    Player* pPlayer = pCreature2->getPlayer();
    Assert(pPlayer != NULL);

    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature2);

    GCNPCResponse okpkt;
    pPlayer->sendPacket(&okpkt);

    GCShopVersion pkt;
    pkt.setObjectID(pNPC->getObjectID());
    pkt.setVersion(SHOP_RACK_NORMAL, pNPC->getShopVersion(SHOP_RACK_NORMAL));
    pkt.setVersion(SHOP_RACK_SPECIAL, pNPC->getShopVersion(SHOP_RACK_SPECIAL));
    pkt.setVersion(SHOP_RACK_MYSTERIOUS, pNPC->getShopVersion(SHOP_RACK_MYSTERIOUS));

    int taxratio = pNPC->getTaxRatio(pPC);

    if (taxratio != 100)
        pkt.setMarketCondSell(taxratio);
    else
        pkt.setMarketCondSell(pNPC->getMarketCondSell());

    pPlayer->sendPacket(&pkt);

    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionSell::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "ActionSell(" << ")";
    return msg.toString();

    __END_CATCH
}
