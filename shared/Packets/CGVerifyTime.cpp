//////////////////////////////////////////////////////////////////////////////
// Filename    : CGVerifyTime.cpp
// Written By  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGVerifyTime.h"

#ifdef __GAME_SERVER__
#include "GamePlayer.h"
#endif

void CGVerifyTime::read(SocketInputStream& iStream) throw(ProtocolException, Error) {
    __BEGIN_TRY
    __END_CATCH
}

void CGVerifyTime::write(SocketOutputStream& oStream) const throw(ProtocolException, Error) {
    __BEGIN_TRY
    __END_CATCH
}

void CGVerifyTime::execute(Player* pPlayer) throw(ProtocolException, Error) {
    __BEGIN_TRY

#ifndef __GAME_CLIENT__
    CGVerifyTimeHandler::execute(this, pPlayer);
#endif

    __END_CATCH
}

std::string CGVerifyTime::toString() const throw() {
    __BEGIN_TRY

    StringStream msg;
    msg << "CGVerifyTime()";
    return msg.toString();

    __END_CATCH
}
