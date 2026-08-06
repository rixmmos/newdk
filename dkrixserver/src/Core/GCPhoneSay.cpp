//////////////////////////////////////////////////////////////////////
//
// Filename    : GCPhoneSay.cpp
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "GCPhoneSay.h"


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCPhoneSay::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    iStream.read(m_SlotID);

    BYTE szMessage;

    iStream.read(szMessage);

    if (szMessage == 0)
        throw InvalidProtocolException("szMessage == 0");

    if (szMessage > 128)
        throw InvalidProtocolException("too large message length");

    iStream.read(m_Message, szMessage);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCPhoneSay::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    oStream.write(m_SlotID);

    BYTE szMessage = m_Message.size();

    if (szMessage == 0)
        throw InvalidProtocolException("szMessage == 0");

    if (szMessage > 128)
        throw InvalidProtocolException("too large message length");

    oStream.write(szMessage);

    oStream.write(m_Message);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCPhoneSay::execute(Player* pPlayer)

{
    __BEGIN_TRY

    GCPhoneSayHandler::execute(this, pPlayer);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// get packet's debug string
//////////////////////////////////////////////////////////////////////
string GCPhoneSay::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "GCPhoneSay("
        << "SlotID:" << (int)m_SlotID << ",Message:" << m_Message << ")";
    return msg.toString();

    __END_CATCH
}
