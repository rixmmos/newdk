//////////////////////////////////////////////////////////////////////////////
// Filename    : CGExchangeList.cpp
// Written By  : Exchange System
// Description : Client requests listing list from server
//////////////////////////////////////////////////////////////////////////////

#include "CGExchangeList.h"

void CGExchangeList::read(SocketInputStream& iStream) {
    __BEGIN_TRY

    BYTE sellerSize = 0;
    iStream.read(m_Page);
    iStream.read(m_PageSize);
    iStream.read(m_ItemClass);
    iStream.read(m_ItemType);
    iStream.read(m_MinPrice);
    iStream.read(m_MaxPrice);
    iStream.read(sellerSize);
    iStream.read(m_SellerFilter, sellerSize);

    __END_CATCH
}

void CGExchangeList::write(SocketOutputStream& oStream) const {
    __BEGIN_TRY

    BYTE sellerSize = m_SellerFilter.size();
    oStream.write(m_Page);
    oStream.write(m_PageSize);
    oStream.write(m_ItemClass);
    oStream.write(m_ItemType);
    oStream.write(m_MinPrice);
    oStream.write(m_MaxPrice);
    oStream.write(sellerSize);
    oStream.write(m_SellerFilter);

    __END_CATCH
}

string CGExchangeList::toString() const {
    StringStream msg;
    msg << "CGExchangeList("
        << "Page:" << (int)m_Page
        << ", PageSize:" << (int)m_PageSize
        << ", ItemClass:" << (int)m_ItemClass
        << ", ItemType:" << (int)m_ItemType
        << ", MinPrice:" << (int)m_MinPrice
        << ", MaxPrice:" << (int)m_MaxPrice
        << ", SellerFilter:" << m_SellerFilter
        << ")";
    return msg.toString();
}

void CGExchangeList::execute(Player* pPlayer) {
    __BEGIN_TRY

#ifndef __GAME_CLIENT__
    CGExchangeListHandler::execute(this, pPlayer);
#endif

    __END_CATCH
}
