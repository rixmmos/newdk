//////////////////////////////////////////////////////////////////////////////
// Filename    : CGGetEventItem.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGGetEventItem.h"

void CGGetEventItem::read(SocketInputStream& iStream) throw(ProtocolException, Error) {
    __BEGIN_TRY
    iStream.read(m_EventType);
    __END_CATCH
}

void CGGetEventItem::write(SocketOutputStream& oStream) const throw(ProtocolException, Error) {
    __BEGIN_TRY
    oStream.write(m_EventType);
    __END_CATCH
}

void CGGetEventItem::execute(Player* pPlayer) throw(ProtocolException, Error) {
    __BEGIN_TRY
#ifndef __GAME_CLIENT__
    CGGetEventItemHandler::execute(this, pPlayer);
#endif
    __END_CATCH
}

std::string CGGetEventItem::toString() const throw() {
    __BEGIN_TRY
    StringStream msg;
    msg << "CGGetEventItem(EventType:" << (int)m_EventType << ")";
    return msg.toString();
    __END_CATCH
}
