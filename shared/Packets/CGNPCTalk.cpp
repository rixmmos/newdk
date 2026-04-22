//////////////////////////////////////////////////////////////////////////////
// Filename    : CGNPCTalk.cpp
// Written By  : Reiot
//////////////////////////////////////////////////////////////////////////////

#include "CGNPCTalk.h"

void CGNPCTalk::read(SocketInputStream& iStream) throw(ProtocolException, Error) {
    __BEGIN_TRY
    iStream.read(m_ObjectID);
    __END_CATCH
}

void CGNPCTalk::write(SocketOutputStream& oStream) const throw(ProtocolException, Error) {
    __BEGIN_TRY
    oStream.write(m_ObjectID);
    __END_CATCH
}

void CGNPCTalk::execute(Player* pPlayer) throw(ProtocolException, Error) {
    __BEGIN_TRY
#ifndef __GAME_CLIENT__
    CGNPCTalkHandler::execute(this, pPlayer);
#endif
    __END_CATCH
}

std::string CGNPCTalk::toString() const throw() {
    __BEGIN_TRY
    StringStream msg;
    msg << "CGNPCTalk(ObjectID:" << (int)m_ObjectID << ")";
    return msg.toString();
    __END_CATCH
}
