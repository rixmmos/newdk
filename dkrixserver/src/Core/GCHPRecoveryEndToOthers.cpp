//////////////////////////////////////////////////////////////////////
//
// Filename    : GCHPRecoveryEndToOthers.cpp
// Written By  : Reiot
//
//////////////////////////////////////////////////////////////////////

// include files
#include "GCHPRecoveryEndToOthers.h"

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
GCHPRecoveryEndToOthers::GCHPRecoveryEndToOthers()

{
    __BEGIN_TRY
    m_CurrentHP = 0;
    m_ObjectID = 0;
    __END_CATCH
}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
GCHPRecoveryEndToOthers::~GCHPRecoveryEndToOthers()

{
    __BEGIN_TRY
    __END_CATCH_NO_RETHROW
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCHPRecoveryEndToOthers::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    iStream.read(m_ObjectID);
    iStream.read(m_CurrentHP);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCHPRecoveryEndToOthers::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    oStream.write(m_ObjectID);
    oStream.write(m_CurrentHP);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCHPRecoveryEndToOthers::execute(Player* pPlayer)

{
    __BEGIN_TRY

    GCHPRecoveryEndToOthersHandler::execute(this, pPlayer);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// get packet's debug string
//
//////////////////////////////////////////////////////////////////////
string GCHPRecoveryEndToOthers::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "GCHPRecoveryEndToOthers("
        << "ObjectID:" << m_ObjectID << ",CurrentHP:" << (int)m_CurrentHP << ")";
    return msg.toString();

    __END_CATCH
}
