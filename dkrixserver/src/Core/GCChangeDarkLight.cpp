//--------------------------------------------------------------------------------
//
// Filename    : GCChangeDarkLight.cpp
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "GCChangeDarkLight.h"


//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void GCChangeDarkLight::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    iStream.read(m_DarkLevel);
    iStream.read(m_LightLevel);

    __END_CATCH
}


//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void GCChangeDarkLight::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    oStream.write(m_DarkLevel);
    oStream.write(m_LightLevel);

    __END_CATCH
}


//--------------------------------------------------------------------------------
// execute packet's handler
//--------------------------------------------------------------------------------
void GCChangeDarkLight::execute(Player* pPlayer)

{
    __BEGIN_TRY

    GCChangeDarkLightHandler::execute(this, pPlayer);

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get packet's debug string
//--------------------------------------------------------------------------------
string GCChangeDarkLight::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "GCChangeDarkLight("
        << "DarkLevel:" << (int)m_DarkLevel << ",LightLevel:" << (int)m_LightLevel << ")";
    return msg.toString();

    __END_CATCH
}
