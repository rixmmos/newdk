//----------------------------------------------------------------------
//
// Filename    : GSExpelGuildMemberHandler.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "Assert1.h"
#include "GSExpelGuildMember.h"

#ifdef __SHARED_SERVER__

#include "DB.h"
#include "GameServerManager.h"
#include "Guild.h"
#include "GuildManager.h"
#include "PreparedStatement.h"
#include "Properties.h"
#include "SGDeleteGuildOK.h"
#include "SGExpelGuildMemberOK.h"

#endif

//----------------------------------------------------------------------
//
// GSExpelGuildMemberHandler::execute()
//
//----------------------------------------------------------------------
void GSExpelGuildMemberHandler::execute(GSExpelGuildMember* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __SHARED_SERVER__
        // cout << "GSExpelGuildMember received" << endl;

        Assert(pPacket != NULL);

    
    Guild* pGuild = g_pGuildManager->getGuild(pPacket->getGuildID());
    // try { Assert(pGuild != NULL); } catch (Throwable& ) { return; }
    if (pGuild == NULL)
        return;

    
    GuildMember* pGuildMember = pGuild->getMember(pPacket->getName());
    // try { Assert(pGuildMember != NULL); } catch (Throwable& ) { return; }
    if (pGuildMember == NULL)
        return;

    
    filelog("GuildExit.log", "GuildID: %d, GuildName: %s, Expel: %s, By: %s", pGuild->getID(),
            pGuild->getName().c_str(), pPacket->getName().c_str(), pPacket->getSender().c_str());

    ///////////////////////////////////////////////////////////////////
    
    ///////////////////////////////////////////////////////////////////
    Statement* pStmt = NULL;
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

    
    pGuildMember->expire();

    
    pGuild->deleteMember(pGuildMember->getName());

    
    SGExpelGuildMemberOK sgExpelGuildMemberOK;
    sgExpelGuildMemberOK.setGuildID(pGuild->getID());
    sgExpelGuildMemberOK.setName(pPacket->getName());
    sgExpelGuildMemberOK.setSender(pPacket->getSender());

    
    g_pGameServerManager->broadcast(&sgExpelGuildMemberOK);

    
    if (pGuild->getState() == Guild::GUILD_STATE_ACTIVE && pGuild->getActiveMemberCount() < MIN_GUILDMEMBER_COUNT) {
        
        filelog("GuildBroken.log", "GuildID: %d, GuildName: %s, MemberCount: %d, Expel: %s", pGuild->getID(),
                pGuild->getName().c_str(), pGuild->getActiveMemberCount(), pPacket->getName().c_str());

        
        HashMapGuildMember& Members = pGuild->getMembers();
        HashMapGuildMemberItor itr = Members.begin();

        BEGIN_DB {
            Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

            for (; itr != Members.end(); itr++) {
                GuildMember* pGuildMember = itr->second;

                ///////////////////////////////////////////////////////////////////

                ///////////////////////////////////////////////////////////////////
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


                pGuildMember->expire();

                // pGuildMember->destroy();


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

#endif

    __END_DEBUG_EX __END_CATCH
}
