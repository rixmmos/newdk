//----------------------------------------------------------------------
//
// Filename    : SGAddGuildMemberOKHandler.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "Properties.h"
#include "SGAddGuildMemberOK.h"

#ifdef __GAME_SERVER__

#include <stdio.h>

#include "DB.h"
#include "GCModifyInformation.h"
#include "GCOtherModifyInfo.h"
#include "GCSystemMessage.h"
#include "Guild.h"
#include "GuildManager.h"
#include "PCFinder.h"
#include "Player.h"
#include "PlayerCreature.h"
#include "Properties.h"
#include "StringPool.h"
#include "StringStream.h"
#include "Zone.h"

#endif

//----------------------------------------------------------------------
//
// SGAddGuildMemberOKHandler::execute()
//
//----------------------------------------------------------------------
void SGAddGuildMemberOKHandler::execute(SGAddGuildMemberOK* pPacket)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        
        GuildMember* pGuildMember = new GuildMember();
    pGuildMember->setGuildID(pPacket->getGuildID());
    pGuildMember->setName(pPacket->getName());
    pGuildMember->setRank(pPacket->getGuildMemberRank());

    
    Guild* pGuild = g_pGuildManager->getGuild(pGuildMember->getGuildID());
    pGuild->addMember(pGuildMember);

    
    __ENTER_CRITICAL_SECTION((*g_pPCFinder))

    Creature* pCreature = g_pPCFinder->getCreature_LOCKED(pGuildMember->getName());
    if (pCreature != NULL && pCreature->isPC()) {
        Player* pPlayer = pCreature->getPlayer();
        Assert(pPlayer != NULL);

        if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_MASTER ||
            pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_SUBMASTER) 
        {
            PlayerCreature* pPlayerCreature = dynamic_cast<PlayerCreature*>(pCreature);
            Assert(pPlayerCreature != NULL);

            Gold_t Fee;
            if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_MASTER)
                Fee = REQUIRE_SLAYER_MASTER_GOLD;
            else if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_SUBMASTER)
                Fee = REQUIRE_SLAYER_SUBMASTER_GOLD;
            else
                Fee = 0;

            Gold_t CurMoney = pPlayerCreature->getGold();
            if (CurMoney < Fee) {
                
                CurMoney = 0;
            } else
                CurMoney -= Fee;

            pPlayerCreature->setGoldEx(CurMoney);

            if (Fee != 0) {
                GCModifyInformation gcModifyInformation;
                gcModifyInformation.addLongData(MODIFY_GOLD, CurMoney);

                
                pPlayer->sendPacket(&gcModifyInformation);
            }

            
            GCSystemMessage gcSystemMessage;
            if (pGuild->getRace() == Guild::GUILD_RACE_SLAYER)
                gcSystemMessage.setMessage(g_pStringPool->getString(STRID_TEAM_JOIN_ACCEPTED));
            else if (pGuild->getRace() == Guild::GUILD_RACE_VAMPIRE)
                gcSystemMessage.setMessage(g_pStringPool->getString(STRID_CLAN_JOIN_ACCEPTED));
            else if (pGuild->getRace() == Guild::GUILD_RACE_OUSTERS)
                gcSystemMessage.setMessage(g_pStringPool->getString(STRID_CLAN_JOIN_ACCEPTED));
            pPlayer->sendPacket(&gcSystemMessage);

        } else if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_WAIT) {
            
            GCSystemMessage gcSystemMessage;
            if (pGuild->getRace() == Guild::GUILD_RACE_SLAYER)
                gcSystemMessage.setMessage(g_pStringPool->getString(STRID_TEAM_JOIN_TRY));
            else if (pGuild->getRace() == Guild::GUILD_RACE_VAMPIRE)
                gcSystemMessage.setMessage(g_pStringPool->getString(STRID_CLAN_JOIN_TRY));
            else if (pGuild->getRace() == Guild::GUILD_RACE_OUSTERS)
                gcSystemMessage.setMessage(g_pStringPool->getString(STRID_CLAN_JOIN_TRY));

            pPlayer->sendPacket(&gcSystemMessage);
        }
    } else {
        

        
        
        if ((pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_MASTER ||
             pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_SUBMASTER) 
            &&
            pPacket->getServerGroupID() == g_pConfig->getPropertyInt("ServerID")) 
        {
            Gold_t Fee;
            if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_MASTER)
                Fee = REQUIRE_SLAYER_MASTER_GOLD;
            else if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_SUBMASTER)
                Fee = REQUIRE_SLAYER_SUBMASTER_GOLD;
            else
                Fee = 0;

            string table = "";
            if (pGuild->getRace() == Guild::GUILD_RACE_SLAYER) {
                table = "Slayer";
            } else if (pGuild->getRace() == Guild::GUILD_RACE_VAMPIRE) {
                table = "Vampire";
            } else if (pGuild->getRace() == Guild::GUILD_RACE_OUSTERS) {
                table = "Ousters";
            }

            if (table != "" && Fee != 0) {
                Statement* pStmt = NULL;

                BEGIN_DB {
                    pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
                    pStmt->executeQuery("UPDATE %s SET Gold = IF (%u > Gold , 0, Gold - %u ) WHERE Name = '%s'",
                                        table.c_str(), Fee, Fee, pGuildMember->getName().c_str());
                }
                END_DB(pStmt)
            }
        }
    }

    
    pCreature = g_pPCFinder->getCreature_LOCKED(pGuild->getMaster());
    if (pCreature != NULL && pCreature->isPC() && pGuildMember->getRank() != GuildMember::GUILDMEMBER_RANK_MASTER) {
        Player* pPlayer = pCreature->getPlayer();
        Assert(pPlayer != NULL);

        char msg[100];

        if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_SUBMASTER) {
            if (pGuild->getRace() == Guild::GUILD_RACE_SLAYER)
                sprintf(msg, g_pStringPool->c_str(STRID_TEAM_JOIN_ACCEPTED_2), pGuildMember->getName().c_str());
            else if (pGuild->getRace() == Guild::GUILD_RACE_VAMPIRE)
                sprintf(msg, g_pStringPool->c_str(STRID_CLAN_JOIN_ACCEPTED_2), pGuildMember->getName().c_str());
            else if (pGuild->getRace() == Guild::GUILD_RACE_OUSTERS)
                sprintf(msg, g_pStringPool->c_str(STRID_CLAN_JOIN_ACCEPTED_2), pGuildMember->getName().c_str());
        } else if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_WAIT) {
            if (pGuild->getRace() == Guild::GUILD_RACE_SLAYER)
                sprintf(msg, g_pStringPool->c_str(STRID_TEAM_JOIN_TRY_2), pGuildMember->getName().c_str());
            else if (pGuild->getRace() == Guild::GUILD_RACE_VAMPIRE)
                sprintf(msg, g_pStringPool->c_str(STRID_CLAN_JOIN_TRY_2), pGuildMember->getName().c_str());
            else if (pGuild->getRace() == Guild::GUILD_RACE_OUSTERS)
                sprintf(msg, g_pStringPool->c_str(STRID_CLAN_JOIN_TRY_2), pGuildMember->getName().c_str());
        }

        GCSystemMessage gcSystemMessage;
        gcSystemMessage.setMessage(msg);
        pPlayer->sendPacket(&gcSystemMessage);
    }

    __LEAVE_CRITICAL_SECTION((*g_pPCFinder))

#endif

    __END_DEBUG_EX __END_CATCH
}
