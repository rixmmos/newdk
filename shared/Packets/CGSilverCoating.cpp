//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSilverCoating.cpp
//////////////////////////////////////////////////////////////////////////////

#include "CGSilverCoating.h"

void CGSilverCoating::read(SocketInputStream& iStream) throw(ProtocolException, Error) {
    __BEGIN_TRY
    iStream.read(m_ObjectID);
    __END_CATCH
}

void CGSilverCoating::write(SocketOutputStream& oStream) const throw(ProtocolException, Error) {
    __BEGIN_TRY
    oStream.write(m_ObjectID);
    __END_CATCH
}

void CGSilverCoating::execute(Player* pPlayer) throw(ProtocolException, Error) {
    __BEGIN_TRY
#ifndef __GAME_CLIENT__
    CGSilverCoatingHandler::execute(this, pPlayer);
#endif
    __END_CATCH
}

std::string CGSilverCoating::toString() const throw() {
    __BEGIN_TRY
    StringStream msg;
    msg << "CGSilverCoating(ObjectID:" << (int)m_ObjectID << ")";
    return msg.toString();
    __END_CATCH
}
