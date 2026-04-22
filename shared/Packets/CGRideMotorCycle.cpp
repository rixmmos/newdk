//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRideMotorCycle.cpp
//////////////////////////////////////////////////////////////////////////////

#include "CGRideMotorCycle.h"

CGRideMotorCycle::CGRideMotorCycle() throw() {
    __BEGIN_TRY
    __END_CATCH
}

CGRideMotorCycle::~CGRideMotorCycle() throw() {
    __BEGIN_TRY
    __END_CATCH
}

void CGRideMotorCycle::read(SocketInputStream& iStream) throw(ProtocolException, Error) {
    __BEGIN_TRY
    iStream.read(m_ObjectID);
    iStream.read(m_X);
    iStream.read(m_Y);
    __END_CATCH
}

void CGRideMotorCycle::write(SocketOutputStream& oStream) const throw(ProtocolException, Error) {
    __BEGIN_TRY
    oStream.write(m_ObjectID);
    oStream.write(m_X);
    oStream.write(m_Y);
    __END_CATCH
}

void CGRideMotorCycle::execute(Player* pPlayer) throw(ProtocolException, Error) {
    __BEGIN_TRY
#ifndef __GAME_CLIENT__
    CGRideMotorCycleHandler::execute(this, pPlayer);
#endif
    __END_CATCH
}

std::string CGRideMotorCycle::toString() const throw() {
    __BEGIN_TRY
    StringStream msg;
    msg << "CGRideMotorCycle(ObjectID:" << (int)m_ObjectID << ",X:" << (int)m_X << ",Y:" << (int)m_Y << ")";
    return msg.toString();
    __END_CATCH
}
