//----------------------------------------------------------------------
//
// Filename    : GSAddGuildMemberHandler.cpp
// Written By  :
// Description :
//
//----------------------------------------------------------------------

// include files
#include "Assert.h"
#include "GSAddGuildMember.h"

#ifdef __SHARED_SERVER__

#include "DB.h"
#include "GameServerManager.h"
#include "Guild.h"
#include "GuildManager.h"
#include "PreparedStatement.h"
#include "SGAddGuildMemberOK.h"
#include "SGModifyGuildOK.h"
#include "StringPool.h"

#endif

//----------------------------------------------------------------------
//
// GSAddGuildHandler::execute()
//
//----------------------------------------------------------------------
void GSAddGuildMemberHandler::execute(GSAddGuildMember* pPacket, Player* pPlayer) {
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __SHARED_SERVER__

        Assert(pPacket != NULL);

    GuildMember* pGuildMember = new GuildMember();
    pGuildMember->setGuildID(pPacket->getGuildID());
    pGuildMember->setName(pPacket->getName());
    pGuildMember->setRank(pPacket->getGuildMemberRank());

    if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_WAIT) {
        pGuildMember->setRequestDateTime(VSDateTime::currentDateTime());
    }

    
    pGuildMember->create();

    
    pGuildMember->saveIntro(pPacket->getGuildMemberIntro());

    
    Guild* pGuild = g_pGuildManager->getGuild(pPacket->getGuildID());
    pGuild->addMember(pGuildMember);

    
    SGAddGuildMemberOK sgAddGuildMemberOK;
    sgAddGuildMemberOK.setGuildID(pGuildMember->getGuildID());
    sgAddGuildMemberOK.setName(pGuildMember->getName());
    sgAddGuildMemberOK.setGuildMemberRank(pGuildMember->getRank());
    sgAddGuildMemberOK.setServerGroupID(pPacket->getServerGroupID());

    
    g_pGameServerManager->broadcast(&sgAddGuildMemberOK);

    
    if (pGuild->getState() == Guild::GUILD_STATE_WAIT && pGuild->getActiveMemberCount() > 4) {
        HashMapGuildMember& Members = pGuild->getMembers();
        HashMapGuildMemberItor itr = Members.begin();
        for (; itr != Members.end(); itr++) {
            pGuildMember = itr->second;

            ///////////////////////////////////////////////////////////////////////////
            
            
            ///////////////////////////////////////////////////////////////////////////
            Statement* pStmt = NULL;
            BEGIN_DB {
                Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

                PreparedStatement requestDateStmt(pConn,
                                                   "UPDATE GuildMember SET RequestDateTime=now() WHERE Name=?");
                requestDateStmt.bindString(1, pGuildMember->getName());
                requestDateStmt.execute();

                if (pGuild->getRace() == Guild::GUILD_RACE_SLAYER) {
                    PreparedStatement guildIdStmt(pConn, "UPDATE Slayer SET GuildID = ? WHERE Name = ?");
                    guildIdStmt.bindInt(1, pGuild->getID());
                    guildIdStmt.bindString(2, pGuildMember->getName());
                    guildIdStmt.execute();

                    if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_MASTER) {
                        PreparedStatement msgStmt(pConn, "INSERT INTO Messages ( Receiver, Message ) VALUES ( ?, ? )");
                        msgStmt.bindString(1, pGuildMember->getName());
                        msgStmt.bindString(2, g_pStringPool->c_str(STRID_TEAM_REGISTRATION_ACCEPT));
                        msgStmt.execute();
                    } else {
                        PreparedStatement msgStmt(pConn, "INSERT INTO Messages ( Receiver, Message ) VALUES ( ?, ? )");
                        msgStmt.bindString(1, pGuildMember->getName());
                        msgStmt.bindString(2, g_pStringPool->c_str(STRID_TEAM_REGISTRATION_ACCEPT_2));
                        msgStmt.execute();
                    }
                } else if (pGuild->getRace() == Guild::GUILD_RACE_VAMPIRE) {
                    PreparedStatement guildIdStmt(pConn, "UPDATE Vampire SET GuildID = ? WHERE Name = ?");
                    guildIdStmt.bindInt(1, pGuild->getID());
                    guildIdStmt.bindString(2, pGuildMember->getName());
                    guildIdStmt.execute();

                    if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_MASTER) {
                        PreparedStatement msgStmt(pConn, "INSERT INTO Messages ( Receiver, Message ) VALUES ( ?, ? )");
                        msgStmt.bindString(1, pGuildMember->getName());
                        msgStmt.bindString(2, g_pStringPool->c_str(STRID_CLAN_REGISTRATION_ACCEPT));
                        msgStmt.execute();
                    } else {
                        PreparedStatement msgStmt(pConn, "INSERT INTO Messages ( Receiver, Message ) VALUES ( ?, ? )");
                        msgStmt.bindString(1, pGuildMember->getName());
                        msgStmt.bindString(2, g_pStringPool->c_str(STRID_CLAN_REGISTRATION_ACCEPT_2));
                        msgStmt.execute();
                    }
                } else if (pGuild->getRace() == Guild::GUILD_RACE_OUSTERS) {
                    PreparedStatement guildIdStmt(pConn, "UPDATE Ousters SET GuildID = ? WHERE Name = ?");
                    guildIdStmt.bindInt(1, pGuild->getID());
                    guildIdStmt.bindString(2, pGuildMember->getName());
                    guildIdStmt.execute();

                    if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_MASTER) {
                        PreparedStatement msgStmt(pConn, "INSERT INTO Messages ( Receiver, Message ) VALUES ( ?, ? )");
                        msgStmt.bindString(1, pGuildMember->getName());
                        msgStmt.bindString(2, g_pStringPool->c_str(STRID_CLAN_REGISTRATION_ACCEPT));
                        msgStmt.execute();
                    } else {
                        PreparedStatement msgStmt(pConn, "INSERT INTO Messages ( Receiver, Message ) VALUES ( ?, ? )");
                        msgStmt.bindString(1, pGuildMember->getName());
                        msgStmt.bindString(2, g_pStringPool->c_str(STRID_CLAN_REGISTRATION_ACCEPT_2));
                        msgStmt.execute();
                    }
                }
            }
            END_DB(pStmt)
        }

        pGuild->setState(Guild::GUILD_STATE_ACTIVE);
        pGuild->save();

        SGModifyGuildOK sgModifyGuildOK;
        sgModifyGuildOK.setGuildID(pGuild->getID());
        sgModifyGuildOK.setGuildState(pGuild->getState());

        g_pGameServerManager->broadcast(&sgModifyGuildOK);
    }

#endif

    __END_DEBUG_EX __END_CATCH
}
