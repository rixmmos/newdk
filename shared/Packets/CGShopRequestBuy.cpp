////////////////////////////////////////////////////////////////////////////////
// Filename    : CGShopRequestBuy.cpp

// Description :



////////////////////////////////////////////////////////////////////////////////

#include "CGShopRequestBuy.h"

void CGShopRequestBuy::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    iStream.read(m_ObjectID);
    iStream.read(m_RackType);
    iStream.read(m_RackIndex);
    iStream.read(m_Num);
    iStream.read(m_X);
    iStream.read(m_Y);

    __END_CATCH
}

void CGShopRequestBuy::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    oStream.write(m_ObjectID);
    oStream.write(m_RackType);
    oStream.write(m_RackIndex);
    oStream.write(m_Num);
    oStream.write(m_X);
    oStream.write(m_Y);

    __END_CATCH
}

void CGShopRequestBuy::execute(Player* pPlayer)

{
    __BEGIN_TRY

#ifndef __GAME_CLIENT__
    CGShopRequestBuyHandler::execute(this, pPlayer);
#endif

    __END_CATCH
}

string CGShopRequestBuy::toString() const {
    __BEGIN_TRY

    StringStream msg;
    msg << "CGShopRequestBuy("
        << "ObjectID:" << (int)m_ObjectID << ","
        << "RackType:" << (int)m_RackType << ","
        << "RackIndex:" << (int)m_RackIndex << ","
        << "NumberOfItems:" << (int)m_Num << ","
        << "InvenX" << (int)m_X << ","
        << "InvenY" << (int)m_Y << ","
        << ")";
    return msg.toString();

    __END_CATCH
}
