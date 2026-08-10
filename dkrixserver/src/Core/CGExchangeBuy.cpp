//////////////////////////////////////////////////////////////////////////////
// Filename    : CGExchangeBuy.cpp
// Written By  : Exchange System
// Description : Client requests to buy an item
//////////////////////////////////////////////////////////////////////////////

#include "CGExchangeBuy.h"
#include "GCExchangeBuy.h"

#include "PlayerCreature.h"
#include "../server/gameserver/exchange/ExchangeService.h"

void CGExchangeBuy::read(SocketInputStream& iStream) {
    __BEGIN_TRY

    uint64_t listingID;
    iStream.read(listingID);
    m_ListingID = (int64_t)listingID;

    // SECURITY: this was `iStream.read(m_IdempotencyKey)`. SocketInputStream has
    // no one-argument read(string&) overload, so that bound to the raw template
    // read<T>(T&) with T = std::string, which does `buf = *(T*)(m_Buffer +
    // m_Head)` -- it reinterpreted 32 bytes of wire data as a live std::string
    // and copy-assigned from it, letting the client choose both the source
    // pointer and the length (arbitrary-address read; the ring-buffer-wrap
    // branch memcpy'd over the string's internals so the destructor freed an
    // attacker-chosen pointer). Use the house convention instead: a BYTE length
    // prefix, capped, then the two-argument read. Cap 64 matches
    // USERINFO.sql's IdempotencyKey VARCHAR(64) and the size this packet's
    // factory already advertises. An empty key is legal -- it means "server,
    // generate one" (see ExchangeDB).
    BYTE szKey;
    iStream.read(szKey);

    if (szKey > 64)
        throw InvalidProtocolException("CGExchangeBuy: idempotency key too long");

    m_IdempotencyKey.clear();
    if (szKey > 0)
        iStream.read(m_IdempotencyKey, szKey);

    __END_CATCH
}

void CGExchangeBuy::write(SocketOutputStream& oStream) const {
    __BEGIN_TRY

    oStream.write((uint64_t)m_ListingID);

    // Mirror read(): length-prefixed, so the two sides agree on a format.
    // Previously this wrote the raw bytes with no prefix, which no reader could
    // have parsed.
    BYTE szKey = (BYTE)m_IdempotencyKey.size();
    if (szKey > 64)
        throw InvalidProtocolException("CGExchangeBuy: idempotency key too long");

    oStream.write(szKey);
    if (szKey > 0)
        oStream.write(m_IdempotencyKey.c_str(), szKey);

    __END_CATCH
}

PacketSize_t CGExchangeBuy::getPacketSize() const {
    PacketSize_t size = 0;
    size += sizeof(m_ListingID);
    size += szBYTE; // the length prefix written by write()
    size += m_IdempotencyKey.size();
    return size;
}

string CGExchangeBuy::toString() const {
    StringStream msg;
    msg << "CGExchangeBuy("
        << "ListingID:" << (int)m_ListingID
        << ",IdempotencyKey:" << m_IdempotencyKey
        << ")";
    return msg.toString();
}

void CGExchangeBuy::execute(Player* pPlayer) {
    __BEGIN_TRY

    CGExchangeBuyHandler::execute(this, pPlayer);

    __END_CATCH
}
