//////////////////////////////////////////////////////////////////////////////
// Filename    : CGTryJoinGuildHandler.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGTryJoinGuild.h"

#ifdef __GAME_SERVER__
#include <stdio.h>

#include "Assert1.h"
#include "DB.h"
#include "GCNPCResponse.h"
#include "GCShowGuildJoin.h"
#include "GCSystemMessage.h"
#include "GamePlayer.h"
#include "Guild.h"
#include "GuildManager.h"
#include "Ousters.h"
#include "PreparedStatement.h"
#include "Slayer.h"
#include "StringPool.h"
#include "SystemAvailabilitiesManager.h"
#include "Vampire.h"
#include "VariableManager.h"
#endif // __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGTryJoinGuildHandler::execute(CGTryJoinGuild* pPacket, Player* pPlayer)

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

    // cout << pPacket->toString() << endl;

    Guild* pGuild = g_pGuildManager->getGuild(pPacket->getGuildID());
    // try { Assert(pGuild != NULL); } catch (Throwable& t ) { cout << "xx"<< endl; return; }

    if (pGuild == NULL) {
        GCNPCResponse response;
        response.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
        pPlayer->sendPacket(&response);

        return;
    }

    Statement* pStmt = NULL;
    Result* pResult;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement memberStmt(pConn, "SELECT GuildID, ExpireDate,`Rank` FROM GuildMember WHERE Name = ?");
        memberStmt.bindString(1, pCreature->getName());
        pResult = memberStmt.execute();

        /*
        if (pResult->getRowCount() == 0 )
        {
            cout << "no result" << endl;
        }
        */

        if (pResult->next()) {
            string ExpireDate = pResult->getString(2);
            // int GuildID = pResult->getInt(1);
            // int rank = pResult->getInt(3);

            
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
                    //					if (rank==GuildMember::GUILDMEMBER_RANK_DENY
                    //						&& GuildID != pGuild->getID())
                    //					{
                    
                    //					}
                    //					else
                    //					{

                    GCNPCResponse response;

                    /*						if (GuildID == pGuild->getID())
                                            {
                                                if (pCreature->isSlayer() )
                                                    response.setCode(NPC_RESPONSE_TEAM_STARTING_FAIL_DENY);
                                                else if (pCreature->isVampire() )
                                                    response.setCode(NPC_RESPONSE_CLAN_STARTING_FAIL_DENY);
                                                else if (pCreature->isOusters() )
                                                    response.setCode(NPC_RESPONSE_GUILD_STARTING_FAIL_DENY);
                                            }
                                            else
                                            {*/
                    if (pCreature->isSlayer())
                        response.setCode(NPC_RESPONSE_TEAM_STARTING_FAIL_QUIT_TIMEOUT);
                    else if (pCreature->isVampire())
                        response.setCode(NPC_RESPONSE_CLAN_STARTING_FAIL_QUIT_TIMEOUT);
                    else if (pCreature->isOusters())
                        response.setCode(NPC_RESPONSE_GUILD_STARTING_FAIL_QUIT_TIMEOUT);
                    //}

                    pPlayer->sendPacket(&response);

                    return;
                    //					}
                }
            } else {
                if (pCreature->isSlayer()) {
                    GCNPCResponse response;
                    response.setCode(NPC_RESPONSE_TEAM_STARTING_FAIL_ALREADY_JOIN);
                    pPlayer->sendPacket(&response);
                } else if (pCreature->isVampire()) {
                    GCNPCResponse response;
                    response.setCode(NPC_RESPONSE_CLAN_STARTING_FAIL_ALREADY_JOIN);
                    pPlayer->sendPacket(&response);
                } else if (pCreature->isOusters()) {
                    GCNPCResponse response;
                    response.setCode(NPC_RESPONSE_GUILD_STARTING_FAIL_ALREADY_JOIN);
                    pPlayer->sendPacket(&response);
                }

                return;
            }
        }
    }
    END_DB(pStmt)

    if (pPacket->getGuildMemberRank() == GuildMember::GUILDMEMBER_RANK_SUBMASTER) {
        if (pCreature->isSlayer()) {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
            Assert(pSlayer != NULL);

            SkillDomainType_t highest = pSlayer->getHighestSkillDomain();

            
            if (pSlayer->getSkillDomainLevel(highest) < REQUIRE_SLAYER_SUBMASTER_SKILL_DOMAIN_LEVEL) {
                
                GCNPCResponse response;
                response.setCode(NPC_RESPONSE_TEAM_STARTING_FAIL_LEVEL);
                pPlayer->sendPacket(&response);

                return;
            }
            if (pSlayer->getGold() < REQUIRE_SLAYER_SUBMASTER_GOLD) {
                
                GCNPCResponse response;
                response.setCode(NPC_RESPONSE_TEAM_STARTING_FAIL_MONEY);
                pPlayer->sendPacket(&response);

                return;
            }
            if (pSlayer->getFame() < REQUIRE_SLAYER_SUBMASTER_FAME[highest]) {
                
                GCNPCResponse response;
                response.setCode(NPC_RESPONSE_TEAM_STARTING_FAIL_FAME);
                pPlayer->sendPacket(&response);

                return;
            }

            
            GCShowGuildJoin gcShowGuildJoin;
            gcShowGuildJoin.setGuildID(pGuild->getID());
            gcShowGuildJoin.setGuildName(pGuild->getName());
            gcShowGuildJoin.setGuildMemberRank(pPacket->getGuildMemberRank());
            gcShowGuildJoin.setJoinFee(REQUIRE_SLAYER_SUBMASTER_GOLD);
            pPlayer->sendPacket(&gcShowGuildJoin);

            
        } else if (pCreature->isVampire()) {
            Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
            Assert(pVampire != NULL);

            
            if (pVampire->getLevel() < REQUIRE_VAMPIRE_SUBMASTER_LEVEL) {
                
                GCNPCResponse response;
                response.setCode(NPC_RESPONSE_CLAN_STARTING_FAIL_LEVEL);
                pPlayer->sendPacket(&response);

                return;
            }
            if (pVampire->getGold() < REQUIRE_VAMPIRE_SUBMASTER_GOLD) {
                
                GCNPCResponse response;
                response.setCode(NPC_RESPONSE_CLAN_STARTING_FAIL_MONEY);
                pPlayer->sendPacket(&response);

                return;
            }
            //			if (pVampire->getFame() < 400000 )
            //			{
            
            //				GCNPCResponse response;
            //				response.setCode(NPC_RESPONSE_CLAN_STARTING_FAIL_FAME);
            //				pPlayer->sendPacket(&response);
            //
            //				return;
            //			}

            
            GCShowGuildJoin gcShowGuildJoin;
            gcShowGuildJoin.setGuildID(pGuild->getID());
            gcShowGuildJoin.setGuildName(pGuild->getName());
            gcShowGuildJoin.setGuildMemberRank(pPacket->getGuildMemberRank());
            gcShowGuildJoin.setJoinFee(REQUIRE_VAMPIRE_SUBMASTER_GOLD);
            pPlayer->sendPacket(&gcShowGuildJoin);

            // cout << gcShowGuildJoin.toString() << endl;
            
        } else if (pCreature->isOusters()) {
            Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
            Assert(pOusters != NULL);

            
            if (pOusters->getLevel() < REQUIRE_OUSTERS_SUBMASTER_LEVEL) {
                
                GCNPCResponse response;
                response.setCode(NPC_RESPONSE_GUILD_STARTING_FAIL_LEVEL);
                pPlayer->sendPacket(&response);

                return;
            }
            if (pOusters->getGold() < REQUIRE_OUSTERS_SUBMASTER_GOLD) {
                
                GCNPCResponse response;
                response.setCode(NPC_RESPONSE_GUILD_STARTING_FAIL_MONEY);
                pPlayer->sendPacket(&response);

                return;
            }

            
            GCShowGuildJoin gcShowGuildJoin;
            gcShowGuildJoin.setGuildID(pGuild->getID());
            gcShowGuildJoin.setGuildName(pGuild->getName());
            gcShowGuildJoin.setGuildMemberRank(pPacket->getGuildMemberRank());
            gcShowGuildJoin.setJoinFee(REQUIRE_OUSTERS_SUBMASTER_GOLD);
            pPlayer->sendPacket(&gcShowGuildJoin);

            // cout << gcShowGuildJoin.toString() << endl;
            
        }
    } else if (pPacket->getGuildMemberRank() == GuildMember::GUILDMEMBER_RANK_WAIT) {
        if (pGuild->getWaitMemberCount() >= MAX_GUILDMEMBER_WAIT_COUNT) {
            GCNPCResponse response;
            response.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
            pPlayer->sendPacket(&response);

            GCSystemMessage msg;
            msg.setMessage(g_pStringPool->getString(STRID_GUILD_WAIT_MEMBER_FULL));
            pPlayer->sendPacket(&msg);

            return;
        }

        
        
        GCShowGuildJoin gcShowGuildJoin;
        gcShowGuildJoin.setGuildID(pGuild->getID());
        gcShowGuildJoin.setGuildName(pGuild->getName());
        gcShowGuildJoin.setGuildMemberRank(pPacket->getGuildMemberRank());
        gcShowGuildJoin.setJoinFee(0);
        pPlayer->sendPacket(&gcShowGuildJoin);

        // cout << gcShowGuildJoin.toString() << endl;
        
    }

#endif // __GAME_SERVER__

    __END_DEBUG_EX __END_CATCH
}
