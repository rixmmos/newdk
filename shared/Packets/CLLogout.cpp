//////////////////////////////////////////////////////////////////////////////
// Filename    : CLLogout.cpp
// Written By  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CLLogout.h"

void CLLogout::read(SocketInputStream& iStream)

{
    __BEGIN_TRY
    __END_CATCH
}

void CLLogout::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY
    __END_CATCH
}

void CLLogout::execute(Player* pPlayer)

{
    __BEGIN_TRY

#ifndef __GAME_CLIENT__
    CLLogoutHandler::execute(this, pPlayer);
#endif

    __END_CATCH
}
