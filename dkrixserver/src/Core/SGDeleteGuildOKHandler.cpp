//----------------------------------------------------------------------
//
// Filename    : SGDeleteGuildOKHandler.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "SGDeleteGuildOK.h"

#ifdef __GAME_SERVER__

#include "Assert1.h"
#include "DB.h"
#include "GCModifyGuildMemberInfo.h"
#include "GCModifyInformation.h"
#include "GCOtherModifyInfo.h"
#include "GCSystemMessage.h"
#include "Guild.h"
#include "GuildManager.h"
#include "GuildUnion.h"
#include "PCFinder.h"
#include "Player.h"
#include "PlayerCreature.h"
#include "Properties.h"
#include "ResurrectLocationManager.h"
#include "Zone.h"
#endif

//----------------------------------------------------------------------
//
// SGDeleteGuildOKHandler::execute()
//
//----------------------------------------------------------------------
void SGDeleteGuildOKHandler::execute(SGDeleteGuildOK* pPacket)

{
    __BEGIN_TRY

#ifdef __GAME_SERVER__

    
    
    
    

    Assert(pPacket != NULL);

    
    Guild* pGuild = g_pGuildManager->getGuild(pPacket->getGuildID());
    try {
        Assert(pGuild != NULL);
    } catch (Throwable&) {
        return;
    }


    
    if (pGuild->getState() == Guild::GUILD_STATE_ACTIVE) {
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

                
                if (pPlayerCreature->isSlayer()) {
                    pPlayerCreature->setGuildID(99); 

                    
                    GCModifyGuildMemberInfo gcModifyGuildMemberInfo;
                    gcModifyGuildMemberInfo.setGuildID(pPlayerCreature->getGuildID());
                    gcModifyGuildMemberInfo.setGuildName("");
                    gcModifyGuildMemberInfo.setGuildMemberRank(GuildMember::GUILDMEMBER_RANK_DENY);
                    pPlayer->sendPacket(&gcModifyGuildMemberInfo);
                } else if (pPlayerCreature->isVampire()) {
                    pPlayerCreature->setGuildID(0); 

                    
                    GCModifyGuildMemberInfo gcModifyGuildMemberInfo;
                    gcModifyGuildMemberInfo.setGuildID(pPlayerCreature->getGuildID());
                    gcModifyGuildMemberInfo.setGuildName("");
                    gcModifyGuildMemberInfo.setGuildMemberRank(GuildMember::GUILDMEMBER_RANK_DENY);
                    pPlayer->sendPacket(&gcModifyGuildMemberInfo);
                } else if (pPlayerCreature->isOusters()) {
                    pPlayerCreature->setGuildID(66); 

                    
                    GCModifyGuildMemberInfo gcModifyGuildMemberInfo;
                    gcModifyGuildMemberInfo.setGuildID(pPlayerCreature->getGuildID());
                    gcModifyGuildMemberInfo.setGuildName("");
                    gcModifyGuildMemberInfo.setGuildMemberRank(GuildMember::GUILDMEMBER_RANK_DENY);
                    pPlayer->sendPacket(&gcModifyGuildMemberInfo);
                }

                
                GCOtherModifyInfo gcOtherModifyInfo;
                gcOtherModifyInfo.setObjectID(pCreature->getObjectID());
                gcOtherModifyInfo.addShortData(MODIFY_GUILDID, pPlayerCreature->getGuildID());

                Zone* pZone = pCreature->getZone();
                Assert(pZone != NULL);

                pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcOtherModifyInfo, pCreature);
            }

            __LEAVE_CRITICAL_SECTION((*g_pPCFinder))

            
            SAFE_DELETE(pGuildMember);
        }

        
        Members.clear();

        
        g_pGuildManager->deleteGuild(pGuild->getID());

        
        SAFE_DELETE(pGuild);
    } else if (pGuild->getState() == Guild::GUILD_STATE_WAIT) {
        HashMapGuildMember& Members = pGuild->getMembers();
        HashMapGuildMemberItor itr = Members.begin();

        Statement* pStmt = NULL;
        Result* pResult = NULL;

        BEGIN_DB {
            pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

            for (; itr != Members.end(); itr++) {
                GuildMember* pGuildMember = itr->second;

                
                __ENTER_CRITICAL_SECTION((*g_pPCFinder))

                Creature* pCreature = g_pPCFinder->getCreature_LOCKED(pGuildMember->getName());
                if (pCreature != NULL && pCreature->isPC()) {
                    Player* pPlayer = pCreature->getPlayer();
                    Assert(pPlayer != NULL);

                    PlayerCreature* pPlayerCreature = dynamic_cast<PlayerCreature*>(pCreature);
                    Assert(pPlayerCreature != NULL);

                    
                    Gold_t Gold = pPlayerCreature->getGold();
                    if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_MASTER) {
                        Gold = min((uint64_t)(Gold + RETURN_SLAYER_MASTER_GOLD), (uint64_t)2000000000);
                    } else if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_SUBMASTER) {
                        Gold = min((uint64_t)(Gold + RETURN_SLAYER_SUBMASTER_GOLD), (uint64_t)2000000000);
                    }

                    pPlayerCreature->setGoldEx(Gold);

                    GCModifyInformation gcModifyInformation;
                    gcModifyInformation.addLongData(MODIFY_GOLD, Gold);
                    pPlayer->sendPacket(&gcModifyInformation);

                    
                    pResult = pStmt->executeQuery("SELECT Message FROM Messages WHERE Receiver = '%s'",
                                                  pCreature->getName().c_str());

                    while (pResult->next()) {
                        GCSystemMessage message;
                        message.setMessage(pResult->getString(1));
                        pPlayer->sendPacket(&message);
                    }

                    pStmt->executeQuery("DELETE FROM Messages WHERE Receiver = '%s'", pCreature->getName().c_str());
                }

                __LEAVE_CRITICAL_SECTION((*g_pPCFinder))

                
                SAFE_DELETE(pGuildMember);
            }

            
            Members.clear();

            
            g_pGuildManager->deleteGuild(pGuild->getID());
            GuildUnionManager::Instance().removeMasterGuild(pGuild->getID());

            
            SAFE_DELETE(pGuild);

            SAFE_DELETE(pStmt);
        }
        END_DB(pStmt)
    }

#endif

    __END_CATCH
}
