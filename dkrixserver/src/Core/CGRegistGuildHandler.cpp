//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRegistGuildHandler.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGRegistGuild.h"

#ifdef __GAME_SERVER__
#include <stdio.h>

#include "DB.h"
#include "GCNPCResponse.h"
#include "GSAddGuild.h"
#include "GamePlayer.h"
#include "Guild.h"
#include "Ousters.h"
#include "PreparedStatement.h"
#include "Properties.h"
#include "SharedServerManager.h"
#include "Slayer.h"
#include "SystemAvailabilitiesManager.h"
#include "Vampire.h"
#include "VariableManager.h"

#endif // __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGRegistGuildHandler::execute(CGRegistGuild* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    SYSTEM_ASSERT(SYSTEM_GUILD);

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    Assert(pGamePlayer != NULL);

    Creature* pCreature = pGamePlayer->getCreature();
    Assert(pCreature != NULL);

    string GuildName = pPacket->getGuildName();

    if (GuildName.find_first_of("'\\") < GuildName.size()) {
        
        if (pCreature->isSlayer()) {
            GCNPCResponse response;
            response.setCode(NPC_RESPONSE_TEAM_REGIST_FAIL_NAME);
            pPlayer->sendPacket(&response);
        } else if (pCreature->isVampire()) {
            GCNPCResponse response;
            response.setCode(NPC_RESPONSE_CLAN_REGIST_FAIL_NAME);
            pPlayer->sendPacket(&response);
        } else if (pCreature->isOusters()) {
            GCNPCResponse response;
            response.setCode(NPC_RESPONSE_GUILD_REGIST_FAIL_NAME);
            pPlayer->sendPacket(&response);
        }

        return;
    }

    Statement* pStmt = NULL;
    Result* pResult;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement guildNameStmt(
            pConn, "SELECT GuildID FROM GuildInfo WHERE GuildName = ? AND GuildState IN ( 0, 1 )");
        guildNameStmt.bindString(1, pPacket->getGuildName());
        pResult = guildNameStmt.execute();

        if (pResult->getRowCount() != 0) {

            if (pCreature->isSlayer()) {
                GCNPCResponse response;
                response.setCode(NPC_RESPONSE_TEAM_REGIST_FAIL_NAME);
                pPlayer->sendPacket(&response);
            } else if (pCreature->isVampire()) {
                GCNPCResponse response;
                response.setCode(NPC_RESPONSE_CLAN_REGIST_FAIL_NAME);
                pPlayer->sendPacket(&response);
            } else if (pCreature->isOusters()) {
                GCNPCResponse response;
                response.setCode(NPC_RESPONSE_GUILD_REGIST_FAIL_NAME);
                pPlayer->sendPacket(&response);
            }

            return;
        }

        PreparedStatement memberRankStmt(pConn, "SELECT `Rank`, ExpireDate FROM GuildMember WHERE Name = ?");
        memberRankStmt.bindString(1, pCreature->getName());
        pResult = memberRankStmt.execute();

        if (pResult->next()) {
            int Rank = pResult->getInt(1);
            string ExpireDate = pResult->getString(2);

            
            if (ExpireDate.size() == 7 && Rank == GuildMember::GUILDMEMBER_RANK_LEAVE) {
                time_t daytime = time(0);
                tm Time;
                Time.tm_year = atoi(ExpireDate.substr(0, 3).c_str());
                Time.tm_mon = atoi(ExpireDate.substr(3, 2).c_str());
                Time.tm_mday = atoi(ExpireDate.substr(5, 2).c_str());

                
                if (difftime(daytime, mktime(&Time)) <
                    g_pVariableManager->getVariable(QUIT_GUILD_PENALTY_TERM) * 24 * 3600) 
                {
                    
                    GCNPCResponse response;

                    if (pCreature->isSlayer())
                        response.setCode(NPC_RESPONSE_TEAM_REGIST_FAIL_DENY);
                    else if (pCreature->isVampire())
                        response.setCode(NPC_RESPONSE_CLAN_REGIST_FAIL_DENY);
                    else if (pCreature->isOusters())
                        response.setCode(NPC_RESPONSE_GUILD_REGIST_FAIL_DENY);

                    pPlayer->sendPacket(&response);

                    return;
                }
            }

            PreparedStatement deleteMemberStmt(pConn, "DELETE FROM GuildMember WHERE Name=?");
            deleteMemberStmt.bindString(1, pCreature->getName());
            deleteMemberStmt.execute();

            // SAFE_DELETE( pStmt );
            // return;
        }
    }
    END_DB(pStmt)


    if (pCreature->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
        Assert(pSlayer != NULL);

        SkillDomainType_t highest = pSlayer->getHighestSkillDomain();

        
        if ((pSlayer->getGold() >= REQUIRE_SLAYER_MASTER_GOLD) &&                               
            (pSlayer->getFame() >= REQUIRE_SLAYER_MASTER_FAME[highest]) &&                      
            (pSlayer->getSkillDomainLevel(highest) >= REQUIRE_SLAYER_MASTER_SKILL_DOMAIN_LEVEL) 
        ) {
            GSAddGuild gsAddGuild;

            gsAddGuild.setGuildName(pPacket->getGuildName());
            gsAddGuild.setGuildMaster(pSlayer->getName());
            gsAddGuild.setGuildIntro(pPacket->getGuildIntro());
            gsAddGuild.setGuildState(Guild::GUILD_STATE_WAIT);
            gsAddGuild.setGuildRace(Guild::GUILD_RACE_SLAYER);
            gsAddGuild.setServerGroupID(g_pConfig->getPropertyInt("ServerID"));

            g_pSharedServerManager->sendPacket(&gsAddGuild);

            GCNPCResponse response;
            response.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
            pPlayer->sendPacket(&response);
        }

    } else if (pCreature->isVampire()) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
        Assert(pVampire != NULL);

        
        if ((pVampire->getGold() >= REQUIRE_VAMPIRE_MASTER_GOLD) && 
            (pVampire->getLevel() >= REQUIRE_VAMPIRE_MASTER_LEVEL)) {
            GSAddGuild gsAddGuild;

            gsAddGuild.setGuildName(pPacket->getGuildName());
            gsAddGuild.setGuildMaster(pVampire->getName());
            gsAddGuild.setGuildIntro(pPacket->getGuildIntro());
            gsAddGuild.setGuildState(Guild::GUILD_STATE_WAIT);
            gsAddGuild.setGuildRace(Guild::GUILD_RACE_VAMPIRE);
            gsAddGuild.setServerGroupID(g_pConfig->getPropertyInt("ServerID"));

            g_pSharedServerManager->sendPacket(&gsAddGuild);

            GCNPCResponse response;
            response.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
            pPlayer->sendPacket(&response);
        }
    } else if (pCreature->isOusters()) {
        Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
        Assert(pOusters != NULL);

        
        if ((pOusters->getGold() >= REQUIRE_OUSTERS_MASTER_GOLD) && 
            (pOusters->getLevel() >= REQUIRE_OUSTERS_MASTER_LEVEL)) {
            GSAddGuild gsAddGuild;

            gsAddGuild.setGuildName(pPacket->getGuildName());
            gsAddGuild.setGuildMaster(pOusters->getName());
            gsAddGuild.setGuildIntro(pPacket->getGuildIntro());
            gsAddGuild.setGuildState(Guild::GUILD_STATE_WAIT);
            gsAddGuild.setGuildRace(Guild::GUILD_RACE_OUSTERS);
            gsAddGuild.setServerGroupID(g_pConfig->getPropertyInt("ServerID"));

            g_pSharedServerManager->sendPacket(&gsAddGuild);

            GCNPCResponse response;
            response.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
            pPlayer->sendPacket(&response);
        }
    }

#endif // __GAME_SERVER__

    __END_DEBUG_EX __END_CATCH
}
