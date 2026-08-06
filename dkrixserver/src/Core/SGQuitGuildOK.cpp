//////////////////////////////////////////////////////////////////////
//
// Filename    : SGQuitGuildOK.cpp
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "SGQuitGuildOK.h"


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void SGQuitGuildOK::read(SocketInputStream& iStream) {
    __BEGIN_TRY

    iStream.read(m_GuildID);

    BYTE szName;

    iStream.read(szName);

    if (szName == 0)
        throw InvalidProtocolException("szName == 0");
    if (szName > 20)
        throw InvalidProtocolException("too long name length");

    iStream.read(m_Name, szName);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void SGQuitGuildOK::write(SocketOutputStream& oStream) const {
    __BEGIN_TRY

    BYTE szName = m_Name.size();

    if (szName == 0)
        throw InvalidProtocolException("szName == 0");
    if (szName > 20)
        throw InvalidProtocolException("too long name length");

    oStream.write(m_GuildID);
    oStream.write(szName);
    oStream.write(m_Name);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void SGQuitGuildOK::execute(Player* pPlayer) {
#ifdef __GAME_SERVER__

    __BEGIN_TRY

    SGQuitGuildOKHandler::execute(this);

    __END_CATCH

#endif
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string SGQuitGuildOK::toString() const {
    StringStream msg;

    msg << "SGQuitGuildOK("
        << "GuildID:" << (int)m_GuildID << "Name:" << m_Name << ")";

    return msg.toString();
}
