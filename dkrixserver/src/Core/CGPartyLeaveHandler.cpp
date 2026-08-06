//////////////////////////////////////////////////////////////////////////////
// Filename    : CGPartyLeaveHandler.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGPartyLeave.h"

#ifdef __GAME_SERVER__
#include "Creature.h"
#include "GCPartyError.h"
#include "GCPartyLeave.h"
#include "GamePlayer.h"
#include "PCFinder.h"
#include "Party.h"
#include "SystemAvailabilitiesManager.h"
#include "Zone.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGPartyLeaveHandler::execute(CGPartyLeave* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    SYSTEM_ASSERT(SYSTEM_PARTY);

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    string TargetName = pPacket->getTargetName();

    Creature* pCreature = pGamePlayer->getCreature();
    Assert(pCreature != NULL);

    Zone* pZone = pCreature->getZone();
    Assert(pZone != NULL);

    int PartyID = pCreature->getPartyID();
    if (PartyID == 0) {
        
        // pCreature->toString().c_str());
        throw ProtocolException();
    }

    GCPartyError gcPartyError;

    
    if (TargetName == "") {
        
        
        
        
        
        // cout << g_pGlobalPartyManager->toString() << endl;
        // cout << "================================================" << endl;

        g_pGlobalPartyManager->deletePartyMember(PartyID, pCreature);

        
        // cout << g_pGlobalPartyManager->toString() << endl;
        // cout << "================================================" << endl;

        
        LocalPartyManager* pLocalPartyManager = pZone->getLocalPartyManager();
        Assert(pLocalPartyManager != NULL);
        pLocalPartyManager->deletePartyMember(PartyID, pCreature);
    }
    
    else {
        
        // cout << g_pGlobalPartyManager->toString() << endl;
        // cout << "=======================================" << endl;

        
        
        
        
        g_pGlobalPartyManager->expelPartyMember(PartyID, pCreature, TargetName);

        
        // cout << g_pGlobalPartyManager->toString() << endl;
        // cout << "=======================================" << endl;

        
        __ENTER_CRITICAL_SECTION((*g_pPCFinder))

        Creature* pTargetCreature = g_pPCFinder->getCreature_LOCKED(TargetName);
        // Assert(pTargetCreature != NULL);

        
        if (pTargetCreature == NULL) {
            g_pPCFinder->unlock();
            return;
        }

        Zone* pTargetZone = pTargetCreature->getZone();
        Assert(pTargetZone != NULL);
        LocalPartyManager* pLocalPartyManager = pTargetZone->getLocalPartyManager();
        Assert(pLocalPartyManager != NULL);

        
        // cout << pLocalPartyManager->toString() << endl;
        // cout << "=======================================" << endl;

        pLocalPartyManager->deletePartyMember(PartyID, pTargetCreature);
        

        __LEAVE_CRITICAL_SECTION((*g_pPCFinder))
    }

#endif

    __END_DEBUG_EX __END_CATCH
}
