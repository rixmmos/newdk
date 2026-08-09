//////////////////////////////////////////////////////////////////////////////
// Filename    : CGShopRequestList.cpp

// Description :



//////////////////////////////////////////////////////////////////////////////

#include "CGShopRequestList.h"

void CGShopRequestList::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    iStream.read(m_ObjectID);
    iStream.read(m_RackType);

    __END_CATCH
}

void CGShopRequestList::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    oStream.write(m_ObjectID);
    oStream.write(m_RackType);

    __END_CATCH
}

void CGShopRequestList::execute(Player* pPlayer)

{
    __BEGIN_TRY

#ifndef __GAME_CLIENT__
    CGShopRequestListHandler::execute(this, pPlayer);
#endif

    __END_CATCH
}

string CGShopRequestList::toString() const {
    StringStream msg;
    msg << "CGShopRequestList("
        << "ObjectID:" << (int)m_ObjectID << "RackType:" << (int)m_RackType << ")";
    return msg.toString();
}
