//----------------------------------------------------------------------
//
// Filename    : SGExpelGuildMemberOKHandler.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "Properties.h"
#include "SGExpelGuildMemberOK.h"

#ifdef __GAME_SERVER__

#include <stdio.h>

#include "GCModifyGuildMemberInfo.h"
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
// SGExpelGuildMemberOKHandler::execute()
//
//----------------------------------------------------------------------
void SGExpelGuildMemberOKHandler::execute(SGExpelGuildMemberOK* pPacket)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        // cout << "SGExpelGuildMember received" << endl;

        Assert(pPacket != NULL);

    
    Guild* pGuild = g_pGuildManager->getGuild(pPacket->getGuildID());
    try {
        Assert(pGuild != NULL);
    } catch (Throwable&) {
        return;
    }

    
    GuildMember* pGuildMember = pGuild->getMember(pPacket->getName());
    try {
        Assert(pGuildMember != NULL);
    } catch (Throwable&) {
        return;
    }

    if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_WAIT) {
        //////////////////////////////////////////////////////////
        
        //////////////////////////////////////////////////////////

        
        pGuild->deleteMember(pGuildMember->getName());

        
        __ENTER_CRITICAL_SECTION((*g_pPCFinder))

        Creature* pCreature = g_pPCFinder->getCreature_LOCKED(pPacket->getName());
        if (pCreature != NULL && pCreature->isPC()) {
            Player* pPlayer = pCreature->getPlayer();
            Assert(pPlayer != NULL);

            //			StringStream msg;
            

            char msg[100];
            if (pGuild->getRace() == Guild::GUILD_RACE_SLAYER)
                sprintf(msg, g_pStringPool->c_str(STRID_TEAM_JOIN_DENY), pGuild->getName().c_str());
            else if (pGuild->getRace() == Guild::GUILD_RACE_VAMPIRE)
                sprintf(msg, g_pStringPool->c_str(STRID_CLAN_JOIN_DENY), pGuild->getName().c_str());
            else if (pGuild->getRace() == Guild::GUILD_RACE_OUSTERS)
                sprintf(msg, g_pStringPool->c_str(STRID_CLAN_JOIN_DENY), pGuild->getName().c_str());
            
            GCSystemMessage gcSystemMessage;
            gcSystemMessage.setMessage(msg);
            pPlayer->sendPacket(&gcSystemMessage);
        }

        
        pCreature = g_pPCFinder->getCreature_LOCKED(pPacket->getSender());
        if (pCreature != NULL && pCreature->isPC()) {
            Player* pPlayer = pCreature->getPlayer();
            Assert(pPlayer != NULL);

            //			StringStream msg;
            

            char msg[100];
            if (pGuild->getRace() == Guild::GUILD_RACE_SLAYER)
                sprintf(msg, g_pStringPool->c_str(STRID_TEAM_JOIN_DENY_2), pPacket->getName().c_str());
            else if (pGuild->getRace() == Guild::GUILD_RACE_VAMPIRE)
                sprintf(msg, g_pStringPool->c_str(STRID_CLAN_JOIN_DENY_2), pPacket->getName().c_str());
            else if (pGuild->getRace() == Guild::GUILD_RACE_OUSTERS)
                sprintf(msg, g_pStringPool->c_str(STRID_CLAN_JOIN_DENY_2), pPacket->getName().c_str());

            GCSystemMessage gcSystemMessage;
            gcSystemMessage.setMessage(msg);
            pPlayer->sendPacket(&gcSystemMessage);
        }

        __LEAVE_CRITICAL_SECTION((*g_pPCFinder))
    } else {
        ///////////////////////////////////////////////////////////
        
        ///////////////////////////////////////////////////////////

        
        pGuild->deleteMember(pGuildMember->getName());

        
        __ENTER_CRITICAL_SECTION((*g_pPCFinder))

        Creature* pCreature = g_pPCFinder->getCreature_LOCKED(pPacket->getName());
        if (pCreature != NULL && pCreature->isPC()) {
            Player* pPlayer = pCreature->getPlayer();
            Assert(pPlayer != NULL);

            PlayerCreature* pPlayerCreature = dynamic_cast<PlayerCreature*>(pCreature);
            Assert(pPlayerCreature != NULL);

            if (pPlayerCreature->isSlayer()) {
                pPlayerCreature->setGuildID(99); 

                
                GCModifyGuildMemberInfo gcModifyGuildMember;
                gcModifyGuildMember.setGuildID(pPlayerCreature->getGuildID());
                gcModifyGuildMember.setGuildName("");
                gcModifyGuildMember.setGuildMemberRank(GuildMember::GUILDMEMBER_RANK_DENY);
                pPlayer->sendPacket(&gcModifyGuildMember);
            } else if (pPlayerCreature->isVampire()) {
                pPlayerCreature->setGuildID(0); 

                
                GCModifyGuildMemberInfo gcModifyGuildMember;
                gcModifyGuildMember.setGuildID(pPlayerCreature->getGuildID());
                gcModifyGuildMember.setGuildName("");
                gcModifyGuildMember.setGuildMemberRank(GuildMember::GUILDMEMBER_RANK_DENY);
                pPlayer->sendPacket(&gcModifyGuildMember);
            } else if (pPlayerCreature->isOusters()) {
                pPlayerCreature->setGuildID(66); 

                
                GCModifyGuildMemberInfo gcModifyGuildMember;
                gcModifyGuildMember.setGuildID(pPlayerCreature->getGuildID());
                gcModifyGuildMember.setGuildName("");
                gcModifyGuildMember.setGuildMemberRank(GuildMember::GUILDMEMBER_RANK_DENY);
                pPlayer->sendPacket(&gcModifyGuildMember);
            }

            
            GCSystemMessage gcSystemMessage;
            

            if (pGuild->getRace() == Guild::GUILD_RACE_SLAYER)
                gcSystemMessage.setMessage(g_pStringPool->getString(STRID_EXPEL_TEAM_MEMBER));
            else if (pGuild->getRace() == Guild::GUILD_RACE_VAMPIRE)
                gcSystemMessage.setMessage(g_pStringPool->getString(STRID_EXPEL_CLAN_MEMBER));
            else if (pGuild->getRace() == Guild::GUILD_RACE_OUSTERS)
                gcSystemMessage.setMessage(g_pStringPool->getString(STRID_EXPEL_CLAN_MEMBER));

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

        
        pCreature = g_pPCFinder->getCreature_LOCKED(pPacket->getSender());
        if (pCreature != NULL && pCreature->isPC()) {
            Player* pPlayer = pCreature->getPlayer();
            Assert(pPlayer != NULL);

            //			StringStream msg;
            

            char msg[100];
            if (pGuild->getRace() == Guild::GUILD_RACE_SLAYER)
                sprintf(msg, g_pStringPool->c_str(STRID_EXPEL_TEAM_MEMBER_2), pPacket->getName().c_str());
            else if (pGuild->getRace() == Guild::GUILD_RACE_VAMPIRE)
                sprintf(msg, g_pStringPool->c_str(STRID_EXPEL_CLAN_MEMBER_2), pPacket->getName().c_str());
            else if (pGuild->getRace() == Guild::GUILD_RACE_OUSTERS)
                sprintf(msg, g_pStringPool->c_str(STRID_EXPEL_CLAN_MEMBER_2), pPacket->getName().c_str());

            GCSystemMessage gcSystemMessage;
            gcSystemMessage.setMessage(msg);
            pPlayer->sendPacket(&gcSystemMessage);
        }

        __LEAVE_CRITICAL_SECTION((*g_pPCFinder))
    }

#endif

    __END_DEBUG_EX __END_CATCH
}
