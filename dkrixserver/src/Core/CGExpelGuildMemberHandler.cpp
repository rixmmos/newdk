//////////////////////////////////////////////////////////////////////////////
// Filename    : CGExpelGuildMemberHandler.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGExpelGuildMember.h"

#ifdef __GAME_SERVER__
#include "GSExpelGuildMember.h"
#include "Guild.h"
#include "GuildManager.h"
#include "Player.h"
#include "PlayerCreature.h"
#include "SharedServerManager.h"
#include "Slayer.h"
#include "SystemAvailabilitiesManager.h"
#include "Vampire.h"
#endif // __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGExpelGuildMemberHandler::execute(CGExpelGuildMember* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

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

    SYSTEM_ASSERT(SYSTEM_GUILD);

    
    Guild* pGuild = g_pGuildManager->getGuild(pPacket->getGuildID());
    try {
        Assert(pGuild != NULL);
    } catch (Throwable& t) {
        return;
    }

    
    if (pGuild->getMaster() != pPlayerCreature->getName()) {
        
        return;
    }

    GSExpelGuildMember gsExpelGuildMember;
    gsExpelGuildMember.setGuildID(pGuild->getID());
    gsExpelGuildMember.setName(pPlayerCreature->getName());
    gsExpelGuildMember.setSender(pPlayerCreature->getName());

    g_pSharedServerManager->sendPacket(&gsExpelGuildMember);

#endif // __GAME_SERVER__

    __END_DEBUG_EX __END_CATCH
}
