//-------------------------------------------------------------------------------- //
// Filename    : GCRequestPowerPointResult.cpp
// Written By  : reiot@ewestsoft.com
// Description :
//
//--------------------------------------------------------------------------------

// include files
#include "GCRequestPowerPointResult.h"


//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
GCRequestPowerPointResult::GCRequestPowerPointResult()

    : m_ErrorCode(0), m_SumPowerPoint(0), m_RequestPowerPoint(0) {}

//--------------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------------
GCRequestPowerPointResult::~GCRequestPowerPointResult()

{}

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void GCRequestPowerPointResult::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    // Error code
    iStream.read(m_ErrorCode);

    
    iStream.read(m_SumPowerPoint);

    
    iStream.read(m_RequestPowerPoint);

    __END_CATCH
}


//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void GCRequestPowerPointResult::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    // Error code
    oStream.write(m_ErrorCode);

    
    oStream.write(m_SumPowerPoint);

    
    oStream.write(m_RequestPowerPoint);

    __END_CATCH
}


//--------------------------------------------------------------------------------
// execute packet's handler
//--------------------------------------------------------------------------------
void GCRequestPowerPointResult::execute(Player* pPlayer)

{
    __BEGIN_TRY

    GCRequestPowerPointResultHandler::execute(this, pPlayer);

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get packet's debug string
//--------------------------------------------------------------------------------
string GCRequestPowerPointResult::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "GCRequestPowerPointResult("
        << "ErrorCode:" << (int)m_ErrorCode << ",SumPowerPoint:" << m_SumPowerPoint
        << ",RequestPowerPoint:" << m_RequestPowerPoint << ")";

    return msg.toString();

    __END_CATCH
}
