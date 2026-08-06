//----------------------------------------------------------------------
//
// Filename    : SGModifyGuildOKHandler.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "Properties.h"
#include "SGModifyGuildOK.h"

#ifdef __GAME_SERVER__

#include "DB.h"
#include "GCModifyGuildMemberInfo.h"
#include "GCOtherModifyInfo.h"
#include "GCSystemMessage.h"
#include "Guild.h"
#include "GuildManager.h"
#include "PCFinder.h"
#include "Player.h"
#include "PlayerCreature.h"
#include "Properties.h"
#include "Zone.h"
#include "ZoneGroupManager.h"
#include "ZoneInfoManager.h"

#endif

//----------------------------------------------------------------------
//
// SGModifyGuildOKHandler::execute()
//
//----------------------------------------------------------------------
void SGModifyGuildOKHandler::execute(SGModifyGuildOK* pPacket)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Guild* pGuild = g_pGuildManager->getGuild(pPacket->getGuildID());
    Assert(pGuild != NULL);

    if (pGuild->getState() == Guild::GUILD_STATE_WAIT && pPacket->getGuildState() == Guild::GUILD_STATE_ACTIVE) {
        /////////////////////////////////////////////////////////////
        
        /////////////////////////////////////////////////////////////
         

        
        pGuild->setState(Guild::GUILD_STATE_ACTIVE);

        HashMapGuildMember& Members = pGuild->getMembers();
        HashMapGuildMemberItor itr = Members.begin();
        for (; itr != Members.end(); itr++) {
            GuildMember* pGuildMember = itr->second;

            
            __ENTER_CRITICAL_SECTION((*g_pPCFinder))

            Creature* pCreature = g_pPCFinder->getCreature_LOCKED(pGuildMember->getName());
            if (pCreature != NULL && pCreature->isPC()) {
                Player* pPlayer = pCreature->getPlayer();
                Assert(pPlayer != NULL);

                PlayerCreature* pPlayerCreature = dynamic_cast<PlayerCreature*>(pCreature);
                Assert(pPlayerCreature != NULL);

                
                pPlayerCreature->setGuildID(pGuild->getID());

                
                GCModifyGuildMemberInfo gcModifyGuildMemberInfo;
                gcModifyGuildMemberInfo.setGuildID(pGuild->getID());
                gcModifyGuildMemberInfo.setGuildName(pGuild->getName());
                gcModifyGuildMemberInfo.setGuildMemberRank(pGuildMember->getRank());
                pPlayer->sendPacket(&gcModifyGuildMemberInfo);

                
                Zone* pZone = pCreature->getZone();
                Assert(pZone != NULL);

                GCOtherModifyInfo gcOtherModifyInfo;
                gcOtherModifyInfo.setObjectID(pCreature->getObjectID());
                gcOtherModifyInfo.addShortData(MODIFY_GUILDID, pPlayerCreature->getGuildID());

                pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcOtherModifyInfo, pCreature);

                
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
            }

            __LEAVE_CRITICAL_SECTION((*g_pPCFinder))
        }
    }

#endif

    __END_DEBUG_EX __END_CATCH
}
