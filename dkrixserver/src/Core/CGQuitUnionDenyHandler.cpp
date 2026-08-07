
//////////////////////////////////////////////////////////////////////////////
// Filename    : CGQuitUnionDenyHandler.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGQuitUnionDeny.h"

#ifdef __GAME_SERVER__
#include "Assert.h"
#include "DB.h"
#include "GCGuildResponse.h"
#include "GCSystemMessage.h"
#include "GamePlayer.h"
#include "Guild.h"
#include "GuildManager.h"
#include "GuildUnion.h"
#include "PlayerCreature.h"
#include "StringPool.h"
#include "SystemAvailabilitiesManager.h"
#endif // __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGQuitUnionDenyHandler::execute(CGQuitUnionDeny* pPacket, Player* pPlayer)

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

    uint result = GuildUnionOfferManager::Instance().denyQuit(pPacket->getGuildID());

    gcGuildResponse.setCode(result);
    pPlayer->sendPacket(&gcGuildResponse);

    ////////////////////

    Guild* pGuild = g_pGuildManager->getGuild(pPacket->getGuildID());

    if (pGuild == NULL) {
        return;
    }
    string TargetGuildMaster = pGuild->getMaster();


    Statement* pStmt = NULL;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        pStmt->executeQuery("INSERT INTO Messages (Receiver, Message) values('%s','%s')", TargetGuildMaster.c_str(),
                            g_pStringPool->c_str(376));
        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)

#endif // __GAME_SERVER__

    __END_DEBUG_EX __END_CATCH
}
