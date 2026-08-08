////////////////////////////////////////////////////////////////////////////// // Filename    : CGJoinGuildHandler.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGJoinGuild.h"

#ifdef __GAME_SERVER__
#include <stdio.h>

#include "DB.h"
#include "GCNPCResponse.h"
#include "GSAddGuildMember.h"
#include "GamePlayer.h"
#include "Guild.h"
#include "GuildManager.h"
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
void CGJoinGuildHandler::execute(CGJoinGuild* pPacket, Player* pPlayer)

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

    Player* pPlayer = pCreature->getPlayer();
    Assert(pPlayer != NULL);

    Statement* pStmt = NULL;
    Result* pResult;

    // cout << pPacket->toString() << endl;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement memberStmt(pConn, "SELECT GuildID, `Rank`, ExpireDate FROM GuildMember WHERE Name = ?");
        memberStmt.bindString(1, pCreature->getName());
        pResult = memberStmt.execute();

        if (pResult->next()) {
            GuildID_t GuildID = pResult->getInt(1);
            int Rank = pResult->getInt(2);
            string ExpireDate = pResult->getString(3);

            if (ExpireDate.size() == 7) {
                time_t daytime = time(0);

                tm Time;

                Time.tm_year = atoi(ExpireDate.substr(0, 3).c_str());
                Time.tm_mon = atoi(ExpireDate.substr(3, 2).c_str());
                Time.tm_mday = atoi(ExpireDate.substr(5, 2).c_str());
                Time.tm_hour = 0;
                Time.tm_min = 0;
                Time.tm_sec = 0;

                
                if (difftime(daytime, mktime(&Time)) <
                    g_pVariableManager->getVariable(QUIT_GUILD_PENALTY_TERM) * 24 * 3600) 
                {
                     


                    // 2003. 6. 25 by bezz
                    return;
                }
            } else {
                return;
            }
        }
    }
    END_DB(pStmt)


    
    if (pPacket->getGuildMemberRank() == GuildMember::GUILDMEMBER_RANK_SUBMASTER) {
        
        Guild* pGuild = g_pGuildManager->getGuild(pPacket->getGuildID());
        if (pGuild == NULL)
            return;

        if (pGuild->getState() != Guild::GUILD_STATE_WAIT)
            return;

        if (pCreature->isSlayer()) {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
            Assert(pSlayer != NULL);

            SkillDomainType_t highest = pSlayer->getHighestSkillDomain();

            
            if ((pSlayer->getGold() >= REQUIRE_SLAYER_SUBMASTER_GOLD) &&                               
                (pSlayer->getFame() >= REQUIRE_SLAYER_SUBMASTER_FAME[highest]) &&                      
                (pSlayer->getSkillDomainLevel(highest) >= REQUIRE_SLAYER_SUBMASTER_SKILL_DOMAIN_LEVEL) 
            ) {
                GSAddGuildMember gsAddGuildMember;

                gsAddGuildMember.setGuildID(pPacket->getGuildID());
                gsAddGuildMember.setName(pSlayer->getName());
                gsAddGuildMember.setGuildMemberRank(pPacket->getGuildMemberRank());
                gsAddGuildMember.setGuildMemberIntro(pPacket->getGuildMemberIntro());
                gsAddGuildMember.setServerGroupID(g_pConfig->getPropertyInt("ServerID"));

                g_pSharedServerManager->sendPacket(&gsAddGuildMember);
            }

        } else if (pCreature->isVampire()) {
            Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
            Assert(pVampire != NULL);

            
            if ((pVampire->getGold() >= REQUIRE_VAMPIRE_SUBMASTER_GOLD) && 
                (pVampire->getLevel() >= REQUIRE_VAMPIRE_SUBMASTER_LEVEL)  
            ) {
                GSAddGuildMember gsAddGuildMember;

                gsAddGuildMember.setGuildID(pPacket->getGuildID());
                gsAddGuildMember.setName(pVampire->getName());
                gsAddGuildMember.setGuildMemberRank(pPacket->getGuildMemberRank());
                gsAddGuildMember.setGuildMemberIntro(pPacket->getGuildMemberIntro());
                gsAddGuildMember.setServerGroupID(g_pConfig->getPropertyInt("ServerID"));

                g_pSharedServerManager->sendPacket(&gsAddGuildMember);
            }
        } else if (pCreature->isOusters()) {
            Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
            Assert(pOusters != NULL);

            
            if ((pOusters->getGold() >= REQUIRE_OUSTERS_SUBMASTER_GOLD) && 
                (pOusters->getLevel() >= REQUIRE_OUSTERS_SUBMASTER_LEVEL)  
            ) {
                GSAddGuildMember gsAddGuildMember;

                gsAddGuildMember.setGuildID(pPacket->getGuildID());
                gsAddGuildMember.setName(pOusters->getName());
                gsAddGuildMember.setGuildMemberRank(pPacket->getGuildMemberRank());
                gsAddGuildMember.setGuildMemberIntro(pPacket->getGuildMemberIntro());
                gsAddGuildMember.setServerGroupID(g_pConfig->getPropertyInt("ServerID"));

                g_pSharedServerManager->sendPacket(&gsAddGuildMember);
            }
        }
    } else if (pPacket->getGuildMemberRank() == GuildMember::GUILDMEMBER_RANK_WAIT) {
        
        GSAddGuildMember gsAddGuildMember;

        gsAddGuildMember.setGuildID(pPacket->getGuildID());
        gsAddGuildMember.setName(pCreature->getName());
        gsAddGuildMember.setGuildMemberRank(pPacket->getGuildMemberRank());
        gsAddGuildMember.setGuildMemberIntro(pPacket->getGuildMemberIntro());
        gsAddGuildMember.setServerGroupID(g_pConfig->getPropertyInt("ServerID"));

        g_pSharedServerManager->sendPacket(&gsAddGuildMember);
    }

    GCNPCResponse response;
    response.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
    pPlayer->sendPacket(&response);

#endif // __GAME_SERVER__

    __END_DEBUG_EX __END_CATCH
}
