//////////////////////////////////////////////////////////////////////////////
// Filename    : CGPhoneDisconnectHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGPhoneDisconnect.h"

#ifdef __GAME_SERVER__
#include "GCPhoneDisconnected.h"
#include "GamePlayer.h"
#include "Slayer.h"
#include "TelephoneCenter.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGPhoneDisconnectHandler::execute(CGPhoneDisconnect* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

    Creature* pCreature = pGamePlayer->getCreature();

    Assert(pCreature->isSlayer());

    Player* pTargetPlayer = NULL;

    bool Success = false;

    Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

    SlotID_t PhoneSlot = pPacket->getSlotID();
    SlotID_t TargetPhoneSlot = MAX_PHONE_SLOT;

    Assert(PhoneSlot < MAX_PHONE_SLOT);

    PhoneNumber_t PhoneNumber = pSlayer->getPhoneNumber();
    PhoneNumber_t TargetPhoneNumber = pSlayer->getPhoneSlotNumber(PhoneSlot);

    Assert(TargetPhoneNumber != 0);

    Slayer* pTargetSlayer = g_pTelephoneCenter->getSlayer(TargetPhoneNumber);

    // cout << "Disconnect PhoneSlot : " << (int)PhoneSlot << ", Phone Number : " << (int)TargetPhoneNumber << endl;

    
    if (pTargetSlayer != NULL) {
        pTargetPlayer = pTargetSlayer->getPlayer();

        if (pTargetSlayer->isSlotByPhoneNumber(PhoneNumber)) {
            
            TargetPhoneSlot = pTargetSlayer->getSlotWithPhoneNumber(PhoneNumber);

            Success = true;
        }
    }

    
    if (Success) {
        
        pTargetSlayer->setPhoneSlotNumber(TargetPhoneSlot, 0);
        pSlayer->setPhoneSlotNumber(PhoneSlot, 0);

        
        GCPhoneDisconnected gcPhoneDisconnected;
        gcPhoneDisconnected.setPhoneNumber(TargetPhoneNumber);
        gcPhoneDisconnected.setSlotID(PhoneSlot);

        
        GCPhoneDisconnected gcPhoneDisconnected2;
        gcPhoneDisconnected2.setPhoneNumber(PhoneNumber);
        gcPhoneDisconnected2.setSlotID(TargetPhoneSlot);

        pPlayer->sendPacket(&gcPhoneDisconnected);
        pTargetPlayer->sendPacket(&gcPhoneDisconnected2);

        // cout << "Disconnected Successfull" << endl;

        
    } else {
        
        
        pSlayer->setPhoneSlotNumber(PhoneSlot, 0);

        
        GCPhoneDisconnected gcPhoneDisconnected;
        gcPhoneDisconnected.setPhoneNumber(TargetPhoneNumber);
        gcPhoneDisconnected.setSlotID(PhoneSlot);

        pPlayer->sendPacket(&gcPhoneDisconnected);

        // cout << "Not found That Slot PhoneNumber User" << endl;
    }

#endif // __GAME_SERVER__

    __END_DEBUG_EX __END_CATCH
}
