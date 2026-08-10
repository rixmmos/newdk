//////////////////////////////////////////////////////////////////////////////
// Filename    : CGConnect.cpp
// Written By  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGConnect.h"

void CGConnect::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    // read authentication key
    iStream.read(m_Key);

    // read PC type
    BYTE pcType;
    iStream.read(pcType);

    // SECURITY: PCType defines three values but this is a raw wire BYTE, so
    // 0-255 arrives here. It is then indexed into the three-entry
    // PCType2String[] by toString(), which SocketInputStream::readPacket()
    // calls on every packet -- before this connection is authenticated.
    // Reject it at the wire boundary, the same way the name length below is.
    if (pcType > PC_OUSTERS)
        throw InvalidProtocolException("invalid pc type");

    m_PCType = PCType(pcType);

    // read PC name
    BYTE szPCName;
    iStream.read(szPCName);

    if (szPCName == 0)
        throw InvalidProtocolException("szPCName == 0");

    if (szPCName > 20)
        throw InvalidProtocolException("too long pc name length");

    iStream.read(m_PCName, szPCName);
    iStream.read((char*)m_MacAddress, 6);

    __END_CATCH
}

void CGConnect::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    // write authentication key
    oStream.write(m_Key);

    // write PC type
    oStream.write((BYTE)m_PCType);

    // write PC name
    BYTE szPCName = m_PCName.size();

    if (szPCName == 0)
        throw InvalidProtocolException("szPCName == 0");

    if (szPCName > 20)
        throw InvalidProtocolException("too long pc name length");

    oStream.write(szPCName);
    oStream.write(m_PCName);

    oStream.write((char*)m_MacAddress, 6);

    __END_CATCH
}

void CGConnect::execute(Player* pPlayer)

{
    __BEGIN_TRY

#ifndef __GAME_CLIENT__
    CGConnectHandler::execute(this, pPlayer);
#endif

    __END_CATCH
}

string CGConnect::toString() const

{
    // SECURITY: bounded independently of read()'s check above. toString() is
    // called by SocketInputStream::readPacket() on every packet received, so
    // it must not be able to fault whatever reaches it -- and it is also
    // called on locally built CGConnects (LCReconnectHandler) whose PCType
    // never went through read(). "UNKNOWN" matches CGLearnSkill::toString().
    const unsigned int pcTypeIndex = (unsigned int)m_PCType;
    string pcType = "UNKNOWN";
    if (pcTypeIndex <= (unsigned int)PC_OUSTERS)
        pcType = PCType2String[pcTypeIndex];

    StringStream msg;
    msg << "CGConnect("
        << "KEY:" << m_Key << ",PCType:" << pcType << ",PCName:" << m_PCName << ")";
    return msg.toString();
}
