//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAddWolf.cc
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "GCAddWolf.h"

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCAddWolf::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    
    iStream.read(m_ObjectID);

    BYTE szName;

    iStream.read(szName);

    if (szName == 0)
        throw InvalidProtocolException("szName == 0");

    if (szName > 20)
        throw InvalidProtocolException("too large name length");

    iStream.read(m_Name, szName);
    /*
        iStream.read(m_SpriteType);

        iStream.read(m_SubColor);
    */
    iStream.read(m_MainColor);
    iStream.read(m_ItemType);
    iStream.read(m_X);
    iStream.read(m_Y);
    iStream.read(m_Dir);
    iStream.read(m_CurrentHP);
    iStream.read(m_MaxHP);
    iStream.read(m_GuildID);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCAddWolf::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    
    oStream.write(m_ObjectID);

    BYTE szName = m_Name.size();

    if (szName == 0)
        throw InvalidProtocolException("szName == 0");

    if (szName > 20)
        throw InvalidProtocolException("too large name length");

    oStream.write(szName);
    oStream.write(m_Name);
    /*
        oStream.write(m_SpriteType);

        oStream.write(m_SubColor);
    */
    oStream.write(m_MainColor);
    oStream.write(m_ItemType);
    oStream.write(m_X);
    oStream.write(m_Y);
    oStream.write(m_Dir);
    oStream.write(m_CurrentHP);
    oStream.write(m_MaxHP);
    oStream.write(m_GuildID);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCAddWolf::execute(Player* pPlayer)

{
    __BEGIN_TRY

    GCAddWolfHandler::execute(this, pPlayer);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// get packet's debug string
//
//////////////////////////////////////////////////////////////////////
string GCAddWolf::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "GCAddWolf("
        << "ObjectID:" << m_ObjectID << ",Name:" << m_Name << ",Color:" << m_MainColor
        << ",ItemType:" << (int)m_ItemType << ",X:" << (int)m_X << ",Y:" << (int)m_Y << ",Dir: " << (int)m_Dir
        << ",CurrentHP: " << (int)m_CurrentHP << ",MaxHP: " << (int)m_MaxHP << ",GuildID: " << (int)m_GuildID << ")";

    return msg.toString();

    __END_CATCH
}
