//////////////////////////////////////////////////////////////////////////////
// Filename    : CGPhoneSayHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGPhoneSay.h"

#ifdef __GAME_SERVER__
#include "GCPhoneDisconnected.h"
#include "GCPhoneSay.h"
#include "GamePlayer.h"
#include "Slayer.h"
#include "TelephoneCenter.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGPhoneSayHandler::execute(CGPhoneSay* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

    bool Success = false;

    Creature* pCreature = pGamePlayer->getCreature();

    Assert(pCreature->isSlayer());

    Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

    SlotID_t PhoneSlot = pPacket->getSlotID();

    Assert(PhoneSlot < MAX_PHONE_SLOT);

    PhoneNumber_t PhoneNumber = pSlayer->getPhoneNumber();

    PhoneNumber_t TargetPhoneNumber = pSlayer->getPhoneSlotNumber(PhoneSlot);

    Slayer* pTargetSlayer = g_pTelephoneCenter->getSlayer(TargetPhoneNumber);

    // cout << "PhoneSlot : " << (int)PhoneSlot << ", Message : " << pPacket->getMessage() << endl;

    if (pTargetSlayer != NULL) {
        if (pTargetSlayer->isSlotByPhoneNumber(PhoneNumber)) {
            Success = true;
        }
    }

    
    if (Success) {
        SlotID_t TargetPhoneSlot = pTargetSlayer->getSlotWithPhoneNumber(PhoneNumber);

        Player* pTargetPlayer = pTargetSlayer->getPlayer();

        
        GCPhoneSay gcPhoneSay;

        
        gcPhoneSay.setSlotID(TargetPhoneSlot);
        gcPhoneSay.setMessage(pPacket->getMessage());

        pTargetPlayer->sendPacket(&gcPhoneSay);

        // cout << "Phone Say Successfull " << endl;

        
    } else {
        GCPhoneDisconnected gcPhoneDisconnected;
        gcPhoneDisconnected.setSlotID(PhoneSlot);

        pGamePlayer->sendPacket(&gcPhoneDisconnected);

        // cout << "Phone Say Failed" << endl;
    }

#endif

    __END_DEBUG_EX __END_CATCH
}
