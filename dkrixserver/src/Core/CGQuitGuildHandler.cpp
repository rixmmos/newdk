//////////////////////////////////////////////////////////////////////////////
// Filename    : CGQuitGuildHandler.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGQuitGuild.h"

#ifdef __GAME_SERVER__
#include "GSQuitGuild.h"
#include "Guild.h"
#include "GuildManager.h"
#include "Player.h"
#include "PlayerCreature.h"
#include "SharedServerManager.h"
#include "SystemAvailabilitiesManager.h"
#endif // __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGQuitGuildHandler::execute(CGQuitGuild* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    SYSTEM_ASSERT(SYSTEM_GUILD);

    // NOTE: Player and PlayerCreature are unrelated hierarchies (Player is the
    // session, PlayerCreature derives from Creature), so this cast always
    // yields NULL and this handler has never done anything. Every sibling
    // handler goes GamePlayer -> getCreature() -> PlayerCreature instead.
    // Correcting the cast would make a dormant guild operation live again, so
    // it is left for the owner to decide; the check below only stops the
    // dereference below from being a NULL deref once Assert() is compiled out.
    PlayerCreature* pPlayerCreature = dynamic_cast<PlayerCreature*>(pPlayer);
    if (pPlayerCreature == NULL)
        return;


    Guild* pGuild = g_pGuildManager->getGuild(pPacket->getGuildID());
    try {
        Assert(pGuild != NULL);
    } catch (Throwable& t) {
        return;
    }

    
    GuildMember* pGuildMember = pGuild->getMember(pPlayerCreature->getName());
    if (pGuildMember == NULL)
        return;

    GSQuitGuild gsQuitGuild;
    gsQuitGuild.setGuildID(pGuild->getID());
    gsQuitGuild.setName(pPlayerCreature->getName());

    g_pSharedServerManager->sendPacket(&gsQuitGuild);

#endif // __GAME_SERVER__

    __END_DEBUG_EX __END_CATCH
}
