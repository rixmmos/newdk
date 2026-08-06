//////////////////////////////////////////////////////////////////////
//
// Filename    : GSModifyGuildIntro.cpp
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "GSModifyGuildIntro.h"


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GSModifyGuildIntro::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    BYTE szGuildIntro;

    iStream.read(m_GuildID);
    iStream.read(szGuildIntro);

    if (szGuildIntro > 255)
        throw InvalidProtocolException("too long szGuildIntro length");

    if (szGuildIntro > 0)
        iStream.read(m_GuildIntro, szGuildIntro);
    else
        m_GuildIntro = "";

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GSModifyGuildIntro::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    BYTE szGuildIntro = m_GuildIntro.size();

    if (szGuildIntro > 255)
        throw InvalidProtocolException("too long szGuildIntro length");

    oStream.write(m_GuildID);
    oStream.write(szGuildIntro);

    if (szGuildIntro > 0)
        oStream.write(m_GuildIntro);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GSModifyGuildIntro::execute(Player* pPlayer)

{
    __BEGIN_TRY

    GSModifyGuildIntroHandler::execute(this, pPlayer);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string GSModifyGuildIntro::toString() const

{
    StringStream msg;

    msg << "GSModifyGuildIntro ("
        << "GuildID:" << (int)m_GuildID << "GuildIntro:" << m_GuildIntro << " )";

    return msg.toString();
}
