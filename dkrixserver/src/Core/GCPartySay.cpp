//////////////////////////////////////////////////////////////////////
//
// Filename    : GCPartySay.cpp
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "GCPartySay.h"


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCPartySay::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    BYTE szName;
    iStream.read(szName);
    iStream.read(m_Name, szName);
    iStream.read(m_Color);
    iStream.read(szName);
    iStream.read(m_Message, szName);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCPartySay::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    BYTE szName = m_Name.size();
    oStream.write(szName);
    oStream.write(m_Name);
    oStream.write(m_Color);
    szName = m_Message.size();
    oStream.write(szName);
    oStream.write(m_Message);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCPartySay::execute(Player* pPlayer)

{
    __BEGIN_TRY

    GCPartySayHandler::execute(this, pPlayer);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// get packet's debug string
//
//////////////////////////////////////////////////////////////////////
string GCPartySay::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "GCPartySay("
        << "Name : " << m_Name << ", Message : " << m_Message << ")";
    return msg.toString();

    __END_CATCH
}
