//////////////////////////////////////////////////////////////////////
//
// Filename    : LGKickCharacter.cpp
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "LGKickCharacter.h"


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void LGKickCharacter::read(Datagram& iDatagram)

{
    __BEGIN_TRY

    iDatagram.read(m_ID);

    //--------------------------------------------------
    // read creature's name
    //--------------------------------------------------
    BYTE szPCName;

    iDatagram.read(szPCName);

    if (szPCName == 0)
        throw InvalidProtocolException("szPCName == 0");

    if (szPCName > 20)
        throw InvalidProtocolException("too long name length");

    iDatagram.read(m_PCName, szPCName);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void LGKickCharacter::write(Datagram& oDatagram) const

{
    __BEGIN_TRY

    oDatagram.write(m_ID);

    //--------------------------------------------------
    // write PC name
    //--------------------------------------------------
    BYTE szPCName = m_PCName.size();

    if (szPCName == 0)
        throw InvalidProtocolException("szPCName == 0");

    if (szPCName > 20)
        throw InvalidProtocolException("too long name length");

    oDatagram.write(szPCName);

    oDatagram.write(m_PCName);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void LGKickCharacter::execute(Player* pPlayer)

{
    __BEGIN_TRY

    LGKickCharacterHandler::execute(this);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string LGKickCharacter::toString() const

{
    StringStream msg;

    msg << "LGKickCharacter("
        << "ID:" << m_ID << ",PCName:" << m_PCName << ")";

    return msg.toString();
}
