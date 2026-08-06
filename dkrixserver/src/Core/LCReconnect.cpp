//----------------------------------------------------------------------
//
// Filename    : LCReconnect.cpp
// Written By  : reiot@ewestsoft.com
// Description :
//
//----------------------------------------------------------------------

// include files
#include "LCReconnect.h"


//----------------------------------------------------------------------

//----------------------------------------------------------------------
void LCReconnect::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    //--------------------------------------------------
    // read game server's ip
    //--------------------------------------------------
    BYTE szGameServerIP;

    iStream.read(szGameServerIP);

    if (szGameServerIP == 0)
        throw InvalidProtocolException("szGameServerIP == 0");

    if (szGameServerIP > 15)
        throw InvalidProtocolException("too long IP length");

    iStream.read(m_GameServerIP, szGameServerIP);

    //--------------------------------------------------
    // read game server's port
    //--------------------------------------------------
    iStream.read(m_GameServerPort);

    //--------------------------------------------------
    // read auth-key
    //--------------------------------------------------
    iStream.read(m_Key);

    __END_CATCH
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------
void LCReconnect::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    //--------------------------------------------------
    // write game server's ip
    //--------------------------------------------------
    BYTE szGameServerIP = m_GameServerIP.size();

    if (szGameServerIP == 0)
        throw InvalidProtocolException("szGameServerIP == 0");

    if (szGameServerIP > 15)
        throw InvalidProtocolException("too long IP length");

    oStream.write(szGameServerIP);

    oStream.write(m_GameServerIP);

    //--------------------------------------------------
    // write game server's port
    //--------------------------------------------------
    oStream.write(m_GameServerPort);

    //--------------------------------------------------
    // write auth-key
    //--------------------------------------------------
    oStream.write(m_Key);

    __END_CATCH
}


//----------------------------------------------------------------------
// execute packet's handler
//----------------------------------------------------------------------
void LCReconnect::execute(Player* pPlayer)

{
    __BEGIN_TRY

    LCReconnectHandler::execute(this, pPlayer);

    __END_CATCH
}


//----------------------------------------------------------------------
// get packet's debug string
//----------------------------------------------------------------------
string LCReconnect::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "LCReconnect(GameServerIP:" << m_GameServerIP << ",GameServerPort:" << m_GameServerPort << ",KEY:" << m_Key
        << ")";

    return msg.toString();

    __END_CATCH
}
