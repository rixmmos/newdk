//////////////////////////////////////////////////////////////////////////////
// Filename    : CGDialUpHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGDialUp.h"

#ifdef __GAME_SERVER__
#include "GCPhoneConnected.h"
#include "GCPhoneConnectionFailed.h"
#include "GCRing.h"
#include "GamePlayer.h"
#include "Slayer.h"
#include "TelephoneCenter.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGDialUpHandler::execute(CGDialUp* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

    Creature* pCreature = pGamePlayer->getCreature();

    Assert(pCreature->isSlayer());

    bool Success = false;

    Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

    Player* pTargetPlayer = NULL;

    SlotID_t PSlot = MAX_PHONE_SLOT;
    SlotID_t PSlot2 = MAX_PHONE_SLOT;

    
    PhoneNumber_t PNumber = pPacket->getPhoneNumber();

    Slayer* pTargetSlayer = g_pTelephoneCenter->getSlayer(PNumber);

    if (pSlayer->findEmptyPhoneSlot() != MAX_PHONE_SLOT && pSlayer != pTargetSlayer) {
        if (pTargetSlayer != NULL) {
            pTargetPlayer = pTargetSlayer->getPlayer();

            if (pTargetSlayer->isEmptyPhoneSlot()) {
                if (!(pTargetSlayer->isSlotByPhoneNumber(PNumber))) {
                    Success = true;
                }
            }
        }
    }

    if (Success) {
        
        PSlot2 = pTargetSlayer->findEmptyPhoneSlot();

        
        PSlot = pSlayer->findEmptyPhoneSlot();

        
        pTargetSlayer->setPhoneSlotNumber(PSlot2, pSlayer->getPhoneNumber());

        
        pSlayer->setPhoneSlotNumber(PSlot, PNumber);

        // cout << "Call Number : " << pSlayer->getPhoneNumber() << ", Called Number : " << PNumber << endl;
        // cout << "Call Slot : " << PSlot << ", Called Slot : " << PSlot2 << endl;

        
        GCRing gcRing;
        gcRing.setPhoneNumber(pSlayer->getPhoneNumber());
        gcRing.setSlotID(PSlot2);
        gcRing.setName(pSlayer->getName());

        
        GCPhoneConnected gcPhoneConnected;
        gcPhoneConnected.setSlotID(PSlot);
        gcPhoneConnected.setName(pTargetSlayer->getName());

        pTargetPlayer->sendPacket(&gcRing);
        pGamePlayer->sendPacket(&gcPhoneConnected);
    } else {
        GCPhoneConnectionFailed gcPhoneConnectionFailed;
        pGamePlayer->sendPacket(&gcPhoneConnectionFailed);
    }

#endif // __GAME_SERVER__

    __END_DEBUG_EX __END_CATCH
}
