//----------------------------------------------------------------------
//
// Filename    : SGModifyGuildMemberOKHandler.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "Properties.h"
#include "SGModifyGuildMemberOK.h"

#ifdef __GAME_SERVER__

#include <stdio.h>

#include "DB.h"
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
// SGModifyGuildMemberOKHandler::execute()
//
//----------------------------------------------------------------------
void SGModifyGuildMemberOKHandler::execute(SGModifyGuildMemberOK* pPacket)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        // cout << "SGModifyGuildMember received" << endl;

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

    if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_WAIT &&
        pPacket->getGuildMemberRank() == GuildMember::GUILDMEMBER_RANK_NORMAL) {
        ////////////////////////////////////////////////////////////////////////////
        
        ////////////////////////////////////////////////////////////////////////////

        
        pGuild->modifyMemberRank(pGuildMember->getName(), pPacket->getGuildMemberRank());

        
        __ENTER_CRITICAL_SECTION((*g_pPCFinder))

        Creature* pCreature = g_pPCFinder->getCreature_LOCKED(pGuildMember->getName());
        if (pCreature != NULL && pCreature->isPC()) {
            PlayerCreature* pPlayerCreature = dynamic_cast<PlayerCreature*>(pCreature);
            Assert(pPlayerCreature != NULL);

            
            pPlayerCreature->setGuildID(pGuildMember->getGuildID());

            Player* pPlayer = pCreature->getPlayer();
            Assert(pPlayer != NULL);

            Zone* pZone = pCreature->getZone();
            Assert(pZone != NULL);

            
            GCModifyGuildMemberInfo gcModifyGuildMemberInfo;
            gcModifyGuildMemberInfo.setGuildID(pGuild->getID());
            gcModifyGuildMemberInfo.setGuildName(pGuild->getName());
            gcModifyGuildMemberInfo.setGuildMemberRank(pGuildMember->getRank());
            pPlayer->sendPacket(&gcModifyGuildMemberInfo);

            
            Statement* pStmt = NULL;
            Result* pResult = NULL;

            BEGIN_DB {
                pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
                pResult = pStmt->executeQuery("SELECT Message FROM Messages WHERE Receiver = '%s'",
                                              pGuildMember->getName().c_str());

                while (pResult->next()) {
                    GCSystemMessage gcSystemMessage;
                    gcSystemMessage.setMessage(pResult->getString(1));
                    pPlayer->sendPacket(&gcSystemMessage);
                }

                pStmt->executeQuery("DELETE FROM Messages WHERE Receiver = '%s'", pGuildMember->getName().c_str());

                SAFE_DELETE(pStmt);
            }
            END_DB(pStmt)

            
            GCOtherModifyInfo gcOtherModifyInfo;
            gcOtherModifyInfo.setObjectID(pCreature->getObjectID());
            gcOtherModifyInfo.addShortData(MODIFY_GUILDID, pGuildMember->getGuildID());

            pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcOtherModifyInfo);
        }
        
        pCreature = g_pPCFinder->getCreature_LOCKED(pPacket->getSender());
        if (pCreature != NULL && pCreature->isPC()) {
            Player* pPlayer = pCreature->getPlayer();
            Assert(pPlayer != NULL);

            //			StringStream msg;
            

            char msg[100];
            if (pGuild->getRace() == Guild::GUILD_RACE_SLAYER)
                sprintf(msg, g_pStringPool->c_str(STRID_ACCEPT_TEAM_JOIN), pGuildMember->getName().c_str());
            else if (pGuild->getRace() == Guild::GUILD_RACE_VAMPIRE)
                sprintf(msg, g_pStringPool->c_str(STRID_ACCEPT_CLAN_JOIN), pGuildMember->getName().c_str());
            else if (pGuild->getRace() == Guild::GUILD_RACE_OUSTERS)
                sprintf(msg, g_pStringPool->c_str(STRID_ACCEPT_CLAN_JOIN), pGuildMember->getName().c_str());

            GCSystemMessage gcSystemMessage;
            gcSystemMessage.setMessage(msg);
            pPlayer->sendPacket(&gcSystemMessage);
        }

        __LEAVE_CRITICAL_SECTION((*g_pPCFinder))
    } else if (pGuildMember->getRank() != GuildMember::GUILDMEMBER_RANK_MASTER &&
               pPacket->getGuildMemberRank() == GuildMember::GUILDMEMBER_RANK_MASTER) {
        ///////////////////////////////////////////////////////////
        
        ///////////////////////////////////////////////////////////

        string sMaster = pGuild->getMaster();

        
        pGuild->modifyMemberRank(sMaster, pGuildMember->getRank());
        
        pGuild->modifyMemberRank(pGuildMember->getName(), pPacket->getGuildMemberRank());
        
        pGuild->setMaster(pGuildMember->getName());

        
        __ENTER_CRITICAL_SECTION((*g_pPCFinder))

        
        Creature* pCreature = g_pPCFinder->getCreature_LOCKED(pGuildMember->getName());
        if (pCreature != NULL && pCreature->isPC()) {
            PlayerCreature* pPlayerCreature = dynamic_cast<PlayerCreature*>(pCreature);
            Assert(pPlayerCreature != NULL);

            Player* pPlayer = pCreature->getPlayer();
            Assert(pPlayer != NULL);

            
            GCModifyGuildMemberInfo gcModifyGuildMemberInfo;
            gcModifyGuildMemberInfo.setGuildID(pGuild->getID());
            gcModifyGuildMemberInfo.setGuildName(pGuild->getName());
            gcModifyGuildMemberInfo.setGuildMemberRank(pGuildMember->getRank());
            pPlayer->sendPacket(&gcModifyGuildMemberInfo);
        }

        
        pCreature = g_pPCFinder->getCreature_LOCKED(sMaster);
        if (pCreature != NULL && pCreature->isPC()) {
            PlayerCreature* pPlayerCreature = dynamic_cast<PlayerCreature*>(pCreature);
            Assert(pPlayerCreature != NULL);

            Player* pPlayer = pCreature->getPlayer();
            Assert(pPlayer != NULL);

            
            GCModifyGuildMemberInfo gcModifyGuildMemberInfo;
            gcModifyGuildMemberInfo.setGuildID(pGuild->getID());
            gcModifyGuildMemberInfo.setGuildName(pGuild->getName());
            gcModifyGuildMemberInfo.setGuildMemberRank(pGuildMember->getRank());
            pPlayer->sendPacket(&gcModifyGuildMemberInfo);
        }

        
        pCreature = g_pPCFinder->getCreature_LOCKED(pPacket->getSender());
        if (pCreature != NULL && pCreature->isPC()) {
            Player* pPlayer = pCreature->getPlayer();
            Assert(pPlayer != NULL);

            //			StringStream msg;
            
            

            char msg[200];
            sprintf(msg, g_pStringPool->c_str(STRID_MODIFY_GUILD_MASTER), pGuild->getName().c_str(), sMaster.c_str(),
                    pGuildMember->getName().c_str());

            GCSystemMessage gcSystemMessage;
            gcSystemMessage.setMessage(msg);
            pPlayer->sendPacket(&gcSystemMessage);
        }

        __LEAVE_CRITICAL_SECTION((*g_pPCFinder))
    } else {
        ///////////////////////////////////////////////////////////
        
        ///////////////////////////////////////////////////////////
        pGuild->modifyMemberRank(pGuildMember->getName(), pPacket->getGuildMemberRank());

        
        __ENTER_CRITICAL_SECTION((*g_pPCFinder))

        Creature* pCreature = g_pPCFinder->getCreature_LOCKED(pGuildMember->getName());
        if (pCreature != NULL && pCreature->isPC()) {
            Player* pPlayer = pCreature->getPlayer();
            Assert(pPlayer != NULL);

            GCSystemMessage gcSystemMessage;
            if (pGuild->getRace() == Guild::GUILD_RACE_SLAYER)
                gcSystemMessage.setMessage(g_pStringPool->getString(STRID_TEAM_RIGHT_CHANGED));
            else if (pGuild->getRace() == Guild::GUILD_RACE_VAMPIRE)
                gcSystemMessage.setMessage(g_pStringPool->getString(STRID_CLAN_RIGHT_CHANGED));
            else if (pGuild->getRace() == Guild::GUILD_RACE_OUSTERS)
                gcSystemMessage.setMessage(g_pStringPool->getString(STRID_CLAN_RIGHT_CHANGED));

            pPlayer->sendPacket(&gcSystemMessage);
        }

        
        pCreature = g_pPCFinder->getCreature_LOCKED(pPacket->getSender());
        if (pCreature != NULL && pCreature->isPC()) {
            Player* pPlayer = pCreature->getPlayer();
            Assert(pPlayer != NULL);

            //			StringStream msg;
            

            char msg[100];
            if (pGuild->getRace() == Guild::GUILD_RACE_SLAYER)
                sprintf(msg, g_pStringPool->c_str(STRID_TEAM_RIGHT_CHANGED_2), pGuildMember->getName().c_str());
            else if (pGuild->getRace() == Guild::GUILD_RACE_VAMPIRE)
                sprintf(msg, g_pStringPool->c_str(STRID_CLAN_RIGHT_CHANGED_2), pGuildMember->getName().c_str());
            else if (pGuild->getRace() == Guild::GUILD_RACE_OUSTERS)
                sprintf(msg, g_pStringPool->c_str(STRID_CLAN_RIGHT_CHANGED_2), pGuildMember->getName().c_str());


            GCSystemMessage gcSystemMessage;
            gcSystemMessage.setMessage(msg);
            pPlayer->sendPacket(&gcSystemMessage);
        }

        __LEAVE_CRITICAL_SECTION((*g_pPCFinder))
    }

#endif

    __END_DEBUG_EX __END_CATCH
}
