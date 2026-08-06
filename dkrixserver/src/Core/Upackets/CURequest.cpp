//--------------------------------------------------------------------------------
//
// Filename    : CURequest.cpp
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "CURequest.h"


//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void CURequest::read(SocketInputStream& iStream) throw(ProtocolException, Error) {
    __BEGIN_TRY

    m_Resource.read(iStream);

    __END_CATCH
}


//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void CURequest::read(Socket* pSocket) throw(ProtocolException, Error) {
    __BEGIN_TRY

    m_Resource.read(pSocket);

    __END_CATCH
}


//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void CURequest::write(SocketOutputStream& oStream) const throw(ProtocolException, Error) {
    __BEGIN_TRY

    m_Resource.write(oStream);

    __END_CATCH
}


//--------------------------------------------------------------------------------
// execute packet's handler
//--------------------------------------------------------------------------------
void CURequest::execute(Player* pPlayer) throw(ProtocolException, Error) {
    __BEGIN_TRY

    CURequestHandler::execute(this, pPlayer);

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string CURequest::toString() const throw() {
    StringStream msg;

    msg << "CURequest(" << m_Resource.toString() << ")";

    return msg.toString();
}
