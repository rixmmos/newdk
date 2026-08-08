//////////////////////////////////////////////////////////////////////////////
// Filename    : CGQuitUnionHandler.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGQuitUnion.h"

#ifdef __GAME_SERVER__
#include "Assert.h"
#include "DB.h"
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
#include "PreparedStatement.h"
#include "StringPool.h"
#include "SystemAvailabilitiesManager.h"
#endif // __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGQuitUnionHandler::execute(CGQuitUnion* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        //	//cout << "enter cgquitunionhandler::execute" << endl;
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
        //		//cout << " out 1 " << endl;
        return;
    }

    uint tempUnionID = pUnion->getUnionID();


    if (!g_pGuildManager->isGuildMaster(pPlayerCreature->getGuildID(), pPlayerCreature)
        //|| pUnion->getMasterGuildID() != pPlayerCreature->getGuildID()
    ) {
        gcGuildResponse.setCode(GuildUnionOfferManager::SOURCE_IS_NOT_MASTER);
        pPlayer->sendPacket(&gcGuildResponse);

        // cout << " out 2 " << endl;
        return;
    }

    // cout << "CGQuitUnion - Guild : " << pPacket->getGuildID() << ", Method : " << (int)pPacket->getQuitMethod() <<
    // endl;


    if (pPacket->getQuitMethod() == CGQuitUnion::QUIT_NORMAL) {
        uint result = GuildUnionOfferManager::Instance().offerQuit(pPlayerCreature->getGuildID());

        gcGuildResponse.setCode(result);
        pPlayer->sendPacket(&gcGuildResponse);
        // cout << "normal send quit result : " << result << endl;
    }

    else if (pPacket->getQuitMethod() == CGQuitUnion::QUIT_QUICK) {
        string TargetGuildMaster = g_pGuildManager->getGuild(pUnion->getMasterGuildID())->getMaster();

        if (GuildUnionManager::Instance().removeGuild(pUnion->getUnionID(), pPlayerCreature->getGuildID())) {
            gcGuildResponse.setCode(GuildUnionOfferManager::OK);
            pPlayer->sendPacket(&gcGuildResponse);


            Statement* pStmt = NULL;
            BEGIN_DB {
                Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

                string escapeGuildName = g_pGuildManager->getGuildName(pPlayerCreature->getGuildID());
                string escapeGuildNotice = "[" + escapeGuildName + "] " + g_pStringPool->c_str(378);

                PreparedStatement msgStmt(pConn, "INSERT INTO Messages (Receiver, Message) values(?,?)");
                msgStmt.bindString(1, TargetGuildMaster);
                msgStmt.bindString(2, escapeGuildNotice);
                msgStmt.execute();

                PreparedStatement offerStmt(pConn, "INSERT INTO GuildUnionOffer values(?,'ESCAPE',?,now())");
                offerStmt.bindInt(1, (int)tempUnionID);
                offerStmt.bindInt(2, (int)pPacket->getGuildID());
                offerStmt.execute();

                PreparedStatement countStmt(pConn, "SELECT count(*) FROM GuildUnionMember WHERE UnionID=?");
                countStmt.bindInt(1, (int)tempUnionID);
                Result* pResult = countStmt.execute();
                pResult->next();

                if (pResult->getInt(1) == 0) {
                    // (int)tempUnionID << endl;
                    PreparedStatement deleteUnionStmt(pConn, "DELETE FROM GuildUnionInfo WHERE UnionID=?");
                    deleteUnionStmt.bindInt(1, (int)tempUnionID);
                    deleteUnionStmt.execute();

                    PreparedStatement brokenMsgStmt(pConn, "INSERT INTO Messages (Receiver, Message) values(?,?)");
                    brokenMsgStmt.bindString(1, TargetGuildMaster);
                    brokenMsgStmt.bindString(2, g_pStringPool->c_str(379));
                    brokenMsgStmt.execute();
                    GuildUnionManager::Instance().reload();
                }
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
            GCModifyInformation gcModifyInformation2;
            makeGCModifyInfoGuildUnion(&gcModifyInformation2, pTargetCreature);
            pTargetCreature->getPlayer()->sendPacket(&gcModifyInformation2);

            __LEAVE_CRITICAL_SECTION((*g_pPCFinder))

            //////////////////////////////


            sendGCOtherModifyInfoGuildUnion(pTargetCreature);
            sendGCOtherModifyInfoGuildUnion(pCreature);


            GuildUnionManager::Instance().sendModifyUnionInfo(
                dynamic_cast<PlayerCreature*>(pTargetCreature)->getGuildID());
            GuildUnionManager::Instance().sendModifyUnionInfo(dynamic_cast<PlayerCreature*>(pCreature)->getGuildID());


            // cout << "quick send quit result : OK" << endl;

        } else {
            gcGuildResponse.setCode(GuildUnionOfferManager::NOT_YOUR_UNION);
            pPlayer->sendPacket(&gcGuildResponse);

            // cout << "quick quit failed " << endl;
        }
    }

#endif // __GAME_SERVER__

    __END_DEBUG_EX __END_CATCH
}
