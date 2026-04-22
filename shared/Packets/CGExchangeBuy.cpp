//////////////////////////////////////////////////////////////////////////////
// Filename    : CGExchangeBuy.cpp
// Written By  : Exchange System
// Description : Client buys an item from the exchange
//////////////////////////////////////////////////////////////////////////////

#include "CGExchangeBuy.h"

void CGExchangeBuy::read(SocketInputStream& iStream) {
    __BEGIN_TRY

    BYTE keySize = 0;
    iStream.read(m_ListingID);
    iStream.read(keySize);
    iStream.read(m_IdempotencyKey, keySize);

    __END_CATCH
}

void CGExchangeBuy::write(SocketOutputStream& oStream) const {
    __BEGIN_TRY

    BYTE keySize = m_IdempotencyKey.size();
    oStream.write(m_ListingID);
    oStream.write(keySize);
    oStream.write(m_IdempotencyKey);

    __END_CATCH
}

string CGExchangeBuy::toString() const {
    StringStream msg;
    msg << "CGExchangeBuy(ListingID:" << (int)m_ListingID
        << ",IdempotencyKey:" << m_IdempotencyKey << ")";
    return msg.toString();
}

void CGExchangeBuy::execute(Player* pPlayer) {
    __BEGIN_TRY

#ifndef __GAME_CLIENT__
    CGExchangeBuyHandler::execute(this, pPlayer);
#endif

    __END_CATCH
}
