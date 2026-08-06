//----------------------------------------------------------------------
//
// Filename    : SGQuitGuildOKHandler.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "Properties.h"
#include "SGQuitGuildOK.h"

#ifdef __GAME_SERVER__

#include <stdio.h>

#include "GCModifyGuildMemberInfo.h"
#include "GCModifyInformation.h"
#include "GCOtherModifyInfo.h"
#include "GCSystemMessage.h"
#include "Guild.h"
#include "GuildManager.h"
#include "PCFinder.h"
#include "Player.h"
#include "PlayerCreature.h"
#include "StringPool.h"
#include "Zone.h"

#endif

//----------------------------------------------------------------------
//
// SGQuitGuildOKHandler::execute()
//
//----------------------------------------------------------------------
void SGQuitGuildOKHandler::execute(SGQuitGuildOK* pPacket)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);

    
    Guild* pGuild = g_pGuildManager->getGuild(pPacket->getGuildID());
    // try { Assert(pGuild != NULL); } catch (Throwable& ) { return; }
    if (pGuild == NULL)
        return;

    
    GuildMember* pGuildMember = pGuild->getMember(pPacket->getName());
    // try { Assert(pGuildMember != NULL); } catch (Throwable& ) { return; }
    if (pGuildMember == NULL)
        return;

    string memberName = pGuildMember->getName();

    
    __ENTER_CRITICAL_SECTION((*g_pPCFinder))

    Creature* pCreature = g_pPCFinder->getCreature_LOCKED(memberName);
    if (pCreature != NULL && pCreature->isPC()) {
        Player* pPlayer = pCreature->getPlayer();
        Assert(pPlayer != NULL);

        PlayerCreature* pPlayerCreature = dynamic_cast<PlayerCreature*>(pCreature);
        Assert(pPlayerCreature != NULL);

        if (pGuild->getState() == Guild::GUILD_STATE_ACTIVE) {
            ////////////////////////////////////////////////////////////////////////////////
            
            ////////////////////////////////////////////////////////////////////////////////
            if (pPlayerCreature->isSlayer()) {
                pPlayerCreature->setGuildID(99); 

                
                GCModifyGuildMemberInfo gcModifyGuildMemberInfo;
                gcModifyGuildMemberInfo.setGuildID(pGuild->getID());
                gcModifyGuildMemberInfo.setGuildName(pGuild->getName());
                gcModifyGuildMemberInfo.setGuildMemberRank(pGuildMember->getRank());
                pPlayer->sendPacket(&gcModifyGuildMemberInfo);
            } else if (pPlayerCreature->isVampire()) {
                pPlayerCreature->setGuildID(0); 

                
                GCModifyGuildMemberInfo gcModifyGuildMemberInfo;
                gcModifyGuildMemberInfo.setGuildID(pGuild->getID());
                gcModifyGuildMemberInfo.setGuildName(pGuild->getName());
                gcModifyGuildMemberInfo.setGuildMemberRank(pGuildMember->getRank());
                pPlayer->sendPacket(&gcModifyGuildMemberInfo);
            } else if (pPlayerCreature->isOusters()) {
                pPlayerCreature->setGuildID(66); 

                
                GCModifyGuildMemberInfo gcModifyGuildMemberInfo;
                gcModifyGuildMemberInfo.setGuildID(pGuild->getID());
                gcModifyGuildMemberInfo.setGuildName(pGuild->getName());
                gcModifyGuildMemberInfo.setGuildMemberRank(pGuildMember->getRank());
                pPlayer->sendPacket(&gcModifyGuildMemberInfo);
            }
        }

        if (pGuild->getState() == Guild::GUILD_STATE_WAIT &&
            pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_SUBMASTER) {
            ///////////////////////////////////////////////////////////
            
            ///////////////////////////////////////////////////////////
            Gold_t Gold = pPlayerCreature->getGold();
            Gold = min((uint64_t)(Gold + RETURN_SLAYER_SUBMASTER_GOLD), (uint64_t)2000000000);
            pPlayerCreature->setGoldEx(Gold);

            GCModifyInformation gcModifyInformation;
            gcModifyInformation.addLongData(MODIFY_GOLD, Gold);
            pPlayer->sendPacket(&gcModifyInformation);
        }

        
        GCSystemMessage gcSystemMessage;
        if (pGuild->getRace() == Guild::GUILD_RACE_SLAYER)
            gcSystemMessage.setMessage(g_pStringPool->getString(STRID_QUIT_TEAM));
        else if (pGuild->getRace() == Guild::GUILD_RACE_VAMPIRE)
            gcSystemMessage.setMessage(g_pStringPool->getString(STRID_QUIT_CLAN));
        else if (pGuild->getRace() == Guild::GUILD_RACE_OUSTERS)
            gcSystemMessage.setMessage(g_pStringPool->getString(STRID_QUIT_CLAN));
        pPlayer->sendPacket(&gcSystemMessage);

        if (pGuild->getState() == Guild::GUILD_STATE_ACTIVE) {
            
            Zone* pZone = pCreature->getZone();
            Assert(pZone != NULL);

            GCOtherModifyInfo gcOtherModifyInfo;
            gcOtherModifyInfo.setObjectID(pCreature->getObjectID());
            gcOtherModifyInfo.addShortData(MODIFY_GUILDID, pPlayerCreature->getGuildID());

            pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcOtherModifyInfo, pCreature);
        }
    }

    
    pGuild->deleteMember(memberName);

    
    pCreature = g_pPCFinder->getCreature_LOCKED(pGuild->getMaster());
    if (pCreature != NULL && pCreature->isPC()) {
        Player* pPlayer = pCreature->getPlayer();
        Assert(pPlayer != NULL);

        //		StringStream msg;
        

        char msg[100];
        if (pGuild->getRace() == Guild::GUILD_RACE_SLAYER)
            sprintf(msg, g_pStringPool->c_str(STRID_QUIT_TEAM_2), memberName.c_str());
        else if (pGuild->getRace() == Guild::GUILD_RACE_VAMPIRE)
            sprintf(msg, g_pStringPool->c_str(STRID_QUIT_CLAN_2), memberName.c_str());
        else if (pGuild->getRace() == Guild::GUILD_RACE_OUSTERS)
            sprintf(msg, g_pStringPool->c_str(STRID_QUIT_CLAN_2), memberName.c_str());

        GCSystemMessage gcSystemMessage;
        gcSystemMessage.setMessage(msg);
        pPlayer->sendPacket(&gcSystemMessage);
    } else {
        
    }

    __LEAVE_CRITICAL_SECTION((*g_pPCFinder))


#endif

    __END_DEBUG_EX __END_CATCH
}
