//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSMSAddressListHandler.cpp
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGSMSAddressList.h"

#ifdef __GAME_SERVER__
#include "GCSMSAddressList.h"
#include "GamePlayer.h"
#include "PlayerCreature.h"
#include "SMSAddressBook.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSMSAddressListHandler::execute(CGSMSAddressList* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    Assert(pGamePlayer != NULL);

    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pGamePlayer->getCreature());
    Assert(pPC != NULL);

    SMSAddressBook* pBook = pPC->getAddressBook();
    if (pBook != NULL) {
        // getGCSMSAddressList() allocates the packet and one AddressUnit per
        // entry; sendPacket() serializes synchronously and does not take
        // ownership, so the caller must free it. Leaked once per request
        // before, and a client may send this packet freely.
        GCSMSAddressList* pAddressList = pBook->getGCSMSAddressList();
        if (pAddressList != NULL) {
            pGamePlayer->sendPacket(pAddressList);
            SAFE_DELETE(pAddressList);
        }
    }

#endif // __GAME_SERVER__

    __END_DEBUG_EX __END_CATCH
}
