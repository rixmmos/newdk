//////////////////////////////////////////////////////////////////////////////
// Filename    : CGMixItem.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGMixItem.h"

void CGMixItem::read(SocketInputStream& iStream) throw(ProtocolException, Error) {
    __BEGIN_TRY
    iStream.read(m_ObjectID);
    iStream.read(m_InvenX);
    iStream.read(m_InvenY);
    iStream.read(m_TargetObjectID[0]);
    iStream.read(m_TargetObjectID[1]);
    __END_CATCH
}

void CGMixItem::write(SocketOutputStream& oStream) const throw(ProtocolException, Error) {
    __BEGIN_TRY
    oStream.write(m_ObjectID);
    oStream.write(m_InvenX);
    oStream.write(m_InvenY);
    oStream.write(m_TargetObjectID[0]);
    oStream.write(m_TargetObjectID[1]);
    __END_CATCH
}

void CGMixItem::execute(Player* pPlayer) throw(ProtocolException, Error) {
    __BEGIN_TRY
#ifndef __GAME_CLIENT__
    CGMixItemHandler::execute(this, pPlayer);
#endif
    __END_CATCH
}

std::string CGMixItem::toString() const throw() {
    __BEGIN_TRY
    StringStream msg;
    msg << "CGMixItem("
        << "ObjectID:" << (int)m_ObjectID
        << ",InvenX:" << (int)m_InvenX
        << ",InvenY:" << (int)m_InvenY
        << ",ObjectID:" << (int)m_TargetObjectID[0]
        << ",ObjectID:" << (int)m_TargetObjectID[1]
        << ")";
    return msg.toString();
    __END_CATCH
}
