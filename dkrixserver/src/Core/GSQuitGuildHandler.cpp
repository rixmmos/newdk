//----------------------------------------------------------------------
//
// Filename    : GSQuitGuildHandler.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "Assert1.h"
#include "GSQuitGuild.h"

#ifdef __SHARED_SERVER__

#include "DB.h"
#include "GameServerManager.h"
#include "Guild.h"
#include "GuildManager.h"
#include "PreparedStatement.h"
#include "Properties.h"
#include "SGDeleteGuildOK.h"
#include "SGQuitGuildOK.h"
#include "StringPool.h"

#endif

//----------------------------------------------------------------------
//
// GSQuitGuildHandler::execute()
//
//----------------------------------------------------------------------
void GSQuitGuildHandler::execute(GSQuitGuild* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __SHARED_SERVER__

        Assert(pPacket != NULL);

    
    Guild* pGuild = g_pGuildManager->getGuild(pPacket->getGuildID());
    // try { Assert(pGuild != NULL); } catch (Throwable& ) { return; }
    if (pGuild == NULL)
        return;

    
    GuildMember* pGuildMember = pGuild->getMember(pPacket->getName());
    // try { Assert(pGuildMember != NULL); } catch (Throwable& ) { return; }
    if (pGuildMember == NULL)
        return;

    Statement* pStmt = NULL;

    
    GuildMemberRank_t rank = pGuildMember->getRank();
    if (rank == GuildMember::GUILDMEMBER_RANK_NORMAL || rank == GuildMember::GUILDMEMBER_RANK_MASTER ||
        rank == GuildMember::GUILDMEMBER_RANK_SUBMASTER)
        filelog("GuildExit.log", "GuildID: %d, GuildName: %s, Quit: %s", pGuild->getID(), pGuild->getName().c_str(),
                pPacket->getName().c_str());

    //////////////////////////////////////////////////////////////////////////////
    
    
    //////////////////////////////////////////////////////////////////////////////
    if (pGuild->getState() == Guild::GUILD_STATE_ACTIVE) {
        
        if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_MASTER)
            return;

        
        BEGIN_DB {
            Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

            if (pGuild->getRace() == Guild::GUILD_RACE_SLAYER) {
                PreparedStatement guildIdStmt(pConn, "UPDATE Slayer SET GuildID = 99 WHERE Name = ?");
                guildIdStmt.bindString(1, pGuildMember->getName());
                guildIdStmt.execute();
            } else if (pGuild->getRace() == Guild::GUILD_RACE_VAMPIRE) {
                PreparedStatement guildIdStmt(pConn, "UPDATE Vampire SET GuildID = 0 WHERE Name = ?");
                guildIdStmt.bindString(1, pGuildMember->getName());
                guildIdStmt.execute();
            } else if (pGuild->getRace() == Guild::GUILD_RACE_OUSTERS) {
                PreparedStatement guildIdStmt(pConn, "UPDATE Ousters SET GuildID = 66 WHERE Name = ?");
                guildIdStmt.bindString(1, pGuildMember->getName());
                guildIdStmt.execute();
            }
        }
        END_DB(pStmt)

        
        pGuildMember->leave();

        
        pGuild->deleteMember(pGuildMember->getName());

        
        SGQuitGuildOK sgQuitGuildOK;
        sgQuitGuildOK.setGuildID(pGuild->getID());
        sgQuitGuildOK.setName(pPacket->getName());

        
        g_pGameServerManager->broadcast(&sgQuitGuildOK);

        
        if (pGuild->getState() == Guild::GUILD_STATE_ACTIVE && pGuild->getActiveMemberCount() < MIN_GUILDMEMBER_COUNT) {
            
            filelog("GuildBroken.log", "GuildID: %d, GuildName: %s, MemberCount: %d, Quit: %s", pGuild->getID(),
                    pGuild->getName().c_str(), pGuild->getActiveMemberCount(), pPacket->getName().c_str());

            
            HashMapGuildMember& Members = pGuild->getMembers();
            HashMapGuildMemberItor itr = Members.begin();

            BEGIN_DB {
                Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

                for (; itr != Members.end(); itr++) {
                    GuildMember* pGuildMember = itr->second;

                    if (pGuild->getRace() == Guild::GUILD_RACE_SLAYER) {
                        PreparedStatement guildIdStmt(pConn, "UPDATE Slayer SET GuildID = 99 WHERE Name = ?");
                        guildIdStmt.bindString(1, pGuildMember->getName());
                        guildIdStmt.execute();

                        PreparedStatement msgStmt(pConn, "INSERT INTO Messages (Receiver, Message ) VALUES (?, ? )");
                        msgStmt.bindString(1, pGuildMember->getName());
                        msgStmt.bindString(2, g_pStringPool->c_str(STRID_TEAM_BROKEN));
                        msgStmt.execute();
                    } else if (pGuild->getRace() == Guild::GUILD_RACE_VAMPIRE) {
                        PreparedStatement guildIdStmt(pConn, "UPDATE Vampire SET GuildID = 0 WHERE Name = ?");
                        guildIdStmt.bindString(1, pGuildMember->getName());
                        guildIdStmt.execute();

                        PreparedStatement msgStmt(pConn, "INSERT INTO Messages (Receiver, Message ) VALUES (?, ? )");
                        msgStmt.bindString(1, pGuildMember->getName());
                        msgStmt.bindString(2, g_pStringPool->c_str(STRID_CLAN_BROKEN));
                        msgStmt.execute();
                    } else if (pGuild->getRace() == Guild::GUILD_RACE_OUSTERS) {
                        PreparedStatement guildIdStmt(pConn, "UPDATE Ousters SET GuildID = 0 WHERE Name = ?");
                        guildIdStmt.bindString(1, pGuildMember->getName());
                        guildIdStmt.execute();

                        PreparedStatement msgStmt(pConn, "INSERT INTO Messages (Receiver, Message ) VALUES (?, ? )");
                        msgStmt.bindString(1, pGuildMember->getName());
                        msgStmt.bindString(2, g_pStringPool->c_str(STRID_CLAN_BROKEN));
                        msgStmt.execute();
                    }


                    pGuildMember->expire();


                    SAFE_DELETE(pGuildMember);
                }
            }
            END_DB(pStmt)

            Members.clear();

            
            pGuild->setState(Guild::GUILD_STATE_BROKEN);
            pGuild->save();

            SAFE_DELETE(pGuild);
            g_pGuildManager->deleteGuild(pPacket->getGuildID());

            
            SGDeleteGuildOK sgDeleteGuildOK;
            sgDeleteGuildOK.setGuildID(pPacket->getGuildID());

            g_pGameServerManager->broadcast(&sgDeleteGuildOK);
        }
    } else if (pGuild->getState() == Guild::GUILD_STATE_WAIT) {
        if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_MASTER) {
            ////////////////////////////////////////////////////////////
            
            
            ////////////////////////////////////////////////////////////
            HashMapGuildMember& Members = pGuild->getMembers();
            HashMapGuildMemberItor itr = Members.begin();

            BEGIN_DB {
                Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

                for (; itr != Members.end(); itr++) {
                    GuildMember* pGuildMember = itr->second;

                    string Table = "";
                    string Message = "";
                    Gold_t Gold = 0;

                    if (pGuild->getRace() == Guild::GUILD_RACE_SLAYER) {
                        Table = "Slayer";
                        Message = g_pStringPool->getString(STRID_TEAM_CANCEL);
                    } else if (pGuild->getRace() == Guild::GUILD_RACE_VAMPIRE) {
                        Table = "Vampire";
                        Message = g_pStringPool->getString(STRID_CLAN_CANCEL);
                    } else if (pGuild->getRace() == Guild::GUILD_RACE_OUSTERS) {
                        Table = "Ousters";
                        Message = g_pStringPool->getString(STRID_CLAN_CANCEL);
                    }
                    if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_MASTER)
                        Gold = RETURN_SLAYER_MASTER_GOLD;
                    else if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_SUBMASTER)
                        Gold = RETURN_SLAYER_SUBMASTER_GOLD;

                    if (!Table.empty() && !Message.empty() && Gold != 0) {
                        PreparedStatement msgStmt(pConn, "INSERT INTO Messages (Receiver, Message ) VALUES (?, ? )");
                        msgStmt.bindString(1, pGuildMember->getName());
                        msgStmt.bindString(2, Message);
                        msgStmt.execute();

                        // Table is one of the three fixed literals assigned above, never
                        // user-controlled, so it is safe to splice into the SQL text; Gold
                        // and Name remain bound parameters.
                        PreparedStatement goldStmt(pConn, "UPDATE " + Table + " SET Gold = Gold + ? WHERE Name = ?");
                        goldStmt.bindInt(1, (int)Gold);
                        goldStmt.bindString(2, pGuildMember->getName());
                        goldStmt.execute();
                    }


                    pGuildMember->expire();


                    SAFE_DELETE(pGuildMember);
                }
            }
            END_DB(pStmt)

            Members.clear();

            
            pGuild->setState(Guild::GUILD_STATE_CANCEL);
            pGuild->save();

            SAFE_DELETE(pGuild);

            g_pGuildManager->deleteGuild(pPacket->getGuildID());

            
            SGDeleteGuildOK sgDeleteGuildOK;
            sgDeleteGuildOK.setGuildID(pPacket->getGuildID());

            g_pGameServerManager->broadcast(&sgDeleteGuildOK);
        } else if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_SUBMASTER) {
            //////////////////////////////////////////////////////////
            
            
            //////////////////////////////////////////////////////////
            pGuildMember->expire();

            
            pGuild->deleteMember(pGuildMember->getName());

            
            SGQuitGuildOK sgQuitGuildOK;
            sgQuitGuildOK.setGuildID(pGuild->getID());
            sgQuitGuildOK.setName(pPacket->getName());

            
            g_pGameServerManager->broadcast(&sgQuitGuildOK);
        }
    }

#endif

    __END_DEBUG_EX __END_CATCH
}
