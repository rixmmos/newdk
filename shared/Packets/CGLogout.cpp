//////////////////////////////////////////////////////////////////////////////
// Filename    : CGLogout.cpp
// Written By  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGLogout.h"

void CGLogout::read(SocketInputStream& iStream)

{
    __BEGIN_TRY
    __END_CATCH
}

void CGLogout::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY
    __END_CATCH
}

void CGLogout::execute(Player* pPlayer)

{
    __BEGIN_TRY

#ifndef __GAME_CLIENT__
    CGLogoutHandler::execute(this, pPlayer);
#endif

    __END_CATCH
}
