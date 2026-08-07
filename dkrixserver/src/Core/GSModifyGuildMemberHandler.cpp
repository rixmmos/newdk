//----------------------------------------------------------------------
//
// Filename    : GSModifyGuildMemberHandler.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "Assert1.h"
#include "GSModifyGuildMember.h"

#ifdef __SHARED_SERVER__

#include <stdio.h>

#include "DB.h"
#include "GameServerManager.h"
#include "Guild.h"
#include "GuildManager.h"
#include "PreparedStatement.h"
#include "Properties.h"
#include "SGModifyGuildMemberOK.h"
#include "StringPool.h"

#endif

//----------------------------------------------------------------------
//
// GSModifyGuildMemberHandler::execute()
//
//----------------------------------------------------------------------
void GSModifyGuildMemberHandler::execute(GSModifyGuildMember* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __SHARED_SERVER__
        // cout << "GSModifyGuildMember received" << endl;

        Assert(pPacket != NULL);

    
    Guild* pGuild = g_pGuildManager->getGuild(pPacket->getGuildID());
    // try { Assert(pGuild != NULL); } catch (Throwable& ) { return; }
    if (pGuild == NULL)
        return;

    
    GuildMember* pGuildMember = pGuild->getMember(pPacket->getName());
    // try { Assert(pGuildMember != NULL); } catch (Throwable& ) { return; }
    if (pGuildMember == NULL)
        return;

    
    if (pGuild->getMaster() != pPacket->getSender() &&
        pPacket->getGuildMemberRank() != GuildMember::GUILDMEMBER_RANK_MASTER)
        return;

    if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_WAIT &&
        pPacket->getGuildMemberRank() == GuildMember::GUILDMEMBER_RANK_NORMAL) {
        ///////////////////////////////////////////////////////////////////////////////////////
        
        ///////////////////////////////////////////////////////////////////////////////////////
        Statement* pStmt = NULL;

        BEGIN_DB {
            Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

            if (pGuild->getRace() == Guild::GUILD_RACE_SLAYER) {
                PreparedStatement guildIdStmt(pConn, "UPDATE Slayer SET GuildID = ? WHERE Name = ?");
                guildIdStmt.bindInt(1, pGuild->getID());
                guildIdStmt.bindString(2, pGuildMember->getName());
                guildIdStmt.execute();

                PreparedStatement msgStmt(pConn, "INSERT INTO Messages (Receiver, Message ) VALUES (?, ? )");
                msgStmt.bindString(1, pGuildMember->getName());
                msgStmt.bindString(2, g_pStringPool->c_str(STRID_TEAM_JOIN_ACCEPT));
                msgStmt.execute();
            } else if (pGuild->getRace() == Guild::GUILD_RACE_VAMPIRE) {
                PreparedStatement guildIdStmt(pConn, "UPDATE Vampire SET GuildID = ? WHERE Name = ?");
                guildIdStmt.bindInt(1, pGuild->getID());
                guildIdStmt.bindString(2, pGuildMember->getName());
                guildIdStmt.execute();

                PreparedStatement msgStmt(pConn, "INSERT INTO Messages (Receiver, Message ) VALUES (?, ? )");
                msgStmt.bindString(1, pGuildMember->getName());
                msgStmt.bindString(2, g_pStringPool->c_str(STRID_CLAN_JOIN_ACCEPT));
                msgStmt.execute();
            } else if (pGuild->getRace() == Guild::GUILD_RACE_OUSTERS) {
                PreparedStatement guildIdStmt(pConn, "UPDATE Ousters SET GuildID = ? WHERE Name = ?");
                guildIdStmt.bindInt(1, pGuild->getID());
                guildIdStmt.bindString(2, pGuildMember->getName());
                guildIdStmt.execute();

                PreparedStatement msgStmt(pConn, "INSERT INTO Messages (Receiver, Message ) VALUES (?, ? )");
                msgStmt.bindString(1, pGuildMember->getName());
                msgStmt.bindString(2, g_pStringPool->c_str(STRID_CLAN_JOIN_ACCEPT));
                msgStmt.execute();
            }
        }
        END_DB(pStmt)

        
        pGuild->modifyMemberRank(pGuildMember->getName(), pPacket->getGuildMemberRank());
    } else if (pGuildMember->getRank() != GuildMember::GUILDMEMBER_RANK_MASTER &&
               pPacket->getGuildMemberRank() == GuildMember::GUILDMEMBER_RANK_MASTER) {
        
        pGuild->modifyMemberRank(pGuild->getMaster(), pGuildMember->getRank());
        
        pGuild->modifyMemberRank(pGuildMember->getName(), pPacket->getGuildMemberRank());
        
        pGuild->setMaster(pGuildMember->getName());

        
        char field[30];
        sprintf(field, "Master='%s'", pGuildMember->getName().c_str());
        pGuild->tinysave(field);
    } else if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_NORMAL &&
               pPacket->getGuildMemberRank() == GuildMember::GUILDMEMBER_RANK_SUBMASTER) {
        
        pGuild->modifyMemberRank(pGuildMember->getName(), pPacket->getGuildMemberRank());
    } else {
        return;
    }

    
    SGModifyGuildMemberOK sgModifyGuildMemberOK;
    sgModifyGuildMemberOK.setGuildID(pGuild->getID());
    sgModifyGuildMemberOK.setName(pPacket->getName());
    sgModifyGuildMemberOK.setGuildMemberRank(pGuildMember->getRank());
    sgModifyGuildMemberOK.setSender(pPacket->getSender());

    
    g_pGameServerManager->broadcast(&sgModifyGuildMemberOK);

#endif

    __END_DEBUG_EX __END_CATCH
}
