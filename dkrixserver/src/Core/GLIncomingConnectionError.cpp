//////////////////////////////////////////////////////////////////////
//
// Filename    : GLIncomingConnectionError.cpp
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "GLIncomingConnectionError.h"


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GLIncomingConnectionError::read(Datagram& iDatagram)

{
    __BEGIN_TRY

    //--------------------------------------------------
    //--------------------------------------------------
    BYTE szMessage;

    iDatagram.read(szMessage);

    if (szMessage == 0)
        throw InvalidProtocolException("szMessage == 0");

    if (szMessage >= 128)
        throw InvalidProtocolException("too large message length");

    iDatagram.read(m_Message, szMessage);


    //--------------------------------------------------
    //--------------------------------------------------
    BYTE szPlayerID;

    iDatagram.read(szPlayerID);

    if (szPlayerID == 0)
        throw InvalidProtocolException("szPlayerID == 0");

    if (szPlayerID >= 128)
        throw InvalidProtocolException("too large playerID length");

    iDatagram.read(m_PlayerID, szPlayerID);


    __END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GLIncomingConnectionError::write(Datagram& oDatagram) const {
    __BEGIN_TRY

    //--------------------------------------------------
    //--------------------------------------------------
    BYTE szMessage = m_Message.size();

    if (szMessage == 0)
        throw InvalidProtocolException("szMessage == 0");

    if (szMessage >= 128)
        throw InvalidProtocolException("too large message length");

    oDatagram.write(szMessage);

    oDatagram.write(m_Message);


    //--------------------------------------------------
    //--------------------------------------------------
    BYTE szPlayerID = m_PlayerID.size();

    if (szPlayerID == 0)
        throw InvalidProtocolException("szPlayerID == 0");

    if (szPlayerID >= 128)
        throw InvalidProtocolException("too large playerID length");

    oDatagram.write(szPlayerID);

    oDatagram.write(m_PlayerID);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GLIncomingConnectionError::execute(Player* pPlayer) {
    __BEGIN_TRY

    // cout << "GLIncomingConnectionError::execute() called." << endl;

    GLIncomingConnectionErrorHandler::execute(this);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string GLIncomingConnectionError::toString() const {
    __BEGIN_TRY

    StringStream msg;
    msg << "GLIncomingConnectionError("
        << "Message:" << m_Message << "PlayerID:" << m_PlayerID << ")";
    return msg.toString();

    __END_CATCH
}
