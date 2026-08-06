//////////////////////////////////////////////////////////////////////////////
// Filename    : CGPartyInviteHandler.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGPartyInvite.h"

#ifdef __GAME_SERVER__
#include "Creature.h"
#include "CreatureUtil.h"
#include "GCPartyError.h"
#include "GCPartyInvite.h"
#include "GCSystemMessage.h"
#include "GQuestManager.h"
#include "GamePlayer.h"
#include "Party.h"
#include "PlayerCreature.h"
#include "SystemAvailabilitiesManager.h"
#include "Zone.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGPartyInviteHandler::execute(CGPartyInvite* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    SYSTEM_ASSERT(SYSTEM_PARTY);

    ObjectID_t TargetOID = pPacket->getTargetObjectID();
    BYTE CODE = pPacket->getCode();
    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    Assert(pGamePlayer != NULL);

    Creature* pCreature = pGamePlayer->getCreature();
    Assert(pCreature != NULL);

    Zone* pZone = pCreature->getZone();
    Assert(pZone != NULL);

    PartyInviteInfoManager* pPIIM = pZone->getPartyInviteInfoManager();

    
    
     
    /////

    Creature* pTargetCreature = NULL;
    /*
    try
    {
        pTargetCreature = pZone->getCreature(TargetOID);
    }
    catch (NoSuchElementException)
    {
        pPIIM->cancelInvite(pCreature);
        executeError(pPacket, pPlayer, GC_PARTY_ERROR_TARGET_NOT_EXIST);
        return;
    }
    */

    
    pTargetCreature = pZone->getCreature(TargetOID);

    if (pTargetCreature == NULL) {
        pPIIM->cancelInvite(pCreature);
        executeError(pPacket, pPlayer, GC_PARTY_ERROR_TARGET_NOT_EXIST);
        return;
    }


    
    if (!pTargetCreature->isPC() || !isSameRace(pCreature, pTargetCreature)) {
        pPIIM->cancelInvite(pCreature);
        executeError(pPacket, pPlayer, GC_PARTY_ERROR_RACE_DIFFER);
        return;
    }

    
    // PartyInviteInfo* pInfo1 = pPIIM->getInviteInfo(pCreature->getName());
    PartyInviteInfo* pInfo2 = pPIIM->getInviteInfo(pTargetCreature->getName());
    Player* pTargetPlayer = pTargetCreature->getPlayer();

    GCPartyInvite gcPartyInvite;

    
    if (CODE == CG_PARTY_INVITE_REQUEST) {
        // cout << "CODE:CG_PARTY_INVITE_REQUEST" << endl;

        
        
        
        
        
        
        
        /*
        if (pInfo1 != NULL)
        {
            //pPIIM->cancelInvite(pCreature);
            return;
        }
        */

        
        
        if (pInfo2 != NULL) {
            // cout << "pInfo2 != NULL" << endl;

            gcPartyInvite.setTargetObjectID(pPacket->getTargetObjectID());
            gcPartyInvite.setCode(GC_PARTY_INVITE_BUSY);
            pPlayer->sendPacket(&gcPartyInvite);
            return;
        }

        int PartyID = pCreature->getPartyID();
        int TargetPartyID = pTargetCreature->getPartyID();

        // cout << "Creature" << pCreature->getName() << "] PartyID = " << PartyID << endl;
        // cout << "TargetCreature" << pTargetCreature->getName() << "] PartyID = " << PartyID << endl;

        
        if (PartyID != 0 && TargetPartyID != 0) {
            gcPartyInvite.setTargetObjectID(pPacket->getTargetObjectID());
            gcPartyInvite.setCode(GC_PARTY_INVITE_ANOTHER_PARTY);
            pPlayer->sendPacket(&gcPartyInvite);
            return;
        }

        
        if (PartyID != 0) {
            if (!g_pGlobalPartyManager->canAddMember(PartyID)) {
                gcPartyInvite.setTargetObjectID(pPacket->getTargetObjectID());
                gcPartyInvite.setCode(GC_PARTY_INVITE_MEMBER_FULL);
                pPlayer->sendPacket(&gcPartyInvite);
                return;
            }
        }

        
        if (TargetPartyID != 0) {
            if (!g_pGlobalPartyManager->canAddMember(TargetPartyID)) {
                gcPartyInvite.setTargetObjectID(pPacket->getTargetObjectID());
                gcPartyInvite.setCode(GC_PARTY_INVITE_MEMBER_FULL);
                pPlayer->sendPacket(&gcPartyInvite);
                return;
            }
        }

        // cout << "Sending Packet" << endl;

        
        gcPartyInvite.setTargetObjectID(pCreature->getObjectID());
        gcPartyInvite.setCode(GC_PARTY_INVITE_REQUEST);
        pTargetPlayer->sendPacket(&gcPartyInvite);

        // cout << "Sent gcPartyInvite" << endl;

        
        pPIIM->initInviteInfo(pCreature, pTargetCreature);

        // cout << "initInviteInfo OK" << endl;
    }
    
    else if (CODE == CG_PARTY_INVITE_CANCEL) {
        
        if (pPIIM->isInviting(pCreature, pTargetCreature)) {
            
            gcPartyInvite.setTargetObjectID(pCreature->getObjectID());
            gcPartyInvite.setCode(GC_PARTY_INVITE_CANCEL);
            pTargetPlayer->sendPacket(&gcPartyInvite);

            
            pPIIM->cancelInvite(pCreature, pTargetCreature);
        } else {
            executeError(pPacket, pPlayer, GC_PARTY_ERROR_NOT_INVITING);
            return;
        }
    }
    
    else if (CODE == CG_PARTY_INVITE_ACCEPT) {
        if (pPIIM->isInviting(pCreature, pTargetCreature)) {
            int PartyID = pCreature->getPartyID();
            int TargetPartyID = pTargetCreature->getPartyID();

            
            
            if (TargetPartyID != 0 && PartyID == 0) {
                
                if (g_pGlobalPartyManager->addPartyMember(TargetPartyID, pCreature)) {
                    
                    LocalPartyManager* pLocalPartyManager = pZone->getLocalPartyManager();
                    pLocalPartyManager->addPartyMember(TargetPartyID, pCreature);
                }
            }
            
            
            
            else if (PartyID != 0 && TargetPartyID == 0) {
                
                if (g_pGlobalPartyManager->addPartyMember(PartyID, pTargetCreature)) {
                    
                    LocalPartyManager* pLocalPartyManager = pZone->getLocalPartyManager();
                    pLocalPartyManager->addPartyMember(PartyID, pTargetCreature);
                }
            }
            
            
            else {
                
                int NewPartyID = g_pGlobalPartyManager->registerParty();

                
                g_pGlobalPartyManager->createParty(NewPartyID, pTargetCreature->getCreatureClass());
                g_pGlobalPartyManager->addPartyMember(NewPartyID, pCreature);
                g_pGlobalPartyManager->addPartyMember(NewPartyID, pTargetCreature);

                
                LocalPartyManager* pLocalPartyManager = pZone->getLocalPartyManager();
                pLocalPartyManager->createParty(NewPartyID, pTargetCreature->getCreatureClass());
                pLocalPartyManager->addPartyMember(NewPartyID, pCreature);
                pLocalPartyManager->addPartyMember(NewPartyID, pTargetCreature);

                if (pCreature->getLevel() >= 25 && pTargetCreature->getLevel() < 25) {
                    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
                    pPC->getGQuestManager()->eventParty();
                } else if (pTargetCreature->getLevel() >= 25 && pCreature->getLevel() < 25) {
                    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pTargetCreature);
                    pPC->getGQuestManager()->eventParty();
                }
            }

            
            pPIIM->cancelInvite(pCreature, pTargetCreature);
        } else {
            executeError(pPacket, pPlayer, GC_PARTY_ERROR_NOT_INVITING);
            return;
        }
    }
    
    else if (CODE == CG_PARTY_INVITE_REJECT) {
        if (pPIIM->isInviting(pCreature, pTargetCreature)) {
            
            gcPartyInvite.setTargetObjectID(pCreature->getObjectID());
            gcPartyInvite.setCode(GC_PARTY_INVITE_REJECT);
            pTargetPlayer->sendPacket(&gcPartyInvite);

            
            pPIIM->cancelInvite(pCreature, pTargetCreature);
        } else {
            executeError(pPacket, pPlayer, GC_PARTY_ERROR_NOT_INVITING);
            return;
        }
    } else if (CODE == CG_PARTY_INVITE_BUSY) {
        gcPartyInvite.setTargetObjectID(pPacket->getTargetObjectID());
        gcPartyInvite.setCode(GC_PARTY_INVITE_BUSY);
        pPlayer->sendPacket(&gcPartyInvite);
    } else {
        throw ProtocolException("CGPartyInvite::execute() : Unknown Code");
    }

#endif

    __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGPartyInviteHandler::executeError(CGPartyInvite* pPacket, Player* pPlayer, BYTE ErrorCode)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        GCPartyError gcPartyError;
    gcPartyError.setTargetObjectID(pPacket->getTargetObjectID());
    gcPartyError.setCode(ErrorCode);
    pPlayer->sendPacket(&gcPartyError);

#endif

    __END_DEBUG_EX __END_CATCH
}
