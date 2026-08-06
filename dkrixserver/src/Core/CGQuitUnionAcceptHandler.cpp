//////////////////////////////////////////////////////////////////////////////
// Filename    : CGQuitUnionAcceptHandler.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGQuitUnionAccept.h"

#ifdef __GAME_SERVER__
#include "Assert.h"
#include "DB.h"
#include "Exception.h"
#include "GCGuildResponse.h"
#include "GCModifyInformation.h"
#include "GCOtherModifyInfo.h"
#include "GCSystemMessage.h"
#include "GamePlayer.h"
#include "Guild.h"
#include "GuildManager.h"
#include "GuildUnion.h"
#include "PCFinder.h"
#include "PacketUtil.h"
#include "PlayerCreature.h"
#include "StringPool.h"
#include "SystemAvailabilitiesManager.h"

#endif // __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGQuitUnionAcceptHandler::execute(CGQuitUnionAccept* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    Assert(pGamePlayer != NULL);

    PlayerCreature* pPlayerCreature = dynamic_cast<PlayerCreature*>(pGamePlayer->getCreature());
    Assert(pPlayerCreature != NULL);

    SYSTEM_ASSERT(SYSTEM_GUILD);

    GCGuildResponse gcGuildResponse;

    GuildUnion* pUnion = GuildUnionManager::Instance().getGuildUnion(pPlayerCreature->getGuildID());
    if (pUnion == NULL) {
        gcGuildResponse.setCode(GuildUnionOfferManager::NOT_IN_UNION);
        pPlayer->sendPacket(&gcGuildResponse);

        return;
    }

    
    if (!g_pGuildManager->isGuildMaster(pPlayerCreature->getGuildID(), pPlayerCreature) ||
        pUnion->getMasterGuildID() != pPlayerCreature->getGuildID()) {
        
        

        gcGuildResponse.setCode(GuildUnionOfferManager::SOURCE_IS_NOT_MASTER);
        pPlayer->sendPacket(&gcGuildResponse);

        return;
    }

    uint result = GuildUnionOfferManager::Instance().acceptQuit(pPacket->getGuildID());

    gcGuildResponse.setCode(result);
    pPlayer->sendPacket(&gcGuildResponse);
    ////////////////////

    if (result == GuildUnionOfferManager::OK) {
        Guild* pGuild = g_pGuildManager->getGuild(pPacket->getGuildID());

        if (pGuild == NULL) {
            return;
        }
        string TargetGuildMaster = pGuild->getMaster();

        


        Statement* pStmt = NULL;

        BEGIN_DB {
            pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
            pStmt->executeQuery("INSERT INTO Messages (Receiver, Message) values('%s','%s')", TargetGuildMaster.c_str(),
                                g_pStringPool->c_str(375));

            
            Result* pResult =
                pStmt->executeQuery("SELECT count(*) FROM GuildUnionMember WHERE UnionID='%u'", pUnion->getUnionID());
            pResult->next();

            if (pResult->getInt(1) == 0) {
                
                pStmt->executeQuery("DELETE FROM GuildUnionInfo WHERE UnionID='%u'", pUnion->getUnionID());
                GuildUnionManager::Instance().reload();
            }


            SAFE_DELETE(pStmt);
        }
        END_DB(pStmt)

        
        Creature* pCreature = NULL;
        pCreature = pGamePlayer->getCreature();

        if (pCreature == NULL)
            return;

        GCModifyInformation gcModifyInformation;
        makeGCModifyInfoGuildUnion(&gcModifyInformation, pCreature);

        pPlayer->sendPacket(&gcModifyInformation);

        
        Creature* pTargetCreature = NULL;
        __ENTER_CRITICAL_SECTION((*g_pPCFinder))

        pTargetCreature = g_pPCFinder->getCreature_LOCKED(TargetGuildMaster);
        if (pTargetCreature == NULL) {
            g_pPCFinder->unlock();
            return;
        }
        GCModifyInformation gcModifyInformation;
        makeGCModifyInfoGuildUnion(&gcModifyInformation, pTargetCreature);
        pTargetCreature->getPlayer()->sendPacket(&gcModifyInformation);

        __LEAVE_CRITICAL_SECTION((*g_pPCFinder))

        sendGCOtherModifyInfoGuildUnion(pTargetCreature);
        sendGCOtherModifyInfoGuildUnion(pCreature);

        
        GuildUnionManager::Instance().sendModifyUnionInfo(dynamic_cast<PlayerCreature*>(pTargetCreature)->getGuildID());
        GuildUnionManager::Instance().sendModifyUnionInfo(dynamic_cast<PlayerCreature*>(pCreature)->getGuildID());
    }

#endif // __GAME_SERVER__

    __END_DEBUG_EX __END_CATCH
}
