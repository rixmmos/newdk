//----------------------------------------------------------------------
//
// Filename    : GCAddVampireCorpse.cpp
// Written By  : Reiot
//
//----------------------------------------------------------------------

// include files
#include "GCAddVampireCorpse.h"


//----------------------------------------------------------------------

//----------------------------------------------------------------------
void GCAddVampireCorpse::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    m_VampireInfo.read(iStream);
    iStream.read(m_TreasureCount);

    __END_CATCH
}


//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void GCAddVampireCorpse::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    m_VampireInfo.write(oStream);
    oStream.write(m_TreasureCount);

    __END_CATCH
}


//--------------------------------------------------------------------------------
// execute packet's handler
//--------------------------------------------------------------------------------
void GCAddVampireCorpse::execute(Player* pPlayer)

{
    __BEGIN_TRY

    GCAddVampireCorpseHandler::execute(this, pPlayer);

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get packet's debug string
//--------------------------------------------------------------------------------
string GCAddVampireCorpse::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "GCAddVampireCorpse(" << m_VampireInfo.toString() << ", Count : " << (int)m_TreasureCount << ")";

    return msg.toString();

    __END_CATCH
}
